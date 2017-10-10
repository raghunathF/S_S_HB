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

#define CLOCKWISE 0
#define ANTI_CLOCKWISE 1

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

#define MAX_SPEED 80
#define MIN_SPEED  35
#define MAX_PARTION 560
#define HALF_PARTION 280

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
			direction_motion = CLOCKWISE;
		}
		else
		{
			min_diff =  abs(position - pwm_value);
			direction_motion = ANTI_CLOCKWISE;
		}
		
	}
	else
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
	volatile static uint16_t verify_motion[500];
	volatile static uint16_t verify_count=0;
	
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
				set_motion(position,pwm_value);
				 //set_motor(0,0);	
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
		verify_motion[verify_count] = position;
		verify_count++;
		
		if(verify_count == 499 )
		{
			verify_count = 0;
		}
		
		
	//}
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
			check(temp);
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
	
	system_init();
	enable_motor();
	configure_encoder();
	
	//
	//
	//configure_pid_servo_control(); --Read encoders in the main loop
	//initialize_xyz();
	//set_motor(0,80) ;
	/* Insert application code here, after the board has been initialized. */
	//
	//
	
	initialize_find_pwm();
	
	irq_initialize_vectors();
	cpu_irq_enable();
	
	while(1)
	{
		int i=0 ;
		present_pwm_value = check_pwm();
		check_motor(present_pwm_value);
	}
}
