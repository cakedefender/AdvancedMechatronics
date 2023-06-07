#include "nu32dip.h"          // constants, funcs for startup and UART

/*
*
*   servo_control.h
*
*   A library for controlling a servo motor with the PIC32MX170F256B.
*   Servo PWM is output on pin RPb7, using OutputCompare1 and Timer2
*
*/

// OC1RS values for 180 and 0 degrees, out of 60,000
#define PWM_180 8000  //8000
#define PWM_0   1800  //1400
#define INC_FREQ 20

void Servo_Startup(void);
void GoToAngle(float target);
void Sweep(float low, float high, float seconds);
