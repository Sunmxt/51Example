#ifndef TINY_AT24C64_INTERFACES
#define TINY_AT24C64_INTERFACES

#include "i2c.h"

/*

    AT24C64 Interfaces for 51 MCU.
    
*/

/*

*/

/*
    Status codes
*/
#define SUCCEED             0
#define ERR_DEVICE_ERROR  1
#define ERR_DATA_ADDRESS_1  2
#define ERR_DATA_ADDRESS_2  3
#define ERR_DATA            4



uchar AT24C64PageWrite(uchar DeviceAddress, ushort DataAddress, void *Data, uint Size
                        , PinControl SCL, PinControl SDA, PinRead SDARead);

//uchar AT24C64Write(uchar DeviceAddress, ushort DataAddress, uchar Data
//                    , PinControl SCL, PinControl SDA, PinRead SDARead);

#define AT24C64Write(DeviceAddress, DataAddress, Data, SCL, SDA, SDARead)\
        AT24C64PageWrite(DeviceAddress, DataAddress, &(Data), 1, SCL, SDA, SDARead)
        
uchar AT24C64PageRead(uchar DeviceAddress, ushort DataAddress, void *Data, uint Size
                        , PinControl SCL, PinControl SDA, PinRead SDARead);

//uchar AT24C64Read(uchar DeviceAddress, ushort DataAddress ,uchar *Data
//                    , PinControl SCL, PinControl SDA, PinRead SDARead);

#define AT24C64Read(DeviceAddress, DataAddress, DataPtr, Data, SCL, SDA, SDARead)\
        AT24C64PageRead(DeviceAddress, DataAddress, &(Data), 1, SCL, SDA, SDARead)


#endif