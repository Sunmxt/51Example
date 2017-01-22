/*
    Basic implement for PCF8591 interfaces.
*/

#include "pcf8591.h"


uchar PCF8591OpenWrite(uchar Address, SCLControlFunction SCLSet
                        , SDAControlFunction SDASet
                        , SDAReadFunction SDARead)
{
    uchar result;
    
    I2CStart(SCLSet, SDASet);
    result = I2CSendAddress(Address, 0, SCLSet, SDASet, SDARead);
    
    if(!result)
        I2CStop(SCLSet, SDASet);
    return result;
}

                    
uchar PCF8591Send(uchar Data, SCLControlFunction SCLSet, SDAControlFunction SDASet
                                , SDAReadFunction SDARead)
{
    return I2CWrite(Data, SCLSet, SDASet, SDARead);
}

uchar PCF8591OpenRead(uchar Address, SCLControlFunction SCLSet
                        , SDAControlFunction SDASet
                        , SDAReadFunction SDARead)
{
    uchar result;
    
    I2CStart(SCLSet, SDASet);
    result = I2CSendAddress(Address, 1, SCLSet, SDASet, SDARead);
    
    if(!result)
        I2CStop(SCLSet, SDASet);
    
    return result;
}

uchar PCF8591Receive(SCLControlFunction SCLSet, SDAControlFunction SDASet, SDAReadFunction SDARead)
{
    return I2CRead(SCLSet, SDASet, SDARead);
}

void PCF8591ReceiveContinue(SCLControlFunction SCLSet ,SDAControlFunction SDASet)
{
    I2CSendReadContinueSignal(SCLSet, SDASet);
}

void PCF8591Close(SCLControlFunction SCLSet, SDAControlFunction SDASet)
{
    I2CStop(SCLSet, SDASet);
}

