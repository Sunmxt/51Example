#include "at24c64.h"



uchar AT24C64PageWrite(uchar DeviceAddress, ushort DataAddress, void *Data, uint Size
                        ,PinControl SCL, PinControl SDA, PinRead SDARead)
{ 
  uchar status;
  uint i; 
  
  status = SUCCEED;
  
  I2CStart(SCL, SDA);
  
  if(!I2CSendAddress(DeviceAddress, 0,SCL, SDA, SDARead)) {
    status = ERR_DEVICE_ERROR;
    goto AT24C64_Write_Exception;
  }
  if(!I2CWrite(DataAddress >> 8,SCL, SDA, SDARead)) {
    status = ERR_DATA_ADDRESS_1;
    goto AT24C64_Write_Exception;
  }
  if(!I2CWrite(DataAddress & 0x00FF,SCL, SDA, SDARead)) {
    status = ERR_DATA_ADDRESS_2;
    goto AT24C64_Write_Exception;
  }
  
  for(i = 0 ; i < Size ; i++)
    if(!I2CWrite(((uchar*)Data)[i],SCL, SDA, SDARead)) {
      status = ERR_DATA;
      goto AT24C64_Write_Exception;
    }
  
AT24C64_Write_Exception:
  I2CStop(SCL, SDA);

  return status;
}

uchar AT24C64PageRead(uchar DeviceAddress, ushort DataAddress, void *Data, uint Size
                        ,PinControl SCL, PinControl SDA, PinRead SDARead)
{
  uint i;
  uchar status;
  
  status = SUCCEED;
  
  I2CStart(SCL, SDA);
  if(!I2CSendAddress(DeviceAddress , 0,SCL, SDA, SDARead)) {
    status = ERR_DEVICE_ERROR;
    goto AT24C64_Page_Read_Exception;
  }
  if(!I2CWrite(DataAddress >> 8,SCL, SDA, SDARead)) {
    status = ERR_DATA_ADDRESS_1;
    goto AT24C64_Page_Read_Exception;
  }
  if(!I2CWrite(DataAddress & 0x00FF,SCL, SDA, SDARead)) {
    status = ERR_DATA_ADDRESS_2;
    goto AT24C64_Page_Read_Exception;
  }
    
  I2CStart(SCL, SDA);
  if(!I2CSendAddress(DeviceAddress , 1,SCL, SDA, SDARead)){
    status = ERR_DEVICE_ERROR;
    goto AT24C64_Page_Read_Exception;
  }
  
  i = 0;
  for(;;)
  {
    ((uchar*)Data)[i] = I2CRead(SCL, SDA, SDARead);
    i++;
    if(i >= Size) break;
    I2CSendReadContinueSignal(SCL, SDA);
  }
  
AT24C64_Page_Read_Exception:
  I2CStop(SCL, SDA);
  
  return status;
}