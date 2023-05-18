#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "mpu6050.h"
#include <stdio.h>

// NOTE: use IMU_ARRAY_LEN constant to set length of the data array you pass into IMU_getData

int IMU_Startup();
void IMU_getData(float* data);
