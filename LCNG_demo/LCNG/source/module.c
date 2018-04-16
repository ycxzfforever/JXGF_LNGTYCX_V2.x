/*
 * module.c
 *
 * Created: 2012-8-9 16:33:03
 *  Author: luom
 */ 
#include "global_def.h"

//static unsigned char cur_module_test_sw = 0;
//static unsigned char cur_module_page_cnt = 0;
volatile unsigned char cur_module_test_sw = 0;
volatile unsigned char cur_module_page_cnt = 0;
void omodule_event_menu_ret_pro(void);

/*
void omodule_test_pressure_ret_pro(void)
{
	unsigned char id = odisp_menu_getid();
	omodule_event_pro(id);
}
*/
//float	       cur_ad_arr[16];
//unsigned char  cur_ad_arr_len;
void omodule_test_presure_disp_pro(void)
{
	unsigned int     curadv = 0;	
	//unsigned char    i = 0;
	double			 tmp = 0.0;

	float		     pzero  = 0;
	float            pratio = 0;
    if(_PRESSURE_G_SW_ != cur_module_test_sw && _PRESSURE_L_SW_ != cur_module_test_sw) return;
	if(_PRESSURE_G_SW_ == cur_module_test_sw)
	{
		pzero  = cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO];    //压力零点
		pratio = cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT]; //比例系数  
	}
	else if(_PRESSURE_L_SW_ == cur_module_test_sw)
	{
		pzero  = cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO];    //压力零点
		pratio = cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT]; //比例系数  
	}	
		
    curadv = ovctrl_get_pressure_adc(cur_module_test_sw);
	ocomm_printf(CH_DISP,3,1,"当前AD值:%d", (unsigned int)curadv);	
	tmp =  curadv;
	tmp =  tmp - pzero;
	tmp    = tmp/pratio;
	/*
	cur_ad_arr[cur_ad_arr_len++] =  tmp;
	if(cur_ad_arr_len >= 16) cur_ad_arr_len = 0;
	tmp = 0;
	for(i=0; i<16; i++)
	{
		tmp += 	cur_ad_arr[i];
	}		
	ocomm_printf(CH_DISP,4,1,"压力:%.02f",(double)tmp/16); 
       */
       ocomm_printf(CH_DISP,4,1,"压力:%.02f",tmp); 
	otimer_reqire(LCNGCTRL_DISP_EV, 1, 0, 1);       
}
/*---------------------------------------------------------------------------------

----------------------------------------------------------------------------------*/
extern void omodule_test_pressure(void);
void omodule_test_pressure_set_zero_pro(void)
{
	unsigned int  curadv = ovctrl_get_pressure_adc(cur_module_test_sw);
	unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
    olcm_clrscr();
    ocomm_printf(CH_DISP,1,1,"压力零点设置"); 
    ocomm_printf(CH_DISP,2,1,"零点AD值：%d",curadv); 
	if(_PRESSURE_G_SW_ == cur_module_test_sw) 
	{
		cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO] = (float)curadv*1.0;
	}
	else if(_PRESSURE_L_SW_ == cur_module_test_sw)
	{
		cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO] = (float)curadv*1.0;
	}
	else
	{
		okernel_putevent(KEY_RET_EV,0);
		return;
	}		
    eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0]  , (unsigned int*)eepaddr ,sizeof(struct def_SysParas)); 
    ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]); 
	ocomm_message(4,1,"设置成功",10,NULL); //dlycnt
}
void omodule_test_pressure_ratio_ok_pro(void)
{
	int	  dlycnt  = 10;	
	float vftmp = 0.0 ;
	unsigned int eepaddr = EEPROM_SYSPARAS_ADDR;
	//unsigned int vpadv_max = ovctrl_get_pressure_adc(cur_module_test_sw);  
	float vpadv_max = (float)ovctrl_get_pressure_adc(cur_module_test_sw);
	
	if(strlen((char*)cur_input_cursor.str_input) >0 ) 
	{
       //vftmp = atof((char*)cur_input_cursor.str_input);
	  vftmp = atof((char*)cur_input_cursor.str_input)*100;  
	  if(_PRESSURE_G_SW_ == cur_module_test_sw)
	  {
		    //dlycnt = 10;
			vpadv_max = vpadv_max - cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO];
			vpadv_max = vpadv_max*10000;
			cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT] = vpadv_max/(vftmp*100);
			//cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT] = (vpadv_max - cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO])/vftmp;
	  }	  		
	  else if(_PRESSURE_L_SW_ == cur_module_test_sw)
	  {

			vpadv_max = vpadv_max - cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO];
			vpadv_max = vpadv_max*10000;
			cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT] = vpadv_max/(vftmp*100);			
			//cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT] = (vpadv_max - cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO])/vftmp;		
	  }	  		
 	  eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0]  , (unsigned int*)eepaddr  ,sizeof(struct def_SysParas)); 
	  ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
	  ocomm_message(4,1,"设置成功",dlycnt,NULL);
	  return;
	}
	ocomm_message(4,1,"    ",dlycnt,NULL);
}

void omodule_test_pressure_set_ratio_pro(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = true;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, omodule_test_pressure);
    okernel_addeventpro(KEY_OK_EV,  omodule_test_pressure_ratio_ok_pro);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"压力系数设置"); 
	ocomm_printf(CH_DISP,2,1,"请输入压力值:"); 
	
}
/*
Task_Event  _OMODULE_TEST_PG_SET_Pro[]  = 
{
  {KEY_RET_EV,			omodule_test_pressure}, 		  
  {KEY_1_EV,			omodule_test_pressure_set_zero_pro}, 		  
  {KEY_2_EV,			omodule_test_pressure_set_ratio_pro}, 		  	  	  
};
*/

void omodule_test_pressure_set_pro(void)
{
   otimer_release(LCNGCTRL_DISP_EV); 	  	
   olcm_clrscr();
   if(_PRESSURE_G_SW_ == cur_module_test_sw)      ocomm_printf(CH_DISP,1,1,"气相压力设置"); 
   else if(_PRESSURE_L_SW_ == cur_module_test_sw) ocomm_printf(CH_DISP,1,1,"液相压力设置"); 
   ocomm_printf(CH_DISP,2,1,"按【1 】  零点"); 
   ocomm_printf(CH_DISP,3,1,"按【2 】  系数"); 
   okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, omodule_test_pressure);
    okernel_addeventpro(KEY_1_EV,  omodule_test_pressure_set_zero_pro);	   
	okernel_addeventpro(KEY_2_EV,  omodule_test_pressure_set_ratio_pro);	   
}

void omodule_test_pressure(void)
{
   olcm_clrscr();
   /*ocomm_printf(CH_DISP,1,1,"零点:%d",(unsigned int)cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO]);
   ocomm_printf(CH_DISP,2,1,"系数:%.02f",cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT]);  
   if(_PRESSURE_G_SW_ == cur_module_test_sw)      ocomm_printf(CH_DISP,1,7,"气相"); 
   else if(_PRESSURE_L_SW_ == cur_module_test_sw) ocomm_printf(CH_DISP,1,7,"液相"); */
   ocomm_printf(CH_DISP,1,1,"零点:");
   ocomm_printf(CH_DISP,2,1,"系数:");
   if(_PRESSURE_G_SW_ == cur_module_test_sw){
      ocomm_printf(CH_DISP,1,4,"%.02f",(double)cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO]);
	  ocomm_printf(CH_DISP,2,4,"%.02f",(double)cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT]);  
   }   	  
   else if(_PRESSURE_L_SW_ == cur_module_test_sw){
	 ocomm_printf(CH_DISP,1,4,"%.04f",(double)cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO]);
	 ocomm_printf(CH_DISP,2,4,"%.04f",(double)cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT]);  
   }   	 
   if(_PRESSURE_G_SW_ == cur_module_test_sw)      ocomm_printf(CH_DISP,1,8,"气"); 
   else if(_PRESSURE_L_SW_ == cur_module_test_sw) ocomm_printf(CH_DISP,1,8,"液"); 
      
    okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV,      omodule_event_menu_ret_pro);
    okernel_addeventpro(KEY_STOP_EV,     omodule_test_pressure_set_pro);	   
	okernel_addeventpro(LCNGCTRL_DISP_EV,omodule_test_presure_disp_pro);
	   
    otimer_reqire(LCNGCTRL_DISP_EV, 1, 0, 1); 	  //100ms//otimer_reqire(LCNGCTRL_DISP_EV, 1, 1, 1); 	  //100ms
}


void omodule_test_vctrl_lowv_pro(void)
{
   VCTRL_MID_OUT();
   VCTRL_HIGH_OUT();
   VCTRL_LOW_OUT();
   VCTRL_KEY_OUT();  
   
    VCTRL_OPENKEY();
    VCTRL_MID_CLOSE();
    VCTRL_HIGH_CLOSE();
	ocomm_printf(CH_DISP,3,1,"●  ○  ○  ○");
    VCTRL_LOW_OPEN();	
}
void omodule_test_vctrl_midv_pro(void)
{
    VCTRL_MID_OUT();
   VCTRL_HIGH_OUT();
   VCTRL_LOW_OUT();
   VCTRL_KEY_OUT(); 
   
    VCTRL_OPENKEY();
    VCTRL_LOW_CLOSE();
    VCTRL_HIGH_CLOSE();
	ocomm_printf(CH_DISP,3,1,"○  ●  ○  ○");
    VCTRL_MID_OPEN();	
}
void omodule_test_vctrl_highv_pro(void)
{
   VCTRL_MID_OUT();
   VCTRL_HIGH_OUT();
   VCTRL_LOW_OUT();
   VCTRL_KEY_OUT(); 
   
    VCTRL_OPENKEY();
    VCTRL_LOW_CLOSE();
    VCTRL_MID_CLOSE();
	ocomm_printf(CH_DISP,3,1,"○  ○  ●  ○");
    VCTRL_HIGH_OPEN();	
}

void omodule_test_vctrl_allv_pro(void)
{
    VCTRL_MID_OUT();
   VCTRL_HIGH_OUT();
   VCTRL_LOW_OUT();
   VCTRL_KEY_OUT(); 
   
    VCTRL_OPENKEY();
	ocomm_printf(CH_DISP,3,1,"●  ●  ●  ●");
    VCTRL_LOW_OPEN();	
	VCTRL_MID_OPEN();	
	VCTRL_HIGH_OPEN();	
}

void omodule_test_vctrl_allstop_pro(void)
{
     VCTRL_MID_OUT();
     VCTRL_HIGH_OUT();
     VCTRL_LOW_OUT();
     VCTRL_KEY_OUT(); 
	 
      VCTRL_CLOSEKEY();
	ocomm_printf(CH_DISP,3,1,"○  ○  ○  ○");
      VCTRL_LOW_CLOSE();	
      VCTRL_MID_CLOSE();	
      VCTRL_HIGH_CLOSE();	
}

/*
Task_Event  _OMODULE_VCTRL_Pro[]  = 
{
  {KEY_RET_EV,			omodule_event_menu_ret_pro}, 		  
  {KEY_STOP_EV,			omodule_test_vctrl_allstop_pro}, 		  	  
  {KEY_1_EV,			omodule_test_vctrl_lowv_pro}, 		  
  {KEY_2_EV,			omodule_test_vctrl_midv_pro}, 		  	  
  {KEY_3_EV,			omodule_test_vctrl_highv_pro}, 		  	  	  
  {KEY_4_EV,			omodule_test_vctrl_allv_pro}, 	
};  
*/
void omodule_test_vctrl(void)
{
  olcm_clrscr();	
  ocomm_printf(CH_DISP,1,1,"电磁阀测试");  
  ocomm_printf(CH_DISP,2,1,"低  中  高  总");
  ocomm_printf(CH_DISP,3,1,"○  ○  ○  ○");
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV,      omodule_event_menu_ret_pro);
  okernel_addeventpro(KEY_STOP_EV,     omodule_test_vctrl_allstop_pro);	   
  okernel_addeventpro(KEY_1_EV,omodule_test_vctrl_lowv_pro);  
  okernel_addeventpro(KEY_2_EV,omodule_test_vctrl_midv_pro);  
  okernel_addeventpro(KEY_3_EV,omodule_test_vctrl_highv_pro);  
  okernel_addeventpro(KEY_4_EV,omodule_test_vctrl_allv_pro);  
}



/*--------------------------------------------------------------------

    控制信号                pump状态       
	  00                      停机
	  //01                      变频(变小)
	  //10                      变频(变大)
	  10                      预冷 
	  01                      待机
	  11                      加液
CVK5:  PK2
CVK6:  PK3	  
    深冷控制:
    控制信号                pump状态       
	  00                      停机
	  01                      预冷
	  10                      加液
	  11                      待机
环保调整:
	  00                      停机
	  //01                      变频(变小)
	  //10                      变频(变大)
	  10                      预冷 
	  01                      加液
	  11                      待机	  
--------------------------------------------------------------------*/

void omodule_pumpctrl_stop_pro(void)
{
	LNG_PUMP_STOP();//ovctrl_pump_switch(0);	
    ocomm_printf(CH_DISP,3,1,"●  ○  ○  ○"); 
}

void omodule_pumpctrl_precool_pro(void)
{
	LNG_PUMP_PRECOOL();//ovctrl_pump_switch(1);	
    ocomm_printf(CH_DISP,3,1,"○  ●  ○  ○"); 
}
void omodule_pumpctrl_lng_pro(void)
{
	LNG_PUMP_CTRL();//ovctrl_pump_switch(2);	
    ocomm_printf(CH_DISP,3,1,"○  ○  ●  ○"); 
}
void omodule_pumpctrl_idle_pro(void)
{
	LNG_PUMP_IDLE();//ovctrl_pump_switch(3);	
    ocomm_printf(CH_DISP,3,1,"○  ○  ○  ●"); 
}

void omodule_test_pumpctrl(void)
{
  olcm_clrscr();	
  ocomm_printf(CH_DISP,1,1,"泵联动测试");  
  ocomm_printf(CH_DISP,2,1,"停  预  加  待");
  ocomm_printf(CH_DISP,3,1,"○  ○  ○  ●");
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV,      omodule_event_menu_ret_pro);
  okernel_addeventpro(KEY_1_EV,omodule_pumpctrl_stop_pro);  
  okernel_addeventpro(KEY_2_EV,omodule_pumpctrl_precool_pro);  
  okernel_addeventpro(KEY_3_EV,omodule_pumpctrl_lng_pro);  
  okernel_addeventpro(KEY_4_EV,omodule_pumpctrl_idle_pro);    
}

static unsigned char _cur_test_modbus_sw;
void omodule_test_modbus(void);
void omodule_modbus_test_event_pro(unsigned char id);
void omodule_modbus_test_cancel_pro(void);
prog_char  _MODBUS_TEST_MENU_[][16]={
  "流速",	      
  "流量",	
  "温度",	  
  "密度", //"增益", //
 "状态",
  "清零流量计",
  "小流量切除",	    	    	    
 // "空管检测", //气相  5106
 // "电流输出", //      5810
 // "频率输出", //      频率输出  3203  脉冲输出  3223
  "自动扫描缓冲",  
};

bool omodbus_read_temp(unsigned char sw, float* vfrd);
extern unsigned char omodbus_clrtotalreg_test(unsigned char sw);
void omodule_modbus_test_disp_pro(void)
{
	float vfrd   = 0;
	//unsigned int vrd = 0;
	unsigned char id = odisp_menu_getid();
	if(cur_module_test_sw == _MODBUS_L_ && id == 3)
	{
		ocomm_printf(CH_DISP,2,1,"增益");	   
	}
	else
	{
		olcm_dispprogconst(2,1,&_MODBUS_TEST_MENU_[id][0],1);	
	}

	//if(!omodbus_read_temp(cur_module_test_sw,&vfrd))
	if(!omodbus_read_auto_buffer(cur_module_test_sw,5))	//if(!omodbus_read_float(cur_module_test_sw, regaddr[id], &vfrd))
	{
		modbus_err_rd_cnt++;
	}	 
	else 
	{
       modbus_chk_cnt++;
	/*   if(id == 4)
	   {
		    if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM) return;
			vrd = omodbus_read_auto_uint_value(cur_module_test_sw, 4);
			ocomm_printf(CH_DISP,2,3,":%d",vrd);	   
	   }
	   else
	   {*/
			vfrd = omodbus_read_auto_float_value(cur_module_test_sw, id);
			ocomm_printf(CH_DISP,2,3,":%.04f",vfrd);
	 //  }
	}	   
    ocomm_printf(CH_DISP,4,1,"%ld      %ld",modbus_chk_cnt, modbus_err_rd_cnt);
	otimer_reqire(LCNGCTRL_DISP_EV, 4, 0, 1);
}
void  omodule_modbus_test_cancel_pro(void)
{
	unsigned char id = 0;
	unsigned char maxid = sizeof(_MODBUS_TEST_MENU_)/sizeof(_MODBUS_TEST_MENU_[0]);
	otimer_release(LCNGCTRL_DISP_EV);
	id = odisp_menu_getid();
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          omodule_modbus_test_event_pro(id);
}
/*
Task_Event  _OMODULE_MODBUS_TEST_DISP_Pro_[]  = 
{
  {KEY_RET_EV,			omodule_modbus_test_cancel_pro}, 		  
  {LCNGCTRL_DISP_EV, 	omodule_modbus_test_disp_pro}, 
};
*/
void omodule_modbus_test_read_paras_pro(void)
{
	otimer_release(LCNGCTRL_DISP_EV);
	olcm_clrscr();
	if( _MODBUS_L_ == cur_module_test_sw ) ocomm_printf(CH_DISP,1,1,"液相流量计测试");
	else								   ocomm_printf(CH_DISP,1,1,"气相流量计测试");
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV,      omodule_modbus_test_cancel_pro);
    okernel_addeventpro(LCNGCTRL_DISP_EV,omodule_modbus_test_disp_pro);  	
	otimer_reqire(LCNGCTRL_DISP_EV, 4, 0, 1); 	//400 ms//otimer_reqire(LCNGCTRL_DISP_EV, 4, 1, 1); 	//400 ms
}
//清零流量计
void omodule_modbus_test_clear_modbus_pro(void)
{
	otimer_release(LCNGCTRL_DISP_EV);
	olcm_clrscr();
	if( _MODBUS_L_ == cur_module_test_sw )		   ocomm_printf(CH_DISP,1,1,"液相流量计清零");
	else if( _MODBUS_G_ == cur_module_test_sw )	   ocomm_printf(CH_DISP,1,1,"气相流量计清零");
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV,      omodule_modbus_test_cancel_pro);
    okernel_addeventpro(LCNGCTRL_DISP_EV,omodule_modbus_test_disp_pro);  	
	if(omodbus_clrtotalreg(cur_module_test_sw))  ocomm_printf(CH_DISP,2,1,"清零成功");
	else                                         ocomm_printf(CH_DISP,2,1,"清零失败");
}

//小流量切除

//unsigned char omodbus_write_reg_vavle_float(unsigned char sw ,unsigned int reg_id, float v_value);
void omodule_modbus_flowcut_setok_pro(void)
{
	float  vftmp = 0;
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //数据输入
	{
		vftmp = atof((char*)cur_input_cursor.str_input);
		if(omodbus_write_float(cur_module_test_sw, 5137,vftmp))
		{
			ocomm_message(4,1,"设置成功",10, NULL);	
			return;
		}
	}	
	ocomm_message(4,1,"设置失败",10, NULL);		
}
/*
Task_Event  _OMODULE_MODBUS_FLOWCUT_CHK_Pro_[]  = 
{
  {KEY_OK_EV,			omodule_modbus_flowcut_setok_pro}, 	
  {KEY_RET_EV,			omodule_modbus_test_cancel_pro}, 		  
};
*/
void omodule_modbus_test_lowflow_cut_pro(void)
{
	float vftmp = 0;
	olcm_clrscr();
	if( _MODBUS_L_ == cur_module_test_sw ) ocomm_printf(CH_DISP,1,1,"液相小流量切除");
	else								   ocomm_printf(CH_DISP,1,1,"气相小流量切除");
	if(!omodbus_read_auto_buffer(cur_module_test_sw,6))	
	{
		ocomm_printf(CH_DISP,4,1,"流量计通讯错误");
		return;
	}
	vftmp  = omodbus_read_auto_float_value(cur_module_test_sw, AUTO_SCAN_LCNG_FLOWCUT);
	
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = true;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 4;
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, omodule_modbus_test_cancel_pro);
    okernel_addeventpro(KEY_OK_EV,  omodule_modbus_flowcut_setok_pro);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,2,1,"请输入:");
	ocomm_printf(CH_DISP,2,5,"%.02f", vftmp);	
}

//自动扫描缓冲
unsigned char omodbus_write_reg_vavle_ex(unsigned char sw ,unsigned int reg_id, unsigned int v_value);
void omodule_modbus_test_auto_buffer_pro(void)
{
	
	//unsigned char m,n = 0;
	//unsigned char ret = 0;
	unsigned int rd1, rd2 = 0;
	//unsigned char ret = 0;
	//float vfrd = 0;
	otimer_release(LCNGCTRL_DISP_EV);
	olcm_clrscr();
	if( _MODBUS_L_ == cur_module_test_sw ) ocomm_printf(CH_DISP,1,1,"液相自动扫描缓冲");
	else								   ocomm_printf(CH_DISP,1,1,"气相自动扫描缓冲");
	//okernel_clreventpro();
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)
	{
		omodbus_read_int(cur_module_test_sw, 750, &rd1); //_MODBUS_G_
		omodbus_read_int(cur_module_test_sw, 751, &rd2);
		ocomm_printf(CH_DISP,2,1,"%d %d",rd1,rd2);
		omodbus_read_int(cur_module_test_sw, 752, &rd1);
		omodbus_read_int(cur_module_test_sw, 753, &rd2);
		ocomm_printf(CH_DISP,3,1,"%d %d",rd1,rd2);
	}
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)
	{	
		omodbus_read_int(cur_module_test_sw, 5000, &rd1); //_MODBUS_G_
		omodbus_read_int(cur_module_test_sw, 5001, &rd2);
		ocomm_printf(CH_DISP,2,1,"%d %d",rd1,rd2);
		omodbus_read_int(cur_module_test_sw, 5002, &rd1);
		omodbus_read_int(cur_module_test_sw, 5003, &rd2);
		ocomm_printf(CH_DISP,3,1,"%d %d",rd1,rd2);
		omodbus_read_int(cur_module_test_sw, 5004, &rd1);
		omodbus_read_int(cur_module_test_sw, 5005, &rd2);
		ocomm_printf(CH_DISP,4,1,"%d %d",rd1,rd2); 
	}	
	//omodbus_read_int(cur_module_test_sw, 5005, &rd2);
	//ocomm_printf(CH_DISP,4,1,"%d %d",rd1,rd2); 
   /* ret = omodbus_read_int(cur_module_test_sw, 5000, &rd1);
	ocomm_printf(CH_DISP,4,1,"%d %d",ret,rd1); */
#if 0   

   //测试罗斯蒙特流量计 250:温度
	//omodbus_write_multi_reg(cur_module_test_sw, 654, 250);
/*
	OS_DISABLE_KEYSCAN();	
	ret = omodbus_read_reg(cur_module_test_sw,654,1); //38  1132
	OS_ENABLE_KEYSCAN();  
	 */	  
   //omodbus_write_reg_vavle(cur_module_test_sw, 38, 0xFF48);// 	
   //OS_DISABLE_KEYSCAN();
   //ret = omodbus_write_reg_vavle(cur_module_test_sw, 55, 0xFF00); 
   //ret = omodbus_write_multi_reg(cur_module_test_sw, 38, 0x48); 
   //ret = omodbus_write_reg_vavle(cur_module_test_sw, 654, 250); 
   //OS_ENABLE_KEYSCAN();
   //omodbus_write_multi_reg(cur_module_test_sw, 38, 0xFF48);
   //omodbus_write_reg_vavle_ex(cur_module_test_sw, 38, 72);
	//OS_DISABLE_KEYSCAN();	
	//ret = omodbus_read_reg(cur_module_test_sw,38,1); //38  1132
	//OS_ENABLE_KEYSCAN();  
    
   //omodbus_read_int(cur_module_test_sw, 1132, &rd1);  
   //omodbus_read_float(cur_module_test_sw, 250, &vfrd); 
   //omodbus_write_multi_reg(cur_module_test_sw, 38, 0x49); 
   ret = omodbus_write_multi_reg(cur_module_test_sw, 750, 1); 
   //ret = omodbus_write_multi_reg(cur_module_test_sw, 750, 251); 
   //omodbus_read_int(cur_module_test_sw, 38, &rd1);
   /*omodbus_read_int(cur_module_test_sw, 654, &rd2); */
   //ocomm_printf(CH_DISP,2,1,"%d %d %d",ret, rd1, rd2);
   if(omodbus_read_float(cur_module_test_sw,782 , &vfrd)) //686750
   {
	   ocomm_printf(CH_DISP,2,1,"%.02f",vfrd);
   }
   else/**/
   {
   ocomm_printf(CH_DISP,2,1,"%d %d %d %d",_modbus_g_buf[0], _modbus_g_buf[1], _modbus_g_buf[2], _modbus_g_buf[3]);
   ocomm_printf(CH_DISP,3,1,"%d %d %d %d",_modbus_g_buf[4], _modbus_g_buf[5], _modbus_g_buf[6], _modbus_g_buf[7]);
   ocomm_printf(CH_DISP,4,1,"%d %d %d %d",_modbus_g_buf[8], _modbus_g_buf[9], ret, rd2);
   }
#endif   
}

void omodule_modbus_test_retok_pro(void)
{
	_cur_test_modbus_sw = odisp_menu_getid();
	modbus_chk_cnt=modbus_err_rd_cnt = 0;
	switch(_cur_test_modbus_sw)
	{
		case 0: //流速
		case 1://流量
		case 2://温度
		case 3://密度
		case 4://状态
		//case 4://压力
			omodule_modbus_test_read_paras_pro(); 	
			break;
		case 5: //清零流量计
			omodule_modbus_test_clear_modbus_pro();
			break;
		case 6:	//小流量切除	
			omodule_modbus_test_lowflow_cut_pro();
			break;			
		case 7://自动扫描缓冲
			omodule_modbus_test_auto_buffer_pro();
			break;
		default:
			okernel_putevent(KEY_RET_EV,0);	
			return;
	}	
}
/*
Task_Event  _MODBUS_TEST_MENU_Pro[]  = 
{
  {KEY_OK_EV,			omodule_modbus_test_retok_pro}, 	
  {KEY_RET_EV,			omodule_test_modbus}, 		  
};
*/
void omodule_modbus_test_event_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_MODBUS_TEST_MENU_)/sizeof(_MODBUS_TEST_MENU_[0]);
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, omodule_test_modbus);
    okernel_addeventpro(KEY_OK_EV,  omodule_modbus_test_retok_pro);	

	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_MODBUS_TEST_MENU_, maxid,id);
}
void omodule_modbus_test_l_pro(void)
{
  modbus_chk_cnt=modbus_err_rd_cnt = 0;
  cur_module_test_sw = _MODBUS_L_;
  omodule_modbus_test_event_pro(0);
  
}
void omodule_modbus_test_g_pro(void)
{
  modbus_chk_cnt=modbus_err_rd_cnt = 0;	
  cur_module_test_sw = _MODBUS_G_;
  omodule_modbus_test_event_pro(0);
}
void omodule_event_modbus_ret_pro(void);
/*
Task_Event  _OMODULE_MODBUS_TEST_Pro[]  = 
{
  {KEY_RET_EV,			omodule_event_modbus_ret_pro},//omodule_event_menu_ret_pro}, 		  
  {KEY_1_EV,			omodule_modbus_test_g_pro}, 		  
  {KEY_2_EV,			omodule_modbus_test_l_pro}, 
  //{KEY_3_EV,            omodule_modbus_test_clr_pro},  		  	  
};
*/
void omodule_test_modbus(void)
{
  otimer_release(LCNGCTRL_DISP_EV); 	
  olcm_clrscr();	
  ocomm_printf(CH_DISP,1,1,"流量计测试");  
  ocomm_printf(CH_DISP,2,1,"1.气相流量计");    
  ocomm_printf(CH_DISP,3,1,"2.液相流量计");   
  //ocomm_printf(CH_DISP,4,1,"3.清零流量计");      
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, omodule_event_modbus_ret_pro);
  okernel_addeventpro(KEY_1_EV,  omodule_modbus_test_g_pro);	  
  okernel_addeventpro(KEY_2_EV,  omodule_modbus_test_l_pro);	  
}
void omodule_keyboard_test_key1_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:1");
}
void omodule_keyboard_test_key2_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:2");
}
void omodule_keyboard_test_key3_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:3");
}
void omodule_keyboard_test_key4_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:4");
}
void omodule_keyboard_test_key5_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:5");
}
void omodule_keyboard_test_key6_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:6");
}
void omodule_keyboard_test_key7_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:7");
}
void omodule_keyboard_test_key8_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:8");
}
void omodule_keyboard_test_key9_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:9");
}
void omodule_keyboard_test_key0_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "数字键:0");
}

void omodule_keyboard_test_keypt_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 小数点");
}

void omodule_keyboard_test_keyqry_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 查询");
} 		  	  
void omodule_keyboard_test_keyset_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 菜单");
}
void omodule_keyboard_test_keyfix_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 定量");
} 
void omodule_keyboard_test_keyback_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 预冷");
} 		  
void omodule_keyboard_test_keyclr_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 清除");
}
void omodule_keyboard_test_keylcng_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 加气");
}
void omodule_keyboard_test_keystop_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 停止");
}
void omodule_keyboard_test_keyok_pro(void)
{
	ocomm_printf(CH_DISP,2, 1, "功能键: 确定");
}

/*
Task_Event  _OMODULE_KEYBOARD_TEST_Pro_[]  = 
{
  {KEY_RET_EV,			omodule_event_menu_ret_pro}, 		  
  {KEY_1_EV,			omodule_keyboard_test_key1_pro}, 		  
  {KEY_2_EV,			omodule_keyboard_test_key2_pro}, 		  	  
  {KEY_3_EV,			omodule_keyboard_test_key3_pro}, 		  
  {KEY_4_EV,			omodule_keyboard_test_key4_pro}, 		  	  
  {KEY_5_EV,			omodule_keyboard_test_key5_pro}, 		  
  {KEY_6_EV,			omodule_keyboard_test_key6_pro}, 		  	  
  {KEY_7_EV,			omodule_keyboard_test_key7_pro}, 		  
  {KEY_8_EV,			omodule_keyboard_test_key8_pro}, 		  	  
  {KEY_9_EV,			omodule_keyboard_test_key9_pro}, 		  
  {KEY_0_EV,			omodule_keyboard_test_key0_pro}, 		  	  

  {KEY_PT_EV,			omodule_keyboard_test_keypt_pro}, 		  	  
  {KEY_QRY_EV,			omodule_keyboard_test_keyqry_pro}, 		  	  
  {KEY_SET_EV,			omodule_keyboard_test_keyset_pro}, 		  
  {KEY_FIX_EV,			omodule_keyboard_test_keyfix_pro}, 		  	  
  {KEY_BACK_EV,			omodule_keyboard_test_keyback_pro}, 		  
  {KEY_CLR_EV,			omodule_keyboard_test_keyclr_pro}, 		  	  
  {KEY_LCNG_EV,			omodule_keyboard_test_keylcng_pro}, 		  
  {KEY_STOP_EV,			omodule_keyboard_test_keystop_pro}, 		  	  
  {KEY_OK_EV,			omodule_keyboard_test_keyok_pro}, 		  
//  {KEY_RET_EV,			omodule_keyboard_test_keyret_pro}, 		  	  

};
*/
void omodule_keyboard_test(void)
{
  olcm_clrscr();	
  ocomm_printf(CH_DISP,1,1,"键盘测试");  
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, omodule_event_menu_ret_pro);
  okernel_addeventpro(KEY_1_EV,  omodule_keyboard_test_key1_pro);	  
  okernel_addeventpro(KEY_2_EV,  omodule_keyboard_test_key2_pro);  
  okernel_addeventpro(KEY_3_EV,  omodule_keyboard_test_key3_pro);	  
  okernel_addeventpro(KEY_4_EV,  omodule_keyboard_test_key4_pro);  
  okernel_addeventpro(KEY_5_EV,  omodule_keyboard_test_key5_pro);	  
  okernel_addeventpro(KEY_6_EV,  omodule_keyboard_test_key6_pro);  
  okernel_addeventpro(KEY_7_EV,  omodule_keyboard_test_key7_pro);	  
  okernel_addeventpro(KEY_8_EV,  omodule_keyboard_test_key8_pro);  
  okernel_addeventpro(KEY_9_EV,  omodule_keyboard_test_key9_pro);	  
  okernel_addeventpro(KEY_0_EV,  omodule_keyboard_test_key0_pro);  

  okernel_addeventpro(KEY_PT_EV,  omodule_keyboard_test_keypt_pro);	  
  okernel_addeventpro(KEY_QRY_EV,  omodule_keyboard_test_keyqry_pro);  
  okernel_addeventpro(KEY_SET_EV,  omodule_keyboard_test_keyset_pro);	  
  okernel_addeventpro(KEY_FIX_EV,  omodule_keyboard_test_keyfix_pro);  
  okernel_addeventpro(KEY_BACK_EV,  omodule_keyboard_test_keyback_pro);	  
  okernel_addeventpro(KEY_CLR_EV,  omodule_keyboard_test_keyclr_pro);  
  okernel_addeventpro(KEY_LCNG_EV,  omodule_keyboard_test_keylcng_pro);	  
  okernel_addeventpro(KEY_STOP_EV,  omodule_keyboard_test_keystop_pro);  
  okernel_addeventpro(KEY_OK_EV,  omodule_keyboard_test_keyok_pro);

}

void omodule_precool_test_ret_pro(void)
{
	otimer_release(LCNGCTRL_DISP_EV); 
	VCTRL_MID_OPEN();  //开阀2 (中)
	VCTRL_HIGH_CLOSE();//关阀1 (高)
	omodule_event_menu_ret_pro();
}
/*
Task_Event  _OMODULE_PRECOOL_TEST_Pro_[]  = 
{
  {KEY_RET_EV,			omodule_precool_test_ret_pro}, 	
};
*/	  
void omodule_precool_test(void)
{
  olcm_clrscr();	
  if(sys_err&(1<<SYSERR_MODBUS_ERR))//if(sys_err == SYSERR_MODBUS_ERR )
  {
	ocomm_message(4,1,"流量计错误",20, omodule_event_menu_ret_pro);
	return;
  }  
  ocomm_printf(CH_DISP,1,1,"预冷测试");  
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, omodule_precool_test_ret_pro);
  olngctrl_Spre_cool();
}
/*
扇区: 0~15  一共16个
扇区0： 1、 2
扇区1： 4~6
扇区2： 8~10
扇区3： 12~14
扇区4： 16~18

*/
#define  ICCARD_TEST_MAX_CNT	10000
unsigned char icard_test_step = 0;
unsigned char icard_test_err_cnt = 0;
unsigned char icard_test_ok_cnt = 0;
unsigned int  icard_test_cnt = 0;
unsigned char icard_test_wtbuf[16];
#define  TEST_BLOCKS		12
//prog_char _ICCARD_TEST_BLOCKS[15]   = {20,21,22,24,25,26,28,29,30,32,33,34,36,37,38};
//prog_char _ICCARD_TEST_BLOCKS[15]   =   {24,25,26,28,29,30,32,33,34,36,37,38,40,41,42};	
prog_char _ICCARD_TEST_BLOCKS[TEST_BLOCKS]   =   {4,5,6,8,9,10,12,13,14,16,17,18};//,40,41,42};		
void omudule_test_card_write_pro(void)
{
	unsigned char cur_blocks = 0;
	OS_DISABLE_KEYSCAN();  //2013-03-11
	cur_blocks = pgm_read_byte(&_ICCARD_TEST_BLOCKS[icard_test_step]);
	ocomm_printf(CH_DISP,2,1,"写卡:%d",cur_blocks);	
#if ICCARD_M1_MINGTE
	if(!oicard_write(false, cur_blocks, icard_test_wtbuf)) 
#else
	if(!oicard_write(cur_blocks,16, icard_test_wtbuf,0)) 
#endif
	{
		ocomm_printf(CH_DISP,2,5,"%d %d",icard_err_,icard_err_code_);	
		ocomm_printf(CH_DISP,4,1,"%d  %d",icard_test_cnt, ++icard_test_err_cnt);
		oprt_ctrl_iccard_err_info(true);
		oicard_turnoff();
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//500ms	
		OS_ENABLE_KEYSCAN();  //2013-03-11	
		return;
	}
	otimer_release(LCNG_COMM_EVENT1_EV);
	icard_test_step++;//icard_cur_block++;
	if(icard_test_step >= TEST_BLOCKS) //15)
	{
		icard_test_step = 0;
		icard_test_cnt ++;	
		okernel_putevent(LCNG_COMM_EVENT3_EV,0); //otimer_reqire(LCNG_COMM_EVENT3_EV, 3, 0, 1); 	   //
	}
	else
	{
		okernel_putevent(LCNG_COMM_EVENT2_EV,0); //otimer_reqire(LCNG_COMM_EVENT2_EV, 3, 0, 1); 	   //
	}	
	OS_ENABLE_KEYSCAN();  //2013-03-11
}

void omudule_test_card_read_pro(void)
{
	unsigned char cur_blocks =0;//= pgm_read_byte(&_ICCARD_TEST_BLOCKS[icard_test_step]);
	unsigned char rdbuf[16];	
	OS_DISABLE_KEYSCAN();  //2013-03-11	
	ocomm_printf(CH_DISP,3,1,"读卡:%d",cur_blocks);	
	cur_blocks = pgm_read_byte(&_ICCARD_TEST_BLOCKS[icard_test_step]);
	memset(rdbuf,0,16);
#if ICCARD_M1_MINGTE
	if(!oicard_read(false, cur_blocks, rdbuf)) //if(!oicard_read(cur_blocks, rdbuf))
#else
	if(!oicard_read(cur_blocks, 16, rdbuf)) 
#endif
	{
		ocomm_printf(CH_DISP,3,5,"%d %d",icard_err_,icard_err_code_);
		//ocomm_printf(CH_DISP,4,1,"%d  %d",icard_test_cnt, ++icard_test_err_cnt);
		oprt_ctrl_iccard_err_info(false);
		//okernel_putevent(LCNG_COMM_EVENT1_EV,0);//otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//300ms		
		oicard_turnoff();
		otimer_reqire(LCNG_COMM_EVENT1_EV, 4, 0, 1); 	//400ms	
		OS_ENABLE_KEYSCAN();  //2013-03-11		
		return;
	}
	otimer_release(LCNG_COMM_EVENT1_EV);
	if(memcmp(icard_test_wtbuf,rdbuf,16) !=0)
	{
		//oprt_ctrl_iccard_test_info(icard_test_wtbuf,rdbuf);
		icard_test_err_cnt++;
	}
	else
	{
		icard_test_ok_cnt++;
	}
	ocomm_printf(CH_DISP,4,1,"%d  %d  %d",icard_test_cnt, icard_test_err_cnt, icard_test_ok_cnt);	
	icard_test_step++;
	if(icard_test_step >= TEST_BLOCKS)
	{
		icard_test_step = 0;
		if(icard_test_cnt >= ICCARD_TEST_MAX_CNT)
		{
			//okernel_putevent(KEY_RET_EV,0);
			//ocomm_message(4,1,"测试结束",10,NULL);
			ocomm_printf(CH_DISP,4,1,"测试结束:%d %d",icard_test_cnt, icard_test_err_cnt);	
		}	
		else
		{
			////新数据
			ds3231_read_date((unsigned char*)&icard_test_wtbuf[0],true);
			ds3231_read_time((unsigned char*)&icard_test_wtbuf[3],true);	 
			ds3231_read_date((unsigned char*)&icard_test_wtbuf[6],false);
			ds3231_read_time((unsigned char*)&icard_test_wtbuf[9],false);	 
			ds3231_read_date((unsigned char*)&icard_test_wtbuf[12],true);
					
			okernel_putevent(LCNG_COMM_EVENT2_EV,0);  //otimer_reqire(LCNG_COMM_EVENT2_EV, 3, 0, 1); 	//300ms	//
		}		
	}
	else
	{
		okernel_putevent(LCNG_COMM_EVENT3_EV,0);  //otimer_reqire(LCNG_COMM_EVENT3_EV, 10, 0, 1); 	   //300ms	 //
	}		
	OS_ENABLE_KEYSCAN();  //2013-03-11
}
/*
void oapp_iccard_select_pro(void);
Task_Event  _OMODULE_TEST_CARD_Pro_[]  = 
{
  {KEY_RET_EV,		     omodule_event_menu_ret_pro}, 	
  {LCNG_COMM_EVENT1_EV,  oapp_iccard_select_pro },	
  {LCNG_COMM_EVENT2_EV,  omudule_test_card_write_pro },	
  {LCNG_COMM_EVENT3_EV,  omudule_test_card_read_pro },		  	  
};
*/
 
void omodule_test_card(void)
{
  icard_test_cnt = 0; 	
  icard_test_err_cnt = 0;
  icard_test_ok_cnt = 0;
  icard_test_step = 0;
  icard_step_cnt = 0;
  ds3231_read_date((unsigned char*)&icard_test_wtbuf[0],true);
  ds3231_read_time((unsigned char*)&icard_test_wtbuf[3],true);	 
  ds3231_read_date((unsigned char*)&icard_test_wtbuf[6],false);
  ds3231_read_time((unsigned char*)&icard_test_wtbuf[9],false);	 
  ds3231_read_date((unsigned char*)&icard_test_wtbuf[12],true);
   
  olcm_clrscr();	
  ocomm_printf(CH_DISP,1,1,"IC卡测试");  //透过输入扇区号来读取数据显示出来
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, omodule_event_menu_ret_pro);
  okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);
  okernel_addeventpro(LCNG_COMM_EVENT2_EV, omudule_test_card_write_pro);
  okernel_addeventpro(LCNG_COMM_EVENT3_EV, omudule_test_card_read_pro);
  //okernel_putevent(LCNG_COMM_EVENT1_EV,0);
  oicard_turnoff();
  otimer_reqire(LCNG_COMM_EVENT1_EV, 4, 0, 1); 	//400ms		  
}
static unsigned char x_rtcpos = 0;
static unsigned long input_rtc = 0;
static bool          bsettime = false;
void omodule_rtc_test_set_pro(void)
{
	unsigned char buf[3] = {0};
	bool          bsetok = false;	
	if(x_rtcpos >= 6) 
	{
		buf[0] = (unsigned char)(input_rtc/10000);
		buf[1] = (unsigned char)((input_rtc%10000)/100);
		buf[2] = (unsigned char)(input_rtc%100);
		if(bsettime)
			ocomm_printf(CH_DISP,2, 1, "%02d时%02d分%02d秒",buf[0],buf[1], buf[2]);
		else
			ocomm_printf(CH_DISP,2, 1, "%02d年%02d月%02d日",buf[0],buf[1], buf[2]);
		if(bsettime)
		{
			if(ocomm_checktime(buf)) bsetok = true;
		}
		else
		{
			if(ocomm_checkdate(buf)) bsetok = true;
		}						
		if(bsetok)
		{ 
			if(bsettime)
			{
			 if(!ds3231_set_time(&buf[0]))  //设置时间
			 {
				 bsetok = false;
			 }
			}
			else
			{
			 if(!ds3231_set_date(&buf[0]))  //设置日期
			 {
				 bsetok = false;
			 }
			}			 
		}
		if(bsetok)
		{
			bsettime = !bsettime;
			sys_err &= ~(1<<SYSERR_TIME);
		    ocomm_message(4,1,"设置成功",10, NULL);
		}
		else
		{
			ocomm_message(4,1,"设置失败",10, NULL);		
		}
		return;
	}
	if(x_rtcpos ==5)
	{
		if(bsettime)   ocomm_printf(CH_DISP,2, 1, "%02d时%02d分%d",(unsigned char)(input_rtc/1000), (unsigned char)((input_rtc%1000)/10),(unsigned char)(input_rtc%10));
		else	    ocomm_printf(CH_DISP,2, 1, "%02d年%02d月%d",(unsigned char)(input_rtc/1000), (unsigned char)((input_rtc%1000)/10),(unsigned char)(input_rtc%10));
		return;
	}	 
	if(x_rtcpos == 4)
	{
		if(bsettime)   ocomm_printf(CH_DISP,2, 1, "%02d时%02d分",(unsigned char)(input_rtc/100),(unsigned char)(input_rtc%100));
		else		ocomm_printf(CH_DISP,2, 1, "%02d年%02d月",(unsigned char)(input_rtc/100),(unsigned char)(input_rtc%100));
		return;
	}
	if(x_rtcpos == 3)
	{
		if(bsettime)   ocomm_printf(CH_DISP,2, 1, "%02d时%d",(unsigned char)(input_rtc/10),(unsigned char)(input_rtc%10));	
		else	    ocomm_printf(CH_DISP,2, 1, "%02d年%d",(unsigned char)(input_rtc/10),(unsigned char)(input_rtc%10));	
		return;
	}
	if(x_rtcpos == 2)
	{
		if(bsettime) ocomm_printf(CH_DISP,2, 1, "%02d时",(unsigned char)(input_rtc%100));
		else  	  ocomm_printf(CH_DISP,2, 1, "%02d年",(unsigned char)(input_rtc%100));
		return;
	}
	if(x_rtcpos == 1)
	{
		ocomm_printf(CH_DISP,2, 1, "%d",(unsigned char)(input_rtc%100));	
		return;
	}
	ocomm_printf(CH_DISP,2, 1, " ");
    
}
void omodule_rtc_test_key1_pro(void)
{
	input_rtc = input_rtc*10+1;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key2_pro(void)
{
	input_rtc = input_rtc*10+2;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key3_pro(void)
{
	input_rtc = input_rtc*10+3;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key4_pro(void)
{
	input_rtc = input_rtc*10+4;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key5_pro(void)
{
	input_rtc = input_rtc*10+5;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key6_pro(void)
{
	input_rtc = input_rtc*10+6;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key7_pro(void)
{
	input_rtc = input_rtc*10+7;//input_rtc = pow(10,x_rtcpos)+7;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key8_pro(void)
{
	input_rtc = input_rtc*10+8;//input_rtc = pow(10,x_rtcpos)+8;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key9_pro(void)
{
	input_rtc = input_rtc*10+9;//input_rtc = pow(10,x_rtcpos)+9;
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_key0_pro(void)
{
	input_rtc = input_rtc*10;//input_rtc = pow(10,x_rtcpos);
	x_rtcpos++;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_clr_pro(void)
{
	if(x_rtcpos == 0){
		 input_rtc = 0;
		 return;
	}		 
	x_rtcpos--;
	input_rtc = input_rtc/10;
	omodule_rtc_test_set_pro();
}
void omodule_rtc_test_ok_pro(void)
{
	bsettime = !bsettime;
	okernel_putevent(KEY_RET_EV,0);
}
/*
Task_Event  _OMODULE_TEST_RTC_Pro_[]  = 
{
  {KEY_RET_EV,		    omodule_event_menu_ret_pro}, 	
  {KEY_1_EV,			omodule_rtc_test_key1_pro}, 		  
  {KEY_2_EV,			omodule_rtc_test_key2_pro}, 		  	  
  {KEY_3_EV,			omodule_rtc_test_key3_pro}, 		  
  {KEY_4_EV,			omodule_rtc_test_key4_pro}, 		  	  
  {KEY_5_EV,			omodule_rtc_test_key5_pro}, 		  
  {KEY_6_EV,			omodule_rtc_test_key6_pro}, 		  	  
  {KEY_7_EV,			omodule_rtc_test_key7_pro}, 		  
  {KEY_8_EV,			omodule_rtc_test_key8_pro}, 		  	  
  {KEY_9_EV,			omodule_rtc_test_key9_pro}, 		  
  {KEY_0_EV,			omodule_rtc_test_key0_pro}, 	
  {KEY_CLR_EV,          omodule_rtc_test_clr_pro},  	  	  	  
  {KEY_OK_EV,           omodule_rtc_test_ok_pro},  	  	  	  	  
};
*/
void omodule_rtc_time(void)
{
  olcm_clrscr();	
  if(bsettime) ocomm_printf(CH_DISP,1,1,"请输入时间");  
  else         ocomm_printf(CH_DISP,1,1,"请输入日期");  
  x_rtcpos = 0;	
  input_rtc = 0;
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, omodule_event_menu_ret_pro);
  okernel_addeventpro(KEY_0_EV, omodule_rtc_test_key0_pro);
  okernel_addeventpro(KEY_1_EV, omodule_rtc_test_key1_pro);
  okernel_addeventpro(KEY_2_EV, omodule_rtc_test_key2_pro);
  okernel_addeventpro(KEY_3_EV, omodule_rtc_test_key3_pro);
  okernel_addeventpro(KEY_4_EV, omodule_rtc_test_key4_pro);
  okernel_addeventpro(KEY_5_EV, omodule_rtc_test_key5_pro);
  okernel_addeventpro(KEY_6_EV, omodule_rtc_test_key6_pro);  
  okernel_addeventpro(KEY_7_EV, omodule_rtc_test_key7_pro);
  okernel_addeventpro(KEY_8_EV, omodule_rtc_test_key8_pro);
  okernel_addeventpro(KEY_9_EV, omodule_rtc_test_key9_pro);
  okernel_addeventpro(KEY_CLR_EV, omodule_rtc_test_clr_pro);
  okernel_addeventpro(KEY_OK_EV, omodule_rtc_test_ok_pro);    
}

static unsigned char tst_disp_num = 0;
void omodule_splc_disp_num(unsigned char num)
{
  unsigned char disp[22] = {0};  
  memset(&disp[0],num,22);
  if( cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_B) 
  osplc_disp_ex(disp,true);
  else if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_Y){                                                       
  osplc_disp(disp,true);
  }  
  else if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_N){	
  odisp_mem_disp_init0();
  }  
}
void omodule_disp_0_num_pro(void)
{
	omodule_splc_disp_num(0);
}
void omodule_disp_1_num_pro(void)
{
	omodule_splc_disp_num(1);
}
void omodule_disp_2_num_pro(void)
{
	omodule_splc_disp_num(2);
}
void omodule_disp_3_num_pro(void)
{
	omodule_splc_disp_num(3);
}
void omodule_disp_4_num_pro(void)
{
	omodule_splc_disp_num(4);
}
void omodule_disp_5_num_pro(void)
{
	omodule_splc_disp_num(5);
}
void omodule_disp_6_num_pro(void)
{
	omodule_splc_disp_num(6);
}
void omodule_disp_7_num_pro(void)
{
	omodule_splc_disp_num(7);
}
void omodule_disp_8_num_pro(void)
{
	omodule_splc_disp_num(8);
}
void omodule_disp_9_num_pro(void)
{
	omodule_splc_disp_num(9);
}
/*
void omodule_splc_disp(void)
{
	omodule_splc_disp_num(tst_disp_num++);
	if(tst_disp_num > 9) tst_disp_num = 0;
}

Task_Event  _OMODULE_TEST_SPLCDISP_Pro_[]  = 
{
  {KEY_RET_EV,		    omodule_event_menu_ret_pro}, 	
  {KEY_OK_EV,           omodule_splc_disp},  
};
*/
void omodule_splc_disp_pro(void)
{
  tst_disp_num = 0;	
  olcm_clrscr();	
  ocomm_printf(CH_DISP,1,2,"客显测试");  
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, omodule_event_menu_ret_pro);
  okernel_addeventpro(KEY_0_EV,   omodule_disp_0_num_pro);
  okernel_addeventpro(KEY_1_EV,   omodule_disp_1_num_pro);
  okernel_addeventpro(KEY_2_EV,   omodule_disp_2_num_pro);
  okernel_addeventpro(KEY_3_EV,   omodule_disp_3_num_pro);
  okernel_addeventpro(KEY_4_EV,   omodule_disp_4_num_pro);
  okernel_addeventpro(KEY_5_EV,   omodule_disp_5_num_pro);
  okernel_addeventpro(KEY_6_EV,   omodule_disp_6_num_pro);
  okernel_addeventpro(KEY_7_EV,   omodule_disp_7_num_pro);
  okernel_addeventpro(KEY_8_EV,   omodule_disp_8_num_pro);
  okernel_addeventpro(KEY_9_EV,   omodule_disp_9_num_pro);
  //omodule_splc_disp_num(tst_disp_num);
  
}
//extern unsigned char omodbus_write_multi_reg(unsigned char sw, unsigned int regaddr, unsigned int reg);
//extern unsigned char omodbus_read_auto_buffer(unsigned char sw, unsigned int reg, unsigned char regnum);
void omodule_comm_test_pro(void)      
{
	//unsigned int  rd;
    okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV,      omodule_event_menu_ret_pro);
  	
	olcm_clrscr();	
    ocomm_printf(CH_DISP,1,2,"通讯测试");	
#if 0
	//omodbus_read_int(_MODBUS_G_, 5000, &rd);
	//ocomm_printf(CH_DISP,4,1,"%d", rd);  
    ocomm_printf(CH_DISP,2,1,"ung_id: %05ld",cur_sysparas.r_unupld_g_id);  
	ocomm_printf(CH_DISP,3,1,"g_id: %05ld",cur_sysparas.r_cur_g_id);  
	 	
	/*
	//2107温度
	//omodbus_write_multi_reg(_MODBUS_G_, 5000, 2017);
	//9503增益
	//2007
	//omodbus_write_multi_reg(_MODBUS_G_, 5001, 2017);
	//ocomm_printf(CH_DISP,2,1,"%d",ret);  
	//ocomm_delay_ms(1000);  	
	omodbus_read_int(_MODBUS_G_, 5000, &rd);
	omodbus_read_int(_MODBUS_G_, 5001, &rd1);
	ocomm_printf(CH_DISP,2,1,"%d %d",rd, rd1);  
	omodbus_read_int(_MODBUS_G_, 5002, &rd2);
	omodbus_read_int(_MODBUS_G_, 5003, &rd3);
	ocomm_printf(CH_DISP,3,1,"%d %d",rd2, rd3);  
	omodbus_read_int(_MODBUS_G_, 5004, &rd3);
	ocomm_printf(CH_DISP,4,1,"%d", rd3);  
	
	ocomm_delay_ms(4000);  	
	*/	
#endif	
}

void omodule_moxa_test_pro(void)
{
	//float vfrd = 0;
	//unsigned char  cnt ,ret = 0;
	olcm_clrscr();	
    ocomm_printf(CH_DISP,1,2,"MOXA测试");	
	/*if(omodbus_read_auto_buffer(_MODBUS_G_,5))
	{
		ocomm_printf(CH_DISP,1,1,"%.04f",omodbus_read_auto_float_value(0));
		ocomm_printf(CH_DISP,2,1,"%.04f",omodbus_read_auto_float_value(1));
		ocomm_printf(CH_DISP,3,1,"%.04f",omodbus_read_auto_float_value(2));
		ocomm_printf(CH_DISP,4,1,"%d",  omodbus_read_auto_uint_value(4));
	
	//ocomm_printf(CH_DISP,1,1,"%d %d",_modbus_response_g_len, _modbus_g_buf[2]);
	//ocomm_printf(CH_DISP,2,1,"%d %d %d %d",_modbus_g_buf[0],_modbus_g_buf[1],_modbus_g_buf[2],_modbus_g_buf[3]);
	//ocomm_printf(CH_DISP,3,1,"%d %d %d %d",_modbus_g_buf[4],_modbus_g_buf[5],_modbus_g_buf[6],_modbus_g_buf[7]);
	//ocomm_printf(CH_DISP,4,1,"%d %d %d %d",_modbus_g_buf[19],_modbus_g_buf[20],_modbus_g_buf[21],_modbus_g_buf[22]);
	}
	//}  
	ocomm_delay_ms(4000);	
	*/
}

void omodule_print_test_pro(void)
{

}

prog_char  _OMODULE_MENU[][16]=
{
	"通讯测试",
	"Moxa测试",
    "键盘测试",	
	"流量计测试",
	"气相压力测试",
	"液相压力测试",
	"电磁阀测试",
	"IC卡测试",
	"时钟测试",
	"客显测试",	
	"打印测试",	
	"泵联动测试",
	"预冷测试",	
	"打印挂失列表",	
	"清空挂失列表",	
	
};	
void omodule_event_menu_ret_pro(void)
{
	unsigned char maxid, id = odisp_menu_getid();
	maxid = sizeof(_OMODULE_MENU)/sizeof(_OMODULE_MENU[0]);
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else	      omodule_event_pro(id);
}
void omodule_event_modbus_ret_pro(void)
{
	unsigned char maxid, id = 3;
	maxid = sizeof(_OMODULE_MENU)/sizeof(_OMODULE_MENU[0]);
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else	      omodule_event_pro(3);
}

void omodule_prt_losscards_exec(void)
{
  unsigned long addr = 0;	
  unsigned long rdcid = 0;
  unsigned int n = 0;	
  unsigned int num = 0;//oparas_get_cardlost_num(0);
  unsigned char z_id = 0;
  if(strlen((char*)cur_input_cursor.str_input) > 0 ) 
  {
	  z_id = (unsigned char)atoi((char*)cur_input_cursor.str_input);
	  if(z_id > FLASH_L_CARD_MAX_Z ) return;
	  num = oparas_get_cardlost_num(z_id);
	  SELECT_PRINT();
	  oprt_printnormalcmd();
	  ocomm_printf(CH_PRINT,0,0,"挂失卡数量:%d\r\n",num);
	  addr = FLASH_L_CARD_SADDR;
	  for(n=0;n<num;n++)
	  {
		  addr += n*4;
		  oflash_readbuf(addr,4,(unsigned char*)&rdcid);
		  ocomm_printf(CH_PRINT,0,0,"%08ld\r\n",rdcid);
	  }	
	  oprt_lfrow(6);    		
	  SELECT_CARD();  	
   }
   okernel_putevent(KEY_RET_EV,0);
}

void omodule_prt_losscards(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = false;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 2;	
	okernel_addeventpro(KEY_OK_EV,  omodule_prt_losscards_exec);
	okernel_addeventpro(KEY_RET_EV, omodule_event_menu_ret_pro);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"打印挂失卡列表");
	ocomm_printf(CH_DISP,2,1,"请输入站编号:");	
}
void omodule_clear_losscards(void)
{
	unsigned char n = 0;
	unsigned int eepaddr = CARD_LOSTNUM_ADDR;//+z_id*2;
	unsigned int num = 0;
	eeprom_busy_wait();
	for(n=0; n<FLASH_L_CARD_MAX_Z; n++)
	{
		eeprom_write_word((uint16_t*)eepaddr, num);	
		ee24lc_write(eepaddr, 2 ,(unsigned char*)&num);
		eepaddr += n*2;	
	}
	oflash_erase_blocks(0);
	__WATCH_DOG();  
	oflash_erase_blocks(1);
	__WATCH_DOG();  
	oflash_erase_blocks(2);
	__WATCH_DOG();  
	oflash_erase_blocks(3);
	ocomm_message(4,1,"清除成功",10,NULL);
}

void omodule_ok_event(void)
{
	unsigned char sw = odisp_menu_getid();
	switch(sw)
	{
		case 0: //通讯测试
			omodule_comm_test_pro();
			return;//break;
		case 1: //Moxa测试
			omodule_moxa_test_pro();
			break;
		case 2: //键盘测试
			omodule_keyboard_test();
			return;
		case 3: //流量计测试
			omodule_test_modbus();
			return;
		case 4: //气相压力测试
			cur_module_test_sw = _PRESSURE_G_SW_;
			omodule_test_pressure();
			return;
		case 5:	//液相压力测试
			cur_module_test_sw = _PRESSURE_L_SW_;
			omodule_test_pressure();
			return;
		case 6:	//电磁阀测试
			omodule_test_vctrl();
			return;		
		case 7: //IC卡测试
			omodule_test_card();
			return;
		case 8:	//时钟测试
			omodule_rtc_time();
			return;
		case 9: //客显测试
			omodule_splc_disp_pro();
			return;
		case 10:	//打印测试
			omodule_print_test_pro();
			break;//return;
		case 11://泵联动测试
			omodule_test_pumpctrl();
			return;
		case 12://预冷测试
			omodule_precool_test();
			return;
		case 13://打印挂失列表	
			omodule_prt_losscards();
			return;
		case 14://清空挂失列表	
			omodule_clear_losscards();
			return;
		default:
			break;		
	}	
	okernel_putevent(KEY_RET_EV,0);	
}

void omodule_event_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_OMODULE_MENU)/sizeof(_OMODULE_MENU[0]);
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oset_main_pro);
    okernel_addeventpro(KEY_OK_EV,   omodule_ok_event);  
	okernel_addeventpro(KEY_1_EV, omodbus_g_test);
    okernel_addeventpro(KEY_2_EV, omodbus_l_test);
	if(id>maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_OMODULE_MENU,maxid,id);//oset_price_pro();//odisp_menu_pro(_OMODULE_MENU, sizeof(_OMODULE_MENU)/sizeof(_OMODULE_MENU[0]),id);//oset_price_pro();
}
