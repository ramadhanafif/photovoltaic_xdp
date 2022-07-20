/**
 * @file main.cpp
 * @author Muhammad Afif Ramadhan
 * @brief Implementation of AVR's high endurance EEPROM in ESP32 using AT24C32 EEPROM
 * http://ww1.microchip.com/downloads/en/appnotes/doc2526.pdf
 *
 * @version 1
 * @date 2022-07-19
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include <AT24CX.h>

// EEPROM object, set A0 A1 A2
AT24C32 mem(7);

// Pointer space address: 0 - 2047
const uint32_t ptr_faddr_base = 0;

// Data space address: 2048 - 4095
const uint32_t dat_faddr_base = 512;

// Total address for float data type for each space
const uint32_t N_faddr = 512;

// Convert float addressing to byte addressing
// Only take first 9 bit, creating circular effect
inline uint32_t faddr_conv(uint32_t faddr_base, uint32_t faddr)
{
  return ((faddr & 0x1FF) + faddr_base) * sizeof(float);
}

float read_dat(uint32_t faddr)
{
  return mem.readFloat(faddr_conv(dat_faddr_base, faddr));
}

uint32_t read_ptr(uint32_t faddr)
{
  return mem.readLong(faddr_conv(ptr_faddr_base, faddr));
}

// Return faddr
uint32_t get_last_faddr(uint32_t *ptr)
{
  uint32_t current_ptr, next_ptr;

  for (size_t faddr = 0; faddr < N_faddr; faddr++)
  {
    current_ptr = read_ptr(faddr);
    next_ptr = read_ptr(faddr + 1); // 10th bit overflow handled in faddr_conv

    if ((next_ptr == 0xFFFFFFFF) | // previous reset has not reached 512 data-write
        (next_ptr - current_ptr != 1))
    {
      ESP_LOGI("MEM", "current ptr %X, next ptr %X", current_ptr, next_ptr);

      if (faddr != 0)
        *ptr = current_ptr;
      else // faddr==0 & nextptr = 0xffff_ffff. Therefore rest of memory is blank
        *ptr = 0;

      return faddr;
    }
    // all cases considered.
  }
}

// Write data and pointer
void write_mem(float data, uint32_t ptr, uint32_t faddr)
{
  // write data to mem
  mem.writeFloat(faddr_conv(dat_faddr_base, faddr), data);

  // write pointer to mem
  mem.writeLong(faddr_conv(ptr_faddr_base, faddr), ptr);
}

void memwipe()
{
  Serial.print("Reset all data to 0xFF");
  for (int j = 0; j < 4095; j++)
  {
    mem.write(j, 0xFF);
    if (j % 1000 == 0)
      Serial.print(".");
  }
  Serial.println();
}

// setup
void setup()
{
  // serial init
  Serial.begin(115200);
  Serial.println("AT24CX read/write demo");
  Serial.println("----------------------");
}

// main loop
void loop()
{

  // memwipe();

  // float data;
  // uint32_t num_writes = random(500, 700);
  // uint32_t faddr_start = random(3, 511);
  // uint32_t faddr = faddr_start;
  // uint32_t ptr = 0;

  // delay(1000);
  // Serial.printf("Writing %d times, starting at %d\n", num_writes, faddr_start);

  // while (faddr < faddr_start + num_writes)
  // {
  //   data = (float)random(10000, 50000) / random(1, 10);
  //   Serial.printf("Writing in [%3d], with pointer %d, data %f\n", faddr, ptr, data);
  //   write_mem(data, ptr, faddr);
  //   faddr++;
  //   ptr++;
  //   // TODO: buat demo random tulis, terus dia cari last addrnya, terus lanjut tulis dari start itu
  // }

  // delay(10000);

  for (uint32_t j = 0; j < N_faddr; j++)
  {
    Serial.printf("faddr %3d, with pointer %d <-> data %6f\n",
                  j,
                  read_ptr(j),
                  read_dat(j));

    // if (arr[j] == mem.readFloat(faddr_conv(dat_faddr_base, j)))
    //   Serial.println(" OK");
    // else
    //   Serial.println(" FAIL");
  }

  delay(1000);
  uint32_t last_ptr = 0;
  uint32_t last_faddr = get_last_faddr(&last_ptr);

  Serial.printf("faddr is = %d with pointer %d, data %f",
                last_faddr,
                last_ptr,
                read_dat(last_faddr));

  // stop
  while (1)
    ;
}