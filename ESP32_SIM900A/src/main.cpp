#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial gprsSerial(5, 18);

void ShowSerialData();

void setup()
{
  gprsSerial.begin(9600); // the GPRS baud rate
  Serial.begin(115200);   // the GPRS baud rate

  Serial.println("Start Prog");
  delay(10000); // gprsSerial wait for RDY
}

void gprsSend(void)
{
}

void loop()
{

  Serial.println("Start LOOP");

  gprsSerial.println("AT"); /* Check Communication */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  /* Configure bearer profile 1 */
  Serial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\\r\\n");
  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); /* Connection type GPRS */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+SAPBR=3,1,\"APN\",\"internet\"\\r\\n");
  gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"internet\""); /* APN of the provider */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+SAPBR=1,1\\r\\n");
  gprsSerial.println("AT+SAPBR=1,1"); /* Open GPRS context */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+SAPBR=2,1\\r\\n");
  gprsSerial.println("AT+SAPBR=2,1"); /* Query the GPRS context */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPINIT\\r\\n");
  gprsSerial.println("AT+HTTPINIT"); /* Initialize HTTP service */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPPARA=\"CID\",1\\r\\n");
  gprsSerial.println("AT+HTTPPARA=\"CID\",1"); /* Set parameters for HTTP session */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPPARA=\"URL\",\"http://202.148.1.57:2079/data\"");
  gprsSerial.println("AT+HTTPPARA=\"URL\",\"http://202.148.1.57:2079/data\""); /* Set parameters for HTTP session */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  gprsSerial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\""); /* Set HEADER for HTTP session */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  char pvData_char[400] = "{\"idUser\":31,\"VA\":13216075,\"VR\":0,\"mA\":10,\"R\":100,\"S\":100,\"PF\":10,\"T\":1024,\"t\":10}";

  Serial.printf("AT+HTTPDATA=%d,%d\n", strlen(pvData_char), 10000);
  gprsSerial.printf("AT+HTTPDATA=%d,%d\n", strlen(pvData_char), 10000); /* POST data of size 33 Bytes with maximum latency time of 10seconds for inputting the data*/
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.printf("ECHO:%s\n",pvData_char); /* Data to be sent */
  gprsSerial.println(pvData_char);
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPACTION=1\\r\\n");
  gprsSerial.println("AT+HTTPACTION=1"); /* Start POST session */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPREAD\\r\\n");
  gprsSerial.println("AT+HTTPREAD"); /* READ SERVER RESPONSE */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+HTTPTERM\\r\\n");
  gprsSerial.println("AT+HTTPTERM"); /* Terminate HTTP service */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);

  Serial.println("AT+SAPBR=0,1\\r\\n");
  gprsSerial.println("AT+SAPBR=0,1"); /* Close GPRS context */
  delay(5000);
  ShowSerialData(); /* Print response on the serial monitor */
  delay(5000);
}
void ShowSerialData()
{
  while (gprsSerial.available() > 0)
    Serial.print(gprsSerial.readString());
}
