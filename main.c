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
  // Set up IO pins
  DDRB = 0xFD;
  PORTB |= 2;
  DDRD = 0xFF;
  
  setup_pwm();
  setup_adc();
  sei();

  // Time to correct the power factor!
  char ratio = 0;  // desired current-to-voltage ratio
  short p, i = 0;  // current feedback loop accumulators
  for (;;) {
    // Set the buck converter (open-loop from Vdd)
    set_buck(buck_duty[get_vdd()]);
    
    // Prevent Vdd from ever exceeding 50 volts
    if (get_vdd() == 255) {
      set_boost(0);
      _delay_us(1000);
      i = 0;
    }

    // Reset ratio and integrator at each zero crossing
    if (get_vin() < 35) {
      ratio = cs_ratio[get_vdd()];
      i = 0;
    }

    // Magic PI feedback loop for current sense
    i += (p = ((get_vin() * ratio) >> 6) - get_cs());
    set_boost((PINB & 2) ? ((i >> 5) + (p << 1) + 128) : 128);
  }
}
