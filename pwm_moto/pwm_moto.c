#include <reg52.h>
#include <intrins.h>
#include "utils.h"
#include "at24c64.h"
  
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

uchar PWMCounter[4];
uchar PWMDutyRatio[4];

//Buttons
sbit BUTTON1 = P3^5;
sbit BUTTON2 = P3^4;
sbit BUTTON3 = P3^3;
sbit BUTTON4 = P3^2;

ButtonContext ButtonContext4;
ButtonContext ButtonContext3;
ButtonContext ButtonContext2;
ButtonContext ButtonContext1;


//Control values
#define THRUST_BASE     128

#define THRUST_BIAS_1   0
#define THRUST_BIAS_2   0
#define THRUST_BIAS_3   0
#define THRUST_BIAS_4   0

#define LEFT_ONE        -20
#define LEFT_TWO        -80
#define RIGHT_ONE       20
#define RIGHT_TWO       80

uchar Thrust;           /*
                            Thrust
                         
                            +------------------------------------------------->
                            0
                        */
                        
int Turn;               /* 
                            direction turn value, Left(Negative) Right(Positive)
                                 
                            (LEFT)              (NEUTRAL)                 (RIGHT)
                            ------------------------+-------------------------> (Turn)
                                                    0
                            */
                            
                            
typedef struct Initial_Arguments
{
    uchar thrust;
    uchar thrust_bias[4];
    
    int left_one;
    int left_two;
    int right_one;
    int right_two;
}InitialArgument;
InitialArgument init_args;


//I2C Devices
sbit SCL = P3^0;
sbit SDA = P3^1;

#define AT24C64_ADDRESS 0xA0

bit sda_read()
{return SDA;}

void sda_control(uchar Value)
{SDA = Value;}

void scl_control(uchar Value)
{SCL = Value;}

//Detectors
sbit DETECTOR1 = P0^0;
sbit DETECTOR2 = P0^1;
sbit DETECTOR3 = P0^2;
sbit DETECTOR4 = P0^3;


//Buttons
                            
bit button1_read(void)
{
    return BUTTON1;
}

bit button2_read(void)
{
    return BUTTON2;
}

bit button3_read(void)
{
    return BUTTON3;
}

bit button4_read(void)
{
    return BUTTON4;
}

void button_process()
{
    CheckButton(&ButtonContext1, button1_read, 5, 50, 255);
    if(ButtonContext1.status & BUTTON_CLICKED)
        Thrust += 8;
    CLEAR_BUTTON_MESSAGE(ButtonContext1, BUTTON_MESSAGE_MASK);
    
    
    CheckButton(&ButtonContext2, button2_read, 5, 50, 255);
    if(ButtonContext2.status & BUTTON_CLICKED)
        Thrust -= 8;
    CLEAR_BUTTON_MESSAGE(ButtonContext2, BUTTON_MESSAGE_MASK);
    
    CheckButton(&ButtonContext3, button3_read, 5, 50, 255);
    if(ButtonContext3.status & BUTTON_CLICKED)
        Turn += 8;
    CLEAR_BUTTON_MESSAGE(ButtonContext3, BUTTON_MESSAGE_MASK);
    
    CheckButton(&ButtonContext4, button4_read, 5, 50, 255);
    if(ButtonContext4.status & BUTTON_CLICKED)
        Turn -= 8;
    CLEAR_BUTTON_MESSAGE(ButtonContext4, BUTTON_MESSAGE_MASK);
}

void timer0_process() interrupt 1
{
    TH0 = 0xEE;
    TL0 = 0x00;

    button_process();
}

void init()
{
    //Load configure
    if(SUCCEED == AT24C64PageRead(AT24C64_ADDRESS, 0, &init_args, sizeof(InitialArgument)
                    , scl_control, sda_control, sda_read))
        LED7 = 0;
                    
    //PWM Initialize
    PWM0 = 0;
    PWM1 = 0;
    PWM2 = 0;
    PWM3 = 0;
    
    Turn = 0;
    Thrust = THRUST_BASE;

    PWMCounter[0] = 0;
    PWMCounter[1] = 0;
    PWMCounter[2] = 0;
    PWMCounter[3] = 0;
    
    PWMDutyRatio[0] = Thrust;
    PWMDutyRatio[1] = Thrust;
    PWMDutyRatio[2] = Thrust;
    PWMDutyRatio[3] = Thrust;
    
    TMOD = 0x01;

    TH0 = 0xEE;                 //Timer0 for button checking
    TL0 = 0x00;                 
    
    TH1 = 0xEE;                 //Timer1 for control
    TL1 = 0x00;
    
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
    {
        //PWM Generate
        PWMDutyRatio[0] = Thrust + Turn + THRUST_BIAS_1;
        PWMDutyRatio[1] = Thrust + Turn + THRUST_BIAS_2;
        PWMDutyRatio[2] = Thrust - Turn + THRUST_BIAS_3;
        PWMDutyRatio[3] = Thrust - Turn + THRUST_BIAS_4;
        
        PWMControl(PWM0, PWMCounter[0], 1, PWMDutyRatio[0]);        
        PWMControl(PWM1, PWMCounter[1], 1, PWMDutyRatio[1]);
        PWMControl(PWM2, PWMCounter[2], 1, PWMDutyRatio[2]);
        PWMControl(PWM3, PWMCounter[3], 1, PWMDutyRatio[3]);
        
        LED0 = PWM0;
        LED1 = PWM1;
        LED2 = PWM2;
        LED3 = PWM3;
    }
        
}