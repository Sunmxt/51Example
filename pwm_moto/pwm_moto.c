#include <reg52.h>
#include <intrins.h>

#define TINY_TYPES_PREFER_XDATA
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

uchar PWMCounter[4];
uchar PWMDutyRatio[4];

//Buttons
sbit BUTTON1 = P3^0;
sbit BUTTON2 = P3^1;
sbit BUTTON3 = P3^2;
sbit BUTTON4 = P3^3;
sbit BUTTON5 = P3^4;
sbit BUTTON6 = P3^5;
sbit BUTTON7 = P3^6;
sbit BUTTON8 = P3^7;
sbit BUTTON9 = P2^7;
sbit BUTTON10 = P2^6;

xdata uchar ButtonCounter[10];
xdata uchar Button[5];


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
        
xdata struct {
    uchar Thrust;           /*
                                Thrust
                            
                                +------------------------------------------------->
                                0
                            */
    char ThrustBias[4];

                        
    int Turn;               /* 
                                direction turn value, Left(Negative) Right(Positive)
                                 
                                (LEFT)              (NEUTRAL)                 (RIGHT)
                                ------------------------+-------------------------> (Turn)
                                                        0
                                */
    int Left[2];
    int Right[2];
}Params;

xdata uint ConfigureSave;
bit ConfigureChanged;

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


void button_process()
{
    bit modified;
    
    modified = 0;
    
    SCAN_BUTTON(BUTTON9, ButtonCounter[8], 5, Button[4], 0, 50, 200);
    IF_BUTTON_MESSAGE(Button[4], 0, BUTTON_CLICKED)
    {
        Params.Thrust += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[4], 0, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[4], 0, BUTTON_LONG_DOWN)
    {
        Params.Thrust -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[4], 0, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON10, ButtonCounter[9], 5, Button[4], 4, 50, 200);
    IF_BUTTON_MESSAGE(Button[4], 4, BUTTON_LONG_DOWN)
    {
        Params.Thrust = THRUST_BASE;
        Params.ThrustBias[0] = THRUST_BIAS_1;
        Params.ThrustBias[1] = THRUST_BIAS_2;
        Params.ThrustBias[2] = THRUST_BIAS_3;
        Params.ThrustBias[3] = THRUST_BIAS_4;
        Params.Left[0] = LEFT_ONE;
        Params.Left[1] = LEFT_TWO;
        Params.Right[0] = RIGHT_ONE;
        Params.Right[1] = RIGHT_TWO;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[4], 4, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON1, ButtonCounter[0], 5, Button[0], 0, 50, 200);
    IF_BUTTON_MESSAGE(Button[0], 0, BUTTON_CLICKED)
    {
        Params.Left[1] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[0], 0, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[0], 0, BUTTON_LONG_DOWN)
    {
        Params.Left[1] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[0], 0, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON2, ButtonCounter[1], 5, Button[0], 4, 50, 200);
    IF_BUTTON_MESSAGE(Button[0], 4, BUTTON_CLICKED)
    {
        Params.Left[0] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[0], 4, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[0], 4, BUTTON_LONG_DOWN)
    {
        Params.Left[0] -= 8;
        ConfigureChanged = 1;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[0], 4, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON3, ButtonCounter[2], 5, Button[1], 0, 50, 200);
    IF_BUTTON_MESSAGE(Button[1], 0, BUTTON_CLICKED)
    {
        Params.Right[1] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[1], 0, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[1], 0, BUTTON_LONG_DOWN)
    {
        Params.Right[1] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[1], 0, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON4, ButtonCounter[3], 5, Button[1], 4, 50, 200);
    IF_BUTTON_MESSAGE(Button[1], 4, BUTTON_CLICKED)
    {
        Params.Right[0] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[1], 4, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[1], 4, BUTTON_LONG_DOWN)
    {
        Params.Right[0] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[1], 4, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON5, ButtonCounter[4], 5, Button[2], 0, 50, 200);
    IF_BUTTON_MESSAGE(Button[2], 0, BUTTON_CLICKED)
    {
        Params.ThrustBias[0] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[2], 0, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[2], 0, BUTTON_LONG_DOWN)
    {
        Params.ThrustBias[0] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[2], 0, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON6, ButtonCounter[5], 5, Button[2], 4, 50, 200);
    IF_BUTTON_MESSAGE(Button[2], 4, BUTTON_CLICKED)
    {
        Params.ThrustBias[1] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[2], 4, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[2], 4, BUTTON_LONG_DOWN)
    {
        Params.ThrustBias[1] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[2], 4, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON7, ButtonCounter[6], 5, Button[3], 0, 50, 200);
    IF_BUTTON_MESSAGE(Button[3], 0, BUTTON_CLICKED)
    {
        Params.ThrustBias[2] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[3], 0, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[3], 0, BUTTON_LONG_DOWN)
    {
        Params.ThrustBias[2] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[3], 0, BUTTON_LONG_DOWN);
    }
    
    SCAN_BUTTON(BUTTON8, ButtonCounter[7], 5, Button[3], 4, 50, 200);
    IF_BUTTON_MESSAGE(Button[3], 4, BUTTON_CLICKED)
    {
        Params.ThrustBias[3] += 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[3], 4, BUTTON_CLICKED);
    }
    IF_BUTTON_MESSAGE(Button[3], 4, BUTTON_LONG_DOWN)
    {
        Params.ThrustBias[3] -= 8;
        modified = 1;
        CLEAR_BUTTON_MESSAGE(Button[3], 4, BUTTON_LONG_DOWN);
    }
    
    if(modified)
    {
        ConfigureChanged = 1;
        ConfigureSave = 0x0100;
    }
}

#define EEPROM_SECTOR_START_ADDRESS     0x2000
sfr IAP_DATA = 0xE2;
sfr IAP_ADDRH = 0xE3;
sfr IAP_ADDRL = 0xE4;
sfr IAP_CMD = 0xE5;
sfr IAP_TRIG = 0xE6;
sfr IAP_CONTR = 0xE7;

#define IAP_ENABLE      0x81
#define IAP_IDLE        0
#define IAP_READ        1
#define IAP_WRITE       2
#define IAP_ERASE       3

void STC89C52IAPRead(uint Address, void* Buffer, uint Size)
{
    bit ea_tmp;
    register uint i;
    IAP_CONTR = IAP_ENABLE;   //Enable IAP
    
    IAP_CMD = IAP_READ;
    ea_tmp = EA;
    EA = 0;
    for(i = 0 ; i < Size ; i++, Address++)
    {
        IAP_ADDRH = Address >> 8;
        IAP_ADDRL = Address & 0xFF;
        
        IAP_TRIG = 0x46;    //Execute
        IAP_TRIG = 0xB9;
        _nop_();
        
        ((uchar*)Buffer)[i] = IAP_DATA;
    }
    EA = ea_tmp;
    
    
    
    IAP_CONTR = 0;      //Set IAP IDLE State
    IAP_CMD = 0;
    IAP_ADDRH = 0x20;
    IAP_ADDRL = 0;
}

void STC89C52IAPWrite(uint Address, void* Data, uint Size)
{
    bit ea_tmp;
    register uchar i;
    IAP_CONTR = IAP_ENABLE;   //Enable IAP
    

    IAP_CMD = IAP_WRITE;
    
    
    ea_tmp = EA;
    EA = 0;
    for(i = 0 ; i < Size ; i++, Address++)
    {
        IAP_DATA = ((uchar*)Data)[i];
        IAP_ADDRH = Address >> 8;
        IAP_ADDRL = Address & 0xFF;
        
        IAP_TRIG = 0x46;    //Execute
        IAP_TRIG = 0xb9;
        _nop_();
    }
    EA = ea_tmp;
    
    IAP_CONTR = 0;      //Set IAP IDLE State
    IAP_CMD = 0;
    IAP_ADDRH = 0x20;
    IAP_ADDRL = 0;
}

void STC89C52IAPErase(uint Address)
{
    bit ea_tmp;
    IAP_CONTR = IAP_ENABLE;   //Enable IAP
    IAP_ADDRH = Address >> 8;
    IAP_ADDRL = Address & 0xFF;
    IAP_CMD = IAP_ERASE;
    
    ea_tmp = EA;
    EA = 0;
    IAP_TRIG = 0x46;    //Execute
    IAP_TRIG = 0xb9;
    _nop_();
    EA = ea_tmp;
    
    IAP_CONTR = 0;      //Set IAP IDLE State
    IAP_CMD = 0;
    IAP_ADDRH = 0x20;
    IAP_ADDRL = 0;
}

void load_configure()
{
    STC89C52IAPRead(EEPROM_SECTOR_START_ADDRESS, &Params, sizeof(Params));
}

void save_configure()
{
    STC89C52IAPErase(EEPROM_SECTOR_START_ADDRESS);
    STC89C52IAPWrite(EEPROM_SECTOR_START_ADDRESS, &Params, sizeof(Params));
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
        Params.Turn = Params.Left[1];
        SET_STATUS(LastStatus, LEFT_2);
        SET_MAINTAIN_BIT(LastStatus);
        break;
    case 0x08:  //LEFT_2
        Params.Turn = Params.Left[1];
        SET_STATUS(LastStatus, LEFT_2);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x04:  //LEFT_1
        Params.Turn = Params.Left[0];
        SET_STATUS(LastStatus, LEFT_1);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x02:  //RIGHT_1
        Params.Turn = Params.Right[0];
        SET_STATUS(LastStatus, RIGHT_1);
        CLEAR_MAINTAIN_BIT(LastStatus);
        break;
    case 0x03:  //right right-angle
        Params.Turn = Params.Right[1];
        SET_STATUS(LastStatus, RIGHT_2);
        SET_MAINTAIN_BIT(LastStatus);
        break;
    case 0x01:  //RIGHT_2
        Params.Turn = Params.Right[1];
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
    
    PWMDutyRatio[0] = (255 - Params.Thrust - Params.ThrustBias[0]) + Params.Turn;
    PWMDutyRatio[1] = (255 - Params.Thrust - Params.ThrustBias[1]) + Params.Turn;
    PWMDutyRatio[2] = (255 - Params.Thrust - Params.ThrustBias[2]) - Params.Turn;
    PWMDutyRatio[3] = (255 - Params.Thrust - Params.ThrustBias[3]) - Params.Turn;
    
    if(ConfigureChanged)
    {
        ConfigureSave--;
        if(!ConfigureSave)
        {
            ConfigureChanged = 0;
            save_configure();
        }
    }
}

void init()
{
    load_configure();
    
    P0 = 0xFF;  //P0 high inpedance.Whether high or low is depended on input.
    
    LastStatus = NEUTRAL;
    Button[0] = 0;
    Button[1] = 0;
    Button[2] = 0;
    Button[3] = 0;
    Button[4] = 0;

    ButtonCounter[0] = 0;
    ButtonCounter[1] = 0;
    ButtonCounter[2] = 0;
    ButtonCounter[3] = 0;
    ButtonCounter[4] = 0;
    ButtonCounter[5] = 0;
    ButtonCounter[6] = 0;
    ButtonCounter[7] = 0;
    ButtonCounter[8] = 0;
    ButtonCounter[9] = 0;
    
    ConfigureChanged = 0;
    ConfigureSave = 0;

                    
    //PWM Initialize
    PWM0 = 0;
    PWM1 = 0;
    PWM2 = 0;
    PWM3 = 0;

    PWMCounter[0] = 0;
    PWMCounter[1] = 0;
    PWMCounter[2] = 0;
    PWMCounter[3] = 0;
    
    PWMDutyRatio[0] = (255 - Params.Thrust - Params.ThrustBias[0]) + Params.Turn;
    PWMDutyRatio[1] = (255 - Params.Thrust - Params.ThrustBias[1]) + Params.Turn;
    PWMDutyRatio[2] = (255 - Params.Thrust - Params.ThrustBias[2]) - Params.Turn;
    PWMDutyRatio[3] = (255 - Params.Thrust - Params.ThrustBias[3]) - Params.Turn;
    
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