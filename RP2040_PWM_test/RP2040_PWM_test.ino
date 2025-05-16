/*
 Test code for PWM in eurorack
 The rp2040 chould be overclocked to 240MHz or more (gives higher PWM fq) 
 This code will genereate 16 PWM signals matched to midi note 12-127 
 No matching for A at specific voltage just "0-3.3" in 120 steps 
 with a resolution of 1 cent 
 // Agge
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "lut.h"

void setup() {
  // set SMPS mode to PWM for more stable ADC voltage ref (works on GPIO voltage to ?)
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
      12000 was choisen as it maps nice to usefull midi notes and cents
      Measured PWM FQ = 20.00kHz@240MHz overclock
      Bits vs PWM FQ and wrap value 
      12bits 48.81kHz = 4093
      13bits 24.41kHz = 8191
      14bits 11.48kHz = 16383
      15bits 6.10kHz = 32767
      16bits 3.05kHz = 65535
    */
    pwm_set_wrap(slice_num, 12000); // to match a cent for all midi tones 12-127

    // Find out which PWM channel is connected GPIO "i"
    //uint channel_num = pwm_gpio_to_channel (i);

    //pwm_set_chan_level(slice_num, channel_num, 6000);

    // Set the PWM running
    pwm_set_enabled(slice_num, true);

    // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
    // correct slice and channel for a given GPIO.

  }  
}

void loop() {
  
  int k = 0;

    for ( uint j = 12; j <= 127; j++) // plays all midi notes from C0 to C10
    {
    
      for( uint i = 0; i <= 15; i++){ // go thru every GPIO 
        // every step is a PWM step value of 100. 
        // CO = pwm_value 0, C10 = PWM value 12000
        uint16_t pwm_value = k * 100 + midi_notes[j].offset;

        pwm_set_gpio_level(i, pwm_value);
        //Serial.print("PWM value: ");
        //Serial.println(pwm_value);
        delay(1);
      }

     k++;

    }

}
