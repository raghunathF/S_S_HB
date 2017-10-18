/*
 * servo_control.c
 *
 * Created: 8/31/2017 2:04:01 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "servo_control.h"
#include "i2c_encoders.h"

static uint8_t bufferReceive_encoder[7];



	

void get_data_encoders(struct i2c_master_module instance)
{
	struct i2c_master_packet packet = {
		.address     = ENCODER_ADD,
		.data_length = 6,
		.data        = bufferReceive_encoder,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code  = 0x0,
	};
	
	// Read all the registers of the encoder
	while(i2c_master_read_packet_wait(&instance, &packet) != STATUS_OK)
	{
		
	};
	
}


void tc_callback_servo_control(struct tc_module *const module_inst)
{
	static uint16_t servo_pos;
	static uint16_t speed;
	static uint16_t i=0;
	static  uint16_t k=0 ;
	static  bool direction = false;
	tc_disable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
	servo_pos = read_encoders();
	tc_enable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
	i = i + 1;

	if(i == 255)
	{
		i=0;
	}
}





uint16_t read_encoders()
{
	static int16_t i_count;
	//volatile static int16_t verify_position[500];
	//volatile static int16_t verify_count=0;
	
	
	int16_t x_left,y_left,z_left;
	int16_t temp;
	bool conversion_complete;
	uint8_t region;
    uint16_t y_pos;
	
	x_left = 0;
	y_left = 0;
	z_left = 0;
	get_data_encoders(i2c_master_encoder);
	
	temp = bufferReceive_encoder[5] & 0x0010;
	conversion_complete = temp<<5;
	filter_enable = 0;
	position_error = 1;
	if(conversion_complete == 0 )
	{
		x_left = bufferReceive_encoder[0];
		x_left = x_left << 4 ;
		temp = bufferReceive_encoder[4] & 0x00F0;
		temp = temp >> 4;
		x_left = x_left + temp ;
		temp  = x_left & 0x0800;
		x_left = x_left & 0x07FF;
		if(temp > 0 )
		{
			x_left = x_left- 2048;
		}
		x_left_a[i_count] = x_left;
		y_left = bufferReceive_encoder[1];
		y_left = y_left << 4 ;
		temp   = bufferReceive_encoder[4] & 0x000F;
		y_left = y_left + temp ;
		temp   = y_left & 0x0800;
		y_left = y_left & 0x07FF;
		if(temp > 0 )
		{
			y_left = y_left- 2048;
		}
		y_left_a[i_count] = y_left;
		
		z_left = bufferReceive_encoder[2];
		z_left = z_left << 4 ;
		temp = bufferReceive_encoder[5] & 0x000F;
		z_left = z_left + temp ;
		
		temp  = z_left & 0x0800;
		z_left = z_left & 0x07FF;
		if(temp > 0 )
		{
			z_left = z_left- 2048;
		}
		z_left_a[i_count] = z_left;
		
		//i_count = i_count + 1;
		
		filter_enable = 1;
		position_error =0;
		
		//verify_position[verify_count] = x_left_a[i_count];
		//verify_count++;
		
		//if(verify_count == 499 )
		//{
		//	verify_count = 0;
		//}
		
		///if(i_count == 10 )
		//{
			//filter_enable = 1;
			//i_count = 0;
			//set_motor(0,0) ;
		//}
		
	}
	//y_pos = 360 - y_pos;
	return y_pos;
}
