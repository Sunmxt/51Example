#include <reg52.h>

typedef unsigned char uchar;



typedef struct Timer_Context 
{
	unsigned char mode;
		#define TIMER_MODE					0
		#define TIMER_SETTING_MODE	1
		#define DATA_VIEW_MODE			2
	
	unsigned int counter_ms;
	unsigned int hex_loop;
	unsigned char digital[4];
	unsigned char loop_counter;
	
	unsigned int flags;
		#define	RUNNING								0x0001
		#define BUTTON_1_DETECTED			0x0002
		#define BUTTON_1_DOWN					0x0004
		#define BUTTON_1_LONG_PUSH		0x0008
		#define BUTTON_1_STATUS_MASK	0x000E

	unsigned int button_1_counter;
	
	void (*on_button1_click)(struct Timer_Context *context);
	void (*on_button1_long_push)(struct Timer_Context *context);
}TimerContext;

sbit Button1 = P3^5;
sbit Button2 = P3^4;
sbit Button3 = P3^3;
sbit Button4 = P3^2;

sbit HexDot = P0^7;

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};

void timer_mode_on_button1_click(TimerContext *context)
{
	context -> flags ^= RUNNING;
}

void timer_mode_on_button1_long_push(TimerContext *context)
{
	return;
}

void led_hex_display(TimerContext *context)
{
	if(!(context -> loop_counter & 3))
	{
		P2 = ~(0x08 >> context -> hex_loop);
		P0 = DisplayCode[context -> digital[context -> hex_loop]];
		if(context -> hex_loop == 1)
			HexDot = 0;
		else
			HexDot = 1;
		context -> hex_loop ++;
		if(context -> hex_loop > 3)
			context -> hex_loop = 0;
	}
}

void time_count(TimerContext *context)
{
	unsigned int i;
	
	if(context -> flags & RUNNING)
	{
		context -> counter_ms++;
		if(context -> counter_ms == 100)
		{
			i = 0;
			do
			{
				if(context -> digital[i] < 9)
				{
					context -> digital[i]++;
					break;
				}
				else
					context -> digital[i] = 0;
				i++;
			}while(i < 4);
			
			context -> counter_ms = 0;
		}
	}
}



void check_button(TimerContext* context)
{
	if(!Button1)
	{
		if( !(context -> flags & BUTTON_1_LONG_PUSH))
		{
			context -> button_1_counter++;
			context -> flags |= BUTTON_1_DETECTED;
			if(context -> flags & BUTTON_1_DOWN)
			{
				if(context -> button_1_counter >= 500)
					context -> flags |= BUTTON_1_LONG_PUSH;
			}
			else
			{
				if(context -> button_1_counter >= 10)
					context -> flags |= BUTTON_1_DOWN;
			}
		}
	}
	else
	{
		if(context -> flags & BUTTON_1_DETECTED)
		{
			if(context -> flags & BUTTON_1_DOWN)
			{
				if(context -> flags & BUTTON_1_LONG_PUSH)
					context -> on_button1_long_push(context);
				else
					context -> on_button1_click(context);
			}
			context -> flags &= ~BUTTON_1_STATUS_MASK;
			context -> button_1_counter = 0;
		}
	}
	
}

void main()
{
	TimerContext context;
	unsigned int i,time_error_fix;
	
	//init
	time_error_fix = 0;
	context.mode = TIMER_MODE;
	context.digital[0] = 0;
	context.digital[1] = 0;
	context.digital[2] = 0;
	context.digital[3] = 0;
	context.counter_ms = 0;
	context.hex_loop = 0;
	context.flags = RUNNING;
	context.button_1_counter = 0;
	context.loop_counter = 0;
	context.on_button1_click = timer_mode_on_button1_click;
	context.on_button1_long_push = timer_mode_on_button1_long_push;

	//initalize inner timer
	
	TMOD = 0x01; //Timer 0 mode 1
	TH0 = 0xFC;
	TL0 = 0x67;
	TF0 = 0;
	TR0 = 1;
	
	while(1) //main_loop
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
			
			context.loop_counter ++;
			check_button(&context);
			led_hex_display(&context);
			time_count(&context);
		}
	}
}