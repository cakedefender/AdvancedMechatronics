#include "motor.h"


volatile double PWM_left;
volatile double PWM_right;

// 200 Hz Velocity Controller ISR
void __ISR(_TIMER_2_VECTOR, IPL3SOFT) VelocityControllerISR(void){
        // Set motor PWM's
        if (PWM_left < 0){
            // set left backwards
            TRISBCLR = 0b10;
        } else{
            // set left forwards
            TRISBSET = 0b10;
        }
        if (PWM_right < 0){
            // set left backwards
            TRISBCLR = 0b100;
        } else{
            // set left forwards
            TRISBSET = 0b100;
        }

        PWM_left = abs(PWM_left);
        PWM_right = abs(PWM_right);
        
        OC3RS = (int)((PWM_right/100.0) * 2400) - 1;  // RIGHT motor
        OC2RS = (int)((PWM_left/100.0) * 2400) - 1;   // LEFT  motor

        IFS0bits.T2IF = 0; // clear interrupt flag
}

void Motor_Startup(){
    // Set up B1 & B2 as digital I/O pin for motor direction
    ANSELBCLR = 0b110;
    TRISBSET  = 0b110;
    

    // Set up Timer2 for 200 Hz Velocity Controller ISR
    __builtin_disable_interrupts(); // disable interrupts
    PR2 = (30000 - 1);             // frequency = 48,000,000 / prescaler / (PR2) = 200 Hz
    TMR2 = 0;                     // Initialize Timer2 count to 0
    T2CONbits.TCKPS = 0b011;      // Prescaler 1:8
    T2CONbits.ON = 1;             // Enable Timer2

    IPC2bits.T2IP = 3;            // interrupt priority 3
    IPC2bits.T2IS = 0;            // interrupt sub-priority 0
    IFS0bits.T2IF = 0;            // clear the int flag
    IEC0bits.T2IE = 1;            // enable Timer2's interrupt

    // Set up Timer3 for 20 kHz for PWM to motors
    T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1 (1:1)
    PR3 = (2400 - 1);        // frequency = 48,000,000 / prescaler / PR3 => 20 kHz
    TMR3 = 0;                // initial TMR3 count is 0
    T3CONbits.TCKPS = 0;     // Prescaler 1:1
    __builtin_enable_interrupts();  // step 7: enable interrupts


    // Sets remappable pins to use Output-Compare 3 (OC3)...
    RPA3Rbits.RPA3R = 0b0101; // Remap RPA3 to OC3
    RPB8Rbits.RPB8R = 0b0101; // Remap RPB8 to OC2

    // Initializes OC3 to PWM mode, without fault, with a duty cycle of (OC3R / PR3)...
    OC3CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC3CON bits are defaults
    OC3CONbits.OC32 = 0;     // Use a 16-bit timer (Timer2 or Timer3)
    OC3CONbits.OCTSEL = 1;   // Use Timer 3 specifically
    OC3RS = 0;               // duty cycle = OC3RS/(PR3 + 1) = 0%
    OC3R = 0;                // initialize before turning OC3 on; afterward it is read-only
    T3CONbits.ON = 1;        // turn on Timer3
    OC3CONbits.ON = 1;       // turn on OC3

    // Initializes OC2 to PWM mode, without fault, with a duty cycle of (OC2R / PR2)...
    OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC3CON bits are defaults
    OC2CONbits.OC32 = 0;     // Use a 16-bit timer (Timer2 or Timer3)
    OC2CONbits.OCTSEL = 1;   // Use Timer 3 specifically
    OC2RS = 0;               // duty cycle = OC3RS/(PR3 + 1) = 0%
    OC2R = 0;                // initialize before turning OC3 on; afterward it is read-only
    // T3CONbits.ON = 1;        // turn on Timer3
    OC2CONbits.ON = 1;       // turn on OC3


    //
    ///
    //
    //   check if both OC's can use same Timer?
    //
    //
    ////
}

void Motor_SetPWM(double left, double right){
    // Limit PWM to between 0.0 and 100.0
    left = left > 100.0 ? 100.0 : left;
    left = left < 0.0 ? 0.0 : left;
    right = right > 100.0 ? 100.0 : right;
    right = right < 0.0 ? 0 : right;

    PWM_left = left;
    PWM_right = right;
}