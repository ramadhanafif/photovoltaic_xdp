#include <stdint.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// Measurement mode
#define AC 0
#define DC 1

// Register Definition
#define ADSYSCON 0x00
#define ADSYSCON_PGAIA_Pos 0
#define ADSYSCON_PGAU_Pos 2

#define ADEMUCON 0x01  // Read pin Rx : 00 03 FB
#define ADHFConst 0x02 // Read pin Rx : 10 00 ED
#define ADPStart 0x03
#define ADDStart 0x04 // Read pin Rx : 01 20 DA
#define ADGPQA 0x05
#define ADGPQB 0x06
#define ADPhsA 0x07 // Read pin Rx : 00 F8
#define ADPhsB 0x08 // Read pin Rx : 00 F7
#define ADQPHSCAL 0x09
#define ADAPOSA 0x0a
#define ADAPOSB 0x0b // Read pin Rx : 00 00 F4
#define ADRPOSA 0x0c
#define ADRPOSB 0x0d   // Read pin Rx : 00 00 F2
#define ADIARMSOS 0x0e // Read pin Rx : 00 00 F1
#define ADIBRMSOS 0x0f
#define ADIBGain 0x10
#define ADD2FPL 0x11
#define ADD2FPH 0x12
#define ADDCIAH 0x13 // Read pin Rx : 00 00 EC
#define ADDCIBH 0x14
#define ADDCUH 0x15 // Read pin Rx : 00 00 EA
#define ADDCL 0x16  // Read pin Rx : 00 00 E9
#define ADEMUCON2 0x17
#define ADPFCnt 0x20 // Read pin Rx : 00 06 D9
#define ADDFcnt 0x21
#define ADIARMS 0x22
#define ADIBRMS 0x23 // Read pin Rx : 00 00 00 DC
#define ADURMS 0x24
#define ADUFreq 0x25   // Read pin Rx : 22 F5 C3
#define ADPowerPA 0x26 // Read pin Rx : 00 00 03 B4 22
#define ADPowerPB 0x27
#define ADPowerQ 0x28 // Reakif Power
#define ADEnergyP 0x29
#define ADEnergyP2 0x2a // Read pin Rx : 00 00 00 D6
#define ADEnergyD 0x2b
#define ADEnergyD2 0x2c // Read pin Rx : 00 00 00 D3
#define ADEMUStatus 0x2d
#define ADSPL_IA 0x30
#define ADSPL_IB 0x31 // Read pin Rx : 00 00 00 CE
#define ADSPL_U 0x32
#define AD_IE 0x40 // Read pin Rx : 00 BF
#define AD_IF 0x41
#define ADRIF 0x42
#define ADSysStatus 0x43 // Read pin Rx : 01 BB
#define ADRData 0x44
#define ADWData 0x45    // Read pin Rx : 00 00 BA
#define ADDeviceID 0x7f // Read pin Rx : 82 09 00 F5
#define WriteEn 0xea
#define Read 0x00
#define w_rite 0x80 //  di or | dengan address

class RN8209C
{
public:
  // Constructor
  /**
   * @brief Construct a new RN8209C object.
   * Starts UART communication.
   *
   * @param rx_pin GPIO pin used for RX
   * @param tx_pin GPIO pin used for TX and RESET
   * @param mode measuring AC or DC.
   */
  RN8209C(int8_t rx_pin, int8_t tx_pin, uint8_t mode = AC);

  /**
   * @brief Starts UART with the following configuration:
   * BAUD: 4800
   * PARITY: even
   */
  void uart_start();

  /**
   * @brief Disable UART
   *
   */
  void uart_stop();

  /**
   * @brief Send a write command
   *
   * @param reg_address Destination address
   * @param data Array of data to be written
   * @param len Number of elements in array
   */
  void write(uint8_t reg_address, uint8_t *data, size_t len);

  /**
   * @brief Read data from device register
   *
   * @param reg_address Device address to be read
   * @param rx_array Array to store received data
   * @param len Expected data length
   * @return int32_t Checksum fail: (1), success (0)
   */
  int32_t read(uint8_t reg_address, uint8_t *rx_array, size_t len);

  /**
   * @brief Read calibration registers, prints out to Serial
   */
  void readCalibRegs();

  /**
   * @brief Set calibration registers
   *
   */
  void setCalibRegs();

  /**
   * @brief Convert data array in bytes to 32-bit integer
   *
   * @param arr Data array
   * @param len Data array length
   * @return uint32_t
   */
  uint32_t arr2raw(uint8_t *arr, size_t len);

  /**
   * @brief Get measured energy in kWh
   *
   * @return float
   */
  float get_energy();

  /**
   * @brief Get measured VRMS in V
   *
   * @return float
   */
  float get_vrms();

  /**
   * @brief Get measured IRMS in A
   *
   * @return float
   */
  float get_irms();

  /**
   * @brief Get measured active power in W
   *
   * @return float
   */
  float get_power();

  /**
   * @brief Get measured reactive power in VAR
   *
   * @return float
   */
  float get_reactivepower();

  /**
   * @brief Calculate apparent power in VA
   *
   * @return float
   */
  float calc_apprpower();

  /**
   * @brief Calculate power factor
   *
   * @return float
   */
  float calc_powerfactor();

  /**
   * @brief Global reset by pulling down RX Pin.
   * To reset: uart_stop(), rx_reset(), uart_start(), setCalibRegs()
   */
  void rx_reset();

  
  int8_t port_tx;
  int8_t port_rx;
  SoftwareSerial EMserial;
  
  /**
   * @brief Buffer when reading data in array of bytes 
   * 
   */
  uint8_t buffe[5];

private:
  int32_t apparentRaw;
  int32_t reactiveRaw;
  int32_t activeRaw;
  int32_t irmsRaw;
  int32_t vrmsRaw;
  int32_t energyPRaw;
};
