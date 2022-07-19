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

// EEPROM object
AT24C32 mem(7);

// Pointer space address: 0 - 2047
const uint32_t ptr_faddr_base = 0;

// Data space address: 2048 - 4095
const uint32_t dat_faddr_base = 512;

// Total address for float data type for each space
const uint32_t N_faddr = 512;

// Convert float addressing to byte addressing
// Only take first 9 bit, creating circular effect
inline uint32_t addr_conv(uint32_t faddr)
{
  return (faddr & 0x1FF) * sizeof(float);
}

// Return faddr
uint32_t get_last_faddr(uint32_t *ptr)
{
  uint32_t current_ptr, next_ptr;

  for (size_t i = 0; i < N_faddr; i++)
  {
    current_ptr = mem.read(ptr_faddr_base + addr_conv(i));
    next_ptr = mem.read(ptr_faddr_base + addr_conv(i + 1)); // 10th bit overflow handled in conversion

    if (next_ptr - current_ptr != 1)
    {
      *ptr = current_ptr;
      return i;
    }
  }

  /**
   * All address has been read and no current pointer found
   * Possible causes: all ptr is 0xFFFF_FFFF
   * Therefore in blank state, start from 0
   */
  return 0;
}

// Write data and pointer
uint32_t write_mem(float data, uint32_t *ptr, uint32_t faddr)
{
  // write data to mem
  mem.writeFloat(dat_faddr_base + addr_conv(faddr), data);

  // write pointer to mem
  mem.writeLong(ptr_faddr_base + addr_conv(faddr), *ptr);
  (*ptr)++;
  return faddr + 1;
}

void memwipe()
{
  Serial.print("Preset all data to 0xFF");
  for (int j = 0; j < 4095; j++)
  {
    mem.write(j, 0xFF);
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

  float arr[800];
  uint32_t randstart = random(3, 511);
  uint32_t faddr = randstart;
  uint32_t i_offset = random(0,200);
  uint32_t i = i_offset;

  while (faddr < randstart + 512)
  {
    arr[i-i_offset] = (float)random(10000, 50000) / random(1, 10);
    Serial.printf("Writing in [%3d]  %f\n", faddr, arr[i-i_offset]);
    faddr=write_mem(arr[i-i_offset], &i, faddr);
    // TODO: buat demo random tulis, terus dia cari last addrnya, terus lanjut tulis dari start itu
  }

  uint32_t ptr = 0;
  faddr = get_last_faddr(&ptr);
  Serial.printf("faddr = %d, ptr at faddr = %d\n",faddr,ptr);

  // {
  //   arr[i] = (float)random(10000, 50000) / random(1, 10);
  //   Serial.printf("Writing in [%3d]  %f\n", i, arr[i]);
  //   write_mem(arr[i], &i);
  // }

  // for (uint32_t j = 0; j < 512; j++)
  // {
  //   // Address offset from base
  //   uint32_t addr_off = j * sizeof(float);

  //   Serial.printf("[%4d] = %3d <-> %f",
  //                 addr_off,
  //                 mem.readLong(addr_ptrbase + addr_off),
  //                 mem.readFloat(addr_off + addr_datbase));

  //   if (arr[j] == mem.readFloat(addr_off + addr_datbase))
  //     Serial.println(" OK");
  //   else
  //     Serial.println(" FAIL");
  // }

  // stop
  while (1)
    ;
}