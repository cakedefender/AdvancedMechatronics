#include "motor.h"


volatile double PWM_left;
volatile double PWM_right;

// // 200 Hz Velocity Controller ISR
// void __ISR(_TIMER_4_VECTOR, IPL1SOFT) VelocityControllerISR(void){
//         // Set motor PWM's
//         if (PWM_left < 0){
//             // set left backwards
//             TRISBCLR = 0b10;
//         } else{
//             // set left forwards
//             TRISBSET = 0b10;
//         }
//         if (PWM_right < 0){
//             // set left backwards
//             TRISBCLR = 0b100;
//         } else{
//             // set left forwards
//             TRISBSET = 0b100;
//         }

//         PWM_left = abs(PWM_left);
//         PWM_right = abs(PWM_right);

//         NU32DIP_WriteUART1("Got to 1\r\n");
        
//         OC3RS = (int)((PWM_right/100.0) * 2400);  // RIGHT motor
//         OC2RS = (int)((PWM_left/100.0) * 2400);   // LEFT  motor

//         char msg[100];
//         sprintf(msg, "Got to 2\r\nTime: %d", _CP0_GET_COUNT());
//         NU32DIP_WriteUART1(msg);

//         IFS0bits.T2IF = 0; // clear interrupt flag
// }

void Motor_Startup(){
    // Set up B1 & B2 as digital I/O pin for motor direction
    ANSELBCLR = 0b110;
    TRISBSET  = 0b110;
    

    // Set up Timer4 for 200 Hz Velocity Controller ISR
    __builtin_disable_interrupts(); // disable interrupts
    // PR4 = (30000 - 1);             // frequency = 48,000,000 / prescaler / (PR2) = 200 Hz
    // TMR4 = 0;                     // Initialize Timer2 count to 0
    // T4CONbits.TCKPS = 0b011;      // Prescaler 1:8
    // T4CONbits.ON = 1;             // Enable Timer2

    // IPC4bits.T4IP = 1;            // interrupt priority 1
    // IPC4bits.T4IS = 0;            // interrupt sub-priority 0
    // IFS0bits.T4IF = 0;            // clear the int flag
    // IEC0bits.T4IE = 1;            // enable Timer4's interrupt

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
    OC2CONbits.ON = 1;       // turn on OC2
}

// This function used to rely on the ISR, but it was interfering with the main.c infinite while loop,
// so now it just sets OC3RS and OC2RS manually.
void Motor_SetPWM(double left, double right){
    // Limit PWM to between 0.0 and 100.0
    left = left > 100.0 ? 100.0 : left;
    left = left < 0.0 ? 0.0 : left;
    right = right > 100.0 ? 100.0 : right;
    right = right < 0.0 ? 0 : right;

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
    
    OC3RS = (int)((PWM_right/100.0) * 2400);  // RIGHT motor
    OC2RS = (int)((PWM_left/100.0) * 2400);   // LEFT  motor
}