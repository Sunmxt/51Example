#ifndef TINY_UTILS
#define TINY_UTILS

#include "types.h"


//Button checking
typedef bit (*ReadButtonStatus)(void);
typedef struct Button_Context {
    uchar counter;
    uchar status;
    
    #define BUTTON_DETECT_MASK      0x03        //Button detecting control flags
    #define BUTTON_NONE             0x00
    #define BUTTON_DETECTED         0x01
    #define BUTTON_DOWN             0x02
    #define BUTTON_LONG_PUSH        0x03
    
    #define BUTTON_MESSAGE_MASK     0x0C        //Message flags
    #define BUTTON_CLICKED          0x04        //Clicked
    #define BUTTON_LONG_DOWN        0x08        //Continuous Down (only set once)

    
    #define CLEAR_BUTTON_MESSAGE(ButtonContext, Mask) (ButtonContext).status &= ~(Mask)
    #define SET_BUTTON_MESSAGE(ButtonContext, Mask) (ButtonContext).status |= Mask
    #define COUNTER_FORWARD(ButtonContext, Count) (ButtonContext).counter += (Count)
    #define DETECT_STATUS_SET(ButtonContext, Status) \
        (ButtonContext).status = ((ButtonContext).status & (~BUTTON_DETECT_MASK)) | Status
}ButtonContext;


void CheckButton(ButtonContext *Context ,ReadButtonStatus Read, uchar CounterForward, uchar DownTime
                    , uchar LongDownTime);
                    


//PWM Generate
typedef void (*PinSet)(uchar Value);

typedef struct PWM_Pin_Context {
    uchar high_width;
    uchar counter;
}PWMPinContext;

//bit PWMControl(PWMPinContext *Context, uchar CounterForward);
#define PWMControl(PWMOutputPin, Counter, Increment, DutyRadio)             \
            (Counter) += (Increment);                                       \
            (PWMOutputPin) = (Counter) > (DutyRadio)
            
#endif