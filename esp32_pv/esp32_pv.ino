#include <ArduinoJson.h>
#define WIFI_CONNECTION

#ifdef WIFI_CONNECTION
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// #define SERVER_IP "192.168.3.176:2078"
#define SERVER_IP "20.230.176.6:2078"

#ifndef STASSID
#define STASSID "Smart Xirka"
#define STAPSK  "EngInEEr@30"
#endif //End macro condition STA SSID
#endif //End macro condition wifi_connection

StaticJsonDocument<200> pvData_json;

void setup() {
  // Use serial 2 rx to receive data from pv
  Serial2.begin(115200);

  // For debugging
  Serial.begin(115200);

  #ifdef WIFI_CONNECTION
  WiFi.begin(STASSID, STAPSK);

  swapSerial(0);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected with IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void loop() {
  while(!Serial2.available());

  DeserializationError error = deserializeJson(pvData_json, Serial2);
  if (error) {

    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    serializeJson(pvData_json, Serial);
    return;
  }
  else{
    pvData_json["idCustomer"] = 10987654321;

    Serial.println();
    serializeJson(pvData_json,Serial);
  
    /**urutan bagian ini
     * tambah data customer
     * http post
     */

#ifdef WIFI_CONNECTION
      if ((WiFi.status() == WL_CONNECTED)) {
        WiFiClient client;
        HTTPClient http;

        swapSerial(0);
        Serial.print("[HTTP] POST\n");
        // configure traged server and url
        http.begin(client, "http://" SERVER_IP "/esp/"); //HTTP
        http.addHeader("Content-Type", "application/json");

        // Serial.print("[HTTP] POST...\n");
        // start connection and send HTTP header and body
        int httpCode = http.POST(pvData_char);

        // httpCode will be negative on error
        if (httpCode > 0) {
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
        else {
          Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
  }
#endif

  // delayus(1);
}
