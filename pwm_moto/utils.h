#ifndef TINY_UTILS
#define TINY_UTILS

#include "types.h"


//Button checking

//Implement of button scaning with Preprocessor. just tricks.
#define DETECT_BITS_LENGTH          2           //Basic status bit length definition
#define MESSAGE_BITS_LENGTH         2
#define STATUS_VARIABLE_BIT_LENGTH  DETECT_BITS_LENGTH + MESSAGE_BITS_LENGTH

//control status
#define BUTTON_DETECT_MASK      0x03        //Button detecting control flags
#define BUTTON_NONE             0x00
#define BUTTON_DETECTED         0x01
#define BUTTON_DOWN             0x02
#define BUTTON_LONG_PUSH        0x03

//Message
#define BUTTON_LONG_DOWN            2       //Continuous down
#define BUTTON_CLICKED              1       //Clicked

//Masks
#define DETECT_STATUS_MASK(BitStartPosition)\
        (~((~((-1) << BitStartPosition)) | ((-1) << (BitStartPosition + DETECT_BITS_LENGTH))))
#define MESSAGE_MASK(BitStartPosition)\
        (~((~((-1) << (BitStartPosition + DETECT_BITS_LENGTH))) | ((-1) << (BitStartPosition + STATUS_VARIABLE_BIT_LENGTH))))
#define BUTTON_STATUS_MASK(BitStartPosition)\
        (~(((-1) << (BitStartPosition + STATUS_VARIABLE_BIT_LENGTH)) | (~((-1) << BitStartPosition))))
//Basic Operations
#define SET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition, Status) \
        (StatusVariable = StatusVariable & (~DETECT_STATUS_MASK(BitStartPosition)) | Status << BitStartPosition)
#define GET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition) \
        ((StatusVariable & DETECT_STATUS_MASK(BitStartPosition)) >> BitStartPosition)
#define SET_BUTTON_MESSAGE(StatusVariable, BitStartPosition, Message) \
        (StatusVariable |= ((Message) << (BitStartPosition + DETECT_BITS_LENGTH)))
#define CLEAR_BUTTON_MESSAGE(StatusVariable, BitStartPosition, Message) \
        (StatusVariable &= ~((Message) << (BitStartPosition + DETECT_BITS_LENGTH)))
        
//Message handle
#define IF_BUTTON_MESSAGE(StatusVariable, BitStartPosition, Message)\
        if((StatusVariable) & ((Message) << (BitStartPosition + DETECT_BITS_LENGTH)))

#define SCAN_BUTTON(Pin, Counter, CounterIncrement, StatusVariable, BitStartPosition, DownTime  \
                    , LongDownTime)                                                             \
        switch(GET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition))                      \
        {                                                                                       \
        case BUTTON_NONE:                                                                       \
            if(!Pin)                                                                            \
                SET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition, BUTTON_DETECTED);    \
            break;                                                                              \
        case BUTTON_DETECTED:                                                                   \
            if(Pin)                                                                             \
            {                                                                                   \
                SET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition, BUTTON_NONE);        \
                Counter = 0;                                                                    \
                break;                                                                          \
            }                                                                                   \
            Counter += CounterIncrement;                                                        \
            if(Counter >= DownTime)                                                             \
                SET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition, BUTTON_DOWN);        \
            break;                                                                              \
        case BUTTON_DOWN:                                                                       \
            if(Pin)                                                                             \
            {                                                                                   \
                Counter = 0;                                                                    \
                SET_BUTTON_MESSAGE(StatusVariable, BitStartPosition, BUTTON_CLICKED);           \
                SET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition, BUTTON_NONE);        \
                break;                                                                          \
            }                                                                                   \
            Counter += CounterIncrement;                                                        \
            if(Counter >= LongDownTime)                                                         \
            {                                                                                   \
                SET_BUTTON_MESSAGE(StatusVariable, BitStartPosition, BUTTON_LONG_DOWN);         \
                SET_BUTTON_DETECT_STATUS(StatusVariable , BitStartPosition, BUTTON_LONG_PUSH);  \
            }                                                                                   \
        case BUTTON_LONG_PUSH:                                                                  \
            if(Pin)                                                                             \
            {                                                                                   \
                SET_BUTTON_DETECT_STATUS(StatusVariable, BitStartPosition, BUTTON_NONE);        \
                Counter = 0;                                                                    \
            }                                                                                   \
            break;                                                                              \
        }    

typedef bit (*ReadButtonStatus)(void);
typedef struct Button_Context {
    uchar counter;
    uchar status;
}ButtonContext;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))





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