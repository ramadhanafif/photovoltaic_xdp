#include "rn8209c.h"

RN8209C::RN8209C(int8_t rx_pin, int8_t tx_pin, uint8_t mode)
{
  port_tx = tx_pin;
  port_rx = rx_pin;

  rx_reset();

  uart_start();
}

void RN8209C::uart_start()
{
#ifdef ESP32
  EMserial.begin(4800, SWSERIAL_8E1, port_rx, port_tx, false); // Parity Even
#endif
}

void RN8209C::uart_stop()
{
  EMserial.end();
}

void RN8209C::write(uint8_t reg_address, uint8_t *data, size_t len)
{
  uint8_t cmd = reg_address | (1 << 7);
  uint8_t checksum = cmd;

  EMserial.write(cmd);
  // Serial.printf("AFIFADDR %X|", cmd);
  for (int i = 0; i < len; i++)
  {
    EMserial.write(data[i]);
    checksum += data[i];
    // Serial.printf("%x|", data[i]);
  }
  checksum = ~checksum;
  // Serial.printf("CHK %x\n", checksum);

  EMserial.write((uint8_t)checksum);
}

// Overload for writing known constants (such as calibration val)
void RN8209C::write(uint8_t reg_address, const uint8_t *data, size_t len)
{
  write(reg_address,(uint8_t *)data,len);
}

int32_t RN8209C::read(uint8_t reg_address, uint8_t *rx_array, size_t len)
{
  uint8_t cmd = reg_address;
  uint8_t checksum = cmd;
  EMserial.write(cmd);

  for (uint32_t i = 0; i < len + 1; i++)
  {
    int x = millis();
    while (!EMserial.available())
    {
      if (millis() - x > 100)
        break;
    } // Timeout in wating Serial

    if (EMserial.available())
    {
      if (i != len)
      {
        rx_array[i] = EMserial.read();
        checksum += rx_array[i];
      }
      else
      {
        checksum = ~checksum;
        uint8_t incoming_checksum = EMserial.read();

        if (incoming_checksum != checksum)
        {
          // Serial.printf("CHECKSUM FAIL receive: %x, checksum: %x, bool:%d", i, incoming_checksum, checksum, incoming_checksum == checksum);
          return 1;
        }
      }
    }
  }

  return 0;
}

void RN8209C::readCalibRegs()
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

  read(ADSYSCON, bacaADSYSCON, 3 - 1);
  sysconRead = arr2raw(bacaADSYSCON, 3 - 1);
  Serial.print("ADSYSCON (0x1602) : 0x");
  Serial.println(sysconRead, HEX);

  read(ADPhsA, bacaADPhsA, 2 - 1);
  Serial.print("ADPhsA (40) : ");
  Serial.println((signed char)bacaADPhsA[0]);

  read(ADAPOSA, bacaADAPOSA, 3 - 1);
  offsetActiveRead = arr2raw(bacaADAPOSA, 3 - 1);
  Serial.print("ADAPOSA (0) : ");
  Serial.println(offsetActiveRead);

  read(ADGPQA, bacaADGPQA, 3 - 1);
  gainActiveRead = arr2raw(bacaADGPQA, 3 - 1);
  Serial.print("AME ADGPQA (12640) : ");
  Serial.println(gainActiveRead);

  read(ADRPOSA, bacaADRPOSA, 3 - 1);
  offsetReactiveRead = arr2raw(bacaADRPOSA, 3 - 1);
  Serial.print("ADRPOSA (0) : ");
  Serial.println(offsetReactiveRead);

  read(ADQPHSCAL, bacaADQPHSCAL, 3 - 1);
  phaseReactiveRead = arr2raw(bacaADQPHSCAL, 3 - 1);
  Serial.print("ADQPHSCAL (-3072) : ");
  Serial.println(phaseReactiveRead);

  read(ADEMUCON, bacaADEMUCON, 3 - 1);
  ADEMUCONRead = arr2raw(bacaADEMUCON, 3 - 1);
  Serial.print("ADEMUCON (0x3) : 0x");
  Serial.println(ADEMUCONRead, HEX);
}

void RN8209C::setCalibRegs()
{
  // Write settings
  const uint8_t writeEnable[] = {0xE5};
  const uint8_t writeProtect[] = {0xDC};

  // Analog Gain Setup
  const uint8_t sysconByte[] = {0x16, (0b10 << ADSYSCON_PGAIA_Pos)};

  // Energy CLR
  const uint8_t emuconEnergyCLRByte[] = {0b01 << 7, 0x03 << 0}; // 14 April 2022

  // Offset Correction
  const int16_t offsetActive = 0;
  const uint8_t offsetActiveByte[] = {(uint8_t)(offsetActive >> 8), (uint8_t)offsetActive};

  // Gain Correction
  const int16_t gainActive = 12640; // 28 Maret
  const uint8_t gainActiveByte[] = {(uint8_t)(gainActive >> 8), (uint8_t)gainActive};

  // Phase Correction
  const uint8_t phaseByte[] = {40};

  // Reactive Power Correction
  const int16_t offsetReactive = 0; // 28 Maret
  const uint8_t offsetReactiveByte[] = {(uint8_t)(offsetReactive >> 8), (uint8_t)offsetReactive};

  const int16_t phaseReactive = -3072; // 28 Maret
  unsigned char phaseReactiveByte[] = {(uint8_t)(phaseReactive >> 8), (uint8_t)phaseReactive};

  // Enter calibration settings
  write(SPECIAL_CMD, (uint8_t *)writeEnable, 1);
  write(ADSYSCON, (uint8_t *)sysconByte, 2);
  write(ADAPOSA, (uint8_t *)offsetActiveByte, 2);
  write(ADGPQA, (uint8_t *)gainActiveByte, 2);
  write(ADPhsA, (uint8_t *)phaseByte, 1);
  write(ADRPOSA, (uint8_t *)offsetReactiveByte, 2);
  write(ADQPHSCAL, (uint8_t *)phaseReactiveByte, 2);
  write(SPECIAL_CMD, (uint8_t *)writeProtect, 1);
}

uint32_t RN8209C::arr2raw(uint8_t *arr, size_t len)
{
  uint32_t out = 0;
  for (size_t i = 0; i < len; i++)
  {
    out = out | (arr[i] << (len - i - 1) * 8);
  }
  return out;
}

void RN8209C::rx_reset()
{
  pinMode(port_tx, OUTPUT);
  digitalWrite(port_tx, LOW);
  delay(50);
  digitalWrite(port_tx, HIGH);
  delay(50);
}

float RN8209C::get_energy()
{
  float y;
  while (read(ADEnergyP2, buffe, 4 - 1))
  {
    delay(1000);
    ESP_LOGW("EM", "Energy read checksum failed");
  }
  energyPRaw = arr2raw(buffe, 4 - 1);

  y = (0.000083 * energyPRaw) - 0.000131;
  if (y < 0)
    y = 0;
  // Serial.printf("\n energy read: %4f\n",y);
  return y;
}

float RN8209C::get_vrms()
{
  float y;

  while (read(ADURMS, buffe, 4 - 1))
  {
    delay(1000);
    ESP_LOGW("EM", "VRMS read checksum failed");
  }

  vrmsRaw = arr2raw(buffe, 4 - 1);
  y = (vrmsRaw - 181) * 0.000223485;
  if (y < 0)
    y = 0;

  return y;
}

float RN8209C::get_irms()
{
  float y;

  while (read(ADIARMS, buffe, 4 - 1))
  {
    delay(1000);
    ESP_LOGW("EM", "IRMS read checksum failed");
  }

  int32_t irmsRaw = arr2raw(buffe, 4 - 1);

  y = (irmsRaw)*0.0000028924337884678;
  if (y < 0)
    y = 0;

  return y;
}

float RN8209C::get_power()
{
  float y;

  while (read(ADPowerPA, buffe, 5 - 1))
  {
    delay(1000);
    ESP_LOGW("EM", "WATT read checksum failed");
  }

  activeRaw = arr2raw(buffe, 5 - 1);
  y = (float)activeRaw / (1L << 16); // 28 Maret Check
  if (y < 0)
    y = 0;

  return y;
}

float RN8209C::get_reactivepower()
{
  float y;

  while (read(ADPowerQ, buffe, 5 - 1))
  {
    delay(1000);
    ESP_LOGW("EM", "VAR read checksum failed");
  }

  reactiveRaw = arr2raw(buffe, 5 - 1);
  y = (float)reactiveRaw / (1L << 16);
  if (y < 0)
    y = 0;
  return y;
}

float RN8209C::calc_apprpower()
{
  float y;

  apparentRaw = sqrt(((int64_t)activeRaw) * activeRaw + ((int64_t)reactiveRaw) * reactiveRaw);
  y = (float)apparentRaw / (1L << 16);

  if (y < 0)
    y = 0;
  return y;
}

float RN8209C::calc_powerfactor()
{
  float y;

  y = (float)fabs(activeRaw) / apparentRaw;
  return y;
}
