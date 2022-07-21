/**
 * @file main.cpp
 * @author Muhammad Afif Ramadhan
 * @brief example on how to use wear-leveling AT24C32 library
 *
 * @version 1
 * @date 2022-07-19
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include "WL_AT24C32.h"

WL_AT24C32 mem(7);

void setup()
{
  Serial.begin(115200);

  // Get last entry location.
  // MUST be called after object constructor otherwise further write/read is invalid.
  mem.find_last_entry();
}

void loop()
{
  delay(2000);

  // Uncomment to wipe all memory to 0XFF, simulate a blank state
  // mem.wipe();

  // Prints all memory contents to Serial
  mem.read_all();

  // Show last pointer, and last data entry
  Serial.printf("last ptr = %d, last data = %f\n", mem.get_current_ptr(), mem.get_last_data());

  delay(1000);
  Serial.println("Simulating data addition 512 times");
  Serial.println("While on this process, reset the MCU to simulate a sudden power loss");
  for (size_t i = 0; i < 512; i++)
  {
    float data_from_energymeter = (float)random(1000, 9000) / random(1, 10);

    // Add new data to memory
    mem.add_data(data_from_energymeter);
    Serial.print(".");
  }
  Serial.println();

  // Show updated data in memory
  mem.read_all();

  // Show new pointer value and last data entry
  Serial.printf("last ptr = %d, last data = %f\n", mem.get_current_ptr(), mem.get_last_data());

  // stop
  while (1)
    ;
}