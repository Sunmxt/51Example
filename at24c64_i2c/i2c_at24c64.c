#include <reg52.h>
#include <intrins.h>

typedef unsigned char uchar;
typedef unsigned char ushort;
typedef unsigned int uint;

void i2c_delay_5us()
{
  _nop_();
}

void I2CStart(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value))
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

void I2CStop(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value))
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

bit I2CTestAcknowledge(bit (*SDARead)())
{
  return !SDARead();
}

bit I2CWrite(uchar Data, void (*SCLControl)(uchar value)
                , void (*SDAControl)(uchar value)
                , bit (*SDARead)())
{
  uchar i;
  
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

uchar I2CRead(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value), bit (*SDARead)())
{
  uchar _data, i;
  
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

void I2CSendReadContinueSignal(void (*SCLControl)(uchar value), void (*SDAControl)(uchar value))
{
  SCLControl(0);
  i2c_delay_5us();
  SDAControl(0);
  i2c_delay_5us();
  SCLControl(1);
  i2c_delay_5us();
}

bit I2CSendAddress(uchar Address, bit IsRead
                , void (*SCLControl)(uchar value)
                , void (*SDAControl)(uchar value)
                , bit (*SDARead)())
{
  return I2CWrite((Address << 1) | IsRead, SCLControl, SDAControl, SDARead);
}


/////////////////////

void at24c64_sda_control(uchar Value)
{
  SDA = Value;
}

void at24c64_scl_control(uchar Value)
{
  SCL = Value;
}

bit at24c64_sda_read()
{return SDA;}

#define SUCCEED             0
#define ERR_DEVICE_ADDRESS  1
#define ERR_DATA_ADDRESS_1  2
#define ERR_DATA_ADDRESS_2  3
#define ERR_DATA            4
uchar AT24C64PageWrite(uchar DeviceAddress, ushort DataAddress, uchar *Data, uint Size)
{ 
  uchar status;
  uint i; 
  
  status = SUCCEED;
  
  I2CStart(at24c64_scl_control, at24c64_sda_control);
  
  if(!I2CSendAddress(DeviceAddress, 0, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
    status = ERR_DEVICE_ADDRESS;
    goto AT24C64_Write_Exception;
  }
  if(!I2CWrite(DataAddress >> 8, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
    status = ERR_DATA_ADDRESS_1;
    goto AT24C64_Write_Exception;
  }
  if(!I2CWrite(DataAddress & 0x00FF, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
    status = ERR_DATA_ADDRESS_2;
    goto AT24C64_Write_Exception;
  }
  
  for(i = 0 ; i < Size ; i++)
    if(!I2CWrite(Data[i], at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
      status = ERR_DATA;
      goto AT24C64_Write_Exception;
    }
  
AT24C64_Write_Exception:
  I2CStop(at24c64_scl_control, at24c64_sda_control);

  return status;
}

uchar AT24C64Write(uchar DeviceAddress, ushort DataAddress, uchar Data)
{return AT24C64PageWrite(DeviceAddress, DataAddress, &Data, 1);}

uchar AT24C64PageRead(uchar DeviceAddress, ushort DataAddress, uchar *Data, uint Size)
{
  uint i;
  uchar status;
  
  status = SUCCEED;
  
  I2CStart(at24c64_scl_control, at24c64_sda_control);
  if(!I2CSendAddress(DeviceAddress , 0, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
    status = ERR_DEVICE_ADDRESS;
    goto AT24C64_Page_Read_Exception;
  }
  if(!I2CWrite(DataAddress >> 8, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
    LED6 = 0;
    status = ERR_DATA_ADDRESS_1;
    goto AT24C64_Page_Read_Exception;
  }
  if(!I2CWrite(DataAddress & 0x00FF, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)) {
    LED5 = 0;
    status = ERR_DATA_ADDRESS_2;
    goto AT24C64_Page_Read_Exception;
  }
    
  I2CStart(at24c64_scl_control, at24c64_sda_control);
  if(!I2CSendAddress(DeviceAddress , 1, at24c64_scl_control, at24c64_sda_control, at24c64_sda_read)){
    status = ERR_DEVICE_ADDRESS;
    goto AT24C64_Page_Read_Exception;
  }
  
  i = 0;
  for(;;)
  {
    Data[i] = I2CRead(at24c64_scl_control, at24c64_sda_control, at24c64_sda_read);
    i++;
    if(i >= Size) break;
    I2CSendReadContinueSignal(at24c64_scl_control, at24c64_sda_control);
  }
  
AT24C64_Page_Read_Exception:
  I2CStop(at24c64_scl_control, at24c64_sda_control);
  
  return status;
}

uchar AT24C64Read(uchar DeviceAddress, ushort DataAddress ,uchar *Data)
{return AT24C64PageRead(DeviceAddress, DataAddress, Data, 1);}

#define AT24C64_ADDRESS 0x50

sbit SDA = P1^3;
sbit SCL = P1^2;
sbit LED8 = P1^7;
sbit LED7 = P1^6;
sbit LED6 = P1^5;
sbit LED5 = P1^4;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};
  
  
uchar Digital[2];
uint counter_ms;
  
void init()
{
  uchar _data;
  
  if(SUCCEED == AT24C64Read(AT24C64_ADDRESS, 0, &_data))
    LED7 = 0;
  else
    LED6 = 0;
  
  Digital[0] = _data % 10;
  Digital[1] = _data / 10;
  
  /*Digital[0] = 0;
  Digital[1] = 0;
  */
  
  counter_ms = 0;
 
  TMOD = 0x01;
  TH0 = 0xFC;
  TL0 = 0x67;
  TF0 = 0;
  TR0 = 1;
  
  ET0 = 1;
  EA = 1;
}

void timer_0_interrupt() interrupt 1
{
  TH0 = 0xFC;
  TL0 = 0x67;
  
  if(counter_ms & 1)
  {
    P2 = 0xF7;
    P0 = DisplayCode[Digital[0]];
  }
  else
  {
    P2 = 0xFB;
    P0 = DisplayCode[Digital[1]];
  }

  counter_ms++;
  if(counter_ms >= 5000)
  {
    counter_ms = 0;
    
    Digital[0] ++;
    if( Digital[0] > 9 )
    {
      Digital[1]++;
      Digital[0] = 0;
    }
    if( Digital[1] > 9)
      Digital[1] = 0;
    
    if(SUCCEED == AT24C64Write(AT24C64_ADDRESS ,0 ,Digital[1]*10 + Digital[0]))
      LED8 = 0;
    else
      LED8 = 1;
  }
}

void main()
{
  init();
  
  /*
  uchar dat;
  uint i ,j;
  
  
  
  
  
  uchar buf[] = {0xAA ,0xAB ,0x89 ,0xC6};
  
  if(SUCCEED == AT24C64PageWrite(0x01, 0, buf, 4))
    LED8 = 0;
  
  */
  /*
  uchar buf[4];
  if(SUCCEED == AT24C64PageRead(0x01 , 0, buf, 4))
    LED8 = 0;
  
  for(j = 0 ; ; j ++)
  {
    if(j > 3) j = 0;
    
    for(i = 65535 ; i ; i --)
      _nop_();
    
    P1 = buf[j];
  }
  */
  while(1)
    _nop_();
}