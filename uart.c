/* ********************************************************************
FILE                 : uart.c

PURPOSE              : 
	 
AUTHOR               : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS           : 

NOTE                 : 								

CHANGE LOGS          :

*****************************************************************************/

#include "main.h"

// ------  data type declarations ---------------------------

// ------  function prototypes ------------------------------
static unsigned int UART_Rx_Char_Proc();

// ------  constants ----------------------------------------


// ------  variables ----------------------------------------
uart_data_t uart_data;


/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Init

DESCRIPTION    : Initialize UART port						 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Init(void)
{
	#ifdef UART_EVENT == INTERRUPT
		Global_Interrupt_Enable();
		ES = 1; //serial Interrupt_Enable
	#endif
	
	SCON = 0x50;    // setup serial port control 
  	TMOD = 0x20;    // hardware (9600 BAUD @11.0592MHZ) 
  	TH1  = 0xFD;    // TH1
  	TR1	 = 1;  		// Timer 1 on
	
	UART_Reset_Datas(UART_RESET_ALL_DATAS);	
	Delay_in_Milli_Sec(1);	
	return;
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Reset_Datas

DESCRIPTION    :					 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int UART_Reset_Datas(const unsigned char uart_reset_type)
{
	switch(uart_reset_type)
	{
		case UART_RESET_ALL_DATAS:
		case UART_RESET_ALL_RCVD_DATAS:
		    uart_data.rcvd_ready_flag = STATE_YES;		 		 
		    uart_data.echo_rcvd_data_flag = STATE_YES;
		case UART_RESET_READ_DATA:
    		uart_data.read_flag = STATE_YES;  
		case UART_RESET_RCVD_BUFFER:		
	        Str_Fill_Char(uart_data.rcvd_str, STR_MAX_NUM_CHARS ,NULL_CHAR); 
	        uart_data.rcvd_str_num_chars = 0;
		    uart_data.rcvd_overflow_flag = STATE_NO;
		    uart_data.rcvd_char = NULL_CHAR;
		    uart_data.rcvd_terminator_char_flag = STATE_NO;		   
		if(uart_reset_type != UART_RESET_ALL_DATAS)
			  break;
		case UART_RESET_ALL_TX_DATAS:
          	uart_data.tx_ready_flag = STATE_YES;
		break;
        default:
           return ERR_DATA_NOT_IN_RANGE; 		
	}
	return SUCCESS;
}
/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Tx_Char

DESCRIPTION    : Transmit Char by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int UART_Tx_Char(const char tx_char)
{
	if(uart_data.tx_ready_flag == STATE_YES)
	{
		SBUF = tx_char;
		uart_data.tx_ready_flag = STATE_NO; 
        if(UART_EVENT == POLLING)
		{			
	         while (TI==0); //Transmit Char by polling 
	         TI = 0;
		     uart_data.tx_ready_flag = STATE_YES; 
		}
		return SUCCESS; 
	}		
	return ERR_UART_TX_NOT_READY; 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Tx_Str

DESCRIPTION    : Transmit String by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int UART_Tx_Str(const char *const tx_str)
{
     unsigned char index;
	 
     for(index = 0; ((*(tx_str + index)) != NULL_CHAR); index++)
	 {
		  if(((UART_Tx_Char(*(tx_str + index))) != SUCCESS))
		      return ERR_UART_TX_NOT_READY;		 
	 }
	 return SUCCESS; 
}

#ifdef UART_EVENT == INTERRUPT
/*------------------------------------------------------------*-
FUNCTION NAME  : Serial_ISR 

DESCRIPTION    : UART by Interrupt 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void Serial_ISR(void) interrupt 4
{
	if(RI == 1)
	{
		uart_data.rcvd_ready_flag = STATE_NO; 
		uart_data.rcvd_char = SBUF; 
		UART_Rx_Char_Proc();		
		RI = 0; //clear uart receive flag		
	}
	if(TI == 1)
	{
	    TI = 0;           //clear uart transmit flag
	    uart_data.tx_ready_flag = STATE_YES; 
	}	
}	

#else
/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Rx_Char

DESCRIPTION    : Receive Char by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
char UART_Rx_Char()
{
	if(uart_data.rcvd_str_num_chars + 1 >= STR_MAX_NUM_CHARS )
	{
		uart_data.rcvd_overflow_flag = STATE_YES;
		return NULL_CHAR;
	}
	uart_data.rcvd_overflow_flag = STATE_NO;
	if(uart_data.rcvd_ready_flag == STATE_YES ) 
	{
		  uart_data.rcvd_ready_flag = STATE_NO; 
	      while (RI==0);
	      uart_data.rcvd_char = SBUF;
		  UART_Rx_Char_Proc();
		  RI = 0;		
		return(uart_data.rcvd_char);		
	}
	return NULL_CHAR;  	
}
#endif

/*------------------------------------------------------------*-
FUNCTION NAME  :  

DESCRIPTION    : 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int UART_Read(const char **const read_str_ptr, const unsigned char *read_num_chars_ptr)
{
	if(read_str_ptr == NULL_PTR || read_num_chars_ptr == NULL_PTR)
	{
		UART_Tx_Str("ERR: UART tead null ptr \r");
		return ERR_NULL_PTR;
	}
	*read_str_ptr = uart_data.rcvd_str;
	read_num_chars_ptr = &uart_data.rcvd_str_num_chars; 
		
	return SUCCESS;
}
/*------------------------------------------------------------*-
FUNCTION NAME  :  UART_Rx_Char_Proc

DESCRIPTION    : 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
static unsigned int UART_Rx_Char_Proc()
{
	uart_data.read_flag = STATE_NO;
	if(uart_data.rcvd_str_num_chars + 1 < STR_MAX_NUM_CHARS)
	{
		 uart_data.rcvd_overflow_flag = STATE_NO;
         switch(uart_data.rcvd_char)
		 {
              case BACKSPACE_CHAR:
		         if(uart_data.rcvd_str_num_chars > 0)
			     {
			        if(uart_data.echo_rcvd_data_flag == STATE_YES)
			             if(((UART_Tx_Char(uart_data.rcvd_char)) != SUCCESS))
							 return ERR_UART_TX_NOT_READY;
			         uart_data.rcvd_str[uart_data.rcvd_str_num_chars--] = NULL_CHAR;
			     }
			  break;
              case TERMINATOR_CHAR:
			     if(uart_data.echo_rcvd_data_flag == STATE_YES)
				      if(((UART_Tx_Char(uart_data.rcvd_char)) != SUCCESS))
							return ERR_UART_TX_NOT_READY; 
                 uart_data.rcvd_terminator_char_flag = STATE_YES;
              break;
              default:
			    if(uart_data.echo_rcvd_data_flag == STATE_YES)
			         if(((UART_Tx_Char(uart_data.rcvd_char)) != SUCCESS))
						  return ERR_UART_TX_NOT_READY; 
			    uart_data.rcvd_terminator_char_flag = STATE_NO; 
		        uart_data.rcvd_str[uart_data.rcvd_str_num_chars++] = uart_data.rcvd_char;
		 }
		 uart_data.rcvd_ready_flag = STATE_YES;
	}
	else
	{
        uart_data.rcvd_overflow_flag = STATE_YES;
		return ERR_UART_RX_OVERFLOW;
	}
	return SUCCESS;  		 
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Tx_Num

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
 unsigned int UART_Tx_Num(const disp_format_t uart_disp_data)
{
    unsigned int tens_thousand_digit,thousands_digit, hundreds_digit,tens_digit, unit_digit;
	unsigned long disp_num, num ;
	char num_data[] ={'0','1','2','3','4','5','6','7','8','9'};  
	char hex_data[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; 
		
	if(uart_disp_data.disp_sign  == STATE_YES) //signed lcd_datanum_disp_format
	{
		if(uart_disp_data.disp_data < 0 )
		{
			UART_Tx_Char('-');
			disp_num = -uart_disp_data.disp_data;
		}
        else
		{
			UART_Tx_Char('+');
			disp_num = uart_disp_data.disp_data;
		}        
	}
	else
	{
    	disp_num = uart_disp_data.disp_data; 
	}
	num =  disp_num ;
	
    switch(uart_disp_data.num_digits_format)
	{
	  case DISP_NUM_DIGIT5:
	 	  num =  disp_num ;
		  tens_thousand_digit = (unsigned int)(num / (unsigned long)(10000UL));
          UART_Tx_Char(num_data[tens_thousand_digit]);
	  case DISP_NUM_DIGIT4:
	      num = disp_num % 10000UL;
	      thousands_digit = (unsigned int)(num / (unsigned long)(1000UL));
	      UART_Tx_Char(num_data[thousands_digit]); 
	  case DISP_NUM_DIGIT3:
	      num = disp_num % 1000UL;
	      hundreds_digit = (unsigned int) (num / (unsigned long) (100));
	      UART_Tx_Char(num_data[hundreds_digit]);
	  case DISP_NUM_DIGIT2:
	      num = disp_num % 100;
          tens_digit = (unsigned int) (num / 10);
          UART_Tx_Char (num_data[tens_digit]); 		  
	  case DISP_NUM_DIGIT1:
	      unit_digit = (unsigned int) (disp_num % 10);
          UART_Tx_Char(num_data[unit_digit]); 
	  break;
	  case DISP_HEX_DIGIT4:
	      //  ( 16 * 16 * 16 *16 )  = 0 as divide by zero warning 
	      //num = disp_num % ( 16 * 16 * 16 *16 );
          thousands_digit = (num / (unsigned long) (16 * 16 * 16));
	      UART_Tx_Char(hex_data[thousands_digit]);
	  case DISP_HEX_DIGIT3:
	      num = disp_num %(unsigned long)(16 * 16 * 16);
	      hundreds_digit = (unsigned int) (num / (unsigned long) (16 * 16));
	      UART_Tx_Char(hex_data[hundreds_digit]);
	  case DISP_HEX_DIGIT2:
	      num = disp_num %(unsigned long)(16 * 16);
          tens_digit = (unsigned int) (num / 16);
          UART_Tx_Char(hex_data[tens_digit]);
	  case DISP_HEX_DIGIT1: 
	      unit_digit = (unsigned int) (disp_num % 16);
          UART_Tx_Char(hex_data[unit_digit]);    
	  break;
	  default:
	     return ERR_DATA_NOT_IN_RANGE;	  
	} 
    return SUCCESS;   	
}
		 
/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/

