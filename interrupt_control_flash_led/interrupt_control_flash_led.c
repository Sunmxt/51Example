#include <reg52.h>
#include <intrins.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uint CounterMs;
uchar Flash;
uchar Run;

void external_interrupt() interrupt 0
{
  Run = ~Run;
}

void timer_interrupt() interrupt 1
{ 
  TH0 = 0xFC;
  TL0 = 0x67;
  
  CounterMs++;
  if(CounterMs > 500)
  {
    CounterMs = 0;
    Flash = Flash ^ Run;
    P1 = Flash;
  }
}

void init()
{
  CounterMs = 0;
  Flash = 0;
  Run = 0xFF;
  P1 = 0;
  
  //interrupt
  IT0 = 0;
  EX0 = 1;
  ET0 = 1;
  EA = 1;
  
  //timer
  TMOD = 0x01;
  TH0 = 0xFC;
  TL0 = 0x67;
  TF0 = 0;
  TR0 = 1;
}

void main()
{
  init();
  for(;;)
    _nop_();
}