/****************************************************************************************************************************
  Esp8266-AdvancedWebServer.ino
  For Esp8266.

  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52 and SAMD21/SAMD51 boards besides ESP8266 and ESP32

  The library provides simple and easy interface for websockets (Client and Server).

  Example first created on: 10.05.2018
  Original Author: Markus Sattler

  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license
  Version: 1.0.7

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      14/07/2020 Initial coding/porting to support nRF52 and SAMD21/SAMD51 boards. Add SINRIC/Alexa support
  1.0.1   K Hoang      16/07/2020 Add support to Ethernet W5x00 to nRF52, SAMD21/SAMD51 and SAM DUE boards
  1.0.2   K Hoang      18/07/2020 Add support to Ethernet ENC28J60 to nRF52, SAMD21/SAMD51 and SAM DUE boards
  1.0.3   K Hoang      18/07/2020 Add support to STM32F boards using Ethernet W5x00, ENC28J60 and LAN8742A 
  1.0.4   K Hoang      27/07/2020 Add support to STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 using 
                                  Ethernet W5x00, ENC28J60, LAN8742A and WiFiNINA. Add examples and Packages' Patches.
  1.0.5   K Hoang      29/07/2020 Sync with ArduinoWebsockets v0.4.18 to fix ESP8266 SSL bug.
  1.0.6   K Hoang      06/08/2020 Add non-blocking WebSocketsServer feature and non-blocking examples.       
  1.0.7   K Hoang      03/10/2020 Add support to Ethernet ENC28J60 using EthernetENC and UIPEthernet v2.0.9
 *****************************************************************************************************************************/

#include "defines.h"

#include <WebSockets2_Generic.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

using namespace websockets2_generic;

WebsocketsServer SocketsServer;

#define WEBSOCKETS_PORT     8080

ESP8266WebServer server(80);

// Select the IP address according to your local network
IPAddress serverIP(192, 168, 2, 95);

IPAddress static_GW(192, 168, 2, 1);
IPAddress static_SN(255, 255, 255, 0);

void handleRoot()
{
#define BUFFER_SIZE     400

  char temp[BUFFER_SIZE];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1,
           "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>AdvancedWebServer %s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h2>WebSockets&WebServer!</h2>\
<h3>both on %s</h3>\
<p>Uptime: %d d %02d:%02d:%02d</p>\
<img src=\"/test.svg\" />\
</body>\
</html>", BOARD_NAME, BOARD_NAME, day, hr % 24, min % 60, sec % 60);

  server.send(200, "text/html", temp);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void drawGraph()
{
  String out;
  out.reserve(3000);
  char temp[70];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"310\" height=\"150\">\n";
  out += "<rect width=\"310\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"5\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"blue\">\n";

  int y = rand() % 130;

  for (int x = 15; x < 300; x += 15)
  {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 15, 140 - y2);
    out += temp;
    y = y2;
  }

  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting WebSockets2_Generic Esp8266-AdvancedWebServer on " + String(BOARD_NAME));

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  WiFi.config(serverIP, static_GW, static_SN);

  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Connected to Wifi, IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nNo WiFi");
    return;
  }

  SocketsServer.listen(WEBSOCKETS_PORT);

  Serial.print(SocketsServer.available() ? "WebSockets Server Running and Ready on " : "Server Not Running on ");
  Serial.println(BOARD_NAME);
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print(", Port: ");
  Serial.println(WEBSOCKETS_PORT);    // Websockets Server Port

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []()
  {
    server.send(200, "text/plain", "This works as well");
  });

  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();

  WebsocketsClient client = SocketsServer.accept();

  if (client.available())
  {
    WebsocketsMessage msg = client.readNonBlocking();

    // log
    Serial.print("Got Message: ");
    Serial.println(msg.data());

    // return echo
    client.send("Echo: " + msg.data());

    // close the connection
    client.close();
  }
}