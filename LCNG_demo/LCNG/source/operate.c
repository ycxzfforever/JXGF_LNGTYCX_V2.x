/*
 * operate.c
 *
 * Created: 2012-8-9 14:31:43
 *  Author: luom
 */ 

#include "global_def.h"

prog_char  _OPERATE_MENU_[][16]={
  "换班",	
  "是否用卡加气",
  "是否自动打印",
  //"是否禁用压力",
  "是否回气计量",
  "是否时间校验",//"流量计实时检测",
  "是否记录校验",
  "是否班累优先",
  "是否检验钢瓶",
  "是否输入车牌",
  "现场解灰",
  "挂失", 
  "清零流量计", 
  "参数备份",
};

void ooperate_event_ret_pro(void)
{
	unsigned char maxid, id = odisp_menu_getid();
	maxid = sizeof(_OPERATE_MENU_)/sizeof(_OPERATE_MENU_[0]);
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          ooperate_event_pro(id);
}
void ooperate_y_n_y_event(void)
{
	unsigned int eepaddr = EEPROM_SYSPARAS_ADDR;
	unsigned char id = odisp_menu_getid();
	eepaddr  += offsetof(Sysparas, b_y_or_n_operate);
	cur_sysparas.b_y_or_n_operate |= (1<<(id-1));
	eeprom_busy_wait();	
	eeprom_write_byte((uint8_t*)eepaddr , cur_sysparas.b_y_or_n_operate);
	ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, b_y_or_n_operate), 1,(unsigned char*)&cur_sysparas.b_y_or_n_operate);
	ocomm_message(4,1,"设置成功",10, ooperate_event_ret_pro);
}
void ooperate_y_n_n_event(void)
{
	unsigned int eepaddr = EEPROM_SYSPARAS_ADDR;
	unsigned char id = odisp_menu_getid();
	eepaddr  += offsetof(Sysparas, b_y_or_n_operate);
	cur_sysparas.b_y_or_n_operate &= ~(1<<(id-1));
	eeprom_busy_wait();	
	//eeprom_write_byte(EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, b_y_or_n_operate), cur_sysparas.b_y_or_n_operate); //08-30
	eeprom_write_byte((uint8_t*)eepaddr, cur_sysparas.b_y_or_n_operate);
	ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, b_y_or_n_operate), 1,(unsigned char*)&cur_sysparas.b_y_or_n_operate);
	ocomm_message(4,1,"设置成功",10, ooperate_event_ret_pro);
}

void ooperate_y_or_n_pro(unsigned char id)
{
	bool		  bok = false;
	unsigned char msk = 0;
	msk = 1<<(id-1);
	if( cur_sysparas.b_y_or_n_operate&msk) bok = true;
	olcm_clrscr();
	olcm_dispprogconst(1,2,&_OPERATE_MENU_[id][0], 1);
	if( true == bok ) ocomm_printf(CH_DISP,1,8,"√"); //√ ●
	else			  ocomm_printf(CH_DISP,1,8,"×"); //×
	ocomm_printf(CH_DISP, 2,1,"按【确认】  是");
	ocomm_printf(CH_DISP, 3,1,"按【停止】  否");		
	ocomm_printf(CH_DISP, 4,1,"按【返回】");		
	okernel_clreventpro();
    okernel_addeventpro(KEY_OK_EV,   ooperate_y_n_y_event);		
    okernel_addeventpro(KEY_STOP_EV, ooperate_y_n_n_event);	
	okernel_addeventpro(KEY_RET_EV,  ooperate_event_ret_pro);		
}

//换班

void ooperate_shift_exchange_setok_event(void)
{
	unsigned char newid = 0 ;
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //数据输入
	{
		newid = atoi((char*)cur_input_cursor.str_input);
		ostore_shift_rpt_save(newid);
		ocomm_message(4,1,"换班成功",10, oui_main_ui);//ooperate_event_ret_pro);
		return;
	}		
	okernel_putevent(KEY_RET_EV,0);
}
void ooperate_exshift_input_event(void)
{
	ocomm_input_clear(); //2013-01-09	
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = false;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 2;	
	okernel_addeventpro(KEY_OK_EV,  ooperate_shift_exchange_setok_event);
	okernel_addeventpro(KEY_RET_EV, ooperate_event_ret_pro);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,3,"换班");	
	ocomm_printf(CH_DISP,2,1,"请输入班号:");
}
void ooperate_exshift_auto_event(void)
{
	ocomm_message(4,1,"换班失败",10, oui_main_ui);//ooperate_event_ret_pro);
}
void ooperate_shift_exchange_event(void)
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,3,"换班");
	ocomm_printf(CH_DISP,2,1,"1.输入班号");
	ocomm_printf(CH_DISP,3,1,"2.自动换班");		
	okernel_addeventpro(KEY_1_EV, ooperate_exshift_input_event);	
	okernel_addeventpro(KEY_2_EV, ooperate_exshift_auto_event);	
	okernel_addeventpro(KEY_RET_EV, ooperate_event_ret_pro);	//okernel_addeventpro(KEY_RET_EV, ooperate_event_ret_pro);	
}
void ooperate_losscard_setok_event(void)
{
	unsigned long lcid = 0 ;
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //数据输入
	{
		lcid = atol((char*)cur_input_cursor.str_input);
		if(ostore_losscard_save(lcid))
		{
			ocomm_message(4,1,"挂失成功",10, ooperate_event_ret_pro);	
			return;
		}
	}		
	okernel_putevent(KEY_RET_EV,0);	
}
void ooperate_losscard_event(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = false;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 9;	
	okernel_addeventpro(KEY_OK_EV,  ooperate_losscard_setok_event);
	okernel_addeventpro(KEY_RET_EV, ooperate_event_ret_pro);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,3,"挂失");
	ocomm_printf(CH_DISP,2,1,"请输入卡号:");	
}
void ooperate_ok_event(void)
{
	unsigned char sw = odisp_menu_getid();
	switch(sw)
	{
		case 0: //换班
			ooperate_shift_exchange_event();
			break;
		case 1: //用卡加气
		case 2: //自动打印
		case 3: //是否回气计量
		case 4:	//时间校验 //流量计实时检测
		case 5:	//流水号校验
		case 6: //班累优先 
		case 7://是否检验钢瓶
		case 8://是否输入车牌
			ooperate_y_or_n_pro(sw);
			break;
		case 9: //现场解灰
			oapp_iccard_ungray_pro();
			break;		
		case 10://挂失
			ooperate_losscard_event();
			break;
		case 11:
			break;	
		default:
			okernel_putevent(KEY_RET_EV,0);	
			return;
	}
	
}
void ooperate_event_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_OPERATE_MENU_)/sizeof(_OPERATE_MENU_[0]);
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oset_main_pro);
    okernel_addeventpro(KEY_OK_EV, ooperate_ok_event);		
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_OPERATE_MENU_, maxid,id);//oset_price_pro();
}
