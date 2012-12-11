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
  short cs_p, cs_i = 0;
  for (;;) {
    // Set the buck converter (open-loop from Vdd)
    set_buck(buck_duty[get_vdd()]);
    
    // Prevent Vdd from ever exceeding 50 volts
    if (get_vdd() == 255) {
      set_boost(0);
      _delay_us(1000);
    }

    // Reset ratio and integrator at each zero crossing
    if (get_vin() < ZERO_CROSSING_THRESHHOLD) {
      ratio = cs_ratio[get_vdd()];
      cs_i = 0;
    }

    // Magic PI feedback loop for current sense
    cs_i += (cs_p = ((get_vin() * ratio) >> 6) - get_cs());
    set_boost((cs_i >> 5) + (cs_p << 1) + 128);
  }
}
