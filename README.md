### ESP32 PIN Alloction

| Pin | Catatan   | Catatan 2          | Peripheral         |
|-----|-----------|--------------------|--------------------|
| D15 | SS        | Strapping Pin HIGH | ETHERNET SS        |
| D2  |           | Strapping Pin      | ?                  |
| D4  |           | Strapping Pin      | ?                  |
| D16 | RX2       |                    | GSM TX             |
| D17 | TX2       |                    | GSM RX             |
| D5  |           |                    | ESP SW RX – EM0 TX |
| D18 |           |                    | ESP SW TX – EM0 RX |
| D19 |           |                    | ?                  |
| D21 | SDA       |                    | EEPROM SDA x3+OLED |
| D3  | RX0       | DEBUG & PROG       |                    |
| D1  | TX0       | DEBUG & PROG       |                    |
| D22 | SCL       |                    | EEPROM SCL x3+OLED |
| D23 |           |                    | ?                  |
| D13 | HSPI MOSI |                    | ETHERNET           |
| D12 | HSPI MISO | Strapping Pin LOW  | ETHERNET           |
| D14 | HSPI SCLK |                    | ETHERNET           |
| D27 |           |                    | ESP SW RX – EM1 TX |
| D26 |           |                    | ESP SW TX – EM1 RX |
| D25 |           |                    | ?                  |
| D33 |           |                    | ESP SW RX – EM2 TX |
| D32 |           |                    | ESP SW TX – EM2 RX |
| D35 | GPI       |                    | INTERNET SEL0 SW   |
| D34 | GPI       |                    | INTERNET SEL1 SW   |
| D39 | GPI       |                    | PUSH BUTTON INPUT 0|
| D36 | GPI       |                    | PUSH BUTTON INPUT 1|