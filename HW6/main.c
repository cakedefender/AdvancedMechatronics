// Library for writing to MCP23008 8-bit I/O expander
// Michael Miller
//  4/2023
//
//

#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"


void setReg(unsigned char address, unsigned char reg, unsigned char data){
    i2c_master_start();
    i2c_master_send(address);
    i2c_master_send(reg);
    i2c_master_send(data);
    i2c_master_stop();
}

unsigned char readReg(unsigned char write_addr, unsigned char read_addr, unsigned char reg){
    i2c_master_start();                 // send start bit
    i2c_master_send(write_addr);
    i2c_master_send(reg);             // want to read the GPIO sfr
    i2c_master_restart();
    i2c_master_send(read_addr);        // tell MCP23008 that we want data from it
    unsigned char data = i2c_master_recv(); // receive data 
//    i2c_master_ack(0);
    i2c_master_stop();
    
    return data;
}

int main(){
    NU32DIP_Startup();
    i2c_master_setup();
    
    // initialize GP7 as output, GP0 as input (and all others as input)
    unsigned char data = 0b00000001;    // the data to send to MCP23008
    unsigned char IODIR = 0x00;
    unsigned char GPIO  = 0x09;
    unsigned char write_addr = 0b01000000;  // address for writing to MCP23008
    unsigned char read_addr  = 0b01000001;  // address for reading from MCP23008
    i2c_master_start();                 // send start bit
    i2c_master_send(write_addr);        // tell MCP23008 that we will talk to it
    i2c_master_send(IODIR);             // want to edit the IODIR sfr
    i2c_master_send(data);              // send which pins to make input, which to make output
    i2c_master_stop();
    
    // set all outputs low
    setReg(write_addr, GPIO, 0b0);
    
    while(1){
        // set up heartbeat at 10 Hz
        static unsigned int heartbeat = 48000000/10;  // 10 Hz
        if(_CP0_GET_COUNT() > heartbeat){
            _CP0_SET_COUNT(0);               // reset system clock
            NU32DIP_GREEN = !NU32DIP_GREEN;  // toggle green LED
        }
        // Read pins
        unsigned char read = readReg(write_addr, read_addr, GPIO);
        if(read & 0b1){
            setReg(write_addr, GPIO, 0b0);
        } else{
            setReg(write_addr, GPIO, 0b1 << 7);
        }
    }
 
    return 0;
}
		
