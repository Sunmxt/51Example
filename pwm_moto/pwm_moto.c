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

uchar LastStatus;
#define LEFT_2  0
#define LEFT_1  1
#define NEUTRAL 2
#define RIGHT_1 3
#define RIGHT_2 4
#define IDLE    5
#define STATUS_MASK     7
#define MAINTAIN_BIT    8
#define SET_STATUS(LastStatus, NewStatus) \
        (LastStatus = ((LastStatus & MAINTAIN_BIT) | NewStatus))
#define SET_MAINTAIN_BIT(LastStatus) \
        (LastStatus |= MAINTAIN_BIT)
#define CLEAR_MAINTAIN_BIT(LastStatus) \
        (LastStatus &= ~MAINTAIN_BIT)
#define CHECK_MAINTAIN_BIT(LastStatus) \
        (LastStatus & MAINTAIN_BIT)

uchar Thrust;           /*
                            Thrust
                         
                            +------------------------------------------------->
                            0
                        */
uchar ThrustBias[4];

                        
int Turn;               /* 
                            direction turn value, Left(Negative) Right(Positive)
                                 
                            (LEFT)              (NEUTRAL)                 (RIGHT)
                            ------------------------+-------------------------> (Turn)
                                                    0
                            */
int Left[2];
int Right[2];

#define THRUST_ADDRESS          0
#define THRUST_BIAS_ADDRESS     THRUST_ADDRESS + sizeof(Thrust)
#define LEFT_ADDRESS            THRUST_BIAS_ADDRESS + sizeof(ThrustBias)
#define RIGHT_ADDRESS           LEFT_ADDRESS + sizeof(Left)

//I2C Devices
sbit SCL = P3^0;
sbit SDA = P3^1;

#define AT24C64_ADDRESS 0x50

bit sda_read()
{return SDA;}

void sda_control(uchar Value)
{SDA = Value;}

void scl_control(uchar Value)
{SCL = Value;}

//Detectors
sbit DETECTOR_LEFT_1 = P0^0;
sbit DETECTOR_LEFT_2 = P0^1;
sbit DETECTOR_RIGHT_1 = P0^2;
sbit DETECTOR_RIGHT_2 = P0^3;
#define DETECTOR_MASK 0x0F

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

void timer1_process() interrupt 3
{
    //Tracking logic
    switch(P0 & DETECTOR_MASK)
    {
    case 0x0C:  //Left right-angle
        Turn = Left[1];
        SET_STATUS(LastStatus, LEFT_2);
        SET_MAINTAIN_BIT(LastStatus);
        break;
    case 0x08:  //LEFT_2
        Turn = Left[1];
        SET_STATUS(LastStatus, LEFT_2);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x04:  //LEFT_1
        Turn = Left[0];
        SET_STATUS(LastStatus, LEFT_1);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x02:  //RIGHT_1
        Turn = Right[0];
        SET_STATUS(LastStatus, RIGHT_1);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x03:  //right right-angle
        Turn = Right[1];
        SET_STATUS(LastStatus, RIGHT_2);
        SET_MAINTAIN_BIT(LastStatus);
        break;
    case 0x01:  //RIGHT_2
        Turn = Right[1];
        SET_STATUS(LastStatus, RIGHT_2);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x0F:  //cross line.maintain.
        break;
    case 0x00:  //no detection.refer to last status.
        if(CHECK_MAINTAIN_BIT(LastStatus))
            break;
        SET_STATUS(LastStatus, NEUTRAL);
        break;
    }
}

void init()
{
    //Load configure
    if(SUCCEED == AT24C64PageRead(AT24C64_ADDRESS, THRUST_ADDRESS, &Thrust, sizeof(Thrust)
                    , scl_control, sda_control, sda_read))
        LED7 = 0;
    else
        Thrust = THRUST_BASE;
    
    if(SUCCEED == AT24C64PageRead(AT24C64_ADDRESS, THRUST_BIAS_ADDRESS, ThrustBias, sizeof(ThrustBias)
                    , scl_control, sda_control, sda_read))
        LED6 = 0;
    else
    {
        ThrustBias[0] = THRUST_BIAS_1;
        ThrustBias[1] = THRUST_BIAS_2;
        ThrustBias[2] = THRUST_BIAS_3;
        ThrustBias[3] = THRUST_BIAS_4;
    }
    if(SUCCEED == AT24C64PageRead(AT24C64_ADDRESS, LEFT_ADDRESS, Left, sizeof(Left)
                    , scl_control, sda_control, sda_read))
        LED5 = 0;
    else
    {
        Left[0] = LEFT_ONE;
        Left[1] = LEFT_TWO;
    }
    if(SUCCEED == AT24C64PageRead(AT24C64_ADDRESS, RIGHT_ADDRESS, Right, sizeof(Right)
                    , scl_control, sda_control, sda_read))
        LED4 = 0;
    else
    {
        Right[0] = RIGHT_ONE;
        Right[1] = RIGHT_TWO;
    }
    
    LastStatus = NEUTRAL;
                    
    //PWM Initialize
    PWM0 = 0;
    PWM1 = 0;
    PWM2 = 0;
    PWM3 = 0;
    
    Turn = 0;

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
        PWMDutyRatio[0] = (255 - Thrust) + Turn + ThrustBias[0];
        PWMDutyRatio[1] = (255 - Thrust) + Turn + ThrustBias[1];
        PWMDutyRatio[2] = (255 - Thrust) - Turn + ThrustBias[2];
        PWMDutyRatio[3] = (255 - Thrust) - Turn + ThrustBias[3];
        
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