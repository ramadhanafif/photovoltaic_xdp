#include <Arduino.h>
#include <SoftwareSerial.h>
#include "rn8209c.h"

void setup()
{
  // swserialbegin();
  // Serial Master
  Serial.begin(115200);
  Serial.println("Halo");

  delay(500); // HARUS ADA AGAR ISR SERVICE BISA START
  RN8209C EM_0(19, 21);
  RN8209C EM_1(5, 18);
  // RN8209C EM_2(22, 23);

  EM_0.setCalibRegs();

  Serial.println("---------EM 0 Regs---------");
  EM_0.readCalibRegs();

  EM_1.setCalibRegs();

  Serial.println("---------EM 1 Regs---------");
  EM_1.readCalibRegs();

  // EM_2.setCalibRegs();
  // EM_2.readCalibRegs();
  int xx = 0;
  while (1)
  {
    Serial.println(++xx);
    delay(1000);
    Serial.print("EM0 ->");
    Serial.print(" | kwh : ");
    Serial.print(EM_0.get_energy(), 4);
    Serial.print(" | vrms: ");
    Serial.print(EM_0.get_vrms(), 2);
    Serial.print(" | irms: ");
    Serial.print(EM_0.get_irms(), 2);
    Serial.print(" | watt: ");
    Serial.print(EM_0.get_power(), 2);
    Serial.print(" | VAR : ");
    Serial.print(EM_0.get_reactivepower(), 2);
    Serial.print(" | VA  : ");
    Serial.print(EM_0.calc_apprpower(), 2);
    Serial.print(" | PF  : ");
    Serial.print(EM_0.calc_powerfactor(), 2);
    Serial.println();

    Serial.print("EM1 ->");
    Serial.print(" | kwh : ");
    Serial.print(EM_1.get_energy(), 4);
    Serial.print(" | vrms: ");
    Serial.print(EM_1.get_vrms(), 2);
    Serial.print(" | irms: ");
    Serial.print(EM_1.get_irms(), 2);
    Serial.print(" | watt: ");
    Serial.print(EM_1.get_power(), 2);
    Serial.print(" | VAR : ");
    Serial.print(EM_1.get_reactivepower(), 2);
    Serial.print(" | VA  : ");
    Serial.print(EM_1.calc_apprpower(), 2);
    Serial.print(" | PF  : ");
    Serial.print(EM_1.calc_powerfactor(), 2);
    Serial.println();

    if (xx % 15 == 0)
    {
      EM_0.uart_stop();
      EM_0.rx_reset();
      delay(100);
      EM_0.uart_start();
      EM_0.setCalibRegs();

      EM_1.uart_stop();
      EM_1.rx_reset();
      delay(100);
      EM_1.uart_start();
      EM_1.setCalibRegs();
    }
  }
}

void loop()
{
}