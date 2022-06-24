#define JSON_TAG "JSON"
#define HTTP_TAG "HTTP"

#include <ArduinoJson.h>

#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid     = "Smart Xirka";
const char* password = "EngInEEr@30";

#define SERVER_IP "202.148.1.57:2078"

StaticJsonDocument<200> pvData_json;

void setup() {
  // Data from PV module received by Serial2
  Serial2.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /**TODO: bagian nunggu available mungkin gak begitu perlu
   * deserialize mungkin dibiarkan jalan terus?
   * atau pake if... else {return;}
   */
  while (!Serial2.available()) {
    ESP_LOGV("SERIAL", "WAITING UART INPUT FROM PV");
    delay(100);
  }

  DeserializationError error = deserializeJson(pvData_json, Serial2);
  if (error) {
    ESP_LOGD(JSON_TAG, "deserializeJson failed %s", error.f_str());

    return; // Break loop
  }
  else {
    // Add idCustomer number
    pvData_json["idCustomer"] = 10987654321;

    char pvData_char[200];
    serializeJson(pvData_json, pvData_char);
    ESP_LOGD(JSON_TAG, "%s", pvData_char);

    if ((WiFi.status() == WL_CONNECTED)) {

      WiFiClient client;
      HTTPClient http;

      http.begin(client, "http://" SERVER_IP "/esp/"); //HTTP
      http.addHeader("Content-Type", "application/json");
      int http_response = http.POST(pvData_char);

      ESP_LOGI(HTTP_TAG, "Response: %d", http_response);
      if (http_response == 200) {
        ESP_LOGI(HTTP_TAG, "%s", "POST OK");
      }
    }
    delay(10);
  }
}