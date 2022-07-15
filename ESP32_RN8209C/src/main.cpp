#include <Arduino.h>
#include "rn8209c.h"

void setup()
{
  // Serial Master
  Serial.begin(115200);
  Serial.println("Halo");

  delay(500); // HARUS ADA AGAR ISR SERVICE BISA START

  // Inget pin 35,34 gabisa dipake
  // RN8209C EM0(19, 21);
  RN8209C EM1(5, 18, DC);
  // RN8209C EM_2(22, 23);

  // EM0.setCalibRegs();

  // Serial.println("---------EM 0 Regs---------");
  // EM0.readCalibRegs();

  EM1.setCalibRegs();

  Serial.println("---------EM 1 Regs---------");
  EM1.readCalibRegs();

  // EM_2.setCalibRegs();
  // EM_2.readCalibRegs();
  int xx = 0;
  while (1)
  {
    Serial.println(++xx);
    delay(3000);

    // Serial.print("EM0 ->");
    // Serial.print("|kwh:");
    // Serial.print(EM0.get_AccumulatedEnergy(), 4);
    // Serial.print("|kwh diff:");
    // Serial.print(EM0.get_DeltaEnergy(), 4);
    // Serial.print("|vrms: ");
    // Serial.print(EM0.get_vrms(), 2);
    // Serial.print("|irms: ");
    // Serial.print(EM0.get_irms(), 2);
    // Serial.print("|watt: ");
    // Serial.print(EM0.get_power(), 2);
    // Serial.print("|VAR:");
    // Serial.print(EM0.get_reactivepower(), 2);
    // Serial.print("|VA :");
    // Serial.print(EM0.calc_apprpower(), 2);
    // Serial.print("|PF :");
    // Serial.print(EM0.calc_powerfactor(), 2);
    // Serial.println();

    // DC mode on EM1: Omit VA, VAR, powerfactor
    Serial.print("EM1 ->");
    Serial.print("|wh:");
    Serial.print(EM1.get_AccumulatedEnergy()*1000, 4);
    Serial.print("|wh diff:");
    Serial.print(EM1.get_DeltaEnergy()*1000, 4);
    Serial.print("|vrms: ");
    Serial.print(EM1.get_vrms(), 2);
    Serial.print("|irms: ");
    Serial.print(EM1.get_irms(), 2);
    Serial.print("|watt: ");
    Serial.print(EM1.get_power(), 2);

    Serial.println();

    // if (xx % 15 == 0)
    // {
    //   Serial.print("\n------RESETTING------\n");
    //   EM0.uart_stop();
    //   EM0.reset(RX_PIN);
    //   delay(100);
    //   EM0.uart_start();
    //   EM0.setCalibRegs();

    //   EM1.uart_stop();
    //   EM1.reset(RX_PIN);
    //   delay(100);
    //   EM1.uart_start();
    //   EM1.setCalibRegs();
    // }
  }
}

void loop()
{
}