#ifndef _CONSTANTS_H
#define _CONSTANTS_H

typedef unsigned char tByte;
typedef unsigned int tWord;
typedef unsigned long tLong;

typedef enum 
{ 
   STATE_OFF, STATE_ON, STATE_NO = 0, STATE_YES
} flags_state_t;

#define NULL_PTR                               ((void *) 0)
#define BACKSPACE_CHAR                         ('\b')
#define NULL_CHAR                              ('\0')
#define NEXT_LINE_CHAR                         ('\r')

typedef enum 
{ 
   POLLING, INTERRUPT 
} event_type_t; 
 
typedef enum 
{  
   DISP_NUM_DIGIT1 = 0x01, DISP_NUM_DIGIT2, DISP_NUM_DIGIT3, DISP_NUM_DIGIT4, DISP_NUM_DIGIT5, 
   DISP_HEX_DIGIT4, DISP_HEX_DIGIT3, DISP_HEX_DIGIT2,  DISP_HEX_DIGIT1
} disp_num_t;

typedef enum
{
	SUCCESS, FAILURE, ERR_NULL_PTR, ERR_INVALID_DATA, ERR_DATA_NOT_IN_RANGE, 
	ERR_UART_TX_NOT_READY, ERR_UART_RX_OVERFLOW,	NUM_ERR_STATUS	
} status_err_t; 

typedef struct
{
	long disp_data; 	
	unsigned int  disp_sign           : 1;
	unsigned int  num_digits_format   : 7;		
} disp_format_t;


#endif