#define ARDUINO_CORE_BUILD
#include "PPP.h"
#if CONFIG_LWIP_PPP_SUPPORT
#include "esp32-hal-periman.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include <string>

typedef struct { void * arg; } PdpContext;
#include "esp_modem_api.h"

static PPPClass * _esp_modem = NULL;
static esp_event_handler_instance_t _ppp_ev_instance = NULL;

static void _ppp_event_cb(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    log_v("PPP EVENT %ld::%ld", (int)event_base, event_id);
    if (event_base == NETIF_PPP_STATUS){
        if(_esp_modem != NULL){
            _esp_modem->_onPppEvent(event_id, event_data);
        }
    }
}

static void _ppp_error_cb(esp_modem_terminal_error_t err){

}

static void onPppConnected(arduino_event_id_t event, arduino_event_info_t info)
{
    if(event == ARDUINO_EVENT_PPP_CONNECTED){
        if (_esp_modem->getStatusBits() & ESP_NETIF_WANT_IP6_BIT){
            esp_err_t err = esp_netif_create_ip6_linklocal(_esp_modem->netif());
            if(err != ESP_OK){
                log_e("Failed to enable IPv6 Link Local on PPP: [%d] %s", err, esp_err_to_name(err));
            } else {
                log_v("Enabled IPv6 Link Local on %s", _esp_modem->desc());
            }
        }
    }
}

esp_modem_dce_t * PPPClass::handle() const {
    return _dce;
}

void PPPClass::_onPppEvent(int32_t event_id, void* event_data){
    arduino_event_t arduino_event;
    arduino_event.event_id = ARDUINO_EVENT_MAX;

    log_v("PPP EVENT %ld", event_id);
    
    // if (event_id == ETHERNET_EVENT_CONNECTED) {
    //     log_v("%s Connected", desc());
    //     arduino_event.event_id = ARDUINO_EVENT_ETH_CONNECTED;
    //     arduino_event.event_info.eth_connected = handle();
    //     setStatusBits(ESP_NETIF_CONNECTED_BIT);
    // } else if (event_id == ETHERNET_EVENT_DISCONNECTED) {
    //     log_v("%s Disconnected", desc());
    //     arduino_event.event_id = ARDUINO_EVENT_ETH_DISCONNECTED;
    //     clearStatusBits(ESP_NETIF_CONNECTED_BIT | ESP_NETIF_HAS_IP_BIT | ESP_NETIF_HAS_LOCAL_IP6_BIT | ESP_NETIF_HAS_GLOBAL_IP6_BIT);
    // } else if (event_id == ETHERNET_EVENT_START) {
    //     log_v("%s Started", desc());
    //     arduino_event.event_id = ARDUINO_EVENT_ETH_START;
    //     setStatusBits(ESP_NETIF_STARTED_BIT);
    // } else if (event_id == ETHERNET_EVENT_STOP) {
    //     log_v("%s Stopped", desc());
    //     arduino_event.event_id = ARDUINO_EVENT_ETH_STOP;
    //     clearStatusBits(ESP_NETIF_STARTED_BIT | ESP_NETIF_CONNECTED_BIT | ESP_NETIF_HAS_IP_BIT | ESP_NETIF_HAS_LOCAL_IP6_BIT | ESP_NETIF_HAS_GLOBAL_IP6_BIT | ESP_NETIF_HAS_STATIC_IP_BIT);
    // }

    if(arduino_event.event_id < ARDUINO_EVENT_MAX){
        Network.postEvent(&arduino_event);
    }
}

PPPClass::PPPClass()
    :_dce(NULL)
    ,_pin_tx(-1)
    ,_pin_rx(-1)
    ,_pin_rts(-1)
    ,_pin_cts(-1)
    ,_pin(NULL)
    ,_apn(NULL)
    ,_rx_buffer_size(1024)
    ,_tx_buffer_size(512)
    ,_mode(ESP_MODEM_MODE_COMMAND)
{
}

PPPClass::~PPPClass()
{}

bool PPPClass::pppDetachBus(void * bus_pointer){
    PPPClass *bus = (PPPClass *) bus_pointer;
    bus->end();
    return true;
}

bool PPPClass::begin(ppp_modem_model_t model, int8_t tx, int8_t rx, int8_t rts, int8_t cts, esp_modem_flow_ctrl_t flow_ctrl){
    esp_err_t ret = ESP_OK;
    bool pin_ok = false;

    if(_esp_netif != NULL || _dce != NULL){
        log_w("PPP Already Started");
        return true;
    }

    if(_apn == NULL){
        log_e("APN is not set. Call 'PPP.setApn()' first");
        return false;
    }

    perimanSetBusDeinit(ESP32_BUS_TYPE_PPP_TX, PPPClass::pppDetachBus);
    perimanSetBusDeinit(ESP32_BUS_TYPE_PPP_RX, PPPClass::pppDetachBus);
    perimanSetBusDeinit(ESP32_BUS_TYPE_PPP_RTS, PPPClass::pppDetachBus);
    perimanSetBusDeinit(ESP32_BUS_TYPE_PPP_CTS, PPPClass::pppDetachBus);

    if(_pin_tx != -1){
        if(!perimanClearPinBus(_pin_tx)){ return false; }
    }
    if(_pin_rx != -1){
        if(!perimanClearPinBus(_pin_rx)){ return false; }
    }
    if(_pin_rts != -1){
        if(!perimanClearPinBus(_pin_rts)){ return false; }
    }
    if(_pin_cts != -1){
        if(!perimanClearPinBus(_pin_cts)){ return false; }
    }

    _flow_ctrl = flow_ctrl;
    _pin_tx = digitalPinToGPIONumber(tx);
    _pin_rx = digitalPinToGPIONumber(rx);
    _pin_rts = digitalPinToGPIONumber(rts);
    _pin_cts = digitalPinToGPIONumber(cts);

    Network.begin();
    _esp_modem = this;
    if(_ppp_ev_instance == NULL && esp_event_handler_instance_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &_ppp_event_cb, NULL, &_ppp_ev_instance)){
        log_e("event_handler_instance_register for NETIF_PPP_STATUS Failed!");
        return false;
    }

    /* Configure the PPP netif */
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_PPP();
    _esp_netif = esp_netif_new(&cfg);
    if(_esp_netif == NULL){
        log_e("esp_netif_new failed");
        return false;
    }

    /* attach to receive events */
    initNetif(ESP_NETIF_ID_PPP);

    /* Configure the DTE */
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    /* setup UART specific configuration based on kconfig options */
    dte_config.uart_config.tx_io_num = _pin_tx;
    dte_config.uart_config.rx_io_num = _pin_rx;
    dte_config.uart_config.rts_io_num = _pin_rts;
    dte_config.uart_config.cts_io_num = _pin_cts;
    dte_config.uart_config.flow_control = _flow_ctrl;
    dte_config.uart_config.rx_buffer_size = _rx_buffer_size;
    dte_config.uart_config.tx_buffer_size = _tx_buffer_size;
    dte_config.uart_config.event_queue_size = 20;
    dte_config.task_stack_size = 2048;
    dte_config.task_priority = 5;
    dte_config.dte_buffer_size = _rx_buffer_size / 2;

    /* Configure the DCE */
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(_apn);
    _dce = esp_modem_new_dev((esp_modem_dce_device_t)model, &dte_config, &dce_config, _esp_netif);
    if(_dce == NULL){
        log_e("esp_modem_new_dev failed");
        goto err;
    }

    esp_modem_set_error_cb(_dce, _ppp_error_cb);

    if (dte_config.uart_config.flow_control == ESP_MODEM_FLOW_CONTROL_HW) {
        ret = esp_modem_set_flow_control(_dce, 2, 2);  //2/2 means HW Flow Control.
        if (ret != ESP_OK) {
            log_e("Failed to set the set_flow_control mode: [%d] %s", ret, esp_err_to_name(ret));
            goto err;
        }
    }

    // check if PIN needed
    if (esp_modem_read_pin(_dce, pin_ok) == ESP_OK && pin_ok == false) {
        if (_pin == NULL || esp_modem_set_pin(_dce, _pin) != ESP_OK) {
            log_e("PIN verification failed!");
            goto err;
        }
        // delay(1000);
    }

    if(_pin_tx != -1){
        if(!perimanSetPinBus(_pin_tx, ESP32_BUS_TYPE_PPP_TX, (void *)(this), -1, -1)){ goto err; }
    }
    if(_pin_rx != -1){
        if(!perimanSetPinBus(_pin_rx, ESP32_BUS_TYPE_PPP_RX, (void *)(this), -1, -1)){ goto err; }
    }
    if(_pin_rts != -1){
        if(!perimanSetPinBus(_pin_rts,  ESP32_BUS_TYPE_PPP_RTS, (void *)(this), -1, -1)){ goto err; }
    }
    if(_pin_cts != -1){
        if(!perimanSetPinBus(_pin_cts,  ESP32_BUS_TYPE_PPP_CTS, (void *)(this), -1, -1)){ goto err; }
    }

    Network.onSysEvent(onPppConnected, ARDUINO_EVENT_PPP_CONNECTED);

    return true;

err:
    log_e("Failed to set all pins bus to ETHERNET");
    PPPClass::pppDetachBus((void *)(this));
    return false;
}

void PPPClass::end(void)
{
    destroyNetif();

    if(_ppp_ev_instance != NULL){
        if(esp_event_handler_unregister(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &_ppp_event_cb) == ESP_OK){
            _ppp_ev_instance = NULL;
        }
    }
    _esp_modem = NULL;

    Network.removeEvent(onPppConnected, ARDUINO_EVENT_PPP_CONNECTED);

    if(_dce != NULL){
        esp_modem_destroy(_dce);
        _dce = NULL;
    }

    if(_pin_tx != -1){
        perimanClearPinBus(_pin_tx);
        _pin_tx = -1;
    }
    if(_pin_rx != -1){
        perimanClearPinBus(_pin_rx);
        _pin_rx = -1;
    }
    if(_pin_rts != -1){
        perimanClearPinBus(_pin_rts);
        _pin_rts = -1;
    }
    if(_pin_cts != -1){
        perimanClearPinBus(_pin_cts);
        _pin_cts = -1;
    }

    _mode = ESP_MODEM_MODE_COMMAND;
}

bool PPPClass::mode(esp_modem_dce_mode_t m){
    if(_dce == NULL){
        return 0;
    }

    if(_mode == m){
        return true;
    }
    esp_err_t err = esp_modem_set_mode(_dce, m);
    if (err != ESP_OK) {
        log_e("esp_modem_set_mode failed with %d %s", err, esp_err_to_name(err));
        return false;
    }
    _mode = m;
    return true;
}

bool PPPClass::setApn(const char * apn){
    if(_apn != NULL){
        free((void *)_apn);
        _apn = NULL;
    }
    if(apn != NULL){
        _apn = strdup(apn);
        if(_apn == NULL){
            log_e("Failed to strdup APN");
            return false;
        }
    }
    return true;
}

bool PPPClass::setPin(const char * pin){
    if(_pin != NULL){
        free((void *)_pin);
        _pin = NULL;
    }
    if(pin != NULL){
        for(int i=0; i<strlen(pin); i++){
            if(pin[i] < 0x30 || pin[i] > 0x39){
                log_e("Bad character '%c' in PIN. Should be only digits", pin[i]);
                return false;
            }
        }
        _pin = strdup(pin);
        if(_pin == NULL){
            log_e("Failed to strdup PIN");
            return false;
        }
    }
    return true;
}

int PPPClass::RSSI() const
{
    if(_dce == NULL){
        return 0;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return 0;
    }

    int rssi, ber;
    esp_err_t err = esp_modem_get_signal_quality(_dce, rssi, ber);
    if (err != ESP_OK) {
        log_e("esp_modem_get_signal_quality failed with %d %s", err, esp_err_to_name(err));
        return 0;
    }
    return rssi;
}

int PPPClass::BER() const
{
    if(_dce == NULL){
        return 0;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return 0;
    }

    int rssi, ber;
    esp_err_t err = esp_modem_get_signal_quality(_dce, rssi, ber);
    if (err != ESP_OK) {
        log_e("esp_modem_get_signal_quality failed with %d %s", err, esp_err_to_name(err));
        return 0;
    }
    return ber;
}

String PPPClass::IMSI() const
{
    if(_dce == NULL){
        return String();
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return String();
    }

    char imsi[32];
    esp_err_t err = esp_modem_get_imsi(_dce, (std::string&)imsi);
    if (err != ESP_OK) {
        log_e("esp_modem_get_imsi failed with %d %s", err, esp_err_to_name(err));
        return String();
    }

    return String(imsi);
}

String PPPClass::IMEI() const
{
    if(_dce == NULL){
        return String();
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return String();
    }

    char imei[32];
    esp_err_t err = esp_modem_get_imei(_dce, (std::string&)imei);
    if (err != ESP_OK) {
        log_e("esp_modem_get_imei failed with %d %s", err, esp_err_to_name(err));
        return String();
    }

    return String(imei);
}

String PPPClass::moduleName() const
{
    if(_dce == NULL){
        return String();
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return String();
    }

    char name[32];
    esp_err_t err = esp_modem_get_module_name(_dce, (std::string&)name);
    if (err != ESP_OK) {
        log_e("esp_modem_get_module_name failed with %d %s", err, esp_err_to_name(err));
        return String();
    }

    return String(name);
}

String PPPClass::operatorName() const
{
    if(_dce == NULL){
        return String();
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return String();
    }

    char oper[32];
    int act = 0;
    esp_err_t err = esp_modem_get_operator_name(_dce, (std::string&)oper, act);
    if (err != ESP_OK) {
        log_e("esp_modem_get_operator_name failed with %d %s", err, esp_err_to_name(err));
        return String();
    }

    return String(oper);
}

int PPPClass::networkMode() const
{
    if(_dce == NULL){
        return 0;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return 0;
    }

    int m = 0;
    esp_err_t err = esp_modem_get_network_system_mode(_dce, m);
    if (err != ESP_OK) {
        log_e("esp_modem_get_network_system_mode failed with %d %s", err, esp_err_to_name(err));
        return 0;
    }
    return m;
}

int PPPClass::radioState() const
{
    if(_dce == NULL){
        return 0;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return 0;
    }

    int m = 0;
    esp_err_t err = esp_modem_get_radio_state(_dce, m);
    if (err != ESP_OK) {
        log_e("esp_modem_get_radio_state failed with %d %s", err, esp_err_to_name(err));
        return 0;
    }
    return m;
}

bool PPPClass::attached() const
{
    if(_dce == NULL){
        return false;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return false;
    }

    int m = 0;
    esp_err_t err = esp_modem_get_network_attachment_state(_dce, m);
    if (err != ESP_OK) {
        // log_e("esp_modem_get_network_attachment_state failed with %d %s", err, esp_err_to_name(err));
        return false;
    }
    return m != 0;
}

bool PPPClass::powerDown(){
    if(_dce == NULL){
        return false;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return false;
    }

    esp_err_t err = esp_modem_power_down(_dce);
    if (err != ESP_OK) {
        log_e("esp_modem_power_down failed with %d %s", err, esp_err_to_name(err));
        return false;
    }
    return true;
}

bool PPPClass::reset(){
    if(_dce == NULL){
        return false;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return false;
    }

    esp_err_t err = esp_modem_reset(_dce);
    if (err != ESP_OK) {
        log_e("esp_modem_reset failed with %d %s", err, esp_err_to_name(err));
        return false;
    }
    return true;
}

bool PPPClass::storeProfile(){
    if(_dce == NULL){
        return false;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return false;
    }

    esp_err_t err = esp_modem_store_profile(_dce);
    if (err != ESP_OK) {
        log_e("esp_modem_store_profile failed with %d %s", err, esp_err_to_name(err));
        return false;
    }
    return true;
}


bool PPPClass::sms(const char * num, const char * message) {
    if(_dce == NULL){
        return false;
    }

    if(_mode != ESP_MODEM_MODE_COMMAND){
        log_e("Wrong modem mode. Should be ESP_MODEM_MODE_COMMAND");
        return false;
    }

    for(int i=0; i<strlen(num); i++){
        if(num[i] != '+' && num[i] != '#' && num[i] != '*' && (num[i] < 0x30 || num[i] > 0x39)){
            log_e("Bad character '%c' in SMS Number. Should be only digits and +, # or *", num[i]);
            return false;
        }
    }

    esp_err_t err = esp_modem_sms_txt_mode(_dce, true);
    if (err != ESP_OK) {
        log_e("Setting text mode failed %d %s", err, esp_err_to_name(err));
        return false;
    }

    err = esp_modem_sms_character_set(_dce);
    if (err != ESP_OK) {
        log_e("Setting GSM character set failed %d %s", err, esp_err_to_name(err));
        return false;
    }

    err = esp_modem_send_sms(_dce, num, message);
    if (err != ESP_OK) {
        log_e("esp_modem_send_sms() failed with %d %s", err, esp_err_to_name(err));
        return false;
    }
    return true;
}

size_t PPPClass::printDriverInfo(Print & out) const {
    size_t bytes = 0;
    //bytes += out.print(",");
    return bytes;
}

PPPClass PPP;

#endif /* CONFIG_LWIP_PPP_SUPPORT */
