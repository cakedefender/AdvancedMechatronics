#include "nu32dip.h" // constants, functions for startup and UART
#include "ws2812b.h"

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  ws2812b_setup();   // initialize data output pin on PIC
  
  float hues[] = {0, 45, 90, 135, 180, 225, 270, 315};   // I have 8 led's
  wsColor rainbow[8];
  
  // Reset all LED's
  wsColor off[8];
  for(int i = 0; i < 8; i++){
      off[i] = HSBtoRGB(0, 0, 0);
  }
  ws2812b_setColor(off, 8);
  
  while(1){
    _CP0_SET_COUNT(0);
    // turn hues into wsColors and increment hues
    for (int i = 0; i < 8; i++){
        rainbow[i] = HSBtoRGB(hues[i], 1, .5);
        hues[i] += 0.05;
        if(hues[i] > 360){hues[i] = 0;}  // rollover at 360
    }
      
    // update colors
    ws2812b_setColor(rainbow, 8);
    
    // make sure colors update at consistent Hz
    while(_CP0_GET_COUNT() < 48000){;} // 1kHz
  }
}
		
