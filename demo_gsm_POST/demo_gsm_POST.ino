// Reference to https://www.espruino.com/datasheets/SIM900_AT.pdf

#include <SoftwareSerial.h>
SoftwareSerial myserial(10, 11); // RX: 10, TX:11
#include <ArduinoJson.h>
StaticJsonDocument<200> jsonBuffer;

char t[32] = "13:22:21 02/06/22";
char deviceID[12] = "MYTEST56";

void setup()
{
    myserial.begin(9600); // the GPRS baud rate
    Serial.begin(9600);
    Serial.println("Initializing..........");

    delay(5000);
}

void loop()
{
    Serial.println("");
    Serial.println("************************************************************");
    float humidity = 50.1;
    float temperature = 27;

    // DateTime now = rtc.now();

    // sprintf(t, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
    // "13:22:21 02/06/22"
    Serial.print("Device ID: ");
    Serial.println(deviceID);
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print(F("Time/Date: "));
    Serial.println(t);
    delay(1000);

    /********************GSM Communication Starts********************/

    if (myserial.available())
        Serial.write(myserial.read());

    myserial.println("AT"); // Init
    delay(3000);

    // Set bearer parameters, on profile num 1
    myserial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
    delay(6000);
    ShowSerialData();


    /**
     * APN 
     *  - Indosat: indosatgprs
     *  - Telkomsel: internet
     * 
     * Uname/pwd
     *  - Indosat: indosat/indosat
     *  - Telkomsel: WAP/WAP123
     */

    myserial.println("AT+SAPBR=3,1,\"APN\",\"indosatgprs\""); // APN 
    delay(6000);
    ShowSerialData();

    myserial.println("AT+SAPBR=3,1,\"USER\",\"indosat\""); // APN
    delay(6000);
    ShowSerialData();

    myserial.println("AT+SAPBR=3,1,\"PWD\",\"indosat\""); // APN
    delay(6000);
    ShowSerialData();

    myserial.println("AT+SAPBR=1,1"); // open bearer
    delay(6000);
    ShowSerialData();

    myserial.println("AT+SAPBR=2,1"); // Query parameters
    delay(6000);
    ShowSerialData();

    myserial.println("AT+HTTPINIT");
    delay(6000);
    ShowSerialData();

    myserial.println("AT+HTTPPARA=\"CID\",1");
    delay(6000);
    ShowSerialData();

    jsonBuffer["deviceID"] = deviceID;
    jsonBuffer["humidity"] = humidity;
    jsonBuffer["temperature"] = temperature;
    jsonBuffer["timedate"] = t;

    serializeJson(jsonBuffer, Serial);
    Serial.println(" ");
    String sendtoserver;
    serializeJson(jsonBuffer, sendtoserver);
    delay(4000);

    myserial.println("AT+HTTPPARA=\"URL\",\"http://202.148.1.57:2079/data\""); // Server address
    delay(4000);
    ShowSerialData();

    // myserial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
    // delay(4000);
    // ShowSerialData();

    // myserial.println("AT+HTTPDATA=" + String(sendtoserver.length()) + ",100000");
    // Serial.println(sendtoserver);
    // delay(6000);
    // ShowSerialData();

    // myserial.println(sendtoserver);
    // delay(6000);
    // ShowSerialData;

    /**
     * HTTPACTION=x
     *  - 0 GET
     *  - 1 POST
     *  - 2 HEAD 
     */
    myserial.println("AT+HTTPACTION=0");
    delay(6000);
    ShowSerialData();

    myserial.println("AT+HTTPREAD");
    delay(6000);
    ShowSerialData();

    myserial.println("AT+HTTPTERM");
    delay(10000);
    ShowSerialData;

    /********************GSM Communication Stops********************/
}

void ShowSerialData()
{
    while (myserial.available() != 0)
        Serial.write(myserial.read());
    delay(1000);
}