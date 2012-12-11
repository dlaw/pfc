#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lookup.h"
#include "main.h"

#define ZERO_CROSSING_THRESHHOLD 40
#define ZERO_CROSSING_CNT 3

#define BOOST_SMOOTH 2

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
  char cur_cs_ratio = 0;
  unsigned short desired_cs;

  int zc_cnt;
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
    
    // CURRENT SENSE RATIO IS A FUNCTION OF VDD
    //   (open loop: chosen at each zero crossing)
    // DESIRED CURRENT IS RATIO TIMES VIN
    // FEEDBACK LOOP TO MAKE CS = DESIRED_CS
    
    // Implementation plan:
    // 1. ZERO CROSSING DETECTION
    //   (toggle port b to watch)
    // 2. CURRENT SENSE FEEDBACK CONTROL
    //   (switch between 0.5A and 1A every second)
    // 3. RUNNING WITH CONSTANT CURRENT SENSE RATIO
    //   (output will saturate high or low)
    // 4. RUNNING WITH VARIABLE CURRENT SENSE RATIO


    // safely set desired_cs = get_vin() * cs_scale / 256
    desired_cs = get_vin();
    desired_cs *= cur_cs_ratio;
    desired_cs /= 64;  // scaling built in to cs_ratio lookup table
    if (desired_cs > 255) desired_cs = 255;

    if (get_cs() > desired_cs && boost_val > 0)
      boost_val--;
    if (get_cs() < desired_cs && (boost_val >> BOOST_SMOOTH) < BOOST_MAX)
      boost_val++;
    
    set_boost(boost_val >> BOOST_SMOOTH);

    PORTB ^= 1;  // watch how quickly this loop runs
  }
}
