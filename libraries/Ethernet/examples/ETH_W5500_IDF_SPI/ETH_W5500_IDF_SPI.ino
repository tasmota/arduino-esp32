/*
    This sketch shows the Ethernet event usage

*/

#define ETH_TYPE        ETH_PHY_W5500
#define ETH_ADDR         1
#define ETH_CS          15
#define ETH_IRQ          4
#define ETH_RST          5
#define ETH_SPI_HOST    SPI2_HOST
#define ETH_SPI_SCK     14
#define ETH_SPI_MISO    12
#define ETH_SPI_MOSI    13

#include <ETH.h>

static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH Got IP");
      ETH.printInfo(Serial);
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void setup()
{
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
  ETH.begin(ETH_TYPE, ETH_ADDR, ETH_CS, ETH_IRQ, ETH_RST, ETH_SPI_HOST, ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
}


void loop()
{
  if (eth_connected) {
    testClient("google.com", 80);
  }
  delay(10000);
}
