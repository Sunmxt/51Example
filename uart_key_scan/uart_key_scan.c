#include <reg52.h>

typedef unsigned char uchar;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};

uchar ShowHex[4];
uchar loop_hex;
uchar last_key_code;
  
void uart_interrupt() interrupt 4
{
  if(TI == 1)
    TI = 0;
}
  
void init()
{
  ShowHex[0] = 0;
  ShowHex[1] = 0;
  ShowHex[2] = 0;
  ShowHex[3] = 0;
  
  loop_hex = 0;
  last_key_code = 0;
  
  //timer
  TMOD = 0x21;
  
  TH1 = 0xF4;
  TL1 = 0xF4;
  
  TH0 = 0xFC;
  TL0 = 0x67;
  TR0 = 1;
  TR1 = 1;
  
  //UART
  SM0 = 0;
  SM1 = 1;
  REN = 0;
  TI = 0;
  
  ES = 1;
  EA = 1;
}

void hex_display()
{
  P2 = ~(1 << loop_hex);
  P0 = DisplayCode[ShowHex[3 - loop_hex]];
  
  loop_hex++;
  if(loop_hex > 3)
    loop_hex = 0;
}

void delay()
{
  int i;
  
  for(i = 1000 ; i ; i --);
}

void convert_and_set_hex(uchar Value)
{
  int i;
  
  for(i = 0 ; i < 4 && Value; i++)
  {
    ShowHex[i] = Value % 10;
    Value /= 10;
  }
  
  for(; i < 4 ; i++)
    ShowHex[i] = 0;
}

void key_press(uchar RawKeyCode)
{
  uchar key_code;
  
  key_code = ((RawKeyCode & 0xC0) >> 2) | ((RawKeyCode & 0x30) >> 4);
  
  convert_and_set_hex(key_code);
  
  SBUF = key_code;
}



void key_scan()
{
  uchar key_code;
  
  P1 = 0xBF;
  key_code = P1;
  if(key_code != 0xBF)
  {
    delay();
    key_code = P1;
    if(key_code != 0xBF && last_key_code != key_code)
    {
      key_press(key_code);
      last_key_code = key_code;
    }
    
    P1 = 0xFF;
    return;
  }
  
  
  P1 = 0x7F;
  key_code = P1;
  if(key_code != 0x7F)
  {
    delay();
    key_code = P1;
    if(key_code != 0x7F && last_key_code != key_code)
    {
      key_press(key_code);
      last_key_code = key_code;
    }
    
    P1 = 0xFF;
    return;
  }
  
  last_key_code = 0;
}

void main()
{
  init();
  
  for(;;)
  {
      if(TF0)
      {
        TF0 = 0;
        TH0 = 0xFC;
        TL0 = 0x67;
        
        key_scan();
        hex_display();
      }
  }
}