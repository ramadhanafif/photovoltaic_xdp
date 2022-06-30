/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "Smart Xirka";
const char *password = "EngInEEr@30";

// Your Domain name with URL path or IP address with path
String serverName = "http://20.230.176.6:2078/esp";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  //  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

#define SERVER_IP "20.230.176.6:2078"

void loop()
{
  if ((WiFi.status() == WL_CONNECTED))
  {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] POST\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/esp/"); // HTTP
    http.addHeader("Content-Type", "application/json");

    // Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"VA\": 9999, \"VR\": 0, \"mA\": 0, \"R\": 0, \"S\": 0, \"PF\": 0, \"T\": 1993, \"t\": 32, \"idCustomer\": 10987654321}");

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] response code: %d\n", httpCode);

      // // file found at server
      // if (httpCode == HTTP_CODE_OK) {
      //   const String& payload = http.getString();
      //   Serial.println("received payload:\n<<");
      //   Serial.println(payload);
      //   Serial.println(">>");
      // }
    }
    else
    {
      Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
