/*
 * find_pwm.c
 *
 * Created: 9/8/2017 10:43:38 AM
 *  Author: raghu
 */ 

//Check if there is a PWM signal , check if there is a rising edge 
//Start the timer
//Check for a falling edge
//Stop the timer and rest the timer 
//Calculate Pulse width


#include "find_pwm.h"



//Initialise Timer
void config_tc2()
{
	
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT; //8
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64; //256
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	//config_tc.counter_8_bit.period = 0xFF;
	
	tc_init(&tc_instance2, TC2, &config_tc);
	tc_enable(&tc_instance2);
	
}



void configure_pwm_rising_extint()
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = PWM_PIN_EIC;
	config_extint_chan.gpio_pin_mux       = PWM_PIN_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_RISING;
	extint_chan_set_config(PWM_EIC_LINE, &config_extint_chan);
}

void configure_pwm_falling_extint()
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = PWM_PIN_EIC;
	config_extint_chan.gpio_pin_mux       = PWM_PIN_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(PWM_EIC_LINE, &config_extint_chan);
}

void pwm_detection_callback()
{
	static bool interrupt_edge = true;
	static int count_i = 0;
	Eic *const EIC_module = _extint_get_eic_from_channel(PWM_EIC_LINE);
	if(interrupt_edge == true )
	{
		tc_set_count_value(&tc_instance2, 0);
		//configure_pwm_falling_extint();
		EIC_module->CONFIG[0].reg &= 0x8FFFFFFF;
		EIC_module->CONFIG[0].reg |= 0x20000000;
		interrupt_edge = false;
	}
	else
	{
		pwm_rawvalue[count_i] = tc_get_count_value(&tc_instance2);
		interrupt_edge = true;
		
		//pwm_value = (CONSTANT_TIMER/pwm_rawvalue[count_i]); //gives the hertz
		EIC_module->CONFIG[0].reg &= 0x8FFFFFFF;
		EIC_module->CONFIG[0].reg |= 0x10000000;
		
		count_i ++;
		if(count_i ==  10)
		{
			count_i = 0;
			pwm_available = true;
		}
		
	}
	//Convert raw value
	
}


void configure_ext_callback()
{
	extint_register_callback(pwm_detection_callback,PWM_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(PWM_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
}
//Initialize timer and external interrupt

void initialize_find_pwm()
{
	
	config_tc2();
	configure_pwm_rising_extint();
	configure_ext_callback();
	
}  

//Initialise External interrupt
//Initialise callbacks
