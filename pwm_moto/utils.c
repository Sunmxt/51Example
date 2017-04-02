#include "utils.h"

void CheckButton(ButtonContext *Context ,ReadButtonStatus Read, uchar CounterForward, uchar DownTime
                    , uchar LongDownTime)
{
    bit status;
    
    status = Read();
    SCAN_BUTTON(status, Context -> counter, CounterForward, Context -> status, 0, DownTime ,LongDownTime);
} 

/*
void CheckButton(ButtonContext *Context ,ReadButtonStatus Read, uchar CounterForward, uchar DownTime
                    , uchar LongDownTime)
{
    bit status;
    
    status = Read();
    
    
    switch(Context -> status & BUTTON_DETECT_MASK)
    {
    case BUTTON_NONE:
        if(!status)
            DETECT_STATUS_SET(*Context, BUTTON_DETECTED);
        else
            goto Check_Button_Clear_Return;
        
        break;
    case BUTTON_DETECTED:
        if(status)
            goto Check_Button_Clear_Return;
        
        if(Context -> counter >= DownTime)
            DETECT_STATUS_SET(*Context, BUTTON_DOWN);
        break;
        
    case BUTTON_DOWN:
        if(Context -> counter >= LongDownTime)
        {
            DETECT_STATUS_SET(*Context, BUTTON_LONG_PUSH);
            SET_BUTTON_MESSAGE(*Context, BUTTON_LONG_DOWN);
        }
        
        if(status)
        {
            DETECT_STATUS_SET(*Context, BUTTON_NONE);
            SET_BUTTON_MESSAGE(*Context, BUTTON_CLICKED);
            Context -> counter = 0;
            return;
        }
        break;
    case BUTTON_LONG_PUSH:
        if(status)
            goto Check_Button_Clear_Return;
        
        return;
    }

    Context -> counter += CounterForward;    
    return;
    
Check_Button_Clear_Return:
    Context -> status = 0;
    Context -> counter = 0;
    return;
}*/

/*
bit PWMControl(PWMPinContext *Context, uchar CounterForward)
{
    Context -> counter += CounterForward;
    
    if( !(~Context -> high_width) ) return 1;
    return Context -> counter < Context -> high_width;
}*/