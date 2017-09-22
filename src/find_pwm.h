
/*
 * find_pwm.h
 *
 * Created: 9/8/2017 10:43:57 AM
 *  Author: raghu
 */ 
#include <asf.h>
#define CONSTANT_TIMER 31373
struct tc_module tc_instance2;
void initialize_find_pwm();




extern uint32_t pwm_rawvalue[15];
extern bool pwm_available;


#define PWM_PIN_EIC PIN_PA09A_EIC_EXTINT7
#define PWM_PIN_MUX PINMUX_PA09A_EIC_EXTINT7
#define PWM_EIC_LINE 7