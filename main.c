#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lookup.h"
#include "main.h"

#define ZERO_CROSSING_THRESHHOLD 40
#define ZERO_CROSSING_CNT 3

#define BOOST_SMOOTH 1

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

  char cur_cs_ratio = 0;
  unsigned short desired_cs;
  char zc_cnt;
  
  unsigned short pfc_count = 0;

  short err, i, ii, boost_val;
  for (;;) {
    set_buck(buck_duty[get_vdd()]);
    if (get_vdd() == 255) {
      // disallow capacitor explosions
      set_boost(0);
      _delay_us(1000);
    }
    if (get_vin() >= ZERO_CROSSING_THRESHHOLD) zc_cnt = 0;
    if (zc_cnt++ == ZERO_CROSSING_CNT) {
      // This block runs exactly once per zero crossing.
      PORTB ^= 2;  // to watch what happens
      cur_cs_ratio = cs_ratio[get_vdd()];
    }
    /*
    pfc_count++;
    if (pfc_count < 10000) {
      set_boost(128);
      continue;
    }
    if (pfc_count == 20000) pfc_count = 0;
    */

    desired_cs = get_vin();
    desired_cs *= cur_cs_ratio;
    desired_cs /= 64;
    if (desired_cs > 255) desired_cs = 255;

    // THIS CS CODE WORKS "OKAY"
    err = desired_cs - get_cs();
    i += err;
    short imax = 8191;
    if (i > imax) i = imax;
    if (i < -imax) i = -imax;

    boost_val = i / 2;
    boost_val += err * 40;
    
    boost_val >>= 4;
    boost_val += 128;
    if (boost_val < 0) boost_val = 0;
    if (boost_val > BOOST_MAX) boost_val = BOOST_MAX;
    set_boost(boost_val);

    PORTB ^= 1;  // watch how quickly this loop runs
  }
}
