#include <reg52.h>
#include <intrins.h>
#define TIMER_MODE 2

unsigned char flash;
unsigned int counter_ms;
unsigned char quarter_ms;

void timer0_interrupt() interrupt 1
{
  unsigned int excess;
  
  #if TIMER_MODE == 0
    TH0 = 0xE3;
    TL0 = 0x06;
    excess = 500;
  #elif TIMER_MODE == 1
    TH0 = 0xFC;
    TL0 = 0x67;
    excess = 500;
  #elif TIMER_MODE == 2
    excess = 2000;
  #elif TIMER_MODE == 3
    TL0 = 0x1A;
    excess = 2000;
  #else
    #error #error unsupported timer mode.
  #endif
   
  counter_ms++;
  if(counter_ms > excess)
  {
    counter_ms = 0;
    flash = ~flash;
    P1 = flash;
  }
  
}

void timer_init()
{
  P1 = 0;
  counter_ms = 0;
  quarter_ms = 0;
  flash = 0;
  
  #if TIMER_MODE == 0
    TMOD = 0x00;
    TH0 = 0xE3;
    TL0 = 0x06;
  #elif TIMER_MODE == 1
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;
  #elif TIMER_MODE == 2
    TMOD = 0x02;
    TH0 = 0x1A;
    TL0 = 0x1A;
  #elif TIMER_MODE == 3
    TMOD = 0x03;
    TL0 = 0x1A;
  #else
    #error unsupported timer mode.
  #endif
  
  ET0 = 1;
  EA = 1;
  
  TF0 = 0;
  TR0 = 1;
}

void main()
{  
  timer_init();
  for(;;)
    _nop_();
}