/*
 * serial_debug.c
 *
 * Created: 10/13/2017 1:01:30 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "serial.h"





void configure_usart(void)
{
	//! [setup_config]
	struct usart_config config_usart;
	//! [setup_config]
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_usart.baudrate    = BAUDRATE_SERIAL_DEBUG;
	config_usart.mux_setting = DBG_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = DBG_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = DBG_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = DBG_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = DBG_SERCOM_PINMUX_PAD3;
	//! [setup_change_config]

	//! [setup_set_config]
	while (usart_init(&usart_instance,DBG_MODULE, &config_usart) != STATUS_OK) 
	{
	}
	//! [setup_set_config]

	//! [setup_enable]
	usart_enable(&usart_instance);
	//! [setup_enable]
}

void reverse(uint8_t* input, int count)
{
	int i =0;
	int temp =0;
	
	for(i=0;i<count/2;i++)
	{
		temp = input[i]; 
		input[i]= input[count-1-i];
		input[count-1-i] = temp;
	}
	
	
}


 
 int convert(int k , uint8_t* output)
 {

	 int temp = 0;
	 int count = 0;
	 temp = abs(k);
	 while(temp > 0 )
	 {
		 output[count]= (temp % 10) + 0x30 ;
		 temp = temp / 10 ;
		 count++;
	 }
	 if(k < 0)
	 {
		 output[count] = '-';
		 count ++;
	 }
	 else
	 {
		 if(k == 0)
		 {
			  output[count] = 0x30;
			  count++;
			  output[count] = '\n';
			  count++;
			  output[count] = '\r';
			  count++;
			  return count;
		 }
	 }
	 reverse(output,count);
	 output[count] = '\n';
	 count++;
	 output[count] = '\r';
	 count++;
	 return count ;

 }


void init_serial()
{
	
	//uint8_t exit_string[] = {"X"};
	configure_usart();
	//int k =0;
	int count= 0;
	int i =0;
	//count = 1;
	//for(i= -100;i <100 ; i++)
	//{
		//count = convert(i, print_output);
		//usart_write_buffer_wait(&usart_instance, print_output , count);
	//}
	//usart_write_buffer_wait(&usart_instance, exit_string , sizeof(exit_string));
}