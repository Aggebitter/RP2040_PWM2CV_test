/*
 Test code for PWM in eurorack
 this code will genereate 16 PWM signals matched to midi note 0-127 
 No matching for A at specific voltage just "0-3.3" in 128 steps 
 with a resolution of 1 cent (LUT needed)
 // Agge
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"

void setup() {
  // set SMPS mode to PWM for more stable ADC voltage refc
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);

  Serial.begin(115200);
  for( uint i = 0; i <= 15; i++){

    // Tell GPIO "i""they are allocated to the PWM
    gpio_set_function(i, GPIO_FUNC_PWM);
    //gpio_set_function(1, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO "i" 
    uint slice_num = pwm_gpio_to_slice_num(i);

    /*
      If phase correct is enabled, the pwm frequency is half compared with disabled
      default is false
    */
    pwm_set_phase_correct (slice_num, false);

    /*
      As the wraper does not care about number of bits 
      and just wraps around a defined value we can use any value from 1-65535
      12700 was choisen as it maps nice to midi and cents
      Measured PWM FQ = 15.75kHz
      Bits vs PWM FQ and wrap value 
      12bits 48.81kHz = 4093
      13bits 24.41kHz = 8191
      14bits 11.48kHz = 16383
      15bits 6.10kHz = 32767
      16bits 3.05kHz = 65535
    */
    pwm_set_wrap(slice_num, 12700); // 15.75kHz to match a cent for all midi tones 0-127

    // Find out which PWM channel is connected GPIO "i"
    uint channel_num = pwm_gpio_to_channel (i);

    //pwm_set_chan_level(slice_num, channel_num, 6000);

    // Set the PWM running
    pwm_set_enabled(slice_num, true);

    // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
    // correct slice and channel for a given GPIO.

  }  
}

void loop() {
  

    for ( uint j = 0; j <= 127; j++) // plays all midi notes
    {
      for( uint i = 0; i <= 15; i++){ // go thru every GPIO 
      // every step is a PWM value of 100. this would give us a offset LUT for 127 notes later
        uint16_t pwm_value = j * 100 ;
        pwm_set_gpio_level(i, pwm_value);
        //Serial.print("PWM value: ");
        //Serial.println(pwm_value);
        delay(1);
      }
    }

}
