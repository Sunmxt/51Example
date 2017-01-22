#include <reg52.h>
#include <intrins.h>

typedef unsigned char uchar;

int tick;
uchar flag;

void timer_callback() interrupt 1
{
  TH0 = 0xFC;
  TL0 = 0x67;
  
  tick ++;
  if(tick >= 1000)
  {
    flag = ~flag;
    P1 = flag;
    tick = 0;
  }
}

int main()
{
  flag = 0;
  tick = 0;
  
  P0 = flag;
  
  TMOD = 0x01;
  TH0 = 0xFC;
  TL0 = 0x67;
  TF0 = 0;
  TR0 = 1;
  
  ET0 = 1;
  EA = 1;
  
  while(1) _nop_();
}