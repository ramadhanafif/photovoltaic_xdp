/**Program baca data PV dari uart dan parsing per variabel
   Board:ESP8266

   20-JUL-2022: created
*/

// #define DEBUG_VERBOSE
#define WIFI_CONNECTION

#include <ArduinoJson.h>

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

#ifdef DEBUG_VERBOSE
// pvData components
// used for debugging only
float VA; // Apparent voltage
float VR; // RMS voltage
long mA;  // Current
long R;   // Load
long S;   // ?
long PF;  // Power Factor
long T;   // Time since last data
long t;   // Temperature
#endif

byte current_Serial = 0;
void swapSerial(int uart_no) {
  /**Wrapper to use Serial.swap() in esp8266
     with 10ms delay for stability

     Parameter uart_no:
        0 for uart0 (default)
        2 for uart2 (rx:d7 tx:d8)

     Returns nothing
  */
  delay(5);
  if (uart_no == 0) {
    if (current_Serial == 2) {
      Serial.swap();
      current_Serial = 0;
      // delay(10);
    }
  }
  else if (uart_no == 2) {
    if (current_Serial == 0) {
      Serial.swap();
      current_Serial = 2;
      // delay(10);
    }
  }
  delay(5);
}

void setup() {
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
  String pvData_str;
  char pvData_char[200];

  swapSerial(2);
  if (Serial.available() > 10) {
#ifdef DEBUG_VERBOSE
    long int t1 = millis();
#endif
    // pvData_str = Serial.readStringUntil('\n');
    // pvData_str.toCharArray(pvData_char, 200);
    // DeserializationError error = deserializeJson(pvData_json, pvData_char);

    // swapSerial(0);
    // Serial.println(pvData_char);

    // Parse directly from istream: not consistent.
    // Might be caused by serial swapping
    DeserializationError error = deserializeJson(pvData_json, Serial);
    // swapSerial(0);
    // serializeJson(pvData_json, Serial);

    if (error) {
#ifdef DEBUG_VERBOSE
      swapSerial(0);
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      serializeJson(pvData_json, Serial);
#endif
      return;
    }
    /**In case failed to receive parseable data
        skip current loop and wait for next data
     **/

    else {
      // Add 10 digit customer identifier (var name: idCustomer)
      pvData_json["idCustomer"] = 10987654321;
      serializeJson(pvData_json, pvData_char);

      // Display to serial 0
      swapSerial(0);
      serializeJson(pvData_json, Serial);


#ifdef DEBUG_VERBOSE
      swapSerial(0);
      Serial.print("idCustomerAdded:");
      Serial.println(pvData_char);

      long int t2 = millis();
      swapSerial(0);
      Serial.print("Deserialization time : "); Serial.println(t2 - t1);
#endif

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
#ifdef DEBUG_VERBOSE
      long int t3 = millis();
      swapSerial(0);
      Serial.print("Deserialization to post in : "); Serial.println(t3 - t2);

      // Print data for debug
      swapSerial(0);

      VA = pvData_json["VA"];
      VR = pvData_json["VR"];
      mA = pvData_json["mA"];
      R  = pvData_json["R"];
      S  = pvData_json["S"];
      PF = pvData_json["PF"];
      T  = pvData_json["T"];
      t  = pvData_json["t"];

      Serial.print("VA: "); Serial.println(VA);
      Serial.print("VR: "); Serial.println(VR);
      Serial.print("mA: "); Serial.println(mA);
      Serial.print("R : "); Serial.println(R );
      Serial.print("S : "); Serial.println(S );
      Serial.print("PF: "); Serial.println(PF);
      Serial.print("T : "); Serial.println(T );
      Serial.print("t : "); Serial.println(t );
      Serial.println("**********************");
      serializeJson(pvData_json, Serial);
      Serial.println();
      Serial.println("----------------------");
#endif
    }
  }
  // delay(10);
}
