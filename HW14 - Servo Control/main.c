#include "nu32dip.h"
#include "servo_control.h"

void Sweep(float low, float high, float seconds);

int main(){
    NU32DIP_Startup();
    Servo_Startup();

    // Set up analog input on pin AN12
    ANSELBbits.ANSB12 = 1;
    AD1CHSbits.CH0SB = 0b1100;   // connect chosen pin to AD1 for sampling

    AD1CON1bits.SAMP = 1;      // begin sampling
    long long int elapsed = _CP0_GET_COUNT();
    long long int finish_time = elapsed + 0.0001*48000000;
    while (_CP0_GET_COUNT() < finish_time) {;}   // sample for more than 0.1 ms
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {;}         // wait for the conversion process to finish
    long long int result = ADC1BUF0;  // read the buffer with the result

    char msg[100];
    float angle = 0;
    sprintf(msg, "Angle: %f,  Light: %i\r\n", angle, result);
    NU32DIP_WriteUART1(msg);

    // Sweep(45, 135, 2);

    return 0;
}

// Sweep takes in a low angle, high angle, and time to spend sweeping back and forth between them.
void Sweep(float low, float high, float seconds){
    float angle = low; // start at low position
    float increment = (high - low)/(seconds * INC_FREQ);  // find correct increment to make full sweep over however many 
                                                    // seconds, given 10 Hz update rate
    // send servo to low position
    GoToAngle(angle);
    int dir = 1;

    // sweep servo from low to high position every [seconds] seconds, then back
    while(1){
        _CP0_SET_COUNT(0);

        if(dir == 1){
            angle += increment;
            GoToAngle(angle);
            if(angle >= high){
                dir = 0;
            }
        } else if (dir == 0){
            angle -= increment;
            GoToAngle(angle);
            if(angle <= low){
                dir = 1;
            }
        }

        AD1CON1bits.SAMP = 1;      // begin sampling
        long long int elapsed = _CP0_GET_COUNT();
        long long int finish_time = elapsed + 0.0001*48000000;
        while (_CP0_GET_COUNT() < finish_time) {;}   // sample for more than 0.1 ms
        AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
        while (!AD1CON1bits.DONE) {;}         // wait for the conversion process to finish
        long long int result = ADC1BUF0;  // read the buffer with the result

        // long long int result = 0;
        char msg[100];
        sprintf(msg, "Angle: %f,  Light: %i\r\n", angle, result);
        NU32DIP_WriteUART1(msg);


        while(_CP0_GET_COUNT() < (48000000/INC_FREQ)){;}  // increment servo at INC_FREQ hz
    }
}