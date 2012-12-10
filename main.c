#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lookup.h"
#include "main.h"

#define SMOOTH_SHIFT 7

// PINOUT
//  05 (OC2B): boost
//  11 (OC0B): buck low side
//  16 (OC1B): buck high side
//  27 (ADC4): Current sense
//  28 (ADC5): Vdd divided by 10

int main(void) {
  DDRB = 0xFF;  // output
  DDRC = 0x00;  // input
  DDRD = 0xFF;  // output
  
  setup_pwm();
  setup_adc();
  sei();

  unsigned long boost_val = 0;
  char desired_vdd = 200;  // 40 volts
  for (;;) {
    set_buck(buck_duty[get_vdd()]);
    if (get_vdd() == 255) {
      // disallow capacitor explosions
      set_boost(0);
      _delay_us(1000);
    }
    if (get_vdd() > desired_vdd && boost_val > 0)
      boost_val--;
    if (get_vdd() < desired_vdd && (boost_val >> SMOOTH_SHIFT) < BOOST_MAX)
      boost_val++;
    set_boost(boost_val >> SMOOTH_SHIFT);
    PORTB ^= 1;  // watch how quickly this loop runs
  }
}
