#include "WL_AT24C32.h"

WL_AT24C32::WL_AT24C32(byte index)
{
  init(index, 32);

  set_data_size();
  set_eeprom_param();
}

void WL_AT24C32::set_data_size(size_t data_size)
{
  _dat_size = data_size;
}

void WL_AT24C32::set_eeprom_param(size_t eeprom_size)
{
  _eeprom_size = eeprom_size;
  _eeprom_data_size = _eeprom_size / _dat_size;
  _space_size = _eeprom_data_size / 2; // eeprom effective size

  _ptr_base_faddr = _space_size * 0;  // first half for pointer
  _data_base_faddr = _space_size * 1; // second half for data
}

uint32_t WL_AT24C32::faddr_conv(uint32_t faddr_base, uint32_t faddr)
{
  return ((faddr & 0x1FF) + faddr_base) * _dat_size;
}

float WL_AT24C32::read_dat(uint32_t faddr)
{
  return readFloat(faddr_conv(_data_base_faddr, faddr));
}

uint32_t WL_AT24C32::read_ptr(uint32_t faddr)
{
  return readLong(faddr_conv(_ptr_base_faddr, faddr));
}

uint32_t WL_AT24C32::get_last_faddr(uint32_t *ptr)
{
  uint32_t current_ptr, next_ptr;

  for (size_t faddr = 0; faddr < _space_size; faddr++)
  {
    current_ptr = read_ptr(faddr);
    next_ptr = read_ptr(faddr + 1); // 10th bit overflow handled in faddr_conv

    if (next_ptr - current_ptr != 1)
    {
      *ptr = current_ptr;
      ESP_LOGI("EEPROM", "last faddr = %d, ptr %d", faddr, *ptr);

      return faddr;
    }

    if (next_ptr == 0xFFFFFFFF) // previous reset has not reached a full cycle (512 data-write)
    {
      // faddr 0, ptr 0
      *ptr = 0;
      ESP_LOGI("EEPROM", "last faddr = %d, ptr %d", faddr, *ptr);

      return 0;
    }
  }
  // should be unreachable, added to remove compilation warning
  ESP_LOGE("EEPROM", "get_last_faddr error");
  return 0;
}

void WL_AT24C32::write_mem(float data, uint32_t ptr, uint32_t faddr)
{
  // write data to mem
  writeFloat(faddr_conv(_data_base_faddr, faddr), data);

  // write pointer to mem
  writeLong(faddr_conv(_ptr_base_faddr, faddr), ptr);
}

void WL_AT24C32::wipe()
{
  ESP_LOGD("EEPROM", "DATA WIPING STARTED");
  for (int j = 0; j < (1 << 12); j++)
  {
    if (read(j) != 0xFF)
      write(j, 0xFF);
    if (j % 1000 == 0)
      ESP_LOGD("EEPROM", "%d data wiped, %d to go", j, 4095 - j);
  }
  Serial.println();

  // Reset faddress and pointer to zero
  _current_faddr = 0;
  _current_ptr = 0;
}

void WL_AT24C32::read_all()
{
  for (uint32_t j = 0; j < _space_size; j++)
  {
    Serial.printf("faddr %3d, with pointer %3d <-> data %6f\n",
                  j,
                  read_ptr(j),
                  read_dat(j));
  }
}

void WL_AT24C32::find_last_entry()
{
  _current_faddr = get_last_faddr(&_current_ptr);
}

float WL_AT24C32::get_last_data()
{
  _last_data = read_dat(_current_faddr);
  if (_last_data == 0xFFFFFFFF)
  {
    return 0;
  }
  return _last_data;
}

uint32_t WL_AT24C32::get_current_faddr()
{
  return _current_faddr;
}

uint32_t WL_AT24C32::get_current_ptr()
{
  return _current_ptr;
}

void WL_AT24C32::add_data(float data)
{
  // Increment ptr and faddr
  // If the memory blank, will start writing on faddr 1
  _current_faddr++;
  _current_ptr++;
  write_mem(data, _current_ptr, _current_faddr);
}