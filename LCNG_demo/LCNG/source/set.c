/*
 * set.c
 *
 * Created: 2012-8-7 10:16:30
 *  Author: luom
 */ 
#include "global_def.h"

static unsigned char  cur_setmenu_id = 0;
static unsigned char  cur_setmenu_c_id = 0; 
extern void oset_paras_pswchk(void);
void ooperate_pump_type(void);
void ooperate_pump_interlock_set(void);
void oset_sys_event_pro(unsigned char id);

prog_char  _PARAS_MENU_[][16]={
   "1.单价  2.参数",
   "3.控制  4.系统",
   "5.系数  6.密码",
};

void oparas_basic_event(void)
{
	oparas_basic_pro(0);
};

void oset_ctrl_pro(void) //设置控制
{
	olngctrl_menuevent_pro(0);	 
}

void oset_system_pro(void)
{
	oset_sys_event_pro(0);	
}
prog_char  _OSET_SYS_MENU_[][16]={
  "流量计",
  "客显屏",
  "读卡器",	//电磁阀   
  "模拟加气",	   
  "自动模拟加气",
  "泵连接类型",
};
void oset_system_event_ret_pro(void)
{
	unsigned char maxid, id = odisp_menu_getid();
	maxid = sizeof(_OSET_SYS_MENU_)/sizeof(_OSET_SYS_MENU_[0]);
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          oset_sys_event_pro(id);
}
void ooperate_setcancel_ret(void)
{
	oset_sys_event_pro(cur_setmenu_c_id);
}
void ooperate_setok_save_ret(void)
{
	oset_sys_event_pro(cur_setmenu_id);
}
void ooperate_setok_save_pro(void)
{
	unsigned int eep_addr = EEPROM_SYSPARAS_ADDR+offsetof(Sysparas, b_basic_p);
	eeprom_busy_wait();	
	eeprom_write_block((unsigned char*)&cur_sysparas.b_basic_p[0] , (unsigned int*)eep_addr ,8);	
	ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, b_basic_p), 8,(unsigned char*)&cur_sysparas.b_basic_p[0]);
	ocomm_message(4,1,"设置成功",10, ooperate_setok_save_ret); 	
}
////读卡器设置//////////////////////////////////////////////////////////////////
prog_char  _OSET_CARD_TYPE_MENU_[][16]={
  "M1卡读卡器",	  
  "CPU 读卡器",
  "CPU+PSAM卡",
  "其他",
};

void ooperate_cardtype_menu_ok_event(void)
{
	//cur_setmenu_id = 3;
	ocomm_message(4,1,"设置失败",10, NULL);

}
void ooperate_card_type(unsigned char id)
{
	unsigned char maxid = sizeof(_OSET_CARD_TYPE_MENU_)/sizeof(_OSET_CARD_TYPE_MENU_[0]);
	unsigned char s_id = 1;
	cur_setmenu_c_id = 2;
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV,  ooperate_cardtype_menu_ok_event);
	okernel_addeventpro(KEY_RET_EV, ooperate_setcancel_ret); //	oset_system_event_ret_pro
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else
#ifdef MENU_PRO_EX		
	odisp_menu_pro_ex(_OSET_CARD_TYPE_MENU_, maxid,id,s_id-1);
#else
       odisp_menu_pro(_OSET_CARD_TYPE_MENU_, maxid,id);
#endif
}

////////流量计设置
prog_char  _OSET_MODBUS_TYPE_MENU_[][16]={ //16
  "罗斯蒙特", //加气机类型	  
  "E+H",
  "西门子",
};

void ooperate_modbus_menu_ok_event(void)
{
	//unsigned char mtype=0;
	unsigned char mtype=0, nmtype = 0;
	bool          bmodbus_chk_ok = false;
	//unsigned char nkeyid = 0;
	mtype = odisp_menu_getid()+1;//cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE]  = odisp_menu_getid()+1;
	cur_setmenu_id = 1;
	nmtype = _MODBUS_L_;
	cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] = mtype;

	bmodbus_chk_ok = omobus_init_chk(mtype,nmtype);
	if( false == bmodbus_chk_ok)
	{
		ocomm_message(4,1,"流量计通讯错误",20, ooperate_setok_save_ret); 
		return;
	}
	if(omobus_init_reg())
	{
		ooperate_setok_save_pro();		
	}
}
void ooperate_modbus_type(void)
{
	unsigned char maxid = sizeof(_OSET_MODBUS_TYPE_MENU_)/sizeof(_OSET_MODBUS_TYPE_MENU_[0]);
	unsigned char s_id = cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE];//1;//
	cur_setmenu_c_id = 0;
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV,  ooperate_modbus_menu_ok_event);
	okernel_addeventpro(KEY_RET_EV, ooperate_setcancel_ret);
	if(s_id>maxid) okernel_putevent(KEY_RET_EV,0);
	else
     #ifdef MENU_PRO_EX	
	odisp_menu_pro_ex(_OSET_MODBUS_TYPE_MENU_, maxid,0,s_id-1);
     #else
	odisp_menu_pro(_OSET_MODBUS_TYPE_MENU_, maxid,s_id); 
     #endif
}
///////客显
prog_char  _OSET_DISP_TYPE_MENU_[][16]={ //16
  "黄屏",
  "蓝屏",
  "新屏",
};

void ooperate_disp_menu_ok_event(void)
{
	cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE]  = odisp_menu_getid()+1;
	if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_Y)
	{
		osplc_show(0,0,0,0,true);
	}		
	else if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_B)
	{
		osplc_show_ex(0,0,0,0,true);
	}
	else if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_N){
	  odisp_mem_disp_init0();			
	}
	cur_setmenu_id = 2;
	ooperate_setok_save_pro();	
}
void ooperate_disp_type(void)
{
	unsigned char maxid = sizeof(_OSET_DISP_TYPE_MENU_)/sizeof(_OSET_DISP_TYPE_MENU_[0]);
	unsigned char s_id = cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE];
	cur_setmenu_c_id = 1;
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV,  ooperate_disp_menu_ok_event);
	okernel_addeventpro(KEY_RET_EV, ooperate_setcancel_ret);
	if(s_id>maxid) okernel_putevent(KEY_RET_EV,0);
	else
    #ifdef MENU_PRO_EX		
	odisp_menu_pro_ex(_OSET_DISP_TYPE_MENU_, maxid,0,s_id-1);
    #else
	odisp_menu_pro(_OSET_DISP_TYPE_MENU_, maxid,s_id); 
    #endif
}

////一泵单机和双机设置
prog_char  _OSET_PUMP_TYPE_MENU_[][16]={ //16
  "一泵一枪",
  "一泵双枪",
  "其它"
};

prog_char _OSET_PUMP_INTERLOCK_[][16]={
"互锁线一输入",
"互锁线一输出",
"互锁线二输入",
"互锁线二输出",
};

static unsigned char pump_interlock_var =0;

void ooperate_pump_interlock(void)
{
       unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
        pump_interlock_var = odisp_menu_getid();
        switch(pump_interlock_var) 
        {
                case 0:
			   {
			   	//添加A线输入初使化代码
			   	INTER_LINE_A_IN();
				INTER_LINE_A_LOW();
			   	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_INPUT;
			       eeprom_busy_wait();
				eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
				ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
				ocomm_message(4,1,"设置成功",10, ooperate_pump_interlock_set);		
                	   }
			   break;	
		  case 1:
		  	   {
			   	//添加A线输出初使化代码
			   	INTER_LINE_A_OUT();
			   	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_OUTPUT;
			       eeprom_busy_wait();
				eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
				ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
				ocomm_message(4,1,"设置成功",10, ooperate_pump_interlock_set);	
			    }
			   break;
		  case 2:
		  	   {
			   	//添加B线输入初使化代码
			   	INTER_LINE_B_IN();
				INTER_LINE_B_LOW();
			   	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_INPUT;
			       eeprom_busy_wait();	
				eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
				ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
				ocomm_message(4,1,"设置成功",10, ooperate_pump_interlock_set);	
			   }break;
		  case 3:
		  	    {
				//添加B线输出初使化代码
				INTER_LINE_B_OUT();
			   	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_OUTPUT;
			       eeprom_busy_wait();
				eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
				ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
				ocomm_message(4,1,"设置成功",10, ooperate_pump_interlock_set);		
			    }
		  	    break;
		 default:
		 	  okernel_putevent(KEY_RET_EV,0);
		 	  break;
        }
}


void ooperate_pump_interlock_set(void)
{
   /* unsigned char maxid = sizeof(_OSET_PUMP_INTERLOCK_)/sizeof(_OSET_PUMP_INTERLOCK_[0]);
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, ooperate_pump_type);
	okernel_addeventpro(KEY_OK_EV, ooperate_pump_interlock);
	if(0>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_OSET_PUMP_INTERLOCK_, maxid ,0);*/
#if LNG_LIANDONG
	INTER_LINE_A_IN();
	INTER_LINE_A_LOW();
	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_INPUT;
	INTER_LINE_B_OUT();
	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_OUTPUT;
#else
	INTER_LINE_A_OUT();
	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_OUTPUT;
	INTER_LINE_B_IN();
	INTER_LINE_B_LOW();
	cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_INPUT;
#endif
	eeprom_busy_wait();
	eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] ,EEPROM_SYSPARAS_ADDR,sizeof(struct def_SysParas));
	ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
}

void ooperate_pump_type_ok_event(void)
{
      unsigned int eep_addr = EEPROM_SYSPARAS_ADDR+offsetof(Sysparas, b_basic_p);
       cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] = odisp_menu_getid()+1;
       cur_setmenu_id = 6;
	//ocomm_printf(CH_DISP,3,1,"泵类型:%d",cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH]);    
	//ooperate_setok_save_pro();
	eeprom_busy_wait();
	eeprom_write_block((unsigned char*)&cur_sysparas.b_basic_p[0] , (unsigned int*)eep_addr ,8);	
	ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, b_basic_p), 8,(unsigned char*)&cur_sysparas.b_basic_p[0]);

	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_ONE_MECH)
	{
	      ocomm_message(4,1,"设置成功",10, ooperate_setok_save_ret);
	}
	else if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
             //ooperate_pump_interlock_set();
         ocomm_message(4,1,"设置成功",10, ooperate_pump_interlock_set);    
	}
}

void ooperate_pump_type(void)
{
      unsigned char maxid = sizeof(_OSET_PUMP_TYPE_MENU_)/sizeof(_OSET_PUMP_TYPE_MENU_[0]);
      unsigned char s_id =  cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH];
      	cur_setmenu_c_id = 5;
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV,  ooperate_pump_type_ok_event);
	okernel_addeventpro(KEY_RET_EV, ooperate_setcancel_ret);
	if(s_id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else
    #ifdef MENU_PRO_EX
	odisp_menu_pro_ex(_OSET_PUMP_TYPE_MENU_, maxid,0,s_id-1);
    #else
	odisp_menu_pro(_OSET_PUMP_TYPE_MENU_, maxid,s_id); 
    #endif  
}

void oset_systemok_event(void)
{
unsigned char sw = odisp_menu_getid();
	switch(sw)
	{
		case 0: //流量计
			ooperate_modbus_type();
			break;
		case 1: //客显
			ooperate_disp_type();
			break;
		case 2: //读卡器
			ooperate_card_type(0);
			break;
		case 3:
			b_Simulation = true;
			b_Simu_auto  = false;
			ocomm_message(4,1,"设置成功",10,oui_main_ui);	
			break;
		case 4:
			b_Simulation = true;
			b_Simu_auto  = true;
			ocomm_message(4,1,"设置成功",10,oui_main_ui);
			break;
		case 5:
			 ooperate_pump_type();
			 break;
		default:
			okernel_putevent(KEY_RET_EV,0);	
			break;
	}			
}
void oset_sys_event_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_OSET_SYS_MENU_)/sizeof(_OSET_SYS_MENU_[0]);
	okernel_clreventpro();
    	okernel_addeventpro(KEY_RET_EV, oset_paras);
    	okernel_addeventpro(KEY_OK_EV,  oset_systemok_event);	
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_OSET_SYS_MENU_, maxid,id);
}

void oset_system_pro(void);
void oset_paras(void)
{
  olcm_clrscr();	
  olcm_dispprogconst(1, 1, &_PARAS_MENU_[0][0],1);  
  olcm_dispprogconst(2, 1, &_PARAS_MENU_[1][0],1);
  olcm_dispprogconst(3, 1, &_PARAS_MENU_[2][0],1); 
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
  okernel_addeventpro(KEY_1_EV, oset_price_pro);		//单价	
  okernel_addeventpro(KEY_2_EV, oparas_basic_event);	//参数
  okernel_addeventpro(KEY_3_EV, oset_ctrl_pro);		//控制
  okernel_addeventpro(KEY_4_EV, oset_system_pro);	//系统
  okernel_addeventpro(KEY_5_EV, oparas_factor_pro);	//系数
  okernel_addeventpro(KEY_6_EV, opsw_modify_pro);	//密码
}

void oset_paras_pswchk(void)
{
	unsigned char sw = 0;
	char rdpsw[7] ;
	unsigned char len = 0;	
	unsigned int  eepaddr = SUPER_PSW_ADDR;
eeprom_busy_wait();
eeprom_read_block((unsigned char*)&rdpsw ,(unsigned int*)eepaddr  ,7);

	len = strlen((char*)cur_input_cursor.str_input);
	if(PSW_HI_SUPER == super_psw_type)
	{
		//if(memcmp(cur_input_cursor.str_input,ADMIN_PSW_EX,6) !=0) 
		if(memcmp((unsigned char*)&rdpsw[1],cur_input_cursor.str_input,len)!= 0 && memcmp(cur_input_cursor.str_input,ADMIN_PSW,6) !=0 )
		{
			ocomm_message(4,1,"密码错误",10, NULL);
			//ocomm_message(3,1,"%c %c %c %c %c %c",rdpsw[0],rdpsw[1],rdpsw[2],rdpsw[3],rdpsw[4],rdpsw[5]);
			//ocomm_printf(CH_DISP,4,1,"密码错误");
			//ocomm_printf(CH_DISP,3,1,"%c %c %c %c %c %c %c",rdpsw[0],rdpsw[1],rdpsw[2],rdpsw[3],rdpsw[4],rdpsw[5],rdpsw[6]);
			return;
		}
	}	
	else
	{
		if(PSW_SUPER == super_psw_type)
		{
			if(memcmp(cur_input_cursor.str_input,ADMIN_PSW,6) !=0 && memcmp(cur_input_cursor.str_input,ADMIN_PSW_EX,6) !=0) 
			{
				ocomm_message(4,1,"密码错误",10, NULL);
				return;
			}		
		}
		else if(PSW_NORMAL == super_psw_type)
		{
			if(memcmp((unsigned char*)&rdpsw[1],cur_input_cursor.str_input,len)!= 0 && memcmp(cur_input_cursor.str_input,ADMIN_PSW,6)!=0  && memcmp(cur_input_cursor.str_input,ADMIN_PSW_EX,6) !=0) 
			{
				ocomm_message(4,1,"密码错误",10, NULL);
				return;
			}	
		}
	} 	
	
	if( len >0 ) 
	{
		sw = cur_setmenu_id;
		cur_setmenu_id = 0;
		switch(sw)
		{
			case 1:
				oset_paras();//设置系数
				break;
			case 2:
				ooperate_event_pro(0);//设置操作
				break;
			case 3:
				omodule_event_pro(0);//模块测试
				break;
			case 4:
				ofactory_set_chk_pro();//出厂设置
				break;
			default:
				break;
		}		
	}
	else
	{
		okernel_putevent(KEY_RET_EV,0);
	}
}
void oset_password_pro(unsigned char psw_type)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = true;
	cur_input_cursor.p_x   = 2;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
    	okernel_addeventpro(KEY_OK_EV,  oset_paras_pswchk);
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	super_psw_type = psw_type;
	ocomm_printf(CH_DISP,1,1,"请输入密码:"); 
}

prog_char  _UI_SET_MENU[][16]=
{
	"1.设置参数",
	"2.设置操作",
	"3.模块测试", 
	"4.出厂设置",
};

void oset_paras_event(void) //设置参数
{
	cur_setmenu_id = 1;
	oset_password_pro(PSW_HI_SUPER);
}

void oset_operate_event(void) //设置操作
{
	cur_setmenu_id = 2;
	oset_password_pro(PSW_HI_SUPER);
}
void oset_module_event(void) //模块测试
{
	cur_setmenu_id = 3;
	oset_password_pro(PSW_HI_SUPER);	
}
void oset_factory_event(void) //出厂设置"
{
	cur_setmenu_id = 4;
	oset_password_pro(PSW_HI_SUPER);	
}

void oset_main_pro(void)
{
  olcm_clrscr();	
  olcm_dispprogconst(1, 1, &_UI_SET_MENU[0][0],1);  
  olcm_dispprogconst(2, 1, &_UI_SET_MENU[1][0],1);
  olcm_dispprogconst(3, 1, &_UI_SET_MENU[2][0],1);  
  olcm_dispprogconst(4, 1, &_UI_SET_MENU[3][0],1);  
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oui_main_ui);//返回主界面	
  okernel_addeventpro(KEY_1_EV,  oset_paras_event);//设置参数
  okernel_addeventpro(KEY_2_EV,  oset_operate_event);//设置操作
  okernel_addeventpro(KEY_3_EV,  oset_module_event);	//模块测试
  okernel_addeventpro(KEY_4_EV,  oset_factory_event);	//出厂设置

  //okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
  //otimer_reqire(LCNG_COMM_EVENT4_EV,30, 0, 1);//延时1秒执行 
}
