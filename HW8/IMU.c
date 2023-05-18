#include "IMU.h"

int IMU_Startup(void) {
    init_mpu6050();

    // read who_am_i
    char who = whoami();
    char msg[100];
    // get stuck if we are not talking to the mpu6050
    if(who != 0x68){
        NU32DIP_YELLOW = 0;
        while(1){;}
    }
    return 0;
}
   
void IMU_getData(float* data){
    // read IMU
    uint8_t data_raw[IMU_ARRAY_LEN]; 
    burst_read_mpu6050(data_raw);
        
    // convert data
    data[0] = conv_xXL(data_raw);
    data[1] = conv_yXL(data_raw);
    data[2] = conv_zXL(data_raw);
    data[3] = conv_temp(data_raw);
    data[4] = conv_xG(data_raw);
    data[5] = conv_yG(data_raw);
    data[6] = conv_zG(data_raw);
}
