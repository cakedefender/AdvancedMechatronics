#include <stdio.h>
#include <math.h>

#include "nu32dip.h" // constants, functions for startup and UART

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
      
  char message[100];
  double sinWave[100];
  
  // Generate sin wave
  for(int i = 0; i < 100; i++){
      sinWave[i] = sin(2*M_PI * i/100) + 1;
  }
 
  while (1) {
      if(!NU32DIP_USER){    // USER button is pressed
          _CP0_SET_COUNT(0);
          for(int i = 0; i < 100; i++){
            int t = _CP0_GET_COUNT();
            // Send a number
            sprintf(message, "%lf\n", sinWave[i]);
            NU32DIP_WriteUART1(message);
            
            // wait 0.01 seconds
            while(_CP0_GET_COUNT() < t + 0.01*24000000){}
          }
      }
  }
}
		
