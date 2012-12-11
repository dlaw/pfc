#include <avr/interrupt.h>
#include <avr/io.h>

#include "main.h"

#define NCHAN 3
static volatile int chan = 0;
static char adc_chan[NCHAN] = {3, 4, 5};
static volatile char adc_data[NCHAN];

inline void setup_adc(void) {
  // Configure ADC to grab 8 bits to ADCH with a /128 clock prescaler
  ADMUX = 1<<REFS0 | 1<<ADLAR;
  ADCSRA |= 1<<ADIE | 1<<ADEN | 1<<ADPS1 | 1<<ADPS2;
  ADMUX |= adc_chan[0];
  ADCSRA |= 1<<ADSC;
}
ISR(ADC_vect) {
  adc_data[chan] = ADCH;
  chan = (chan + 1) % NCHAN;
  ADMUX &= ~(1<<MUX0 | 1<<MUX1 | 1<<MUX2 | 1<<MUX3);
  ADMUX |= adc_chan[chan];
  ADCSRA |= 1<<ADSC;
}

inline char get_vin(void) {
  return adc_data[0];
}
inline char get_cs(void) {
  return adc_data[1];
}
inline char get_vdd(void) {
  return adc_data[2];
}
