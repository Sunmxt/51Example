/*
    
    Basic software interface for PCF8591 AD/DA Converter.
    
 */

#ifndef TINY_PCF8591_INTERFACE_HEADER
#define TINY_PCF8591_INTERFACE_HEADER

#include "i2c.h"

typedef void (*SCLControlFunction)(uchar value);
typedef void (*SDAControlFunction)(uchar value);
typedef bit (*SDAReadFunction)();

#define PCF8591_MAKE_CONTROL_BYTE(ADChannel, AutoIncrement, InputMode, DAEnable)\
     (uchar)((ADChannel & 3) | ((uchar)AutoIncrement << 2) \
             | ((InputMode & 3) << 4) | ((uchar)DAEnable << 6))
             
uchar PCF8591OpenWrite(uchar Address, SCLControlFunction SCLSet
                        , SDAControlFunction SDASet
                        , SDAReadFunction SDARead);
                        
uchar PCF8591Send(uchar Data, SCLControlFunction SCLSet, SDAControlFunction SDASet
                                , SDAReadFunction SDARead);
                                
uchar PCF8591OpenRead(uchar Address, SCLControlFunction SCLSet
                        , SDAControlFunction SDASet
                        , SDAReadFunction SDARead);
                        
uchar PCF8591Receive(SCLControlFunction SCLSet, SDAControlFunction SDASet, SDAReadFunction SDARead);
void PCF8591ReceiveContinue(SCLControlFunction SCLSet ,SDAControlFunction SDASet);
void PCF8591Close(SCLControlFunction SCLSet, SDAControlFunction SDASet);


#endif