/**
 * @file main.cpp
 * @author Muhammad Afif Ramadhan
 * @brief Kode dasar, proof of concept untuk PV
 * Terdiri dari 2 bagian utama
 * -> Measure: menangkap data dari energy meter, menghasilkan data PV berupa json
 * -> Upload: upload data menggunakan media yang dipilih dari pin SEL[0] dan SEL[1]
 *
 *
 * @version 1
 * @date 2022-07-18
 *
 */
/* Example Json Format
{
  "idUser": 1234512345,
  "AC0": {
    "E": 0,
    "Vrms": 211.9013214,
    "Irms": 0.013553944,
    "P": 1.240600586,
    "Q": 0.383056641,
    "S": 1.29838562,
    "PF": 0.955494702
  },
  "DC": {
    "V": 0.533682168,
    "I": 0.085859008,
    "P": 0.032104492,
    "E": 0
  },
  "AC1": {
    "E": 0,
    "Vrms": 0.251197129,
    "Irms": 0.001029706,
    "P": 0,
    "Q": 0,
    "S": 0.000030518,
    "PF": 1
  }
}
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include "rn8209c.h"
#include "Ethernet2.h"
// #include <SPI.h>

// Was calculated in https://arduinojson.org/v6/assistant/
typedef StaticJsonDocument<384> pvJson;

// Placeholder functions
// TODO: implement internet connection for each method
void wifi_upload(pvJson pvDataJson)
{
  Serial.println("using wifi to send data");
}

void gprs_upload(pvJson pvDataJson)
{
  Serial.println("using gprs to send data");
}

void ethernet_upload(pvJson pvDataJson)
{
  Serial.println("using ethernet to send data");
}

RN8209C *EM0_AC = NULL;
RN8209C *EM1_DC = NULL;
RN8209C *EM2_AC = NULL;

// PIN 34, 35, 36, 39 input-only. NO INTERNAL PULLUP/DOWN
const uint8_t SEL0 = 34; // LSB
const uint8_t SEL1 = 35; // MSB

void setup()
{
  delay(1000); // allow ISR service to start
  Serial.begin(115200);

  // new = malloc with better exception handling
  EM0_AC = new RN8209C(05, 18, AC);
  EM1_DC = new RN8209C(27, 26, DC);
  EM2_AC = new RN8209C(32, 33, AC);

  sizeof(RN8209C);

  EM0_AC->setCalibRegs();
  EM1_DC->setCalibRegs();
  EM2_AC->setCalibRegs();

  EM0_AC->readCalibRegs();
  EM1_DC->readCalibRegs();
  EM2_AC->readCalibRegs();

  pinMode(SEL0, INPUT);
  pinMode(SEL1, INPUT);
}

void loop()
{
  // Serial.print("EM0 ->");
  // Serial.print("|wh:");
  // Serial.print(EM0_AC->get_AccumulatedEnergy() * 1000, 4);
  // Serial.print("|vrms: ");
  // Serial.print(EM0_AC->get_vrms(), 2);
  // Serial.print("|irms: ");
  // Serial.print(EM0_AC->get_irms(), 2);
  // Serial.print("|watt: ");
  // Serial.print(EM0_AC->get_power(), 2);
  // Serial.print("|VAR:");
  // Serial.print(EM0_AC->get_reactivepower(), 2);
  // Serial.print("|VA :");
  // Serial.print(EM0_AC->calc_apprpower(), 2);
  // Serial.print("|PF :");
  // Serial.print(EM0_AC->calc_powerfactor(), 2);
  // Serial.println();

  // // DC mode on EM1: VA, VAR, powerfactor is omitted
  // Serial.print("EM1 ->");
  // Serial.print("|wh:");
  // Serial.print(EM1_DC->get_AccumulatedEnergy() * 1000, 4);
  // Serial.print("|wh diff:");
  // Serial.print(EM1_DC->get_DeltaEnergy() * 1000, 4);
  // Serial.print("|vrms: ");
  // Serial.print(EM1_DC->get_vrms(), 2);
  // Serial.print("|irms: ");
  // Serial.print(EM1_DC->get_irms(), 2);
  // Serial.print("|watt: ");
  // Serial.print(EM1_DC->get_power(), 2);
  // Serial.println();

  // Serial.print("EM2 ->");
  // Serial.print("|wh:");
  // Serial.print(EM2_AC->get_DeltaEnergy() * 1000, 4);
  // Serial.print("|vrms: ");
  // Serial.print(EM2_AC->get_vrms(), 2);
  // Serial.print("|irms: ");
  // Serial.print(EM2_AC->get_irms(), 2);
  // Serial.print("|watt: ");
  // Serial.print(EM2_AC->get_power(), 2);
  // Serial.print("|VAR:");
  // Serial.print(EM2_AC->get_reactivepower(), 2);
  // Serial.print("|VA :");
  // Serial.print(EM2_AC->calc_apprpower(), 2);
  // Serial.print("|PF :");
  // Serial.print(EM2_AC->calc_powerfactor(), 2);
  // Serial.println();

  // MEASURE starts

  pvJson pvDataJ;

  pvDataJ["idUser"] = 1234512345;

  JsonObject AC0 = pvDataJ.createNestedObject("AC0");
  AC0["E"] = EM0_AC->get_AccumulatedEnergy();
  AC0["Vrms"] = EM0_AC->get_vrms();
  AC0["Irms"] = EM0_AC->get_irms();
  AC0["P"] = EM0_AC->get_power();
  AC0["Q"] = EM0_AC->get_reactivepower();
  AC0["S"] = EM0_AC->calc_apprpower();
  AC0["PF"] = EM0_AC->calc_powerfactor();

  JsonObject DC = pvDataJ.createNestedObject("DC");
  DC["V"] = EM1_DC->get_vrms();
  DC["I"] = EM1_DC->get_irms();
  DC["P"] = EM1_DC->get_power();
  DC["E"] = EM1_DC->get_AccumulatedEnergy();

  JsonObject AC1 = pvDataJ.createNestedObject("AC1");
  AC1["E"] = EM2_AC->get_AccumulatedEnergy();
  AC1["Vrms"] = EM2_AC->get_vrms();
  AC1["Irms"] = EM2_AC->get_irms();
  AC1["P"] = EM2_AC->get_power();
  AC1["Q"] = EM2_AC->get_reactivepower();
  AC1["S"] = EM2_AC->calc_apprpower();
  AC1["PF"] = EM2_AC->calc_powerfactor();

  serializeJsonPretty(pvDataJ, Serial);

  byte selection = digitalRead(SEL0) << 1 | digitalRead(SEL1);

  Serial.print(selection, HEX);

  // MEASURE ends
  // UPLOAD starts

  switch (selection)
  {
  case 0:
    Serial.println("->wifi!");
    // wifi_upload(pvDataJ);
    break;

  case 1:
    Serial.println("->gprs!");
    // gprs_upload(pvDataJ);
    break;

  case 2:
    Serial.println("->ethernet!");
    ethernet_upload(pvDataJ);
    break;

  default: // 3
    Serial.println("->nothing selected");
    break;
  }
  delay(3000);
}
