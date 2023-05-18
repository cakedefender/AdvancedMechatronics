#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "ssd1306.h"
#include "font.h"
#include "IMU.h"

void drawLetter(unsigned char x, unsigned char y, char letter);
void drawString(unsigned char x, unsigned char y, char * s);

int main(void) {
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  i2c_master_setup();
  ssd1306_setup();  
  IMU_Startup();
  
  NU32DIP_GREEN = 1;
  NU32DIP_YELLOW = 0;
  
  while (1) {
      _CP0_SET_COUNT(0);
      NU32DIP_YELLOW = !NU32DIP_YELLOW;  // "heartbeat" indicator
      
      // get IMU acceleration data
      float data[IMU_ARRAY_LEN];
      IMU_getData(data);
      float zAcc = data[2];
      
      // print to screen
      char msg[100];
      sprintf(msg, "Z acceleration: %5.3f g", zAcc);
      drawString(0, 0, msg);
      ssd1306_update();
      
      // print frames per second
      float fps = 48000000.0 / _CP0_GET_COUNT();
      sprintf(msg, "FPS: %4.1f", fps);
      drawString(16, 3, msg);
      ssd1306_update();
      
      // 1Hz update rate
      while (_CP0_GET_COUNT() < 1000){;}
  }
}

void drawString(unsigned char x, unsigned char y, char * s){
    while(*s != '\0'){
        if(x >= 25 && s[1] != '\0'){ // check for text overflow
            drawLetter(x, y, '-');
            y++;
            x = 0;
        }
        drawLetter(x, y, *s);
        x++;
        s++;
    }
}
		
void drawLetter(unsigned char x, unsigned char y, char letter){
    // get the columns of pixels in the letter
    const char * cols = ASCII[letter - 0x20];
    
    unsigned char letterX = x * 5;
    unsigned char letterY = y * 8;
    
    for(int i = 0; i < 5; i++){// Loop thru each column in the letter
        char col = cols[i];
        for(int j = 0; j < 8; j++){ // Loop thru each pixel in the column 
            ssd1306_drawPixel(letterX + i, letterY + j, col>>j & 0b1);
        }
    }
}