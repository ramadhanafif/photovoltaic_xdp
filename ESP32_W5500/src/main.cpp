#include <Arduino.h>
#include <SPI.h>

// These are default SPI pinouts, written explicitly as a reminder.
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS 15

static const int spiClk = 8000000; // 8 MHz for W5500

// uninitalised pointers to SPI objects
SPIClass *hspi = NULL;

void setup()
{
  Serial.begin(115200);
  Serial.println("Simple program to check SPI communication with W5500");

  hspi = new SPIClass(HSPI);

  // For non-standard pin use this line
  // hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS); //SCLK, MISO, MOSI, SS
  hspi->begin();

  // Enable pin slave select as output.
  pinMode(hspi->pinSS(), OUTPUT);
}

void loop()
{
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(hspi->pinSS(), LOW); // pull SS slow to prep other end for transfer
  hspi->transfer(0x00);
  hspi->transfer(0x39);
  hspi->transfer(0x01);
  uint8_t data = hspi->transfer(0);
  digitalWrite(hspi->pinSS(), HIGH); // pull ss high to signify end of data transfer
  hspi->endTransaction();

  Serial.print("W5500 Version: ");
  Serial.println(data);
  delay(1000);
}