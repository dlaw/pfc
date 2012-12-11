// ATmega328 pinout
//  05 (OC2B): boost
//  11 (OC0B): buck low side
//  15 (PB1) : PFC enable
//  16 (OC1B): buck high side
//  26 (ADC3): Vin divided by 7.2
//  27 (ADC4): Current sense
//  28 (ADC5): Vdd divided by 10

// main.c
#define ZERO_CROSSING_THRESHHOLD 35

// adc.c
void setup_adc(void);
char get_vin(void);
char get_cs(void);
char get_vdd(void);

// pwm.c
#define BUCK_DELAY 2  // number of clock cycles of shoot-through delay
#define BUCK_MAX 128  // maximum duty cycle for the buck converter
#define BOOST_MAX 240  // maximum duty cycle for the boost converter
void setup_pwm(void);
void set_buck(char duty);
void set_boost(char duty);
