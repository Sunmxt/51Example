#include <reg52.h>
#include <intrins.h>
#include "pcf8591.h"

#define HIGH_VOLTAGE_REFERENCE 5
#define LOW_VOLTAGE_REGERENCE 0

#define PCF8591_ADDRESS 0x48

typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned int uint16;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 
    0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};

bit update;
uchar DigitalDot[4];
uchar Digital[4];
uchar Counter;

sbit SCL = P1^0;
sbit SDA = P1^1;
sbit LED7 = P1^7;
sbit LED6 = P1^6;
sbit LED5 = P1^5;

void scl_write(uchar value)
{SCL = value;}

void sda_write(uchar value)
{SDA = value;}

bit sda_read()
{return SDA;}

void init()
{
    Counter = 0;
    
    Digital[0] = 0;
    Digital[1] = 0;
    Digital[2] = 0;
    Digital[3] = 0;
    DigitalDot[0] = 0;
    DigitalDot[1] = 0;
    DigitalDot[2] = 0;
    DigitalDot[3] = 0;
    
    TMOD = 0x10;
    TH0 = 0xFC;
    TL0 = 0x67;
    TF0 = 0;
    TR0 = 1;
    
    EA = 1;
    ET0 = 1;
    
    PCF8591OpenWrite(PCF8591_ADDRESS, scl_write, sda_write, sda_read);
    PCF8591Send(PCF8591_MAKE_CONTROL_BYTE(0, 0, 0, 0), scl_write, sda_write, sda_read);
    PCF8591Close(scl_write, sda_write);
}

void timer0_interrupt() interrupt 1
{   
    TH0 = 0xFC;
    TL0 = 0x67;
    
    //Digital Display
    P2 = ~((uchar)1 << (3 - ((Counter >> 2) & 3)));
    P0 = DisplayCode[Digital[(Counter >> 2) & 3]] ^ (DigitalDot[(Counter >> 2) & 3] << 7);
    
    Counter ++;
    
    if(Counter >= 40)
    {
        update = 1;
        Counter = 0;
    }
}
    
void main()
{
     union {
        float volt;
        long li;
    }u2;
    
    union {
        uchar ad_data;
        char power;
    }u1;
    
    init();
    
    while(1)
    {
        _nop_();
        if(update)
        {
            update = 0;
            if(PCF8591OpenRead(PCF8591_ADDRESS, scl_write, sda_write, sda_read))
                LED7 = 0;
            
            u1.ad_data = PCF8591Receive(scl_write ,sda_write, sda_read);
            PCF8591Close(scl_write, sda_write);
        
            u2.volt = LOW_VOLTAGE_REGERENCE 
                    + ((float)HIGH_VOLTAGE_REFERENCE - LOW_VOLTAGE_REGERENCE) / 255 * u1.ad_data;
        
            u1.power = 0;
            while(u2.volt - (long)u2.volt)
            {
                u2.volt *= 10;
                u1.power++;
            }
        
            u2.li = u2.volt;
            while(u2.li > 9999)
            {
                u2.li /= 10;
                u1.power--;
            }
        
            DigitalDot[0] = 0;
            DigitalDot[1] = 0;
            DigitalDot[2] = 0;
            DigitalDot[3] = 0;
        
            if(u1.power <= 3) 
                DigitalDot[u1.power] = 1;
        
            Digital[0] = u2.li % 10;
            u2.li /= 10;
            Digital[1] = u2.li % 10;
            u2.li /= 10;
            Digital[2] = u2.li % 10;
            u2.li /= 10;
            Digital[3] = u2.li % 10;
        }
    }
}

