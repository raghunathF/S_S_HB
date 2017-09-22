/*
 * i2c_encoders.h
 *
 * Created: 8/22/2017 1:18:53 PM
 *  Author: raghu
 */ 
#include <asf.h>

struct tcc_module tcc_instance0;
struct i2c_master_module i2c_master_encoder;
struct tc_module tc_instance1;

#define ENCODER_ADD 0x5E
#define ENCODER_SERCOMM_2 SERCOM0

//Pins
#define ENABLE_MOTOR_1 PIN_PA25
#define ENABLE_MOTOR_2 PIN_PA24
#define SDA_PIN PIN_PA14
#define SLEEP_PIN PIN_PA08


#define DONT_SLEEP true
#define SLEEP false

//TCC
#define MOTOR_CH_1 0
#define MOTOR_CH_2 1
#define MOTOR_WO_1 0
#define MOTOR_WO_2 1

void enable_motor();
void set_motor(uint8_t dir, uint8_t speed);
void configure_encoder();
void get_data_encoders(struct i2c_master_module instance);
void configure_pid_servo_control();