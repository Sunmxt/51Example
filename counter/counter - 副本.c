#include <reg52.h>

typedef unsigned char uchar;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};

void show_hex(unsigned char IO, unsigned char Value)
{
	P2 = IO;
	P0 = DisplayCode[Value];
}

void main()
{
	unsigned int i,hex_loop, time_error_fix;
	unsigned int counter_ms;
	unsigned char c[4];
	
	c[0] = 0;
	c[1] = 0;
	c[2] = 0;
	c[3] = 0;
	hex_loop = 0;
	time_error_fix = 0;
	
	//init
	TMOD = 0x01; //Timer 0 mode 1
	TH0 = 0xFC;
	TL0 = 0x67;
	TF0 = 0;
	TR0 = 1;
	
	while(1)
	{
		if(TF0 == 1)
		{
			TF0 = 0;									
			
			i = 0x66 + 9; //fix time error caused by instructions
			//1ms shoule be 921.6 Tick in 11.0592MHZ. 
			//Each 10ms should be less 4 ticks to fix the error.
			time_error_fix++;
			if(time_error_fix == 10)
			{
				time_error_fix = 0;
				i += 4;
			}
			i += TL0;
			TL0 = i & 0xFF;
			TH0 += 0xFC + (i >> 8);
			
			//scan digtial display
			if(!(counter_ms & 3))
			{
				show_hex(~(0x08 >> hex_loop), c[hex_loop]);
				hex_loop ++;
				if(hex_loop > 3)
				hex_loop = 0;
			}
			
			
			//time counter
			counter_ms++;
			if(counter_ms == 1000)
			{
				i = 0;
				do
				{
					if(c[i] < 9)
					{
						c[i]++;
						break;
					}
					else
						c[i] = 0;
					i++;
				}while(i < 4);

				counter_ms = 0;
			}		
		}
	}
}