#include <reg52.h>
#include <intrins.h>

sbit SERIAL_INPUT_A = P1^2;
sbit OUTPUT_ENABLE = P1^3;
sbit LATCH_CLOCK = P1^4;
sbit SHIFT_CLOCK = P1^5;

typedef unsigned char uchar;

void delay()
{
  int i;
  
  for(i = 20000 ; i ; i--);
}

void delay_5us()
{
  _nop_();
}

void _74HC595Send(uchar Data)
{
  int i;
  
  OUTPUT_ENABLE = 1;
  LATCH_CLOCK = 0;
  SHIFT_CLOCK = 0;
  for(i = 0 ; i < 8 ; i++)
  {
    delay_5us();
    SERIAL_INPUT_A = Data & 0x80;
    Data <<= 1;
    SHIFT_CLOCK = 1;
    SHIFT_CLOCK = 0;
  }
  
  LATCH_CLOCK = 1;
  OUTPUT_ENABLE = 0;
}

void main()
{
  while(1)
  {
    delay();
    _74HC595Send(0x80);
    delay();
    _74HC595Send(0x40);
  }
  
  //for(;;)
   // _nop_();
}