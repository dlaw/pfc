#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lookup.h"
#include "main.h"

// PINOUT
//  05 (OC2B): boost
//  11 (OC0B): buck low side
//  16 (OC1B): buck high side
//  26 (ADC3): Vin divided by 7.2
//  27 (ADC4): Current sense
//  28 (ADC5): Vdd divided by 10

int main(void) {
  DDRB = 0xFF;  // output
  DDRC = 0x00;  // input
  DDRD = 0xFF;  // output
  
  setup_pwm();
  setup_adc();
  sei();

  char ratio = 0;
  unsigned short desired_cs;
  short cs_p, cs_i = 0, boost_val;
  for (;;) {
    set_buck(buck_duty[get_vdd()]);
    if (get_vdd() == 255) {
      // disallow capacitor explosions
      set_boost(0);
      _delay_us(1000);
    }
    if (get_vin() < ZERO_CROSSING_THRESHHOLD) {
      ratio = cs_ratio[get_vdd()];
      cs_i = 0; // no integral windup allowed!
    }

    desired_cs = (get_vin() * cur_cs_ratio) / 64;
    if (desired_cs > 255) desired_cs = 255;

    cs_i += (cs_p = desired_cs - get_cs());
    if (i & 0x40) i &= 0xC0;  // if (abs(i) >= 8192) abs(i) = 8192;
    boost_val = (cs_i >> 5) + (cs_p << 1) + 128;  // magic feedback
    if (boost_val < 0) boost_val = 0;
    if (boost_val > BOOST_MAX) boost_val = BOOST_MAX;
    set_boost(boost_val);
    PORTB ^= 1;  // watch how quickly this loop runs
  }
}
