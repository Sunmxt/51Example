
/*
    Basic sofeware interfaces of I2C Protocal for 51 MCU.
*/

#ifndef TINY_I2C_INTERFACES_HEADER
#define TINY_I2C_INTERFACES_HEADER


#include <intrins.h>

typedef unsigned char uchar;

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
    
*/

bit I2CSendAddress(uchar Address, bit IsRead
                , void (*SCLControl)(uchar value)
                , void (*SDAControl)(uchar value)
                , bit (*SDARead)());

bit I2CWrite(uchar Data, void (*SCLControl)(uchar value)
                , void (*SDAControl)(uchar value)
                , bit (*SDARead)());                

void I2CStart(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value));
void I2CStop(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value));
uchar I2CRead(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value), bit (*SDARead)());

bit I2CTestAcknowledge(bit (*SDARead)());
void I2CSendReadContinueSignal(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value));

#endif