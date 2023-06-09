#include "nu32dip.h"          // constants, funcs for startup and UART
#include "uart2.h" 
#include "motor.h"
#include "servo_control.h"


int main(void) {
  char msg[100];
  double left = 100; // start fast to overcome static friction, then slow down
  double right = 100;

  unsigned int start_time = _CP0_GET_COUNT();

  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  UART2_Startup();
  Servo_Startup();
  Motor_Startup();

  Motor_SetPWM(75.0, 75.0);
  // Servo_GoToAngle(70.0);   // set servo to 0°


  while (1) {
    //
    // GET C.O.M. of line from Pico
    //
    int com = 0;
    //uart2_flag() is 1 when uart2 has rx a message and sprintf'd it into a value
    if(get_uart2_flag()){
        set_uart2_flag(0); // set the flag to 0 to be ready for the next message
        com = get_uart2_value();
        sprintf(msg,"%d\r\n",com);
        NU32DIP_WriteUART1(msg);

        // adjust PWM of motors due to COM of line
        int diff = com - 40;                     // ******CHANGE TARGET COM HERE*******
        double p = .5;                           // ******CHANGE P GAIN HERE***********
        double pwm_offset = 40.0;                  // ******CHANGE PWM OFFSET HERE*******
        // diff is >0 means turn right
        double adj = (diff * p) > pwm_offset ? pwm_offset : (diff * p); // limit dropping below 50.0
        adj = adj < -pwm_offset ? -pwm_offset : adj;                    // limit dropping below 50

        right = (50.0 + pwm_offset) - adj;   
        left  = (50.0 + pwm_offset) + adj;

        sprintf(msg,"Motors: %lf  %lf\r\n",left, right);
        NU32DIP_WriteUART1(msg);

        // right = -right;  // right motor is backwards
        Motor_SetPWM(left, right);
    }

    // SERVO CODE: USEFUL FOR "BULLET BILL" LAUNCH MECHANISM
    // if (_CP0_GET_COUNT() > start_time + (24000000*5)){  // if it's been 5 seconds...
    //   Servo_GoToAngle(0.0); // move servo to 90°
    // }
    
  }


  return 0;
}
