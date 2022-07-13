#include <Arduino.h>
#include "rn8209c.h"

// uint8_t *raw2arr(uint32_t raw, size_t len)
// {
//   static uint8_t *arr = NULL;
//   if (arr == NULL)
//     Serial.println("NULL");
//   arr = (uint8_t *)malloc(sizeof(uint8_t) * 4);
//   arr[3]=0xA;
//   for (size_t i = 0; i < len; i++)
//   {
//     arr[i] = (raw >> (len - i - 1) * 8);
//   }
//   free(arr);
//   arr[3]=0xC;
//   return arr; // return pointer to arr
// }

void setup()
{
  // swserialbegin();
  // Serial Master
  Serial.begin(115200);
  Serial.println("Halo");

  delay(500); // HARUS ADA AGAR ISR SERVICE BISA START

  // uint32_t raw = 0x3E2CA1;
  // size_t len = 3;
  // while (1)
  // {
  //   uint8_t *a;

  //   a = raw2arr(raw, len);

  //   for (int i = 0; i < 4; i++)
  //     Serial.println(a[i], HEX);

  //   Serial.printf("Memory Addr %x\n", a);
  //   delay(100);
  // }
  // while (1)
  //   ;
  // Inget pin 35,34 gabisa dipake
  RN8209C EM0(19, 21);
  RN8209C EM1(5, 18);
  // RN8209C EM_2(22, 23);

  EM0.setCalibRegs();

  Serial.println("---------EM 0 Regs---------");
  EM0.readCalibRegs();

  EM1.setCalibRegs();

  Serial.println("---------EM 1 Regs---------");
  EM1.readCalibRegs();

  // EM_2.setCalibRegs();
  // EM_2.readCalibRegs();
  int xx = 0;
  while (1)
  {
    Serial.println(++xx);
    delay(10000);
    Serial.print("EM0 ->");
    Serial.print("|kwh:");
    Serial.print(EM0.get_AccumulatedEnergy(), 4);
    Serial.print("|kwh diff:");
    Serial.print(EM0.get_DeltaEnergy(), 4);
    Serial.print("|vrms: ");
    Serial.print(EM0.get_vrms(), 2);
    Serial.print("|irms: ");
    Serial.print(EM0.get_irms(), 2);
    Serial.print("|watt: ");
    Serial.print(EM0.get_power(), 2);
    Serial.print("|VAR:");
    Serial.print(EM0.get_reactivepower(), 2);
    Serial.print("|VA :");
    Serial.print(EM0.calc_apprpower(), 2);
    Serial.print("|PF :");
    Serial.print(EM0.calc_powerfactor(), 2);
    Serial.println();

    Serial.print("EM1 ->");
    Serial.print("|kwh:");
    Serial.print(EM1.get_AccumulatedEnergy(), 4);
    Serial.print("|kwh diff:");
    Serial.print(EM1.get_DeltaEnergy(), 4);
    Serial.print("|vrms: ");
    Serial.print(EM1.get_vrms(), 2);
    Serial.print("|irms: ");
    Serial.print(EM1.get_irms(), 2);
    Serial.print("|watt: ");
    Serial.print(EM1.get_power(), 2);
    Serial.print("|VAR:");
    Serial.print(EM1.get_reactivepower(), 2);
    Serial.print("|VA :");
    Serial.print(EM1.calc_apprpower(), 2);
    Serial.print("|PF :");
    Serial.print(EM1.calc_powerfactor(), 2);
    Serial.println();

    if (xx % 15 == 0)
    {
      Serial.print("\n------RESETTING------\n");
      EM0.uart_stop();
      EM0.rx_reset();
      delay(100);
      EM0.uart_start();
      EM0.setCalibRegs();

      EM1.uart_stop();
      EM1.rx_reset();
      delay(100);
      EM1.uart_start();
      EM1.setCalibRegs();
    }
  }
}

void loop()
{
}