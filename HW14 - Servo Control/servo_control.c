#include "nu32dip.h"          // constants, funcs for startup and UART

int main(void) {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init

    // remap OC1 output pin
    RPB7Rbits.RPB7R = 0b0101;

    T2CONbits.TCKPS = 4;     // Timer2 prescaler N=16 (1:16)
    PR2 = 60000-1;              // period = (PR2+1) * N * (1/48000000) = 50 Hz
    TMR2 = 0;                // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL = 0;   // Use timer2
    OC1RS = 3000;             // duty cycle = OC1RS/(PR2+1) = 1.5ms
    OC1R = 4500;              // initialize before turning OC1 on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer2
    OC1CONbits.ON = 1;       // turn on OC1


    // *** NOTE: for servo, OC1RS = 1400 for low (~0.5ms),  8000 high (~2.5ms)

    // send servo to 45 degrees
    OC1RS = 3050;
    int dir = 1;

    // sweep servo from 45 to 135 degrees every 4 seconds, then back
    while(1){
        if(dir == 1){
            OC1RS = OC1RS + 82;
            if(OC1RS >= 6350){
                dir = 0;
            }
        } else if (dir == 0){
            OC1RS = OC1RS - 82;
            if(OC1RS <= 3050){
                dir = 1;
            }
        }
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 4800000){;}  // increment servo at 10 hz
    }

    return 0;
}
