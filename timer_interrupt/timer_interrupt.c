#include <reg52.h>
#include <intrins.h>


typedef unsigned char uchar;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};

unsigned int counter_ms_led;
unsigned int counter_ms_digital;
unsigned char display_counter;
uchar digital[2]; 
uchar led_out;
unsigned char running;
  
void digital_increase()
{
  if(digital[0] < 9)
  {
    digital[0]++;
    return;
  }
  
  digital[0] = 0;
  
  if(digital[1] < 5)
  {
    digital[1]++;
    return;
  }
  
  digital[1] = 0;
}

void extern_interrupt0() interrupt 0
{
  running ^= 1;
}

void timer0_callback() interrupt 1
{
  TH0 = 0xFC;
  TL0 = 0x67;
  
  if(running)
  {
    counter_ms_digital ++;
    if(counter_ms_digital >= 1000)
    {
      digital_increase();
      counter_ms_digital = 0;
    }
  }
  
  display_counter ++;
  if(display_counter & 1)
  {
    P2 = 0xF7;
    P0 = DisplayCode[digital[0]];
  }
  else
  {
    P2 = 0xFB;
    P0 = DisplayCode[digital[1]];
  }
}

void timer1_callback() interrupt 3
{
  TH1 = 0xFC;
  TL1 = 0x67;
  
  counter_ms_led ++;
  if(counter_ms_led >= 500)
  {
    led_out = _crol_(led_out, 1);
    P1 = led_out;
    counter_ms_led = 0;
  }
}

void main()
{
  TMOD = 0x11;
  
  //timer 0
  TH0 = 0xFC;
  TL0 = 0x67;
  TF0 = 0;
  TR0 = 1;

  //timer 1
  TH1 = 0xFC;
  TL1 = 0x67;
  TF1 = 0;
  TR1 = 1;
  
  ET1 = 1;
  ET0 = 1;
  
  IT0 = 1;
  EX0 = 1;
  EA = 1;
  
  led_out = 0xFE;
  P1 = led_out;
  counter_ms_led = 0;
  counter_ms_digital = 0;
  display_counter = 0;
  running = 1;
  digital[0] = 0;
  digital[1] = 0;
  
  while(1) _nop_();
}