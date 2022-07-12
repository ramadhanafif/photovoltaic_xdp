#include <Arduino.h>

#include "rn8209c.h"

#define SERIAL_DEBUG_EN
#define EM1_SERIAL Serial2
#define RST_PIN 17 // pin TXnya ESP

// TODO: Variabel-variabel ganti jadi yang global
// TODO: Buat object based library

// Variabel untuk sampling (DEBUG)
unsigned long start_milis;
unsigned long start_calculation;

unsigned char baca_tulis_address(unsigned char addr, unsigned char len, unsigned char *buf, bool write = false)
{
  if (write)
  {
    addr |= (1 << 7);
  }
  unsigned char checkSum = addr;

  EM1_SERIAL.write(addr);
  start_milis = millis();
  unsigned char i;

  if (write)
  {
    for (i = 0; i < len; i++)
    {
      EM1_SERIAL.write(buf[i]);
      checkSum += buf[i];
    }
    checkSum = ~(checkSum);
    EM1_SERIAL.write(checkSum);
  }

  else
  { // BACA sesuai jumlah bytes (LEN)
    for (i = 0; i < len; i++)
    {
      start_milis = millis();
      while (!EM1_SERIAL.available())
      {
        // Serial.println("halo");
        if (millis() - start_milis > 100)
          break;
      }; // Timeout in wating Serial
      if (EM1_SERIAL.available())
      {
        buf[i] = EM1_SERIAL.read();
      }
      // else
      // {
      //   break;
      // }
    }
  }
  return i;
}

void readCalibRegs()
{

  /****** Vars for printing out register values *****/
  unsigned char bacaADSYSCON[3];
  unsigned char bacaADPhsA[2];
  unsigned char bacaADAPOSA[3];
  unsigned char bacaADGPQA[3];
  unsigned char bacaADRPOSA[3];
  unsigned char bacaADQPHSCAL[3];
  unsigned char bacaADEMUCON[3];

  uint16_t sysconRead;
  int16_t offsetActiveRead;
  int16_t gainActiveRead;
  int16_t offsetReactiveRead;
  int16_t phaseReactiveRead;
  int16_t ADEMUCONRead;

  baca_tulis_address(ADSYSCON, 3, bacaADSYSCON);
  sysconRead = (((uint16_t)(bacaADSYSCON[0])) << 8) | bacaADSYSCON[1];
  Serial.print("ADSYSCON (0x1602) : 0x");
  Serial.println(sysconRead, HEX);

  baca_tulis_address(ADPhsA, 2, bacaADPhsA);
  Serial.print("ADPhsA (40) : ");
  Serial.println((signed char)bacaADPhsA[0]);

  baca_tulis_address(ADAPOSA, 3, bacaADAPOSA);
  offsetActiveRead = ((int16_t)(bacaADAPOSA[0]) << 8) | bacaADAPOSA[1];
  Serial.print("ADAPOSA (0) : ");
  Serial.println(offsetActiveRead);

  baca_tulis_address(ADGPQA, 3, bacaADGPQA);
  gainActiveRead = ((int16_t)(bacaADGPQA[0]) << 8) | bacaADGPQA[1];
  Serial.print("ADGPQA (12640) : ");
  Serial.println(gainActiveRead);

  baca_tulis_address(ADRPOSA, 3, bacaADRPOSA);
  offsetReactiveRead = ((int16_t)(bacaADRPOSA[0]) << 8) | bacaADRPOSA[1];
  Serial.print("ADRPOSA (0) : ");
  Serial.println(offsetReactiveRead);

  baca_tulis_address(ADQPHSCAL, 3, bacaADQPHSCAL);
  phaseReactiveRead = ((int16_t)(bacaADQPHSCAL[0]) << 8) | bacaADQPHSCAL[1];
  Serial.print("ADQPHSCAL (-3072) : ");
  Serial.println(phaseReactiveRead);

  baca_tulis_address(ADEMUCON, 3, bacaADEMUCON);
  ADEMUCONRead = ((int16_t)(bacaADEMUCON[0]) << 8) | bacaADEMUCON[1];
  Serial.print("ADEMUCON (0x3) : 0x");
  Serial.println(ADEMUCONRead, HEX);
}

void setCalibRegs()
{
  // Write settings
  unsigned char writeEnable[] = {0xE5};
  unsigned char writeProtect[] = {0xDC};

  // Analog Gain Setup
  unsigned char sysconByte[] = {0x16, (0b10 << ADSYSCON_PGAIA_Pos)};

  // Energy CLR
  unsigned char emuconEnergyCLRByte[] = {0b01 << 7, 0x03 << 0}; // 14 April 2022

  // Offset Correction
  unsigned long offsetActive = 0;
  unsigned char offsetActiveByte[] = {(uint8_t)(offsetActive >> 8), (uint8_t)offsetActive};

  // Gain Correction
  const int16_t gainActive = 12640; // 28 Maret
  unsigned char gainActiveByte[] = {(uint8_t)(gainActive >> 8), (uint8_t)gainActive};

  // Phase Correction
  unsigned char phaseByte[] = {40};

  // Reactive Power Correction
  const int16_t offsetReactive = 0; // 28 Maret
  unsigned char offsetReactiveByte[] = {(uint8_t)(offsetReactive >> 8), (uint8_t)offsetReactive};

  const int16_t phaseReactive = -3072; // 28 Maret
  unsigned char phaseReactiveByte[] = {(uint8_t)(phaseReactive >> 8), (uint8_t)phaseReactive};

  // Enter calibration settings
  baca_tulis_address(WriteEn, 1, writeEnable, true);
  // baca_tulis_address(ADEMUCON, 2, emuconEnergyCLRByte, true);
  baca_tulis_address(ADSYSCON, 2, sysconByte, true);
  // write(ADSYSCON, 2, sysconByte);
  baca_tulis_address(ADAPOSA, 2, offsetActiveByte, true);
  baca_tulis_address(ADGPQA, 2, gainActiveByte, true);
  baca_tulis_address(ADPhsA, 1, phaseByte, true);
  baca_tulis_address(ADRPOSA, 2, offsetReactiveByte, true);
  baca_tulis_address(ADQPHSCAL, 2, phaseReactiveByte, true);
  // baca_tulis_address(WriteEn, 1, writeProtect, true); // Disable further writing
}

void setup()
{
  // Serial Master
  Serial.begin(115200);
  Serial.println("Halo");

  // Buat reset dulu chip (kalo pake Hardware Serial)
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);
  delay(500);
  digitalWrite(RST_PIN, HIGH);
  delay(500);

  // Serial Slave (kalo pake Hardware Serial)
  EM1_SERIAL.begin(4800, SERIAL_8E1); // Parity Even, Hardware Serial
  delay(1000);
  Serial.println();

  setCalibRegs();
  readCalibRegs();
}
int xx = 0;

void loop()
{
  delay(1000);
  float y;
  unsigned char buffe[5];
  unsigned char __attribute__((unused)) numByte;
  int32_t val;

  Serial.print(xx++);
  Serial.print(" ");

  // Semua bacaan LSB, perlu ditukar
  // Baca Energi Aktif (Hasil Increment)
  numByte = baca_tulis_address(ADEnergyP, 4, buffe); // Active Energy A
  int32_t energyPRaw = 0;
  energyPRaw |= buffe[0];
  energyPRaw = (energyPRaw << 8) | buffe[1];
  energyPRaw = (energyPRaw << 8) | buffe[2];

  y = (0.000083 * energyPRaw) - 0.000131; // Fixed at 01 June 2022
  Serial.print("kWhr= ");
  Serial.print(y, 4);
  Serial.print(' ');

  // Baca Tegangan RMS
  numByte = baca_tulis_address(ADURMS, 4, buffe);
  val = 0;
  val |= buffe[0];
  val = (val << 8) | buffe[1];
  val = (val << 8) | buffe[2];
  val = (val << 8) >> 8;
  // Serial.print((millis()-start_calculation)/100);
  // Serial.print(' ');
  Serial.print("V_RMS= ");
  y = (val - 181) * 0.000223485; // Vrms = (URMS-OFFSET)*GAIN, Gain = VRMS Power Analyzer/angka URMS chip, cukup pas kadang tinggi sedikit kadang rendah sedikit
  Serial.print(y, 2);
  Serial.print(' ');

  // Baca Arus RMS
  numByte = baca_tulis_address(ADIARMS, 4, buffe);
  val = 0;
  val |= buffe[0];
  val = (val << 8) | buffe[1];
  val = (val << 8) | buffe[2];
  val = (val << 8) >> 8;

  Serial.print("I_RMS= ");
  y = (val)*0.0000028924337884678; // Cara (IRMS-OFFSET RMS)*GAIN , Gain = IRMS Asli beban / IRMS Chip
  Serial.print(y, 4);
  Serial.print(' ');

  // Baca Active Power
  numByte = baca_tulis_address(ADPowerPA, 5, buffe);
  int32_t activeRaw = 0;
  activeRaw |= buffe[0];
  activeRaw = (activeRaw << 8) | buffe[1];
  activeRaw = (activeRaw << 8) | buffe[2];
  activeRaw = (activeRaw << 8) | buffe[3];
  // Serial.print((millis()-start_calculation)/100);
  // Serial.print(' ');
  Serial.print("Watt= ");
  y = (float)activeRaw / (1L << 16); // 28 Maret Check
  Serial.print(y, 2);
  Serial.print(' ');

  // Baca Reactive Power
  numByte = baca_tulis_address(ADPowerQ, 5, buffe); // Reaktif
  int32_t reactiveRaw = 0;
  reactiveRaw |= buffe[0];
  reactiveRaw = (reactiveRaw << 8) | buffe[1];
  reactiveRaw = (reactiveRaw << 8) | buffe[2];
  reactiveRaw = (reactiveRaw << 8) | buffe[3];
  // Serial.print(reactiveRaw);    //Power Q atau Reaktif Power
  // Serial.print(' ');
  //  Serial.print((millis() - start_calculation) / 100);
  Serial.print(' ');
  Serial.print("VAR= ");
  y = (float)reactiveRaw / (1L << 16);
  Serial.print(y, 2);
  Serial.print(' ');

  // Kalkulasi Apparent Power
  int64_t apparentRaw = sqrt(((int64_t)activeRaw) * activeRaw + ((int64_t)reactiveRaw) * reactiveRaw);
  // Serial.print((unsigned long)apparentRaw);    //Apparent Power
  // Serial.print(' ');
  // Serial.print((millis()-start_calculation)/100);
  // Serial.print(' ');
  Serial.print("VA= ");
  y = (float)apparentRaw / (1L << 16);
  Serial.print(y, 2);
  Serial.print(' ');

  // Kalkulasi phase (cos Theta)
  // Serial.print((millis()-start_calculation)/100);
  // Serial.print(' ');
  Serial.print("PF= ");
  y = (float)activeRaw / apparentRaw;
  Serial.print(y, 3);
  Serial.print(' ');

  Serial.println();

  if (xx % 1000 == 0)
  {
    Serial.println("-----BEFORE RESET-----");
    readCalibRegs();

    Serial2.end();

    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, LOW);
    delay(500);
    digitalWrite(RST_PIN, HIGH);
    delay(500);

    delay(1000);
    Serial2.begin(4800, SERIAL_8E1); // Parity Even, Hardware Serial

    Serial.println("-----AFTER RESET-----");
    readCalibRegs();

    delay(1000);
    setCalibRegs();

    Serial.println("-----AFTER RECALIBRATION-----");
    readCalibRegs();
  }
}
