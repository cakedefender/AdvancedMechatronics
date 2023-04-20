#include "nu32dip.h"
#include "spi.h"
#include <math.h>

#define PI 3.14159

int main(){
    // init
    NU32DIP_Startup();
    initSPI();
    
    int t = 0;
    
    // generate sin and triangle waveforms
    int sinWave[200]; // 2 cycles at 100 data points/cycle = 200
    int triWave[200];
    
    for (int x = 0; x < 200; x++){
        sinWave[x] = 511 + (int)(512 * sin(x * PI / 50)); // 2 cycles of integer sin wave centered on 512 w/ amplitude 512
        if (x < 100) {
            triWave[x] = 1023 * x / 100;
        } else {
            triWave[x] = 1023 - 1023 * (x-100) / 100;
        }
    }


    // send to DAC
    // [A/B][BUF][Gain][SHDN][10 x data][2 x trash]
    unsigned int testsignalA = 0b0111000000000000 + 0b111111111100;
    unsigned char testsignalA1 = testsignalA >> 8;
    unsigned char testsignalA2 = testsignalA | 0b11111111;

    _CP0_SET_COUNT(0);
    
    while(1){
        
        if(_CP0_GET_COUNT() / 2 > 60000){ // 200Hz
            _CP0_SET_COUNT(0);
            
            // determine integer to send
            int sinSig = sinWave[t];
            int triSig = triWave[t];
            t = t + 1;    // increment t
            if(t >= 200){ // reset t
                t = 0;
            }

            // turn integer into an unsigned short (1 byte)
            unsigned short A1bits = (sinSig & 0x3FF) >> 6;  // first 4 bits of data with leading zeros
            unsigned short A2bits = (sinSig & 0x3F) << 2;    // last 6 bits of data with trailing zeros

            unsigned short B1bits = (triSig & 0x3FF) >> 6;  // first 4 bits of data with leading zeros
            unsigned short B2bits = (triSig & 0x3F) << 2;    // last 6 bits of data with trailing zeros

            // generate config bits to send to MCP4912 DAC
            unsigned short configAbits = 0b0111 << 4;  // config bits are most significant bits of signal byte
            unsigned short configBbits = 0b1111 << 4;

            // put config bits and data bits together
            unsigned short signalA1 = configAbits + A1bits;
            unsigned short signalA2 = A2bits;
            unsigned short signalB1 = configBbits + B1bits;
            unsigned short signalB2 = B2bits;
                      
            // send to DAC
            CS = 0;
            spi_io(signalA1);
            spi_io(signalA2);
            CS = 1;
            CS = 0;
            spi_io(signalB1);
            spi_io(signalB2);
            CS = 1;
        }
        
        
    }
    return 0;
}