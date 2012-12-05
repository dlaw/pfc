// PWM settings
#define BUCK_DELAY 2  // number of clock cycles of shoot-through delay
#define BUCK_MAX 222  // maximum duty cycle for the buck converter
#define BOOST_MAX 222  // maximum duty cycle for the boost converter
void setup_pwm(void);
void set_buck(char duty);
void set_boost(char duty);
