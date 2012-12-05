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

int main(void) {
  DDRB = 0xFF;  // output
  DDRC = 0x00;  // input
  DDRD = 0xFF;  // output
  
  setup_pwm();

  // Configure ADC to grab 8 bits to ADCH with a /128 prescaler
  ADMUX = 1<<REFS0 | 1<<ADLAR | 5;
  ADCSRA |= 1<<ADIE | 1<<ADEN | 1<<ADPS0 | 1<<ADPS1 | 1<<ADPS2;

  sei();
  ADCSRA |= 1<<ADSC;
  set_boost(128);
  for (;;);
}

ISR(ADC_vect) {
  unsigned int duty = 15729 / ADCH;  // magic number for 12 volts
  set_buck(duty < BUCK_MAX ? duty : BUCK_MAX);
  ADCSRA |= 1<<ADSC;  // start conversion
}
