#include <reg52.h>

typedef unsigned char uchar;

typedef struct Timer_Context 
{
	unsigned int mode;
		#define TIMER_MODE									((int)0)
		#define TIMER_SETTING_MODE					((int)1)
		#define DATA_VIEW_MODE							((int)2)
	
	unsigned int counter_ms;
	unsigned int hex_loop;
	unsigned int digital[4];
	unsigned int loop_counter;
	
	unsigned int digital_index;
	unsigned int record_index;
	
	unsigned int flags;
		#define	RUNNING								0x0001
		#define BUTTON_1_DETECTED			0x0002
		#define BUTTON_1_DOWN					0x0004
		#define BUTTON_1_LONG_PUSH		0x0008
		#define BUTTON_1_STATUS_MASK	0x000E

		#define BUTTON_2_DETECTED			0x0010
		#define BUTTON_2_DOWN					0x0020
		#define BUTTON_2_LONG_PUSH		0x0040
		#define BUTTON_2_STATUS_MASK	0x0070
		
		#define BUTTON_3_DETECTED			0x0080
		#define BUTTON_3_DOWN					0x0100
		#define BUTTON_3_LONG_PUSH		0x0200
		#define BUTTON_3_STATUS_MASK	0x0380
		
		#define BUTTON_4_DETECTED			0x0400
		#define BUTTON_4_DOWN					0x0800
		#define BUTTON_4_LONG_PUSH		0x1000
		#define BUTTON_4_STATUS_MASK	0x1C00
		#define REVERSE								0x2000
		#define HEX_FLASH_HIDE				0x4000
		
	unsigned int flash_times[4];
		#define FLASH_HEX_INFINITE		0xFF
		
	unsigned int record[8][4];
}TimerContext;

typedef struct button_io_metadata 
{
	unsigned char io_mask;
	unsigned int long_push_mask;
	unsigned int down_mask;
	unsigned int detected_mask;
	unsigned int counter;
	void (*on_button_click)(TimerContext *context);
	void (*on_button_long_push)(TimerContext *context);
}ButtonMeta;

//basic metadata

/*
sbit Button1 = P3^5;
sbit Button2 = P3^4;
sbit Button3 = P3^3;
sbit Button4 = P3^2;
*/

sbit HexDot = P0^7;


void on_button1_click(TimerContext *context);
void on_button1_long_push(TimerContext *context);
void on_button2_click(TimerContext *context);
void on_button2_long_push(TimerContext *context);
void on_button3_click(TimerContext *context);
void on_button3_long_push(TimerContext *context);
void on_button4_click(TimerContext *context);
void on_button4_long_push(TimerContext *context);


ButtonMeta idata ButtonInfo[] = {
	{0x20, BUTTON_1_LONG_PUSH, BUTTON_1_DOWN, BUTTON_1_DETECTED ,0 ,on_button1_click ,on_button1_long_push},
	{0x10, BUTTON_2_LONG_PUSH, BUTTON_2_DOWN, BUTTON_2_DETECTED ,0 ,on_button2_click ,on_button2_long_push},
	{0x08, BUTTON_3_LONG_PUSH, BUTTON_3_DOWN, BUTTON_3_DETECTED ,0 ,on_button3_click ,on_button3_long_push},
	{0x04, BUTTON_4_LONG_PUSH, BUTTON_4_DOWN, BUTTON_4_DETECTED ,0 ,on_button4_click ,on_button4_long_push}
};

uchar code DisplayCode[] =
	{0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xCA, 0xE1, 0x86, 0x8E};
//policy
  
void recorder_switch(TimerContext *context ,int backword)
{
  if(backword)
  {
    if(context -> record_index)
      context -> record_index--;
    else
      context -> record_index = 7;
  }
  else
  {
    context -> record_index++;
    if(context -> record_index >= 8)
      context -> record_index = 0;
  }
  P1 = ~(1 << context -> record_index);

}

void on_button1_click(TimerContext *context)
{
	if(context -> mode != TIMER_MODE)
		context -> mode = TIMER_MODE;
	else
		context -> flags ^= RUNNING;
}

void on_button1_long_push(TimerContext *context)
{
	if(context -> mode == TIMER_MODE)
	{
		context -> mode = TIMER_SETTING_MODE;
    context -> flags &= ~RUNNING;
		context -> digital_index = 0;
  
    context -> flash_times[1] = 0;
    context -> flash_times[2] = 0;
    context -> flash_times[3] = 0;
    context -> flash_times[0] = 0xFF;
    context -> flags |= HEX_FLASH_HIDE;
		context -> loop_counter = 0;
	}
}

void on_button2_click(TimerContext *context)
{
	int i;
	
	switch(context -> mode)
	{
	case TIMER_SETTING_MODE:
	case TIMER_MODE:
		context -> digital[0] = 0;
		context -> digital[1] = 0;
		context -> digital[2] = 0;
		context -> digital[3] = 0;
		break;
	case DATA_VIEW_MODE:
		for(i = 0 ; i < 4 ; i++)
			context -> record[context -> record_index][i] = 0;
		break;
	}
}

void on_button2_long_push(TimerContext *context)
{
	int i, j;
	switch(context -> mode)
	{
	case TIMER_MODE:
		context -> flags ^= REVERSE;
		context -> flash_times[0] = 1;
		context -> flash_times[1] = 1;
		context -> flash_times[2] = 1;
		context -> flash_times[3] = 1;
		context -> flags |= HEX_FLASH_HIDE;
		context -> loop_counter = 0;
		break;
	
	case DATA_VIEW_MODE:
		for(i = 0 ; i < 8 ;i++)
			for(j = 0 ; j < 4 ; j++)
				context -> record[i][j] = 0;
     break;
  default:
    break;
	}
}

void on_button3_click(TimerContext *context)
{
	int i;
	switch(context -> mode)
	{
	case TIMER_MODE:
		for(i = 0 ; i < 4 ; i++)
			context -> record[context -> record_index][i] = context -> digital[i];
    recorder_switch(context, 0);
		break;
	case TIMER_SETTING_MODE:
		context -> digital[context -> digital_index]++;
		if(context -> digital[context -> digital_index] > 9)
			context -> digital[context -> digital_index] = 0;
		break;
	case DATA_VIEW_MODE:
    recorder_switch(context, 1);
		break;
	}
}

void on_button3_long_push(TimerContext *context)
{
  int i;
  
	switch(context -> mode)
	{
  case TIMER_MODE:
    context -> mode = DATA_VIEW_MODE;
    break;
  case TIMER_SETTING_MODE:
    context -> digital_index++;
    if(context -> digital_index >= 4)
      context -> digital_index = 0;
    for(i = 0 ; i < 0 ; i++)
      context -> flash_times[i] = 0;
      context -> flash_times[context -> digital_index] = 0xFF;
    break;
	}
}

void on_button4_click(TimerContext *context)
{
  switch(context -> mode)
  {
  case TIMER_MODE:
    recorder_switch(context, 0);
    break;
  case TIMER_SETTING_MODE:
		if(context -> digital[context -> digital_index])
			context -> digital[context -> digital_index]--;
    else
      context -> digital[context -> digital_index] = 9;
		break;
  case DATA_VIEW_MODE:
    //view next recorder
    break;
  }
}

void on_button4_long_push(TimerContext *context)
{
  int i;
  if(context -> mode == TIMER_SETTING_MODE)
  {
    if(context -> digital_index)
      context -> digital[context -> digital_index]--;
    else
			context -> digital[context -> digital_index] = 3;
    for(i = 0 ; i < 0 ; i++)
      context -> flash_times[i] = 0;
    context -> flash_times[context -> digital_index] = 0xFF;
  }
}

//code
void led_hex_display(TimerContext *context)
{	
	int i;
	if(!(context -> loop_counter & 3))
	{
		if(context -> loop_counter >= 500)
		{
			context -> flags ^= HEX_FLASH_HIDE;
			if(!(context -> flags & HEX_FLASH_HIDE))
				for(i = 0 ; i < 4 ; i++)
					if(context -> flash_times[i] > 0 && context -> flash_times[i] < 0xFF)
						context -> flash_times[i]--;
			context -> loop_counter = 0;
		}
		if(context -> flags & HEX_FLASH_HIDE && context -> flash_times[context -> hex_loop])
			P2 = 0x0F;
		else
		{
			P2 = ~(0x08 >> context -> hex_loop);
			P0 = DisplayCode[context -> digital[context -> hex_loop]];
			if(context -> hex_loop == 1)
				HexDot = 0;
			else
				HexDot = 1;
		}
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
				if(context -> flags & REVERSE)
				{
					if(context -> digital[i])
					{
						context -> digital[i]--;
						break;
					}
					else
						context -> digital[i] = 9;
				}
				else
				{
					if(context -> digital[i] < 9)
					{
						context -> digital[i]++;
						break;
					}
					else
						context -> digital[i] = 0;
				}
				i++;
			}while(i < 4);
			
			context -> counter_ms = 0;
		}
	}
}

void check_button_io_p3(TimerContext* Context, ButtonMeta *IOInfo)
{
	if(!(P3 & IOInfo -> io_mask))
	{
		if( !(Context -> flags & IOInfo -> long_push_mask))
		{
			IOInfo -> counter ++;
			Context -> flags |= IOInfo -> detected_mask;
			if(Context -> flags & IOInfo -> down_mask) {
				if(IOInfo -> counter >= 350)
				{
					Context -> flags |= IOInfo -> long_push_mask;
					if(IOInfo -> on_button_long_push)
           IOInfo -> on_button_long_push(Context);
				}
			}
			else {
				if(IOInfo -> counter >= 10)
					Context -> flags |= IOInfo -> down_mask;
			}
		}
	}
	else
	{
		if(Context -> flags & IOInfo -> detected_mask) {
			if(Context -> flags & IOInfo -> down_mask) {
				if(!(Context -> flags & IOInfo -> long_push_mask))
					if(IOInfo -> on_button_click)
            IOInfo -> on_button_click(Context);
			}
			Context -> flags &= ~(IOInfo -> long_push_mask | IOInfo -> down_mask | IOInfo -> detected_mask);
			IOInfo -> counter = 0;
		}			
	}
}

void main()
{
	TimerContext context;
	unsigned int i,j,time_error_fix;
  
	//init
	time_error_fix = 0;
	context.mode = TIMER_MODE;
	context.digital[0] = 0;
	context.digital[1] = 0;
	context.digital[2] = 0;
	context.digital[3] = 0;
  //context.flash_times[0] = 0xFF;
	//context.flash_times[1] = 0xFF;
  //context.flash_times[2] = 0xFF;
  //context.flash_times[3] = 0xFF;
  context.counter_ms = 0;
	context.hex_loop = 0;
	context.flags = RUNNING;
	context.loop_counter = 0;
	context.record_index = 0;
  P1 = ~(1 << context.record_index);
	for(i = 0 ; i < 8 ;i++)
		for(j = 0 ; j < 4 ; j++)
			context.record[i][j] = 0;
	
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
			led_hex_display(&context);
			check_button_io_p3(&context, &ButtonInfo[0]);
			check_button_io_p3(&context, &ButtonInfo[1]);
			check_button_io_p3(&context, &ButtonInfo[2]);
			check_button_io_p3(&context, &ButtonInfo[3]);
			time_count(&context);
		}
	}
}