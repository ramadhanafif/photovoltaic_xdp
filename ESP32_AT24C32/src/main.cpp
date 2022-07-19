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

// Float addressing to byte addressing
inline uint32_t faddr2baddr(uint32_t idx){ 
  return idx*sizeof(float); 
}

void write_cmem(float data, uint32_t *idx)
{
  uint32_t i = *idx;
  // Circular indexing & addressing
  if (i >= 512)
  {
    i = 0;
  }

  // Address offset from base
  uint32_t addr_off = i * sizeof(float);

  // write data to mem
  mem.writeFloat(addr_datbase + addr_off, data);

  // write pointer to mem
  mem.writeLong(addr_ptrbase + addr_off, i);

  *idx = i + 1;
}

uint32_t get_lastidx()
{
  for (size_t i = 0; i < N_idx, i++)
  {
    mem_addr = i*data_size;


  }
}

void memwipe()
{
  for (int j = 0; j < 4095; j++)
  {
    mem.write(j, 0xFF);
  }
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

  float arr[512];
  uint32_t randstart = random(0388, 511);
  uint32_t i = randstart;
  while (i < randstart + 512)
  {
    arr[i] = (float)random(10000, 50000) / random(1, 10);
    Serial.printf("Writing in [%3d]  %f\n", i, arr[i]);
    write_cmem(arr[i], &i);
  }

  for (uint32_t j = 0; j < 512; j++)
  {
    // Address offset from base
    uint32_t addr_off = j * sizeof(float);

    Serial.printf("[%4d] = %3d <-> %f",
                  addr_off,
                  mem.readLong(addr_ptrbase + addr_off),
                  mem.readFloat(addr_off + addr_datbase));

    if (arr[j] == mem.readFloat(addr_off + addr_datbase))
      Serial.println(" OK");
    else
      Serial.println(" FAIL");
  }

  // stop
  while (1)
    ;
}