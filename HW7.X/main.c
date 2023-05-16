#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "mpu6050.h"
#include <stdio.h>

void blink(int, int); // blink the LEDs function

int main(void) {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    i2c_master_setup();
    init_mpu6050();
	
//	unsigned char data[14];  // 14 bytes of data 
//
//    float ax, ay, az, gx, gy, gz, t; // floats to store data

    // read who_am_i
    char who = whoami();
    char msg[100];
    // get stuck if we are not talking to the mpu6050
    if(who != 0x68){
        NU32DIP_YELLOW = 0;
        while(1){;}
    }
   
    
    while (1) {
		// use core timer for exactly 100Hz loop
        _CP0_SET_COUNT(0);
        blink(1, 5);

        // read IMU
        uint8_t data_raw[IMU_ARRAY_LEN]; 
        burst_read_mpu6050(data_raw);
        
		// convert data
        float data[IMU_ARRAY_LEN/2];
//        for(int i = 0; i < IMU_ARRAY_LEN/2; i++){
//            data[i] = (data_raw[i*2] << 8) | data_raw[i*2+1];
//        }
        data[0] = conv_xXL(data_raw);
        data[1] = conv_yXL(data_raw);
        data[2] = conv_zXL(data_raw);
        data[3] = conv_temp(data_raw);
        data[4] = conv_xG(data_raw);
        data[5] = conv_yG(data_raw);
        data[6] = conv_zG(data_raw);

//        print out the data
//        for(int i = 0; i < IMU_ARRAY_LEN/2; i++){
//            sprintf(msg, "\n\r%f", data[i]);
//            NU32DIP_WriteUART1(msg);
//        }
//        sprintf(msg, "\n\r");
//        NU32DIP_WriteUART1(msg);
        
        // print one channel of data for plotting
        sprintf(msg, "%f\n", data[0]);
        NU32DIP_WriteUART1(msg);
        
        

        while (_CP0_GET_COUNT() < 48000000 / 2 / 100) {;} // 100 Hz
    }
}

// blink the LEDs
void blink(int iterations, int time_ms) {
    int i;
    unsigned int t;
    for (i = 0; i < iterations; i++) {
        NU32DIP_GREEN = 0; // on
        NU32DIP_YELLOW = 1; // off
        t = _CP0_GET_COUNT(); // should really check for overflow here
        // the core timer ticks at half the SYSCLK, so 24000000 times per second
        // so each millisecond is 24000 ticks
        // wait half in each delay
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }

        NU32DIP_GREEN = 1; // off
        NU32DIP_YELLOW = 0; // on
        t = _CP0_GET_COUNT(); // should really check for overflow here
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }
    }
}
