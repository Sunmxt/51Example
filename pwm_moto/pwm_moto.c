#include <reg52.h>
#include <intrins.h>
#include "utils.h"
  
//LED Indicator
sbit LED0 = P1^0;
sbit LED1 = P1^1;
sbit LED2 = P1^2;
sbit LED3 = P1^3;
sbit LED4 = P1^4;
sbit LED5 = P1^5;
sbit LED6 = P1^6;
sbit LED7 = P1^7;


//PWM Pin
sbit PWM0 = P2^0;
sbit PWM1 = P2^1;
sbit PWM2 = P2^2;
sbit PWM3 = P2^3;
PWMPinContext PWMPin[4];


//Buttons
sbit BUTTON1 = P3^5;
sbit BUTTON2 = P3^4;

ButtonContext ButtonContext2;
ButtonContext ButtonContext1;







//Control values
#define THRUST_BASE     128

uchar Thrust;                     
int Turn;                        //direction turn value, Left(Negative) Right(Positive)




bit button1_read(void)
{
    return BUTTON1;
}

bit button2_read(void)
{
    return BUTTON2;
}


void button_process()
{
    CheckButton(&ButtonContext1, button1_read, 1, 50, 255);
    if(ButtonContext1.status & BUTTON_CLICKED)
    {
        PWMPin[0].high_width += 8;
        PWMPin[1].high_width += 8;
        PWMPin[2].high_width += 8;
        PWMPin[3].high_width += 8;
    }
    CLEAR_BUTTON_MESSAGE(ButtonContext1, BUTTON_MESSAGE_MASK);
    
    
    CheckButton(&ButtonContext2, button2_read, 1, 50, 255);
    if(ButtonContext2.status & BUTTON_CLICKED)
    {
        PWMPin[0].high_width -= 8;
        PWMPin[1].high_width -= 8;
        PWMPin[2].high_width -= 8;
        PWMPin[3].high_width -= 8;
    }
    CLEAR_BUTTON_MESSAGE(ButtonContext2, BUTTON_MESSAGE_MASK);
}

void timer0_process() interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x66;
    
    button_process();
}


void timer1_process() interrupt 3
{
    PWMPin[0].high_width = Thrust + Turn;
    PWMPin[1].high_width = Thrust + Turn;
    PWMPin[2].high_width = Thrust - Turn;
    PWMPin[3].high_width = Thrust - Turn;
    
    PWM0 = PWMControl(PWMPin, 1);
    PWM1 = PWMControl(PWMPin + 1, 1);
    PWM2 = PWMControl(PWMPin + 2, 1);
    PWM3 = PWMControl(PWMPin + 3, 1);
}
void init()
{
    //PWM Initialize
    PWM0 = 0;
    PWM1 = 0;
    PWM2 = 0;
    PWM3 = 0;
    
    Turn = 0;
    
    PWMPin[0].high_width = Thrust;
    PWMPin[1].high_width = Thrust;
    PWMPin[2].high_width = Thrust;
    PWMPin[3].high_width = Thrust;
    
    PWMPin[0].counter = 0;
    PWMPin[1].counter = 0;
    PWMPin[2].counter = 0;
    PWMPin[3].counter = 0;
    
    
    
    TMOD = 0x21;                //Timer0 for PWM signal generating
    TH1 = 0xA3;                 //0.1ms
    TL1 = 0xA3;
    
    TH0 = 0xFC;                 //Timer1 for button checking
    TL0 = 0x66;
    
    TR0 = 1;
    TR1 = 1;
    
    //Interrupt
    ET0 = 1;
    ET1 = 1;
    EA = 1;
}

void main()
{   
    init();
    
    while(1)
        _nop_();
}