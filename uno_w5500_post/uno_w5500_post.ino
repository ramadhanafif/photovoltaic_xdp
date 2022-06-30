/**
 * @file uno_w5500_post.ino
 * @author Muhammad Afif Ramadhan
 * @brief POST data to server, token acquisition process is omitted.
 * Board: Arduino Uno
 * @version 1
 * @date 2022-06-30
 *
 */

/******Minimal POST Template******
POST /data HTTP/1.1
Host: 202.148.1.57
token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZFVzZXIiOiIzMSIsIlZBIjowfQ.8TBNovTzmnAHHqTSOj0A--hRmE0ndFiXYQcC6M8H6qk
Content-Type: application/json
Content-Length: 78

{"idUser":31,"VA":400,"VR":0,"mA":10,"R":100,"S":100,"PF":10,"T":1024,"t":10}
*/

#include <Ethernet2.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

char serverAddress[] = "202.148.1.57";
int port = 2079;
char postData[] = "{\
\"idUser\":31,\
\"VA\":991919,\
\"VR\":0,\
\"mA\":10,\
\"R\":100,\
\"S\":100,\
\"PF\":10,\
\"T\":1024,\
\"t\":10\
}";

EthernetClient ethernet;

long unsigned int t1, t2;

void setup()
{
    Serial.begin(115200);

    while (Ethernet.begin(mac) != 1) //Called from Etherent class, different from EthernetClient class
    {
        // DHCP server required!
        Serial.println("Error getting IP address via DHCP, trying again...");
        delay(1000);
    }

    Serial.println("POST request");

    if (ethernet.connect(serverAddress, port))
    {
        Serial.println("Connected");
        ethernet.println("POST /data HTTP/1.1");
        ethernet.println("Host: 202.148.1.57");
        ethernet.println("Content-Type: application/json");
        ethernet.println("Connection: close");
        ethernet.println("Content-Length: 80");
        ethernet.println("token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZFVzZXIiOiIzMSIsIlZBIjowfQ.8TBNovTzmnAHHqTSOj0A--hRmE0ndFiXYQcC6M8H6qk");
        ethernet.println();
        ethernet.println(postData);
        delay(100); //delay 10-300 ms waiting for server response, depending on geographical location
    }
}
void loop()
{
    if (ethernet.available()) // print out server response
    {
        char c = ethernet.read();
        Serial.print(c);
    }

    if (!ethernet.available()) // if no more reply, then disconnect
    {
        ethernet.stop();
    }
}