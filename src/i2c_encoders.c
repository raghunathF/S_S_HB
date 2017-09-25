

#include <asf.h>
#include "i2c_encoders.h"
#include "servo_control.h"
enum status_code error_code;

/*
 * i2c_encoders.c
 *
 * Created: 8/22/2017 1:18:35 PM
 *  Author: raghu
 */ 


// Helper function to just execute a simple write
void write_data_encoders(uint8_t *bufferSend, uint8_t length, struct i2c_master_module *const module)
{
	struct i2c_master_packet packet = {
		.address     = ENCODER_ADD,
		.data_length= length,
		.data        = bufferSend,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code  = 0x0,
	};
	do {
		error_code = i2c_master_write_packet_wait(module, &packet);
		
	}
	while (error_code != STATUS_OK);
	
}

void config_I2C_SERCOM2()
{
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 40000;
	// Fast mode Baud rate
	config_i2c_master.baud_rate        = I2C_MASTER_BAUD_RATE_400KHZ;
	config_i2c_master.start_hold_time  = I2C_MASTER_START_HOLD_TIME_400NS_800NS;
	/* Initialize and enable device with config. */
	do
	{
		error_code = i2c_master_init(&i2c_master_encoder, ENCODER_SERCOMM_2 , &config_i2c_master);
	}
	while(error_code != STATUS_OK);
	i2c_master_enable(&i2c_master_encoder);
}


void initialize_encoder()
{
	
	unsigned char bufferSend[4];
	// Disable the temperature sensor, Parity test disabled, enable the interrupt, FAST mode
	bufferSend[0] = 0b00000000;
	bufferSend[1] = 0b00000010;
	bufferSend[2] = 0b00000000;
	bufferSend[3] = 0b10000000;
	//
	write_data_encoders(bufferSend, 4 , &i2c_master_encoder);
		
}

void config_encoder()
{
	config_I2C_SERCOM2(); 
	initialize_encoder();
}
void config_encoder_modules()
{
	config_encoder();
}
void configure_encoder()
{
	config_encoder_modules();
}

void config_tc1()
{
	
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT; //8
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV8; //64
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	config_tc.counter_8_bit.period = 0x55;
	tc_init(&tc_instance1, TC1, &config_tc);
	tc_enable(&tc_instance1);
	
	
}


void config_callbacks_tc1()
{
	tc_register_callback(&tc_instance1, tc_callback_servo_control ,TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
}


void configure_pid_servo_control()
{
	config_tc1();
	config_callbacks_tc1();
}

void configure_enable_pins()
{
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
		
	port_pin_set_config(ENABLE_MOTOR_1, &config_port_pin);
	port_pin_set_config(ENABLE_MOTOR_2, &config_port_pin);
	port_pin_set_config(SLEEP_PIN, &config_port_pin);
	
	port_pin_set_output_level(ENABLE_MOTOR_1,true);
	port_pin_set_output_level(ENABLE_MOTOR_2,true);
	port_pin_set_output_level(SLEEP_PIN,DONT_SLEEP);
	
	
}


void config_motors(void)
{
	
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
	config_tcc.counter.period = 0x00FF;
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	
	
	//
	//
	config_tcc.compare.wave_polarity[MOTOR_CH_1]  = TCC_WAVE_POLARITY_0;
	config_tcc.compare.wave_polarity[MOTOR_CH_2]  = TCC_WAVE_POLARITY_0;
	
	//
	//
	config_tcc.compare.match[MOTOR_CH_1]			 = 0;
	config_tcc.pins.enable_wave_out_pin[MOTOR_WO_1] = true;
	
	///
	config_tcc.pins.wave_out_pin[MOTOR_WO_1]        = PIN_PA04F_TCC0_WO0; 
	config_tcc.pins.wave_out_pin_mux[MOTOR_WO_1]    = PINMUX_PA04F_TCC0_WO0;
	
	///
	config_tcc.compare.match[MOTOR_CH_2] = 0;
	config_tcc.pins.enable_wave_out_pin[MOTOR_WO_2] = true;
	config_tcc.pins.wave_out_pin[MOTOR_WO_2]        = PIN_PA05F_TCC0_WO1; 
	config_tcc.pins.wave_out_pin_mux[MOTOR_WO_2]    = PINMUX_PA05F_TCC0_WO1; 
	

	tcc_init(&tcc_instance0, TCC0, &config_tcc);
	tcc_enable(&tcc_instance0);
}

void configure_pins()
{
	configure_enable_pins();
}

void enable_motor()
{
	configure_pins();
	config_motors();
}

// PWM is on AIN1/AIN2 - PA12 and PA20
void set_motor(uint8_t dir, uint8_t speed)
{
	if(dir == 0)
	{
		tcc_set_compare_value(&tcc_instance0, MOTOR_CH_1, speed);
		tcc_set_compare_value(&tcc_instance0, MOTOR_CH_2, 0);
	}
	else
	{
		tcc_set_compare_value(&tcc_instance0, MOTOR_CH_1, 0);
		tcc_set_compare_value(&tcc_instance0, MOTOR_CH_2, speed);	 
	}
	
}

void turn_off_motor()
{
	set_motor(0,0);
	
}


void configure_sda_pin()
{
	//Set SDA pin as output
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SDA_PIN, &config_port_pin);
	port_pin_set_output_level(SDA_PIN,true);
	//Set the output high
}
