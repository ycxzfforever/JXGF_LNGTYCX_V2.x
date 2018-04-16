/*
 * price.c
 *
 * Created: 2012-8-9 16:28:27
 *  Author: luom
 */ 

#include "global_def.h"
static unsigned char  cur_price_id = 0;
extern void oset_card_price(void);
extern void oset_price_pro(void);
/*
Task_Event  _PARAS_CARDPRICE_Pro[]  = 
{
  {KEY_OK_EV,			oset_card_price}, 
  {KEY_RET_EV,			oset_price_pro}, 
};
*/
void oset_cardprice_pro(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_pt  = true;
	cur_input_cursor.b_psw = false;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 5;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 5;
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oset_price_pro);		
    okernel_addeventpro(KEY_OK_EV, oset_card_price);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	switch(cur_price_id)
	{
		case 1:
			ocomm_printf(CH_DISP,1,2,"无卡加气单价"); 
			break;
		case 2:
			ocomm_printf(CH_DISP,1,2,"用户卡单价"); 
			break;
		case 3:
			ocomm_printf(CH_DISP,1,2,"员工卡单价"); 
			break;
		case 4:
			ocomm_printf(CH_DISP,1,2,"记账卡单价"); 
			break;	
		case 5:
			ocomm_printf(CH_DISP,1,2,"维修卡单价"); 
			break;
		case 6:
			ocomm_printf(CH_DISP,1,2,"内部卡单价"); 
			break;
		case 7:
			ocomm_printf(CH_DISP,1,2,"积分卡单价"); 
			break;				
		default:
			okernel_putevent(KEY_RET_EV,0); 
			return;	

	}
	ocomm_printf(CH_DISP,2,1,"单价:%.02f",cur_sysparas.f_price[cur_price_id-1]); 
	ocomm_printf(CH_DISP,3,1,"请输入:");
}

void oset_card_price(void)
{
	unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
	if(strlen((char*)cur_input_cursor.str_input) >0 ) 
	{
		//cur_sysparas.f_price[cur_setmenu_id-1] = atof(cur_input_cursor.str_input);
		cur_sysparas.f_price[cur_price_id-1] = atof((char*)cur_input_cursor.str_input);
		eeprom_busy_wait();
		eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] ,(unsigned int*)eepaddr ,sizeof(struct def_SysParas));
		ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
		cur_price_id = 0;
		return ocomm_message(4,1,"设置成功",10,NULL);
	}	
	okernel_putevent(KEY_RET_EV,0);
}
void oset_nocard_price(void)
{
	cur_price_id = 1;
	oset_cardprice_pro();
}
void oset_usrcard_price(void)
{
	cur_price_id = 2;
	oset_cardprice_pro();
}
void oset_staffcard_price(void)
{
	cur_price_id = 3;
	oset_cardprice_pro();
}
void oset_scorecard_price(void)
{
	cur_price_id = 4;
	oset_cardprice_pro();
}

void oset_membercard_price(void)//内部卡
{
	cur_price_id = 5;
	oset_cardprice_pro();
}
void oset_debitcard_price(void)//记账卡
{
	cur_price_id = 6;
	oset_cardprice_pro();
}
void oset_servicecard_price(void) //维修卡
{
	cur_price_id = 7;
	oset_cardprice_pro();
}

prog_char  _PRICE_PARAS_MENU_[][16]=
{
	"1.无卡  2.用户卡",
	"3.员工卡4.记账卡",
	"5.维修卡6.内部卡",
	"7.积分卡",
};

void oset_price_pro(void)
{
  olcm_clrscr();	
  olcm_dispprogconst(1, 1, &_PRICE_PARAS_MENU_[0][0],1);  
  olcm_dispprogconst(2, 1, &_PRICE_PARAS_MENU_[1][0],1);
  olcm_dispprogconst(3, 1, &_PRICE_PARAS_MENU_[2][0],1);  	
  olcm_dispprogconst(4, 1, &_PRICE_PARAS_MENU_[3][0],1);
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_paras);		
  okernel_addeventpro(KEY_1_EV, oset_nocard_price);  
  okernel_addeventpro(KEY_2_EV, oset_usrcard_price);  
  okernel_addeventpro(KEY_3_EV, oset_staffcard_price);  
  okernel_addeventpro(KEY_4_EV, oset_scorecard_price);  
  okernel_addeventpro(KEY_5_EV, oset_membercard_price);  
  okernel_addeventpro(KEY_6_EV, oset_debitcard_price);  
  okernel_addeventpro(KEY_7_EV, oset_servicecard_price);  
}
