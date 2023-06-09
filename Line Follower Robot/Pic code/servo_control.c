#include "servo_control.h"

/*
*
*   servo_control.c
*
*   A library for controlling a servo motor with the PIC32MX170F256B.
*   Servo PWM is output on pin RPb7, using OutputCompare1 and Timer2
*
*/

// *** NOTE: for my servo, OC1RS = 1400 for low (~0.5ms),  8000 high (~2.5ms)

void Servo_Startup(void) {
    // remap OC1 output pin to B7
    RPB7Rbits.RPB7R = 0b0101;

    T2CONbits.TCKPS = 4;     // Timer2 prescaler N=16 (1:16)
    PR2 = 60000-1;              // period = (PR2+1) * N * (1/48000000) = 50 Hz
    TMR2 = 0;                // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL = 0;   // Use timer2
    OC1R = PWM_0;              // initialize before turning OC1 on; afterward it is read-only
    OC1RS = PWM_0;             // duty cycle = OC1RS/(PR2+1) = 1.5ms
    T2CONbits.ON = 1;        // turn on Timer2
    OC1CONbits.ON = 1;       // turn on OC1
}

void Servo_GoToAngle(float target){
    int target_pwm  = (int)((target/180.0)*(PWM_180 - PWM_0) + PWM_0);
    OC1RS = target_pwm;
}

// // Sweep takes in a low angle, high angle, and time to spend sweeping back and forth between them.
// void Sweep(float low, float high, float seconds){
//     float angle = low; // start at low position
//     float increment = (high - low)/(seconds * INC_FREQ);  // find correct increment to make full sweep over however many 
//                                                     // seconds, given 10 Hz update rate
//     // send servo to low position
//     GoToAngle(angle);
//     int dir = 1;

//     // sweep servo from low to high position every [seconds] seconds, then back
//     while(1){
//         _CP0_SET_COUNT(0);

//         if(dir == 1){
//             angle += increment;
//             GoToAngle(angle);
//             if(angle >= high){
//                 dir = 0;
//             }
//         } else if (dir == 0){
//             angle -= increment;
//             GoToAngle(angle);
//             if(angle <= low){
//                 dir = 1;
//             }
//         }

//         AD1CON1bits.SAMP = 1;      // begin sampling
//         long long int elapsed = _CP0_GET_COUNT();
//         long long int finish_time = elapsed + 0.0001*48000000;
//         while (_CP0_GET_COUNT() < finish_time) {;}   // sample for more than 0.1 ms
//         AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
//         while (!AD1CON1bits.DONE) {;}         // wait for the conversion process to finish
//         long long int result = ADC1BUF0;  // read the buffer with the result

//         char msg[100];
//         sprintf(msg, "Angle: %f,  Light: %i\r\n", angle, result);
//         NU32DIP_WriteUART1(msg);


//         while(_CP0_GET_COUNT() < (48000000/INC_FREQ)){;}  // increment servo at INC_FREQ hz
//     }
// }