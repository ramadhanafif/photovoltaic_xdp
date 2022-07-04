//Mendefinisikan Register
#define ADSYSCON    0x00
#define ADSYSCON_PGAIA_Pos 0
#define ADSYSCON_PGAU_Pos  2
#define ADEMUCON    0x01  //Read pin Rx : 00 03 FB
#define ADHFConst   0x02  //Read pin Rx : 10 00 ED
#define ADPStart    0x03
#define ADDStart    0x04  //Read pin Rx : 01 20 DA
#define ADGPQA      0x05
#define ADGPQB      0x06
#define ADPhsA      0x07  //Read pin Rx : 00 F8
#define ADPhsB      0x08  //Read pin Rx : 00 F7
#define ADQPHSCAL   0x09
#define ADAPOSA     0x0a
#define ADAPOSB     0x0b  //Read pin Rx : 00 00 F4
#define ADRPOSA     0x0c
#define ADRPOSB     0x0d  //Read pin Rx : 00 00 F2
#define ADIARMSOS   0x0e  //Read pin Rx : 00 00 F1
#define ADIBRMSOS   0x0f
#define ADIBGain    0x10
#define ADD2FPL     0x11
#define ADD2FPH     0x12
#define ADDCIAH     0x13  //Read pin Rx : 00 00 EC
#define ADDCIBH     0x14
#define ADDCUH      0x15  //Read pin Rx : 00 00 EA
#define ADDCL       0x16  //Read pin Rx : 00 00 E9
#define ADEMUCON2   0x17
#define ADPFCnt     0x20  //Read pin Rx : 00 06 D9
#define ADDFcnt     0x21
#define ADIARMS     0x22
#define ADIBRMS     0x23  //Read pin Rx : 00 00 00 DC
#define ADURMS      0x24
#define ADUFreq     0x25  //Read pin Rx : 22 F5 C3
#define ADPowerPA   0x26  //Read pin Rx : 00 00 03 B4 22
#define ADPowerPB   0x27
#define ADPowerQ    0x28  //Reakif Power
#define ADEnergyP   0x29
#define ADEnergyP2  0x2a  //Read pin Rx : 00 00 00 D6
#define ADEnergyD   0x2b
#define ADEnergyD2  0x2c  //Read pin Rx : 00 00 00 D3
#define ADEMUStatus 0x2d
#define ADSPL_IA    0x30
#define ADSPL_IB    0x31  //Read pin Rx : 00 00 00 CE
#define ADSPL_U     0x32
#define AD_IE       0x40  //Read pin Rx : 00 BF
#define AD_IF       0x41
#define ADRIF       0x42
#define ADSysStatus 0x43  //Read pin Rx : 01 BB
#define ADRData     0x44
#define ADWData     0x45  //Read pin Rx : 00 00 BA
#define ADDeviceID  0x7f  //Read pin Rx : 82 09 00 F5
#define WriteEn     0xea
#define Read        0x00
#define w_rite      0x80    //  di or | dengan address