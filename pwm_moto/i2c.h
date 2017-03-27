
/*
    Basic sofeware interfaces of I2C Protocal for 51 MCU.
*/

#ifndef TINY_I2C_INTERFACES_HEADER
#define TINY_I2C_INTERFACES_HEADER


#include <intrins.h>
#include "types.h"

/*

Pin control function:

        In order to use interfaces, you need to implement pin control functions and pass them
    as arguments when calling the interfaces. 
    
    void SCLControl(uchar value)
        SCL pin control function implemented by user.
        
    void SDAControl(uchar value)
        SDA pin control function implemented by user.
        
    bit SDARead()
        Read a bit from SDA and return it. Implemented by user.
        
Usage:
    
    1. I2CStart to send start signal
    2. I2CSendAddress to select device
    3. I2CRead/I2CWrite to read/write data.
    4. I2CStop to send stop signal.
    
*/

typedef void (*PinControl)(uchar value);
typedef bit (*PinRead)();

bit I2CSendAddress(uchar Address, bit IsRead
                , PinControl SCLControl
                , PinControl SDAControl
                , PinRead SDARead);

bit I2CWrite(uchar Data, PinControl SCLControl
                , PinControl SDAControl
                , PinRead SDARead);                

void I2CStart(PinControl SCLControl, PinControl SDAControl);
void I2CStop(PinControl SCLControl, PinControl SDAControl);
uchar I2CRead(PinControl SCLControl, PinControl SDAControl, PinRead SDARead);

bit I2CTestAcknowledge(PinRead SDARead);
void I2CSendReadContinueSignal(PinControl SCLControl, PinControl SDAControl);

#endif