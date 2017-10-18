/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "servo_control.h"
#include "i2c_encoders.h"
#include "find_pwm.h"
#include "calibration.h"
#include "serial.h"


#define CLOCKWISE 0
#define ANTI_CLOCKWISE 1

int intersect_min_value = 0;
int intersect_max_value = 0;

uint32_t pwm_rawvalue[15]={0};

int16_t x_left_a[20];
int16_t y_left_a[20];
int16_t z_left_a[20];

#define MARGIN 10
#define X_POS_LIMIT 143


int stop_flag = false;

//volatile static int16_t mode_x[100];
//volatile static int16_t mode_y[100];
//volatile static int16_t mode_z[100];


int filter_enable = false;
bool position_error = true;
bool pwm_available= false;
bool test_main = false;
int position_global = 0;

/*
int find_mode()
{
	int i,k;
	volatile int j =0 ;
	
	volatile static int16_t temp_bin[20];
	volatile static int16_t  count_bin[20];
	volatile static int16_t test_samples=0;
	
	volatile bool create_bin = true;
	
	for(j=0;j<NO_OF_SAMPLES;j++)
	{
		temp_bin[j]  = 0;
		count_bin[j] =0;
	}
	
	for(i=0;i<NO_OF_SAMPLES;i++)
	{
		for(j=0;j<test_samples;j++)
		{
			if(temp_bin[j] == temp_x[i])
			{
				create_bin = false;
				count_bin[j] = count_bin[j] + 1;
				
			}
		}
		if(create_bin == true)
		{
			temp_bin[i] = temp_x[test_samples] ;
			test_samples = test_samples + 1;
		}
		create_bin = true;
	}
}

*/

int find_mode(uint32_t* input)
{
	int i,k;
	int temp_max = 0;
	
	volatile static int j=0;
	volatile uint32_t temp_bin[20];
	volatile uint32_t count_bin[20];
	volatile uint32_t test_samples=0;
	volatile uint32_t max_index=0;
	volatile uint32_t output= 0;
	bool create_bin = true;
	
	for(j=0;j<NO_OF_SAMPLES;j++)
	{
		temp_bin[j]  = 0;
		count_bin[j] = 0;
	}
	
	for(i=0;i<NO_OF_SAMPLES;i++)
	{
		for(j=0;j<test_samples;j++)
		{
			if(input[i] == temp_bin[j])
			{
				create_bin = false;
				count_bin[j] = count_bin[j] + 1;
			}
		}
		if(create_bin == true)
		{
			temp_bin[i] = input[test_samples] ;
			test_samples++;
		}
		create_bin = true;
	}
	
	for(k=0;k<test_samples;k++)
	{
		if( count_bin[k] >= temp_max)
		{
			max_index = k;
			temp_max  =  count_bin[k];
		}
	}
	output = temp_bin[max_index];
	return output;
}

/*
void filter_values(int16_t* x , int16_t * y, int16_t* z)
{
	static int count_i = 0;
	mode_x[count_i] = find_mode(x);
	mode_y[count_i] = find_mode(y);
	mode_z[count_i] = find_mode(z);
	count_i = count_i +1;
	if(count_i == 50)
	{
		count_i=0;
	}
	
}
*/

/*
void check()
{
	
	
	static int16_t temp_x[20] ;
	static int16_t temp_y[20] ;
	static int16_t temp_z[20] ;
	
	int i = 0 ;
	
	if(filter_enable == true)
	{
		filter_enable = false;
		//Disable the interrupts
		tc_disable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
			
		//Copy the values
		for(i=0 ; i< NO_OF_SAMPLES ;i++)
		{
			temp_x[i] = x_left_a[i];
			temp_y[i] = y_left_a[i];
			temp_z[i] = z_left_a[i];
		}
		tc_enable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
		//Enable the interrupts
		filter_values(temp_x,temp_y,temp_z);
		
	}
}
*/
/*
void initialize_xyz()
{
	int i ;
	for(i=0 ; i< NO_OF_SAMPLES ;i++)
	{
		;
	}
}
*/

#define UPWARD true
#define DOWNWARD false

/*
void check()
{
	static int pointer = 0;
	static uint16_t storage_array[20];
	static bool initialize_flag = false;
	static bool direction_flow = true;
	
	
	if(filter_enable == true)
	{
		if(initialize_flag == true)
		{
			storage_array[pointer] = value;
			pointer++;
			if(pointer == NO_OF_SAMPLES)
			{
				initialize_flag = false;
			}
		}
		else
		{
			if(direction_flow == UPWARD)
			{
				storage_array[pointer] = value;
				pointer++;
				if(pointer == NO_OF_SAMPLES )
				{
					direction_flow = DOWNWARD;
					pointer = pointer - 2;
				}
				
			}
			else
			{
				storage_array[pointer] = value;
				pointer--;
				if(pointer < 0 )
				{
					direction_flow = UPWARD;
					pointer = pointer + 2;
				}
				
			}
		}
	}
	
	for(i=pointer;)
	
}
*/

#define MAX_SPEED 100
#define MIN_SPEED  35
#define MAX_PARTION 860
#define HALF_PARTION 420

void set_motion(uint32_t position,uint32_t pwm_value)
{
	bool direction_motion=  CLOCKWISE;
	uint32_t min_diff = 0;
	uint32_t speed = 0;
	
	//volatile static uint16_t verify_motion[500];
	//volatile static uint16_t verify_count=0;
	
	
	if(position > pwm_value)
	{
		if(abs(position - pwm_value)> HALF_PARTION)
		{
			min_diff =  MAX_PARTION - abs(position - pwm_value) ;
			direction_motion = ANTI_CLOCKWISE;
		}
		else
		{
			min_diff =  abs(position - pwm_value);
			direction_motion = CLOCKWISE;
		}
		
	}
	else
	{
		if(abs(position - pwm_value)> HALF_PARTION)
		{
			min_diff =  MAX_PARTION - abs(position - pwm_value) ;
			direction_motion = CLOCKWISE;
		}
		else
		{
			min_diff =  abs(position - pwm_value);
			direction_motion = ANTI_CLOCKWISE;
		}
		
		
	}
	
	
	
	if(min_diff > 50)
	{
		set_motor(direction_motion,MAX_SPEED);
	}
	else
	{	
		speed = (min_diff * MAX_SPEED)/ HALF_PARTION ; 
		if(speed > MAX_SPEED)
		{
			speed = MAX_SPEED;
		}
		else if(speed < MIN_SPEED)
		{
			speed = MIN_SPEED;
		}
		set_motor(direction_motion,speed);
	}
	
	
	//verify_motion[verify_count] = position;
	//verify_count++;
	
	//if(verify_count == 499 )
	//{
	//	verify_count = 0;
	//}
	
}



int  convert2degree(volatile int x , volatile int y)
{
	volatile static int pos;
	static int count = 0;
	int quad_value = intersect_max_value + intersect_min_value;
	//count++;
	if(x >= intersect_max_value )
	{
		//1st Quad
		if((y <= intersect_max_value) && (y >= intersect_min_value) )
		{
			if(y>=0)
			{
				pos = intersect_max_value - y;
			}
			else
			{
				pos = intersect_max_value + abs(y);
			}	
		}
		else if(y > intersect_max_value)
		{
			pos = 0;
		}
		else
		{
			pos = quad_value;
		}
	}
	else if( y <= intersect_min_value)
	{
		//2nd Quad
		if( (x <= intersect_max_value) && (x >= intersect_min_value) )
		{
			if(x>=0)
			{
				pos = (intersect_max_value - x) + quad_value ;
			}
			else
			{
				pos = intersect_max_value + abs(x) + quad_value;
			}
		}
		else if(y > intersect_max_value)
		{
			pos =  quad_value ;
		}
		else
		{
			pos = 2*quad_value;
		}
	}
	
	else if( x <= intersect_min_value)
	{
		//3rd Quad
		if( (y <= intersect_max_value) && (y >= intersect_min_value) )
		{
			if(y>=0)
			{
				pos = (abs(intersect_min_value) + y) + quad_value*2 ;
			}
			else
			{
				pos = abs(intersect_min_value) - abs(y)  + quad_value*2;
			}
		}
		else if(y > intersect_max_value)
		{
			pos =  2*quad_value ;
		}
		else
		{
			pos = 3*quad_value;
		}
	}
	
	else if( x <= intersect_min_value)
	{
		//4th Quad

		if( (x <= intersect_max_value) && (x >= intersect_min_value) )
		{
			if(x>=0)
			{
				pos = (abs(intersect_min_value) + x) + quad_value*3 ;
			}
			else
			{
				pos = abs(intersect_min_value) - abs(x)  + quad_value*3;
			}
		}
		else if(x > intersect_max_value)
		{
			pos =  3*quad_value ;
		}
		else
		{
			pos = 4*quad_value;
		}
	}
	else
	{
		pos = 0;
	}
	return pos;
	
}



/*
int  convert2degree(volatile int x , volatile int y)
{
	volatile static int pos_x[10];
	static int count = 0;
	//count++;
	if(x >= 0  && y < 0 )
	{
	//1st Quad
		pos_x[count] =  x ;	
	}
	
	else if(x >= 0  && y >= 0 )
	{
	//2nd Quad
		pos_x[count] = 2*X_POS_LIMIT - x;
		
	}
	else if(x< 0 && y>=0)
	{
	//3rd Quad
		pos_x[count] = 2*X_POS_LIMIT + abs(x);
	}
	else
	{
	//4th Quad
		pos_x[count] = 4*X_POS_LIMIT - abs(x);
	}
	
	//if(pos_x[count] == 426)
	//{
	//	set_motor(0,0) ;
	//}
	
	//if(count == 360)
	//{
	//	set_motor(0,0) ;
	//}
	//if(count == 5)
	//{
	//	count = 0;
		//set_motor(0,200) ;
	//}
	return pos_x[count];
	
}
*/

//Check
void check(uint32_t pwm_value)
{
	int i;
	static int16_t storage_array_x[10] = {0};
	static int16_t storage_array_y[10] = {0};
	static volatile int avg_x[35];
	static volatile int avg_y[35];
	static int count_i_k = 0;
	//static volatile int test_x[400];
	//volatile static uint16_t verify_motion[500];
	//volatile static uint16_t verify_count=0;
	
	static int count_i=0;
	static int temp_x = 0;
	static int temp_y = 0;
	volatile static int position=0;
	static bool initialization = false;
	
	bool direction_motion = true;
	
	temp_x =  x_left_a[0];
	temp_y =  y_left_a[0];
	
	
	
	//if(filter_enable == true)
	//{
		//filter_enable = false;
		for(i=NO_OF_SAMPLES-1;i>=1;i--)
		{
			storage_array_x[i] = storage_array_x[i-1];	
			storage_array_y[i] = storage_array_y[i-1];	
		}
		if(initialization == true)
		{
			if(abs(abs(temp_x) - abs(storage_array_x[0])) < MARGIN)
			{
					storage_array_x[0] = temp_x ; 
			}
			if(abs(abs(temp_y) - abs(storage_array_y[0])) < MARGIN)
			{
				storage_array_y[0] = temp_y ;
			}
		}
		else
		{
			storage_array_x[0] = temp_x ; 
			storage_array_y[0] = temp_y ; 
		}
		
		avg_x[count_i] = (storage_array_x[4]*10 + storage_array_x[3]*15 + storage_array_x[2]*20 + storage_array_x[1]*25 + storage_array_x[0]*30)/100;
		//count_i++;
		avg_y[count_i] = (storage_array_y[4]*10 + storage_array_y[3]*15 + storage_array_y[2]*20 + storage_array_y[1]*25 + storage_array_y[0]*30)/100;
		
		
		
		if(initialization == true)
		{
			 position = convert2degree(avg_x[count_i],avg_y[count_i]);
			 //count_i++;
			 if(abs(position-pwm_value) < 2)
			 {
				 stop_flag = true;
				 //initialization = false;
				 set_motor(0,0);			
			 }
			 else
			 {
				//set_motion(position,pwm_value);
				 set_motor(0,40);	
			 }

		}
		else
		{
			position = convert2degree(avg_x[count_i],avg_y[count_i]);
			count_i++;
			if(count_i == 6 )
			{
				initialization = true;
				count_i = 0;
				
				//if(position != pwm_value)
				//{
					//set_motor(0,50);
				//}
			}
			
			
		}
		//verify_motion[verify_count] = position;
		//verify_count++;
		
		//if(verify_count == 499 )
		//{
		//	verify_count = 0;
		//}
		
		
	//}
}

void check_infinte_filter(uint32_t pwm_value)
{
	static int temp_x = 0;
	static int temp_y = 0;
	static int temp_z = 0;
	
	
	int count_char=0;
	volatile uint8_t print_output[10];
	
	
	//volatile static int minimum = 0;
	//volatile static int maximum = 0;
	int degree = 0;
	//static int test_count = 1001;
	
	static int temp_sum_x = 0;
	static int temp_sum_y = 0;
	static int temp_sum_z = 0;
	static uint16_t count_i = 0;
	int i =0;

	//volatile static int16_t verify_position_x[600];
	//volatile static int16_t verify_position_y[600];
	
	//volatile static int16_t verify_position[1000];
	//volatile static int16_t coinc_match[20];
	
	//volatile static int16_t verify_position_z[400];

	volatile static int16_t verify_count=0;
	static bool init_temp_sum = false;
	
	temp_x =  x_left_a[0];
	temp_y =  y_left_a[0] + Y_OFFSET;
	
	if(init_temp_sum == false)
	{
		temp_sum_x = temp_x;
		temp_sum_y = temp_y;
		init_temp_sum = true;
	}
	
	//
	temp_sum_x = (temp_sum_x*9 + temp_x*1)/10;
	temp_sum_y = (temp_sum_y*9 + temp_y*1)/10;
	
	degree		= convert2degree2( temp_sum_x , temp_sum_y);
	count_char	= convert(degree, print_output);
	usart_write_buffer_wait(&usart_instance, print_output , count_char);
	
	if(abs(degree-pwm_value) < 2)
	{
		stop_flag = true;
		//initialization = false;
		set_motor(0,0);
	}
	else
	{
		set_motion(degree,pwm_value);
		//set_motor(0,40);
	}

}


uint32_t check_pwm()
{
	static uint32_t present_mode= 0;
	//static uint32_t previous_mode= 0;
	if(pwm_available == true)
	{
		pwm_available = false;
		present_mode = find_mode(pwm_rawvalue);
		//previous_mode = present_mode;
	}
	
	return present_mode;
}

void check_motor(uint32_t present_pwm_value)
{
	static uint32_t prev_pwm_value = 0;
	static uint32_t temp = 0;
	
	//extint_chan_disable_callback(PWM_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	read_encoders();
	//extint_chan_enable_callback(PWM_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	
	if(present_pwm_value > 0)
	{
		if(abs(present_pwm_value - prev_pwm_value) > 2 )
		{
			prev_pwm_value = present_pwm_value ;
			temp = present_pwm_value;
		}
		if(position_error == 0)
		{
			check_infinte_filter(temp);
		}
		
	}
	else
	{
		set_motor(0,0);
	}
	
	
	//if(abs(present_pwm_value - prev_pwm_value) > 2  )//not equal to previous
	//{	
			// Disable external interrupts
			//--Read the sensor value
			// Enable external interrupts
			// Filter the sensor values and set the motor to motion 
			
			
			//extint_chan_disable_callback(PWM_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
			//tc_enable(&tc_instance1);
			//tc_enable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
			//Disable external Interrupts;
			/*
			stop_flag = false;
			prev_pwm_value = present_pwm_value ;
			temp = present_pwm_value;
			temp = temp;
			while(stop_flag == false )
			{
				check(temp);
			}
			*/
			//tc_disable(&tc_instance1);
			//extint_chan_enable_callback(PWM_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);

	//}
}


int main (void)
{	
	//
	uint32_t present_pwm_value = 0;
	
	//
	system_init();
	init_serial();
	
	//
	enable_motor();
	configure_encoder();
	
	
	//--Calibration 
	calibration_init();
	
	//
	//configure_pid_servo_control(); --Read encoders in the main loop
	//initialize_xyz();
	//set_motor(0,80) ;
	/* Insert application code here, after the board has been initialized. */
	//
	//
	//initialize_find_pwm();
	
	irq_initialize_vectors();
	cpu_irq_enable();
	
	delay_init();
	test_main = true;
	position_global = 10 ;
	while(1)
	{
		int i=0 ;
		//present_pwm_value = check_pwm();
		//for(i=0; i<840 ; i++)
		//{
			//delay_ms(1000);
			
			check_motor(position_global);
		//}
	}
}
