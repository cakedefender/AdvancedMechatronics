#ifndef MOTOR__H__
#define MOTOR__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include <stdlib.h>

#include "NU32DIP.h"

void Motor_Startup();
void Motor_SetPWM(double left, double right);

#endif // MOTOR__H__