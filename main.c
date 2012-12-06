#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "main.h"

// PINOUT
//  05 (OC2B): boost
//  11 (OC0B): buck low side
//  16 (OC1B): buck high side
//  27 (ADC4): Current sense
//  28 (ADC5): Vdd divided by 10

#define BUCK_SCALE 15576  // 1298 * buck converter output voltage

int main(void) {
  DDRB = 0xFF;  // output
  DDRC = 0x00;  // input
  DDRD = 0xFF;  // output
  
  setup_pwm();
  setup_adc();
  sei();

  set_boost(128);
  for (;;) {
    uint16_t buck_setting = BUCK_SCALE / get_vdd();
    set_buck((buck_setting <= 255) ? buck_setting : 255);
  }
}
