#include <stdint.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// Measurement mode
#define AC 0
#define DC 1

// Register Definition
enum reg_chanA : uint8_t
{
  ADSYSCON = 0x00,
  ADEMUCON = 0x01,
  ADHFConst = 0x02,
  ADPStart = 0x03,
  ADDStart = 0x04,
  ADGPQA = 0x05,
  ADGPQB = 0x06,
  ADPhsA = 0x07,
  ADPhsB = 0x08,
  ADQPHSCAL = 0x09,
  ADAPOSA = 0x0a,
  ADAPOSB = 0x0b,
  ADRPOSA = 0x0c,
  ADRPOSB = 0x0d,
  ADIARMSOS = 0x0e,
  ADIBRMSOS = 0x0f,
  ADIBGain = 0x10,
  ADD2FPL = 0x11,
  ADD2FPH = 0x12,
  ADDCIAH = 0x13,
  ADDCIBH = 0x14,
  ADDCUH = 0x15,
  ADDCL = 0x16,
  ADEMUCON2 = 0x17,
  ADPFCnt = 0x20,
  ADDFcnt = 0x21,
  ADIARMS = 0x22,
  ADIBRMS = 0x23,
  ADURMS = 0x24,
  ADUFreq = 0x25,
  ADPowerPA = 0x26,
  ADPowerPB = 0x27,
  ADPowerQ = 0x28,
  ADEnergyP = 0x29,
  ADEnergyP2 = 0x2a,
  ADEnergyD = 0x2b,
  ADEnergyD2 = 0x2c,
  ADEMUStatus = 0x2d,
  ADSPL_IA = 0x30,
  ADSPL_IB = 0x31,
  ADSPL_U = 0x32,
  AD_IE = 0x40,
  AD_IF = 0x41,
  ADRIF = 0x42,
  ADSysStatus = 0x43,
  ADRData = 0x44,
  ADWData = 0x45,
  ADDeviceID = 0x7f
};

// Setting Current Channel A analog gain selection, default 16x
#define ADSYSCON_PGAIA_Pos 0
// Setting Voltage channel analog gain selection
#define ADSYSCON_PGAU_Pos 2
// Setting Disable/Enable High Pass Filter for DC Measurement
#define ADEMUCON_HPFIOFF_Pos 6 // En=0;Dis=1; CURRENT digital high-pass filter
#define ADEMUCON_HPFUOFF_Pos 5 // En=0;Dis=1; VOLTAGE digital high-pass filter

#define SPECIAL_CMD 0xEA

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
   * @param mode measuring AC or DC. Defaults to AC.
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
  void write(uint8_t reg_address, const uint8_t *data, size_t len);

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
