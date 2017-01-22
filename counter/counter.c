#include <reg52.h>

typedef unsigned char uchar;

typedef struct Timer_Context 
{
	unsigned char mode;
		#define TIMER_MODE									0
		#define TIMER_SETTING_MODE					1
		#define DATA_VIEW_MODE							2
	
	unsigned int counter_ms;
	unsigned int hex_loop;
	unsigned char digital[4];
	unsigned int loop_counter;
	
	unsigned int digital_index;
	unsigned int record_index;
  unsigned int button_counter[4];
	
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
		
	unsigned char flash_times[4];
		#define FLASH_HEX_INFINITE		0xFF
		
	unsigned char record[4][4];
}TimerContext;

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
      context -> record_index = 3;
  }
  else
  {
    context -> record_index++;
    if(context -> record_index >= 4)
      context -> record_index = 0;
  }
  
  P1 = ~(1 << context -> record_index);
}

void on_button1_click(TimerContext *context)
{
  int i;
  unsigned char tmp;
  
  switch(context -> mode)
  {
  case TIMER_MODE:
    context -> flags ^= RUNNING;
    return;
  
  case TIMER_SETTING_MODE:
    context -> flash_times[0] = 0;
    context -> flash_times[1] = 0;
    context -> flash_times[2] = 0;
    context -> flash_times[3] = 0;
    context -> flags &= ~HEX_FLASH_HIDE; 
    context -> loop_counter = 0;
    break;
  
  case DATA_VIEW_MODE:
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    break;
  }
  
  context -> mode = TIMER_MODE;
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
  if(context -> flags & REVERSE && context -> mode != DATA_VIEW_MODE)
  {
    context -> digital[0] = 9;
    context -> digital[1] = 9;
    context -> digital[2] = 9;
    context -> digital[3] = 9;
  }
  else
  {
    context -> digital[0] = 0;
    context -> digital[1] = 0;
    context -> digital[2] = 0;
    context -> digital[3] = 0;
  }
}

void on_button2_long_push(TimerContext *context)
{
	int i, j;
  unsigned char tmp;
  
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
    on_button2_click(context);
		break;
	
	case DATA_VIEW_MODE:
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    for(i = 0 ; i < 4 ;i++)
			for(j = 0 ; j < 4 ; j++)
				context -> record[i][j] = 0;
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    break;
  }
}

void on_button3_click(TimerContext *context)
{
	int i;
  unsigned char tmp;
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
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    recorder_switch(context, 1);
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    break;
	}
}

void on_button3_long_push(TimerContext *context)
{
  int i;
  unsigned char tmp;
  
	switch(context -> mode)
	{
  case TIMER_MODE:
    context -> flags &= ~RUNNING;
    context -> mode = DATA_VIEW_MODE;
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    break;
  case TIMER_SETTING_MODE:
    context -> digital_index++;
    if(context -> digital_index >= 4)
      context -> digital_index = 0;
    for(i = 0 ; i < 4 ; i++)
      context -> flash_times[i] = 0;
      context -> flash_times[context -> digital_index] = 0xFF;
    break;
	}
}

void on_button4_click(TimerContext *context)
{
  unsigned char tmp;
  int i;
  
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
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    recorder_switch(context, 0);
    for(i = 0 ; i < 4 ; i++)
    {
      tmp = context -> digital[i];
      context -> digital[i] = context -> record[context -> record_index][i];
      context -> record[context -> record_index][i] = tmp;
    }
    break;
  }
}

void on_button4_long_push(TimerContext *context)
{
  int i;
  if(context -> mode == TIMER_SETTING_MODE)
  {
    
    if(context -> digital_index)
      context -> digital_index--;
    else
      context -> digital_index = 3;
    for(i = 0 ; i < 4 ; i++)
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
		context -> counter_ms += 2;
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

void check_button_io_p3(TimerContext* Context, char IOMask, int LongPushMask,
                        int DownMask, int DetectedMask,
                        void (*OnButtonClick)(TimerContext *context),
                        void (*OnButtonLongPush)(TimerContext *context),
                        unsigned int *Counter)
{
	if(!(P3 & IOMask))
	{
		if( !(Context -> flags & LongPushMask))
		{
			(*Counter) ++;
			Context -> flags |= DetectedMask;
			if(Context -> flags & DownMask) {
				if((*Counter) >= 350)
				{
					Context -> flags |= LongPushMask;
					if(OnButtonLongPush)
           OnButtonLongPush(Context);
				}
			}
			else {
				if((*Counter) >= 10)
					Context -> flags |= DownMask;
			}
		}
	}
	else
	{
		if(Context -> flags & DetectedMask) {
			if(Context -> flags & DownMask) {
				if(!(Context -> flags & LongPushMask))
					if(OnButtonClick)
            OnButtonClick(Context);
			}
			Context -> flags &= ~((unsigned int)(LongPushMask | DownMask | DetectedMask));
			(*Counter) = 0;
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
  context.flash_times[0] = 0;
	context.flash_times[1] = 0;
  context.flash_times[2] = 0;
  context.flash_times[3] = 0;
  context.counter_ms = 0;
	context.hex_loop = 0;
	context.flags = 0;
	context.loop_counter = 0;
	context.record_index = 0;
  P1 = ~(1 << context.record_index);
	for(i = 0 ; i < 4 ;i++)
		for(j = 0 ; j < 4 ; j++)
			context.record[i][j] = 0;
	
  //initalize inner timer
	TMOD = 0x01; //Timer 0 mode 1
	TH0 = 0xF8;
	TL0 = 0xCC;
	TF0 = 0;
	TR0 = 1;
	
	while(1) //main_loop
	{
		if(TF0 == 1)
		{
			TF0 = 0;									
			
			i = 0xCB + 10; //fix time error caused by instructions
			//1ms shou4e be 1843.2 Tick in 11.0592MHZ. 
			//Each 10 tick should be less 8 ticks to fix the error.
			time_error_fix ++;
			if(time_error_fix == 10)
			{
				time_error_fix = 0;
				i += 8;
			}
			i += TL0;
			TL0 = i & 0xFF;
			TH0 += 0xF8 + (i >> 8);
			
			context.loop_counter += 2;
			led_hex_display(&context);
      
      check_button_io_p3(&context, 0x20, BUTTON_1_LONG_PUSH, BUTTON_1_DOWN, BUTTON_1_DETECTED, on_button1_click, on_button1_long_push, &context.button_counter[0]);
      check_button_io_p3(&context, 0x10, BUTTON_2_LONG_PUSH, BUTTON_2_DOWN, BUTTON_2_DETECTED, on_button2_click, on_button2_long_push, &context.button_counter[1]);
      check_button_io_p3(&context, 0x08, BUTTON_3_LONG_PUSH, BUTTON_3_DOWN, BUTTON_3_DETECTED, on_button3_click, on_button3_long_push, &context.button_counter[2]);
      check_button_io_p3(&context, 0x04, BUTTON_4_LONG_PUSH, BUTTON_4_DOWN, BUTTON_4_DETECTED, on_button4_click, on_button4_long_push, &context.button_counter[3]);
      
      time_count(&context);
		}
	}
}