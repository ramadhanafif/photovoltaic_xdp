/**
 * @file WL_AT24C32.h
 * @author Muhammad Afif Ramadhan
 * @brief Implementation of AVR's high endurance EEPROM in ESP32 using AT24C32 EEPROM
 * http://ww1.microchip.com/downloads/en/appnotes/doc2526.pdf
 *
 * Inherits AT24C32 class from https://github.com/cyberp/AT24Cx
 *
 * Important note:
 *    faddr = float addressing,
 *            the memory is perceived in 4-bytes data float instead of single byte data
 *
 * @version 1
 * @date 2022-07-20
 *
 */

#include "AT24CX.h"

class WL_AT24C32 : public AT24C32
{
public:
  WL_AT24C32(byte index)
  {
    init(index, 32);

    set_data_size();
    set_eeprom_param();
  }

  void set_data_size(size_t data_size = sizeof(float))
  {
    _dat_size = data_size;
  }

  /**
   * @brief Set parameters
   *
   * _eeprom_size: EEPROM total size in bytes
   * _eeprom_data_size: EEPROM total size for storing data as float
   * _space_size: EEPROM effective size for storing data in float
   * _ptr_base_faddr = Base float-address for storing pointer
   * _data_base_faaddress
   */
  void set_eeprom_param(size_t eeprom_size = 1 << 12)
  {
    _eeprom_size = eeprom_size;
    _eeprom_data_size = _eeprom_size / _dat_size;
    _space_size = _eeprom_data_size / 2; // eeprom effective size

    _ptr_base_faddr = _space_size * 0;  // first half for pointer
    _data_base_faddr = _space_size * 1; // second half for data
  }

  /**
   * @brief Convert FLOAT addressing to BYTE addressing
   * Only take first 9 bit, creating circular effect (512 = 0)
   * @param faddr_base base address for each space
   * @param faddr float-addressing input
   * @return uint32_t address in byte-addressing
   */
  uint32_t faddr_conv(uint32_t faddr_base, uint32_t faddr)
  {
    return ((faddr & 0x1FF) + faddr_base) * _dat_size;
  }

  float read_dat(uint32_t faddr)
  {
    return readFloat(faddr_conv(_data_base_faddr, faddr));
  }

  uint32_t read_ptr(uint32_t faddr)
  {
    return readLong(faddr_conv(_ptr_base_faddr, faddr));
  }

  /**
   * @brief Search EEPROM for last pointer location
   *
   * @param ptr for output only, returns pointer in last faddress
   * @return uint32_t last written address in float-addressing
   */
  uint32_t get_last_faddr(uint32_t *ptr)
  {
    uint32_t current_ptr, next_ptr;

    for (size_t faddr = 0; faddr < _space_size; faddr++)
    {
      current_ptr = read_ptr(faddr);
      next_ptr = read_ptr(faddr + 1); // 10th bit overflow handled in faddr_conv

      if ((next_ptr == 0xFFFFFFFF) | // previous reset has not reached 512 data-write
          (next_ptr - current_ptr != 1))
      {

        if (faddr != 0)
          *ptr = current_ptr;
        else // faddr==0 & nextptr = 0xffff_ffff. Therefore rest of memory is blank
          *ptr = 0;

        ESP_LOGI("EEPROM", "last faddr = %d, ptr %d", faddr, *ptr);
        return faddr;
      }
    }
    // should be unreachable, added to remove compilation warning
    ESP_LOGE("EEPROM", "get_last_faddr error");
    return 0;
  }

  /**
   * @brief Write pointer & data
   *
   * @param data data to be written in data space
   * @param ptr pointer to be written in pointer space
   * @param faddr address in float-addressing
   */
  void write_mem(float data, uint32_t ptr, uint32_t faddr)
  {
    // write data to mem
    writeFloat(faddr_conv(_data_base_faddr, faddr), data);

    // write pointer to mem
    writeLong(faddr_conv(_ptr_base_faddr, faddr), ptr);
  }

  /**
   * @brief Wipe and initialize all data in EEPROM to 0xFF
   *
   */
  void wipe()
  {
    Serial.print("Wiping all data to 0xFF");
    for (int j = 0; j < 4095; j++)
    {
      write(j, 0xFF);
      if (j % 1000 == 0)
        ESP_LOGD("EEPROM", "%d data wiped", j);
    }
    Serial.println();
  }

  /**
   * @brief Read all data stored in EEPROM, prints out to Serial(0)
   *
   */
  void read_all()
  {
    for (uint32_t j = 0; j < _space_size; j++)
    {
      Serial.printf("faddr %3d, with pointer %3d <-> data %6f\n",
                    j,
                    read_ptr(j),
                    read_dat(j));
    }
  }

  float get_last_data()
  {
    uint32_t faddr = get_last_faddr(&_last_ptr);
    return read_dat(faddr);
  }

  uint32_t get_last_ptr()
  {
    get_last_faddr(&_last_ptr);
    return _last_ptr;
  }

private:
  uint32_t _ptr_base_faddr;
  uint32_t _data_base_faddr;
  size_t _dat_size;
  size_t _eeprom_size;
  size_t _eeprom_data_size;
  size_t _space_size;

  uint32_t _last_ptr;
};
