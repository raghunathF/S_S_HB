/*
 * serial.h
 *
 * Created: 10/13/2017 1:02:03 PM
 *  Author: raghu
 */ 

#define DBG_MODULE              SERCOM1
#define DBG_SERCOM_MUX_SETTING  USART_RX_3_TX_2_XCK_3
#define DBG_SERCOM_PINMUX_PAD0  PINMUX_UNUSED
#define DBG_SERCOM_PINMUX_PAD1  PINMUX_UNUSED
#define DBG_SERCOM_PINMUX_PAD2  PINMUX_PA30D_SERCOM1_PAD2
#define DBG_SERCOM_PINMUX_PAD3  PINMUX_PA31D_SERCOM1_PAD3


#define BAUDRATE_SERIAL_DEBUG 115200

void init_serial();
int convert(int k , uint8_t* output);
struct usart_module usart_instance;