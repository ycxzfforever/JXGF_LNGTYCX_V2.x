/*
 * lcngctrl.c
 *
 * Created: 2012/12/25 13:16:56
 *  Author: dell
 */ 

#include "global_def.h"

//unsigned int counter = 0;

void olcng_usr_prtrec(void)
{
	oprt_rec(false,0);  
}

void olcngctrl_ctrl(void)
{
	if(false == blcng_running) 
	{
	     return;
	}
	if(true == b_Simulation)
	{
	    osys_opower_chk();
		ocngctrl_simu_disp_pro_ex();
		
		if(cur_init_lng_chk >1000)
		{
			cur_init_lng_chk = 0;
			halt_err |= 1<<HALTERR_MODBUS_L_DATA_ERR;
			okernel_putevent(KEY_STOP_EV,0); 
		}
		return;
	}
	if(!omodbus_read_auto_buffer(_MODBUS_L_,5))
	{
		if(nmodbus_Errcnt++ > 10) 
		{
			blcng_running = false;
			halt_err |= 1<<HALTERR_MODBUS_L_AUTO_ERR;
			okernel_putevent(KEY_STOP_EV,0); 
		}
		return;
	}
	cur_flowrate_l  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_MASSFLOW);  			
	cur_totalize_l  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TOTALIZE); 			
    vftemp  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TEMPERATURE);
    vfdgain = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_DRVGAIN);
	ovctrl_getpressure(_MODBUS_L_, &cur_pressure_l);
///////////////
	if(!omodbus_read_auto_buffer(_MODBUS_G_,5))
	{
		if(nmodbus_Errcnt++ > 10) 
		{
			blcng_running = false;
			halt_err |= 1<<HALTERR_MODBUS_G_AUTO_ERR;
			okernel_putevent(KEY_STOP_EV,0); 
		}
		return;
	}
	nmodbus_Errcnt = 0;
	cur_flowrate_g  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_MASSFLOW);  			
	cur_totalize_g  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_TOTALIZE); 
	ovctrl_getpressure(_MODBUS_G_, &cur_pressure_g);		
	olngctrl_disp_pro_ex(); ////¼ÆËãÏÔÊ¾	
	if(cur_init_lng_chk >500)
	{
		cur_init_lng_chk = 0;
		halt_err |= 1<<HALTERR_MODBUS_L_DATA_ERR;
		okernel_putevent(KEY_STOP_EV,0); 
	}
}