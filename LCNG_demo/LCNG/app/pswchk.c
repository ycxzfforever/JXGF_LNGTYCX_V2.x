/*
 * pswchk.c
 *
 * Created: 2012-8-15 14:45:54
 *  Author: luom
 */ 
#include "global_def.h"

void oapp_psw_chk_ok_pro(void)
{
	if( strlen((char*)cur_input_cursor.str_input) == 4 ) 
	{
		if(memcmp((char*)cur_input_cursor.str_input,psw,4) == 0)
		{
			okernel_putevent(LCNGCTRL_START_EV,0);		
			return;
		}
	}
	ocomm_message(4,1,"ÃÜÂë´íÎó",20, NULL);	
}