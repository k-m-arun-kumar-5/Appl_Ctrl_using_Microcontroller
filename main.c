/* ********************************************************************
FILE                   : main.c

PROGRAM DESCRIPTION    : Program for the 8051 serial terminal command based home automation by controlling appliances 
                         Command - 1: FAN  1 - ON  , 2: FAN  1 - OFF,  
						           3: LAMP 1 - ON  , 4: LAMP 1 - OFF  
                                   5: TV   1 - ON  , 6: TV   1 - OFF 						 
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  :  			
                                    
CHANGE LOGS           : 

*****************************************************************************/

#include "main.h"
static char *ble_rcvd_str; 
static unsigned char ble_rcvd_str_num_chars;
unsigned int UART_Menu();
unsigned int BLE_Cmd_Proc();
void GPIO_Init();
/*------------------------------------------------------------*-
FUNCTION NAME  : main

DESCRIPTION    :          
								
INPUT          : none

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/

void main(void)
{
	 /* ========= begin : USER CODE INITIALIZE ========== */

	 GPIO_Init(); 
	 UART_Init(); 
	 UART_Menu();
	
   /* ========= end : USER CODE INITIALIZE ========== */
	   
     while(1) // Super Loop
     {
		  while(uart_data.rcvd_terminator_char_flag == STATE_NO);
          UART_Read(&ble_rcvd_str, &ble_rcvd_str_num_chars);
		  UART_Tx_Str("Entered BLE CMD : ");
          UART_Tx_Str(ble_rcvd_str);
		  UART_Tx_Str("\r\r");
          if(((BLE_Cmd_Proc()) != SUCCESS))
		  {
			  ;
		  }			  
		  UART_Menu(); 
	 }	   
	 return; 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : GPIO_Init

DESCRIPTION    :          
								
INPUT          : none

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void GPIO_Init()
{
	 ERROR_LED = 0;         //output for MCU	
     LAMP_01_LED = 0;
	 FAN_01_LED = 0;
	 TV_01_LED = 0;
 	 DRIVER_LAMP_01 = 0;
     DRIVER_FAN_01 = 0;
	 DRIVER_TV_01 = 0;
	 
	 ERROR_LED = STATE_OFF;
	 LAMP_01_LED = STATE_OFF;
	 FAN_01_LED = STATE_OFF;
	 TV_01_LED = STATE_OFF;
	 DRIVER_LAMP_01 = STATE_OFF;
	 DRIVER_FAN_01 = STATE_OFF;
	 DRIVER_TV_01 = STATE_OFF; 
	//test ok
	/* DRIVER_FAN_01 = STATE_ON;
	 DRIVER_LAMP_01 = STATE_ON; 
	 DRIVER_TV_01 = STATE_ON */
	 return;
}	 
/*------------------------------------------------------------*
FUNCTION NAME  : UART_Menu

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned int UART_Menu()
{
	  UART_Reset_Datas(UART_RESET_ALL_DATAS);
	  ble_rcvd_str_num_chars = 0;
	  UART_Tx_Str("MENU: 1: FAN  1 - ON  , 2: FAN  1 - OFF \r");
	  UART_Tx_Str("      3: LAMP 1 - ON  , 4: LAMP 1 - OFF \r");
	  UART_Tx_Str("      5: TV   1 - ON  , 6: TV   1 - OFF \r");
	  UART_Tx_Char('\r');
      UART_Tx_Str("INPUT: Enter BLE CMD : ");  
      return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : BLE_Cmd_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
unsigned int BLE_Cmd_Proc()
{
	static char lamp_str[] = "LAMP ", fan_str[] = "FAN ", tv_str[] = "TV", on_str[] = " ON", off_str[] = " OFF";
	static char appl_id[] = {'1'};			
	if(ble_rcvd_str_num_chars > 1)
	{
		UART_Tx_Str("ERR: Rcvd BLE Str len \r");
		return ERR_DATA_NOT_IN_RANGE;
	}
	ERROR_LED = STATE_OFF;
	switch(ble_rcvd_str[0])
	{
		case '1':
		   DRIVER_FAN_01 = STATE_ON; 
		   FAN_01_LED = STATE_ON;			   
		   UART_Tx_Str(fan_str);
		   UART_Tx_Char(appl_id[0]); 
		   UART_Tx_Str(on_str);
		break;
		case '2':
		   DRIVER_FAN_01 = STATE_OFF;
           FAN_01_LED = STATE_OFF;	
		   UART_Tx_Str(fan_str);
		   UART_Tx_Char(appl_id[0]); 
		   UART_Tx_Str(off_str);
		break;
		case '3':
		   DRIVER_LAMP_01 = STATE_ON;
		   LAMP_01_LED = STATE_ON;
		   UART_Tx_Str(lamp_str);
		   UART_Tx_Char(appl_id[0]); 
		   UART_Tx_Str(on_str);
		break;
		case '4':
		   DRIVER_LAMP_01 = STATE_OFF; 
		   LAMP_01_LED = STATE_OFF;
		   UART_Tx_Str(lamp_str);
		   UART_Tx_Char(appl_id[0]); 
		   UART_Tx_Str(off_str);
		break;	
        case '5':
		   DRIVER_TV_01 = STATE_ON; 
		   TV_01_LED = STATE_ON;
		   UART_Tx_Str(tv_str);
		   UART_Tx_Char(appl_id[0]); 
		   UART_Tx_Str(on_str);
		break;	 
		case '6':
		   DRIVER_TV_01 = STATE_OFF; 
		   TV_01_LED = STATE_OFF;
		   UART_Tx_Str(tv_str);
		   UART_Tx_Char(appl_id[0]); 
		   UART_Tx_Str(off_str);
		break;	 
		default:
		   UART_Tx_Str("ERR: Invalid BLE cmd \r\r");
		   ERROR_LED = STATE_ON; 
		   return ERR_DATA_NOT_IN_RANGE;
	}	
    UART_Tx_Str("\r\r");	
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Interrupt_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
void Global_Interrupt_Enable()
{
	EA = 1;
    return; 	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Delay_in_Micro_Sec

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
void Delay_in_Micro_Sec(const unsigned long num_micro_seconds)
{
	unsigned int i = 0;
	
	for (i = 0;  i < num_micro_seconds; ++i);
	return; 	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Delay_in_Milli_Sec

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :   
-*------------------------------------------------------------*/
void Delay_in_Milli_Sec(const unsigned long num_milli_seconds)
{
	unsigned int i = 0;
	
	for (i = 0;  i < num_milli_seconds; ++i)
	  Delay_in_Micro_Sec(MAX_NUM_CNT_MILLI_SEC);
	return;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Str_Fill_Char

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
unsigned int Str_Fill_Char(char *const dest_str, const unsigned char num_chars, const char fill_char)
{
	unsigned char index;
	
	if(dest_str == NULL_PTR || num_chars > STR_MAX_NUM_CHARS)
	{
		UART_Tx_Str("ERR: Str_Fill\r");
		return ERR_INVALID_DATA;
	}
	for (index = 0 ; index < num_chars ; ++index)
	   *(dest_str + index)  = fill_char;
    *(dest_str + index) = NULL_CHAR;
	
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/	 
