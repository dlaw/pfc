#include <avr/interrupt.h>
#include <avr/io.h>

#include "main.h"

// Initialize timers for 8 bit PWM at 31.25 kHz:
// timer 0 non-inverting output on OC0B (buck low)
// timer 1 inverting output on OC1B (buck high)
// timer 2 inverting output on OC2B (boost)
inline void setup_pwm(void) {
  GTCCR |= 1<<TSM | 1<<PSRSYNC | 1<<PSRASY;  // freeze timers
  TCCR0A |= 1<<WGM00 | 1<<WGM01 | 1<<COM0B1;
  TCCR0B |= 1<<CS00;
  TCCR1A |= 1<<WGM10 | 1<<COM1B0 | 1<<COM1B1;
  TCCR1B |= 1<<CS10 | 1<<WGM12;
  TCCR2A |= 1<<WGM20 | 1<<WGM21 | 1<<COM2B0 | 1<<COM2B1;
  TCCR2B |= 1<<CS20;
  TCNT1 = BUCK_DELAY;  // shoot-through delay for the buck
  OCR0B = OCR1B = OCR2B = 255;  // start with buck and boost off
  GTCCR &= ~(1<<TSM);  // unfreeze timers
}

// Buck output.  (Only change buck duty cycle just after a timer0
// overflow so that timer0 and timer1 change at the same time --
// otherwise pathological timing could result in shoot-through.)
volatile static char _OCR0B, _OCR1B;
inline void set_buck(char duty) {
  if (duty > BUCK_MAX) duty = BUCK_MAX;
  _OCR0B = 255 - duty - (duty ? 2 * BUCK_DELAY : 0);
  _OCR1B = 255 - duty;
  TIMSK0 |= 1<<TOIE0;  // enable TIMER0_OVF interrupt
}
ISR(TIMER0_OVF_vect) {
  OCR0B = _OCR0B;
  OCR1B = _OCR1B;
  TIMSK0 &= ~(1<<TOIE0);  // this is a one-shot interrupt
}

// Boost output
inline void set_boost(char duty) {
  if (duty > BOOST_MAX) duty = BOOST_MAX;
  OCR2B = 255 - duty;
}
