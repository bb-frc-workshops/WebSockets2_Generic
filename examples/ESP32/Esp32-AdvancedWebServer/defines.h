/****************************************************************************************************************************
  defines.h
  For nRF52 with WiFiNINA module/shield.

  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52 and SAMD21/SAMD51 boards besides ESP8266 and ESP32

  The library provides simple and easy interface for websockets (Client and Server).

  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license
 *****************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#if !( defined(ESP8266) ||  defined(ESP32) )
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#elif ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_PROS2 || ARDUINO_MICROS2 )
  #error This code doesn't support ESP32_S2 yet.
  #define BOARD_TYPE      "ESP32-S2"
#else
  #define BOARD_TYPE      "ESP32"
#endif

#ifndef BOARD_NAME
  #define BOARD_NAME    BOARD_TYPE
#endif

#define DEBUG_WEBSOCKETS_PORT     Serial
// Debug Level from 0 to 4
#define _WEBSOCKETS_LOGLEVEL_     3

const char* ssid = "ssid"; //Enter SSID
const char* password = "password"; //Enter Password

const uint16_t websockets_server_port = 8080; // Enter server port

// Select the IP address according to your local network
IPAddress serverIP(192, 168, 2, 95);

IPAddress static_GW(192, 168, 2, 1);
IPAddress static_SN(255, 255, 255, 0);

#endif      //defines_h
