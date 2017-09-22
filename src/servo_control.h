/*
 * servo_control.h
 *
 * Created: 8/31/2017 2:04:57 PM
 *  Author: raghu
 */ 

#include <asf.h>


#ifndef SERVO_CONTROL_H_
#define SERVO_CONTROL_H_

#define NO_OF_SAMPLES 5



extern int filter_enable;
extern int16_t x_left_a[20];
extern int16_t y_left_a[20];
extern int16_t z_left_a[20];

void tc_callback_servo_control(struct tc_module *const module_inst);
uint16_t read_encoders();

#endif /* SERVO_CONTROL_H_ */