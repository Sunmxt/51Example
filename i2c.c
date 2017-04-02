#include "i2c.h"

void i2c_delay_5us()
{
  _nop_();
}

void I2CStart(PinControl SCLControl, PinControl SDAControl)
{
  SCLControl(0);
  i2c_delay_5us();
  
  SDAControl(1);
  i2c_delay_5us();
  
  SCLControl(1);
  i2c_delay_5us();
  
  SDAControl(0);
  i2c_delay_5us();
}

void I2CStop(PinControl SCLControl, PinControl SDAControl)
{
  SCLControl(0);
  i2c_delay_5us();
  
  SDAControl(0);
  i2c_delay_5us();
  
  SCLControl(1);
  i2c_delay_5us();

  SDAControl(1);
  i2c_delay_5us();
}

bit I2CTestAcknowledge(PinRead SDARead)
{
  return !SDARead();
}

bit I2CWrite(uchar Data, PinControl SCLControl
                , PinControl SDAControl
                , PinRead SDARead)
{
  register uchar i;
  
  for(i = 0 ; i < 8 ; i++)
  {
    SCLControl(0);
    _nop_();
    
    SDAControl(Data & 0x80);
    _nop_();
   
    SCLControl(1);
    i2c_delay_5us();
    
    Data <<= 1;
  }
  
  SCLControl(0);
  _nop_();
  SDAControl(1);
  _nop_();
  SCLControl(1);
  i2c_delay_5us();
  
  return I2CTestAcknowledge(SDARead);
}

uchar I2CRead(PinControl SCLControl, PinControl SDAControl, PinRead SDARead)
{
  register uchar _data, i;
  
  _data = 0;
  SCLControl(0);
  i2c_delay_5us();
  SDAControl(1);
  i2c_delay_5us();
  
  SCLControl(1);
  _nop_();
  _data = SDARead();
  for(i = 1 ; i < 8 ; i++)
  {
    SCLControl(0);
    _nop_();
    
    SCLControl(1);
    _data <<= 1;
    _data |= SDARead();
  }
  
  return _data;
}

void I2CSendReadContinueSignal(PinControl SCLControl, PinControl SDAControl)
{
  SCLControl(0);
  i2c_delay_5us();
  SDAControl(0);
  i2c_delay_5us();
  SCLControl(1);
  i2c_delay_5us();
}

bit I2CSendAddress(uchar Address, bit IsRead
                , PinControl SCLControl
                , PinControl SDAControl
                , PinRead SDARead)
{
  return I2CWrite((Address << 1) | IsRead, SCLControl, SDAControl, SDARead);
}