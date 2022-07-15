#include "rn8209c.h"

RN8209C::RN8209C(int8_t rx_pin, int8_t tx_pin, measurementMode mode)
{
  port_tx = tx_pin;
  port_rx = rx_pin;
  _mode = mode;
  reset(RX_PIN);

  uart_start();
}

int32_t RN8209C::uart_start()
{
#ifdef ESP32
  EMserial.begin(4800, SWSERIAL_8E1, port_rx, port_tx, false);

  if (EMserial) // GPIO initialization ok, assigned pins can be used
    return 0;
  else
    return 1; // GPIO assignment failed
              // #else //placeholder for another type of serial libary.

#endif

  // TODO: hardwareserial, arduino-softwareserial
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
  write(reg_address, (uint8_t *)data, len);
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
  uint8_t bacaADSYSCON[3];
  uint8_t bacaADPhsA[2];
  uint8_t bacaADAPOSA[3];
  uint8_t bacaADGPQA[3];
  uint8_t bacaADRPOSA[3];
  uint8_t bacaADQPHSCAL[3];
  uint8_t bacaADEMUCON[3];
  uint8_t bacaADDCIAH[3];
  uint8_t bacaADDCUH[3];

  int16_t sysconRead;
  int16_t offsetActiveRead;
  int16_t gainActiveRead;
  int16_t offsetReactiveRead;
  int16_t phaseReactiveRead;
  int16_t ADEMUCONRead;
  int16_t offsetCurrentRead;
  int16_t offsetVoltageRead;

  read(ADSYSCON, bacaADSYSCON, 2);
  sysconRead = arr2raw(bacaADSYSCON, 2);
  Serial.print("ADSYSCON (0x1602) : 0x");
  Serial.println(sysconRead, HEX);

  read(ADPhsA, bacaADPhsA, 1);
  Serial.print("ADPhsA (40) : ");
  Serial.println((signed char)bacaADPhsA[0]);

  read(ADAPOSA, bacaADAPOSA, 2);
  offsetActiveRead = arr2raw(bacaADAPOSA, 2);
  Serial.print("ADAPOSA (0) : ");
  Serial.println(offsetActiveRead);

  read(ADGPQA, bacaADGPQA, 2);
  gainActiveRead = arr2raw(bacaADGPQA, 2);
  Serial.print("ADGPQA (12640) : ");
  Serial.println(gainActiveRead);

  read(ADRPOSA, bacaADRPOSA, 2);
  offsetReactiveRead = arr2raw(bacaADRPOSA, 2);
  Serial.print("ADRPOSA (0) : ");
  Serial.println(offsetReactiveRead);

  read(ADQPHSCAL, bacaADQPHSCAL, 2);
  phaseReactiveRead = arr2raw(bacaADQPHSCAL, 2);
  Serial.print("ADQPHSCAL (-3072) : ");
  Serial.println(phaseReactiveRead);

  read(ADEMUCON, bacaADEMUCON, 2);
  ADEMUCONRead = arr2raw(bacaADEMUCON, 2);
  Serial.print("ADEMUCON (0x3) : 0x");
  Serial.println(ADEMUCONRead, HEX);

  if (_mode == DC)
  {
    read(ADEMUCON, bacaADEMUCON, 2);
    ADEMUCONRead = arr2raw(bacaADEMUCON, 2);
    Serial.print("ADEMUCON (0x63) : 0x");
    Serial.println(ADEMUCONRead, HEX);

    read(ADDCIAH, bacaADDCIAH, 2);
    offsetCurrentRead = arr2raw(bacaADDCIAH,2);
    Serial.print("ADDCIAH (-150) : ");
    Serial.println(offsetCurrentRead);

    read(ADDCUH, bacaADDCUH, 2);
    offsetVoltageRead = arr2raw(bacaADDCUH,2);
    Serial.print("ADDCUH (12) : ");
    Serial.println(offsetVoltageRead);
  }
}

void RN8209C::setCalibRegs()
{
  // Write settings
  const uint8_t writeEnable[] = {0xE5};
  const uint8_t writeProtect[] = {0xDC};

  // Analog Gain Setup
  const uint8_t sysconByte[] = {0x16, (0b10 << ADSYSCON_PGAIA_Pos)};

  // Energy CLR
  const uint8_t emuconEnergyCLRByte[] = {0b01 << 7, 0x03 << 0};

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

  /******************DC SETTINGS******************/
  // Disable IA Channel digital High Pass Filter if bit5&6 = 1
  // unsigned char ademuconByte[] = { 0x16,(0b100011 << ADEMUCON_PGAIA_Pos) };
  unsigned char ademuconByte[] = {0x00, (0x03 | (1 << ADEMUCON_HPFIOFF_Pos) | (1 << ADEMUCON_HPFUOFF_Pos))};

  // DC Offset Correction
  /*Current
  semakin kecil, hasil arus semakin besar
  *const int16_t offsetCurrent = 0;  //06 Juli - %eIRMS=0.89%
  %e semakin kecil ketika -150
  -158 10 A jadi 10.0183
  */
  const int16_t offsetCurrent = -150; // offset -150 menghasilkan error current < 0.5%
  unsigned char offsetCurrentByte[] = {(uint8_t)(offsetCurrent >> 8), (uint8_t)offsetCurrent};

  /*Voltage
  semakin kecil nilai offset maka semakin besar nilai tegangan
  *const int16_t offsetVoltage = 0;  //06 Juli - %eVRMS = 0.63%
  *%e semakin kecil ketika 12
  */
  const int16_t offsetVoltage = 12; // kalau ofset 12 error jadi < 0.5%
  unsigned char offsetVoltageByte[] = {(uint8_t)(offsetVoltage >> 8), (uint8_t)offsetVoltage};

  /****************DC SETTINGS END****************/

  // Enter calibration settings
  write(SPEC_CMD, writeEnable, 1);

  write(ADSYSCON, sysconByte, 2);
  write(ADAPOSA, offsetActiveByte, 2);
  write(ADGPQA, gainActiveByte, 2);
  write(ADPhsA, phaseByte, 1);
  write(ADRPOSA, offsetReactiveByte, 2);
  write(ADQPHSCAL, phaseReactiveByte, 2);

  if (_mode == DC)
  {
    write(ADEMUCON, ademuconByte, 2);
    write(ADDCIAH, offsetCurrentByte, 2);
    write(ADDCUH, offsetVoltageByte, 2);
  }

  write(SPEC_CMD, writeProtect, 1);
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

// uint8_t *raw2arr(uint32_t raw, size_t len)
// {
//   static uint8_t arr[4] = {1, 2, 3, 4}; // Max size 4 bytes
//   for (size_t i = 0; i < len; i++)
//   {
//     arr[i] = (raw >> (len - i - 1) * 8);
//   }
//   return arr; // return pointer to arr
// }

void RN8209C::reset(resetMode mode)
{
  switch (mode)
  {
  case RX_PIN:
    pinMode(port_tx, OUTPUT);
    digitalWrite(port_tx, LOW);
    delay(50);
    digitalWrite(port_tx, HIGH);
    delay(50);
    break;

  case SPEC_REG:
    // TODO: kerjain ini
    break;
  }
}

float RN8209C::get_AccumulatedEnergy()
{
  float y;
  while (read(ADEnergyP, buffe, 3))
  {
    delay(1000);
    ESP_LOGW("EM", "Accumulated energy read checksum failed");
  }
  energyPRaw = arr2raw(buffe, 3);

  y = (0.000083 * energyPRaw) - 0.000131;
  if (y < 0)
    y = 0;
  // Serial.printf("\n energy read: %4f\n",y);
  return y;
}

float RN8209C::get_DeltaEnergy()
{
  float y;
  while (read(ADEnergyP2, buffe, 3))
  {
    delay(1000);
    ESP_LOGW("EM", "Delta energy read checksum failed");
  }
  energyPRaw = arr2raw(buffe, 3);

  y = (0.000083 * energyPRaw) - 0.000131;
  if (y < 0)
    y = 0;
  // Serial.printf("\n energy read: %4f\n",y);
  return y;
}

float RN8209C::get_vrms()
{
  float y;

  while (read(ADURMS, buffe, 3))
  {
    delay(1000);
    ESP_LOGW("EM", "VRMS read checksum failed");
  }

  vrmsRaw = arr2raw(buffe, 3);
  y = (vrmsRaw - 181) * 0.000223485;
  if (y < 0)
    y = 0;

  return y;
}

float RN8209C::get_irms()
{
  float y;

  while (read(ADIARMS, buffe, 3))
  {
    delay(1000);
    ESP_LOGW("EM", "IRMS read checksum failed");
  }

  int32_t irmsRaw = arr2raw(buffe, 3);

  y = (irmsRaw)*0.0000028924337884678;
  if (y < 0)
    y = 0;

  return y;
}

float RN8209C::get_power()
{
  float y;

  while (read(ADPowerPA, buffe, 4))
  {
    delay(1000);
    ESP_LOGW("EM", "WATT read checksum failed");
  }

  activeRaw = arr2raw(buffe, 4);
  y = (float)activeRaw / (1L << 16); // 28 Maret Check
  if (y < 0)
    y = 0;

  return y;
}

float RN8209C::get_reactivepower()
{
  float y;

  while (read(ADPowerQ, buffe, 4))
  {
    delay(1000);
    ESP_LOGW("EM", "VAR read checksum failed");
  }

  reactiveRaw = arr2raw(buffe, 4);
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
