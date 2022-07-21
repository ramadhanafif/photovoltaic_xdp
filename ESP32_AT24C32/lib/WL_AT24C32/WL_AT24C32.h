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
  /**
   * @brief Construct a new object
   *
   * @param index A0 A1 A2 on EEPROM
   */
  WL_AT24C32(byte index);

  /**
   * @brief Set the data size based on data type
   *
   * @param data_size data size in bytes, default value is sizeof(float) = 4 bytes
   */
  void set_data_size(size_t data_size = sizeof(float));

  /**
   * @brief Set parameters for writing algorithm
   *
   * _eeprom_size: EEPROM total size in bytes
   * _eeprom_data_size: EEPROM total size for storing data as float
   * _space_size: EEPROM effective size for storing data in float
   * _ptr_base_faddr = Base float-address for storing pointer
   * _data_base_faaddress
   */
  void set_eeprom_param(size_t eeprom_size = 1 << 12);

  /**
   * @brief Convert FLOAT addressing to BYTE addressing
   * Only take first 9 bit, creating circular effect (512 = 0)
   * @param faddr_base base address for each space
   * @param faddr float-addressing input
   * @return uint32_t address in byte-addressing
   */
  uint32_t faddr_conv(uint32_t faddr_base, uint32_t faddr);

  /**
   * @brief read data from EEPROM
   *
   * @param faddr address in float-adressing
   * @return float data read from memory
   */
  float read_dat(uint32_t faddr);

  /**
   * @brief Read pointer value
   *
   * @param faddr address in float-adressing
   * @return uint32_t pointer value
   */
  uint32_t read_ptr(uint32_t faddr);

  /**
   * @brief Search EEPROM for last pointer location
   *
   * @param ptr for output only, returns pointer in last faddress
   * @return uint32_t last written address in float-addressing
   */
  uint32_t get_last_faddr(uint32_t *ptr);

  /**
   * @brief Write pointer & data
   *
   * @param data data to be written in data space
   * @param ptr pointer to be written in pointer space
   * @param faddr address in float-addressing
   */
  void write_mem(float data, uint32_t ptr, uint32_t faddr);

  /**
   * @brief Wipe and initialize all data in EEPROM to 0xFF
   *
   */
  void wipe();

  /**
   * @brief Read all data stored in EEPROM, prints out to Serial(0)
   *
   */
  void read_all();

  /**
   * @brief find last pointer and address
   * MUST BE CALLED before get_xxxx methods
   *
   */
  void find_last_entry();

  /**
   * @brief Get the last data stored in memory
   * Requires find_last_entry() to be called beforehand
   *
   * @return float data stored
   */
  float get_last_data();

  /**
   * @brief Get the current float-address
   * Requires find_last_entry() to be called beforehand
   *
   * @return uint32_t current float-address
   */
  uint32_t get_current_faddr();

  /**
   * @brief Get the current pointer
   * Requires find_last_entry() to be called beforehand
   *
   * @return uint32_t current pointer
   */
  uint32_t get_current_ptr();

  /**
   * @brief Add new data entry to memory.
   * All pointer/address is handled automatically in this function
   *
   * @param data
   */
  void add_data(float data);

private:
  uint32_t _ptr_base_faddr;
  uint32_t _data_base_faddr;
  size_t _dat_size;
  size_t _eeprom_size;
  size_t _eeprom_data_size;
  size_t _space_size;

  uint32_t _current_ptr = 0xFFFFFFFF;
  uint32_t _current_faddr = 0xFFFFFFFF;
  uint32_t _last_data;
};
