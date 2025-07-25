/*
 ESP8266WiFiAP.h - esp8266 Wifi support.
 Based on WiFi.h from Arduino WiFi shield library.
 Copyright (c) 2011-2014 Arduino.  All right reserved.
 Modified by Ivan Grokhotkov, December 2014
 Reworked by Markus Sattler, December 2015

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_WIFI_SUPPORTED || CONFIG_ESP_WIFI_REMOTE_ENABLED

#include "esp_wifi_types.h"
#include "WiFiType.h"
#include "WiFiGeneric.h"

#define WIFI_AP_DEFAULT_AUTH_MODE WIFI_AUTH_WPA2_PSK
#define WIFI_AP_DEFAULT_CIPHER    WIFI_CIPHER_TYPE_CCMP  // Disable by default enabled insecure TKIP and use just CCMP.

// ----------------------------------------------------------------------------------------------
// ------------------------------------ NEW AP Implementation  ----------------------------------
// ----------------------------------------------------------------------------------------------

class APClass : public NetworkInterface {
public:
  APClass();
  ~APClass();

  bool begin();
  bool end();

  bool create(
    const char *ssid, const char *passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false,
    wifi_auth_mode_t auth_mode = WIFI_AP_DEFAULT_AUTH_MODE, wifi_cipher_type_t cipher = WIFI_AP_DEFAULT_CIPHER
  );
  bool clear();

  bool bandwidth(wifi_bandwidth_t bandwidth);
  bool enableNAPT(bool enable = true);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 2)
  bool enableDhcpCaptivePortal();
#endif

  String SSID(void) const;
  uint8_t stationCount();

  void _onApEvent(int32_t event_id, void *event_data);

protected:
  network_event_handle_t _wifi_ap_event_handle;

  size_t printDriverInfo(Print &out) const;

  friend class WiFiGenericClass;
  bool onEnable();
  bool onDisable();
};

// ----------------------------------------------------------------------------------------------
// ------------------------------- OLD AP API (compatibility)  ----------------------------------
// ----------------------------------------------------------------------------------------------

class WiFiAPClass {

public:
  APClass AP;

  bool softAP(
    const char *ssid, const char *passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false,
    wifi_auth_mode_t auth_mode = WIFI_AP_DEFAULT_AUTH_MODE, wifi_cipher_type_t cipher = WIFI_AP_DEFAULT_CIPHER
  );
  bool softAP(
    const String &ssid, const String &passphrase = emptyString, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false,
    wifi_auth_mode_t auth_mode = WIFI_AP_DEFAULT_AUTH_MODE, wifi_cipher_type_t cipher = WIFI_AP_DEFAULT_CIPHER
  ) {
    return softAP(ssid.c_str(), passphrase.c_str(), channel, ssid_hidden, max_connection, ftm_responder, auth_mode, cipher);
  }

  bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dhcp_lease_start = (uint32_t)0, IPAddress dns = (uint32_t)0);
  bool softAPdisconnect(bool wifioff = false);

  bool softAPbandwidth(wifi_bandwidth_t bandwidth);

  uint8_t softAPgetStationNum();
  String softAPSSID(void) const;

  IPAddress softAPIP();
  IPAddress softAPBroadcastIP();
  IPAddress softAPNetworkID();
  IPAddress softAPSubnetMask();
  uint8_t softAPSubnetCIDR();

#if CONFIG_LWIP_IPV6
  bool softAPenableIPv6(bool enable = true);
  IPAddress softAPlinkLocalIPv6();
#endif

  const char *softAPgetHostname();
  bool softAPsetHostname(const char *hostname);

  uint8_t *softAPmacAddress(uint8_t *mac);
  String softAPmacAddress(void);

protected:
};

#endif /* SOC_WIFI_SUPPORTED*/
