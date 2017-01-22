#include <reg52.h>

typedef unsigned char uchar;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};

uchar ShowHex[4];
uchar loop_hex;

#define OSCILLATOR_FREQUENCY  11.0592
#define UART_BITRATE          4800
  
#define UART_TIMER_MODE_2_TICK ((unsigned char)(256 - (OSCILLATOR_FREQUENCY*1000000/(12*32*UART_BITRATE))))

void init()
{ 
  loop_hex = 0;
  ShowHex[0] = 0;
  ShowHex[1] = 0;
  ShowHex[2] = 0;
  ShowHex[3] = 0;
  
  //set timer 1
  TMOD = 0x21;
  TH1 = 0xFA;
  TL1 = 0xFA;
  
  TH0 = 0xFC;
  TL0 = 0x67;
  
  //UART mode
  SM0 = 0;
  SM1 = 1;
  
  REN = 1;
  
  //intertupt enable
  ES = 1;   //uart
  EA = 1;   //all
  
  TR1 = 1;  //start timer 1
  TR0 = 1;  //start timer 0
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

void uart_interrupt() interrupt 4
{
  uchar received;
  if(RI)
  {
    received = SBUF;
    RI = 0;
    convert_and_set_hex(received);
  }
}

void hex_display()
{
  P2 = ~(1 << loop_hex);
  P0 = DisplayCode[ShowHex[3 - loop_hex]];
  
  loop_hex++;
  if(loop_hex > 3)
    loop_hex = 0;
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
        
        hex_display();
      }
  }
}