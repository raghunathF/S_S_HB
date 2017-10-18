/*
 * calibration.c
 *
 * Created: 10/10/2017 9:53:40 AM
 *  Author: raghu
 */ 

#include <asf.h>
#include "calibration.h"
#include "servo_control.h"
#include "i2c_encoders.h"
#include "serial.h"

volatile static bool calibration_done = false;
#define MAX_CALIBRATION_COUNT 1500



int  convert2degree2(volatile int x , volatile int y)
{
	volatile static int pos;
	static int count = 0;
	static int region = 0;
	int quad_value = intersect_max_value + abs(intersect_min_value);
	//count++;
	if(x >= intersect_max_value )
	{
		//1st Quad
		region = 1;
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
		region = 2;
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
		region = 3;
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
	
	else if( y >= intersect_max_value)
	{
		//4th Quad
		region = 4;
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
		region = 5;
		if((x<intersect_max_value+10) &&(x>intersect_max_value-10)  )
		{
			if(y>0)
			{
				pos = intersect_max_value - y;
				//if(pos < 0)
				//{
				//	pos= 0 
				//}
			}
			else
			{
				pos = intersect_max_value + abs(y);
				//if(pos > quad_value)
				//{
				//	pos = quad_value;
				//}
			}
		}
		else if((y<intersect_max_value+10) &&(y>intersect_max_value-10)  )
		{
			if(x>0)
			{
				pos = (abs(intersect_min_value) + x) + quad_value*3 ;
			}
			else
			{
				pos = abs(intersect_min_value) - abs(x)  + quad_value*3;
			}
		}
		else if((y<intersect_min_value+10) &&(y>intersect_min_value-10)  )
		{
			if(x>0)
			{
				pos = (intersect_max_value - x) + quad_value ;
			}
			else
			{
				pos = intersect_max_value + abs(x) + quad_value;
			}
		}
		else if((x<intersect_min_value+10) &&(x>intersect_min_value-10)  )
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
		else
		{
			pos = 0;
		}
		
		//pos = 0;
	}
	
	return pos;
	
}


int find_max(int16_t* coinc_match,int count)
{
	int i = 0 ;
	int max_value = 0;
	for(i = 0 ; i <count ; i++ )
	{
		if(max_value < coinc_match[i])
		{
			max_value = coinc_match[i];
		}
	}
	return max_value;
}


int find_min(int16_t* coinc_match,int count)
{
	int i = 0 ;
	int min_value = 0;
	for(i = 0 ; i <count ; i++ )
	{
		if(min_value > coinc_match[i])
		{
			min_value = coinc_match[i];
		}
	}
	return min_value;
}



void infinite_filter()
{
	static int temp_x = 0;
	static int temp_y = 0;
	static int temp_z = 0;
	
	volatile static int minimum = 0;
	volatile static int maximum = 0;
	int degree = 0;
	 static int test_count = 1001;
	
	static int temp_sum_x = 0;
	static int temp_sum_y = 0;
	static int temp_sum_z = 0;
	static uint16_t count_i = 0;
	int i =0;

	//volatile static int16_t verify_position_x[600];
	//volatile static int16_t verify_position_y[600];
		
	volatile static int16_t verify_position[1000];
	volatile static int16_t coinc_match[20];
	
	//volatile static int16_t verify_position_z[400];

	volatile static int16_t verify_count=0;
	static bool init_temp_sum = false;
	
	temp_x =  x_left_a[0];
	temp_y =  y_left_a[0] +Y_OFFSET;
	
	if(init_temp_sum == false)
	{
		temp_sum_x = temp_x;
		temp_sum_y = temp_y;
		init_temp_sum = true;
	}
	
	//
	temp_sum_x = (temp_sum_x*9 + temp_x*1)/10;
	temp_sum_y = (temp_sum_y*9 + temp_y*1)/10;
	
	if((temp_sum_x == temp_sum_y) && (temp_sum_x !=0))
	{
		//coinc_match[count_i] = i;
		//count_i++;
		coinc_match[count_i] = temp_sum_x;
		count_i++;
		if(count_i == 6 && test_count == 1001)
		{
		
			 set_motor(0,0);
			 intersect_min_value = find_min(coinc_match,count_i);
			 intersect_max_value = find_max(coinc_match,count_i);
			 if((intersect_max_value != intersect_min_value)&&(intersect_min_value!=0)&&(intersect_max_value!=0))
			 {
				  //calibration_done = true;
				  test_count = 0;
			 }
			 else
			{
				 count_i = 0;
			 }
		}
		
		
		
	}
	if(test_count < 1000 )
	{
		int count_char=0;
		volatile uint8_t print_output[10];
		
		set_motor(0,50);
		
		degree = convert2degree2( temp_sum_x , temp_sum_y);
		count_char = convert(degree, print_output);
		usart_write_buffer_wait(&usart_instance, print_output , count_char);
		test_count++;
		verify_position[test_count] = degree;		
	
	}
	if(test_count == 1000)

	{
		test_count = 0;
		calibration_done = true;
	}
	//
	/*
	verify_position_x[verify_count] = temp_sum_x;
	verify_position_y[verify_count] = temp_sum_y;
	
	//
	verify_count++;
	
	if(verify_count == 580 )
	{
		set_motor(0,0);
	}
	if(verify_count == 599 )
	{
		verify_count = 0;
		
		for(i=0;i<600;i++)
		{
			if(verify_position_x[i] < minimum)
			{
				minimum = verify_position_x[i]  ;
			}
			else if(verify_position_x[i] > maximum)
			{
				 maximum = verify_position_x[i] ;
			}
			if(verify_position_x[i] == verify_position_y[i])
			{
				coinc_match[count_i] = i;
				count_i++;
				coinc_match[count_i] = verify_position_x[i];
				count_i++;
				
			}
		}
	}
	*/
}




void tc_callback_calibration()
{
	//static int count_calibration = 0 ;
	
	//Read the encoder values and store them
	//get_data_encoders(i2c_master_encoder);
	static int i=0;
	static int count_test = 0;
	static int position_test[10] = {10,100,200,300,400,500,600,700,800,10};
		//,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,
		//220,230,240,250,260,270,280,290,300,310,320,330,340,350,360,370,380,390,400,410,420,420,440,450,460,470,480,490,500,510,520,530,540,550,
		//560,570,580,590,600,610,620,630,640,650,660,670,680,690,700,710,720,730,740,740,760,770,780,790,800,810,820,830,840,10};
	if(test_main == false)
	{
		read_encoders();
		infinite_filter();
	}
	else
	{
		count_test++;
		if(count_test == 1000)
		{
			count_test = 0;
			position_global = position_test[i];
			i++;
			if(i==10)
			{
				i =0;
			}
		}
	}
	
	//Count for a single rotation 
	//Change the flag  to indicate that calibration is done.
	//count_calibration++;
	
	//if(count_calibration == MAX_CALIBRATION_COUNT)
	//{
		//count_calibration = 0;
		//calibration_done = true;
	//}
	
}

void config_tc1_calibration()
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT; //8
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV256; //64
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	config_tc.counter_8_bit.period = 0x55;
	tc_init(&tc_instance1, TC1, &config_tc);
	tc_enable(&tc_instance1);
}


void config_callbacks_tc1_calibration()
{
	tc_register_callback(&tc_instance1, tc_callback_calibration ,TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
}


void timer_calibration_init()
{
	config_tc1_calibration();
	config_callbacks_tc1_calibration();
}

void calibration_init()
{
	set_motor(1,50);
	timer_calibration_init();
	while(calibration_done == false)
	{}
	//tc_disable(&tc_instance1);
	//tc_disable_callback(&tc_instance1, TC_CALLBACK_OVERFLOW);
	//disable the timer and interrupts
}