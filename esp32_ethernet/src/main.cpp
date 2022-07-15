/**
 * @file main.cpp
 * @author Muhammad Afif Ramadhan
 * @brief POSTing data to server with ESP32 and W5500 Ethernet Module
 * @version 1
 * @date 2022-07-15
 *
 */

#include <SPI.h>
#include "Ethernet2.h"
#include <Arduino.h>

// Device MAC Address
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

const char serverAddress[] = "202.148.1.57";
uint16_t port = 2079;

char postData[] = "{\"idUser\":12345,\"AC0\":{\"E\":101,\"Vrms\":221,\"Irms\":0.08,\"P\":0,\"Q\":0,\"S\":0},\"DC\":{\"V\":221,\"I\":0.7,\"P\":30},\"AC1\":{\"E\":0,\"Vrms\":0,\"Irms\":0,\"P\":0,\"Q\":0,\"S\":0}}";

// Create ethernet client
EthernetClient client;

void setup()
{
  Serial.begin(115200);

  while (Ethernet.begin(mac) != 1) // Try DHCP
  {
    Serial.println("DHCP failed, retrying");
    delay(1000);
  }

  // Print local IP address:
  Serial.print("Connected with device IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++)
  {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();

  if (client.connect(serverAddress, port))
  {
    Serial.println("Connected to server! Sending this payload:\n\n");
    Serial.println("POST /data HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(serverAddress);
    Serial.println("Content-Type: application/json");
    Serial.println("Connection: close");
    Serial.print("Content-Length: ");
    Serial.println(strlen(postData));
    Serial.println();
    Serial.println(postData);
    Serial.println();


    client.println("POST /data HTTP/1.1");
    client.print("Host: ");
    client.println(serverAddress);
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(strlen(postData));
    client.println();
    client.println(postData);
    delay(100);
  }

  while (1)
  { // read server response
    if (client.available())
    {
      char c = client.read();
      Serial.print(c);
    }
    else
    {
      client.stop();
    }
  }
}

void loop()
{
}
