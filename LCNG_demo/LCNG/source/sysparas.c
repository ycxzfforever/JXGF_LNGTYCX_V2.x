/*
 * sysparas.c
 *
 * Created: 2012-8-9 10:05:36
 *  Author: luom
 */ 
#include "global_def.h"

static unsigned char cur_setfactor_id = 0;

unsigned int oparas_get_cardlost_num(unsigned char z_id)
{
	unsigned int eepaddr = CARD_LOSTNUM_ADDR+z_id*2;
	eeprom_busy_wait();
	return eeprom_read_word((uint16_t*)eepaddr);
}

bool         oparas_set_cardlost_num(unsigned char z_id,unsigned int num)
{
	unsigned int eepaddr = CARD_LOSTNUM_ADDR+z_id*2;
	eeprom_busy_wait();
	eeprom_write_word((uint16_t*)eepaddr, num);	
	ee24lc_write(eepaddr, 2 ,(unsigned char*)&num);
	return true;
}

bool oparas_factor_value_chk(unsigned char id, float vset)
{
	if(id == BASIC_P_FACTOR_DENSTY+1 && vset> 1.5)
	{
		ocomm_printf(CH_DISP,4,1,"���볬��"); 
		return false;
	}
	return true;
}

void oparas_factor_setok_event(void)
{
	float vftmp = 0.0;
	unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
	if(strlen((char*)cur_input_cursor.str_input) >0 )  //��������
	{
		vftmp = atof((char*)cur_input_cursor.str_input);
		if(oparas_factor_value_chk(cur_setfactor_id, vftmp))
		{
			cur_sysparas.b_factor[cur_setfactor_id-1] = vftmp;
			eeprom_busy_wait();
			eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0]  , (unsigned int*)eepaddr ,sizeof(struct def_SysParas)); ////eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , &ee_addr ,sizeof(struct def_SysParas));
			ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
			//dlycnt = 30;
			cur_setfactor_id = 0;
			ocomm_message(4,1,"���óɹ�",30,NULL);
		}
	}
	cur_setfactor_id = 0;
	okernel_putevent(KEY_RET_EV,0);
}

void oparas_factor_set_pro(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = true;
	cur_input_cursor.p_x   = 2;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oparas_factor_pro);
    okernel_addeventpro(KEY_OK_EV, oparas_factor_setok_event);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"������:%.04f",cur_sysparas.b_factor[cur_setfactor_id-1]); 
}

void oparas_factor_densty_event(void) //����������ϵ��
{
	cur_setfactor_id = 1;
	oparas_factor_set_pro();	
}

void oparas_factor_dfactor_event(void) //��Һǹϵ��
{
	cur_setfactor_id = 2;
	oparas_factor_set_pro();	
}

void oparas_factor_pzero_event(void) //����ѹ�����
{
	cur_setfactor_id = 3;
	oparas_factor_set_pro();	
}

void oparas_factor_pfactor_event(void) //����ѹ��ϵ��
{
	cur_setfactor_id = 4;
	oparas_factor_set_pro();	
}

void oparas_factor_pzero_l_event(void)//Һ��ѹ�����
{
      cur_setfactor_id = 5;
	oparas_factor_set_pro();	
}

void oparas_factor_pfactor_l_event(void)//Һ��ѹ��ϵ��
{
        cur_setfactor_id = 6;
	oparas_factor_set_pro();	
}

prog_char  _FACTOR_MENU_[][16]={
   "1.������ϵ��",
   "2.��Һǹϵ��",
   "3.����ѹ�����",
   "4.����ѹ��ϵ��",
   "5.Һ��ѹ�����",
   "6.Һ��ѹ��ϵ��",
};

unsigned char sw_param = 0;

/////////����ϵ������/////////////////////
void set_parammenu_ok_pro(void)
{
         sw_param = odisp_menu_getid();
	switch(sw_param)
	{
		 case 0://������ϵ��
			oparas_factor_densty_event();
			return;
		case 1: //��Һǹϵ��
			oparas_factor_dfactor_event();
			return;
		case 2: //����ѹ�����
			oparas_factor_pzero_event();
			return;
		case 3: //����ѹ��ϵ��
		      oparas_factor_pfactor_event();
			return;
		case 4: //Һ��ѹ�����
		     oparas_factor_pzero_l_event();
			return;
		case 5:	//Һ��ѹ��ϵ��
		    oparas_factor_pfactor_l_event();
			return;
		default:
			break;
	}
	okernel_putevent(KEY_RET_EV,0);	
}

void oparas_param_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_FACTOR_MENU_)/sizeof(_FACTOR_MENU_[0]);
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV,  oset_paras);	
    okernel_addeventpro(KEY_OK_EV,   set_parammenu_ok_pro);	
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_FACTOR_MENU_, maxid ,id);
}


void oparas_factor_pro(void)
{
  /*
  olcm_clrscr();	
  olcm_dispprogconst(1, 1, &_FACTOR_MENU_[0][0],1);  
  olcm_dispprogconst(2, 1, &_FACTOR_MENU_[1][0],1);
  olcm_dispprogconst(3, 1, &_FACTOR_MENU_[2][0],1);  
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_paras);
  okernel_addeventpro(KEY_1_EV, oparas_factor_densty_event);//�ܶ�
  okernel_addeventpro(KEY_2_EV, oparas_factor_dfactor_event);//ϵ��
  okernel_addeventpro(KEY_3_EV, oparas_factor_pzero_event);  //ѹ�����
  okernel_addeventpro(KEY_4_EV, oparas_factor_pfactor_event);  //ѹ��ϵ��
 */
 oparas_param_pro(sw_param);
}






//////////////////////////////////////////////////////////////////////////////
static unsigned char cur_setbasic_id = 0;
prog_char  _BASIC_PARAS_MENU[][16]={
   "ǹ��",
   "���",
   "�����Ʋ�����",
   "��̨ͨѶ������",
   "����վ���",
   "����С���",
   "������",
   "��ˮ��",
   "�Ѵ���ˮ��", //δ�ϴ�
   "������ˮ��",
   "�Ѵ�������ˮ��",
   "��ǹ��",
   "�ܽ��",
   "��������",
   "������",
};
void oparas_basic_setcardpswok_event(void)
{
	unsigned char len = strlen((char*)cur_input_cursor.str_input);
	unsigned char buf[6] = {'0','0','0','0','0','0'};
	memcpy(&buf[6-len],cur_input_cursor.str_input,len);
	ee24lc_write(CARD_PSW_ADDR, 6 ,buf);
	ocomm_message(4,1,"���óɹ�",10,NULL);	
}
void oparas_basic_idnumsetok_event(unsigned char id, unsigned long vset, unsigned int e_addr)
{
	unsigned long addr = EEPROM_SYSINFO_ADDR;
	switch(id)
	{
		case 7:
			cur_sysparas.r_cur_g_id = vset;
			cur_sysinfo.g_uid = vset;
			ee24lc_write(addr, 4 ,(unsigned char*)&cur_sysinfo.g_uid);
			break;
		case 8:
			if(vset>cur_sysparas.r_cur_g_id)
			{
				return ocomm_message(4,1,"���ô���",10,NULL);
			}		
			cur_sysparas.r_unupld_g_id = vset;
			cur_sysinfo.unupld_rec_id  = vset;
			addr += offsetof(Sysinfo, unupld_rec_id);
			ee24lc_write(addr, 4 ,(unsigned char*)&cur_sysinfo.unupld_rec_id);			
			break;	
		case 9:
			cur_sysparas.r_cur_shift_id = vset;
			cur_sysinfo.shift_uid = vset; 
			addr += offsetof(Sysinfo, shift_uid);
			ee24lc_write(addr, 4 ,(unsigned char*)&cur_sysinfo.shift_uid);
			break;	
		case 10:
			if(vset>cur_sysparas.r_cur_shift_id)
			{
				return ocomm_message(4,1,"���ô���",10,NULL);;
			}
			cur_sysparas.r_unupld_shift_id = vset;
			cur_sysinfo.unupld_rpt_id = vset;
			addr += offsetof(Sysinfo, unupld_rpt_id);
			ee24lc_write(addr, 4 ,(unsigned char*)&cur_sysinfo.unupld_rpt_id);
			break;	
		default:
			okernel_putevent(KEY_RET_EV,0);
			return;	
	}
	ee24lc_write(e_addr, 18 ,(unsigned char*)&cur_sysparas.r_comp_id);
	ocomm_message(4,1,"���óɹ�",10,NULL);	
}
void oparas_basic_setok_event(void)
{
	unsigned long ultmp = 0;
	unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //��������
	{
		ultmp = atol((char*)cur_input_cursor.str_input); //			case BASIC_P_MODBUS_TYPE:
		switch(cur_setbasic_id)
		{
			case BASIC_P_G_ID:
				cur_sysparas.b_basic_p[cur_setbasic_id] = (unsigned char)ultmp;	
				break;	
			case BASIC_P_SHIFT_ID:
				cur_sysparas.b_basic_p[cur_setbasic_id] = (unsigned char)ultmp;	
				ostore_shift_rpt_save((unsigned char)ultmp);
				break;
			case BASIC_P_STATION_ID:
				cur_sysparas.b_basic_p[cur_setbasic_id] = (unsigned char)ultmp;	
				break;			
			case 5:  //����С���
				cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY] = (unsigned char)ultmp;	
				break;
				
			case BASIC_P_FLOW_BAUDRATE:
				cur_sysparas.b_basic_p[BASIC_P_FLOW_BAUDRATE] = (unsigned char)(ultmp/1200);
				omodbus_init((unsigned long)((unsigned long)cur_sysparas.b_basic_p[BASIC_P_FLOW_BAUDRATE]*1200));
				break;
			case BASIC_P_PC_BAUDRATE:
				cur_sysparas.b_basic_p[BASIC_P_PC_BAUDRATE] = (unsigned char)(ultmp/1200);
				ouart2_init((unsigned long)((unsigned long)cur_sysparas.b_basic_p[BASIC_P_PC_BAUDRATE]*1200)); 
				break;	
			case 6:  //������
				return oparas_basic_setcardpswok_event();
			case 7:  //��ˮ��
			case 8:  //δ�ϴ���ˮ��
			case 9:  //������ˮ��
			case 10: //δ�ϴ�������ˮ��
				eepaddr += 	offsetof(Sysparas, r_comp_id);
				return oparas_basic_idnumsetok_event(cur_setbasic_id, ultmp, eepaddr);		
			//case 14: //����С���		
			case 11:
				  memset(cur_sysinfo.g_sum,0,6);
				  ocomm_hex2bcdbuf(ultmp, (char *)cur_sysinfo.g_sum,6);
				 //ocomm_message(4,1,"���óɹ�",10,NULL);
				 break;
			case 12:
				memset(cur_sysinfo.m_amount,0,6);
				ocomm_hex2bcdbuf(ultmp, (char *)cur_sysinfo.m_amount,6);
				 break;
			case 13:
				 cur_sysinfo.gas_count = ultmp;
				 break;
			case 14:
				 memset(cur_sysinfo.gas_total,0,6);
				 ocomm_hex2bcdbuf(ultmp, (char *)cur_sysinfo.gas_total,6);
				 break;
			default:
				okernel_putevent(KEY_RET_EV, 0);
				return;
		}
		//eeprom_busy_wait();	
		//eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
		//ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
		if(cur_setbasic_id <= 10)
		{
			eepaddr += offsetof(Sysparas, b_basic_p);
			eeprom_busy_wait();	
			eeprom_write_block((unsigned char*)&cur_sysparas.b_basic_p[0] , (unsigned int*)eepaddr,sizeof(cur_sysparas.b_basic_p));
			ee24lc_write(eepaddr, sizeof(cur_sysparas.b_basic_p) ,(unsigned char*)&cur_sysparas.b_basic_p[0]);
		}
		else
		{
		       ee24lc_write(EX_EEPROM_SYSINFO_ADDR, sizeof(struct def_SysInfo) ,(unsigned char*)&cur_sysinfo.g_uid);
		}
		ocomm_message(4,1,"���óɹ�",10,NULL);		
		return;
	}
	//ocomm_message(4,1,"���óɹ�",dlycnt,NULL);		
	okernel_putevent(KEY_RET_EV, 0);
}
void oparas_basic_setret_event(void)
{
	oparas_basic_pro(cur_setbasic_id);
}

void oparas_basic_set_pro(void)
{
	unsigned long ultmp = 0;
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = false;
	cur_input_cursor.p_x   = 2;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
    okernel_addeventpro(KEY_OK_EV, oparas_basic_setok_event);
    okernel_addeventpro(KEY_RET_EV, oparas_basic_setret_event);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"������:");//,cur_sysparas.b_factor[cur_setbasic_id]
	
	ultmp = cur_sysparas.b_basic_p[cur_setbasic_id];
		
	switch(cur_setbasic_id) 
	{
		case 0:  //ǹ��
			cur_input_cursor.max   = 2;
			ultmp = cur_sysparas.b_basic_p[cur_setbasic_id];
			break;
		case 1:	 //���	
			cur_input_cursor.max   = 1;
			cur_sysparas.b_basic_p[cur_setbasic_id] = cur_sysinfo.shift_id;
			ultmp = cur_sysparas.b_basic_p[cur_setbasic_id];
			break;
		case 4:  //����վ���
			cur_input_cursor.max   = 2;
			break;
		case 2:
		case 3:
			cur_input_cursor.max   = 5;
			ocomm_printf(CH_DISP,1,5,"[%ld]",(unsigned long)ultmp*1200);
			return;//break;
		case 5: //����С���
			cur_input_cursor.max   = 2;
			ocomm_printf(CH_DISP,1,5,"%d Ԫ",(unsigned char)cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY]);
			return;//break;
		case 6:
			cur_input_cursor.max   = 6;
			cur_input_cursor.b_psw = true;
			return;	
		case 7: 
			ultmp = cur_sysparas.r_cur_g_id;
			break;
		case 8:
			ultmp = cur_sysparas.r_unupld_g_id;
			break;
		case 9:
			ultmp = cur_sysparas.r_cur_shift_id; //cur_sysparas
			break;
		case 10:	
			ultmp = cur_sysparas.r_unupld_shift_id;
			break;
		case 11:
		       cur_input_cursor.p_x   = 3;
			cur_input_cursor.p_y   = 1;
			cur_input_cursor.max   = 16;
			ultmp = ocomm_bcdbuf2hex(cur_sysinfo.g_sum, 6);
                     ocomm_printf(CH_DISP,2,1,"%ld.%02d",ultmp/100,ultmp%100);
			return;
		case 12:
		       cur_input_cursor.p_x   = 3;
			cur_input_cursor.p_y   = 1;
			cur_input_cursor.max   = 16;
			ultmp = ocomm_bcdbuf2hex(cur_sysinfo.m_amount, 6);
                     ocomm_printf(CH_DISP,2,1,"%ld.%02d",ultmp/100,ultmp%100);
			return;
		case 13:
			 cur_input_cursor.p_x   = 3;
			cur_input_cursor.p_y   = 1;
			cur_input_cursor.max   = 16; 
                     ocomm_printf(CH_DISP,2,1,"%ld",cur_sysinfo.gas_count);
			return ;
		case 14:
			cur_input_cursor.p_x   = 3;
			cur_input_cursor.p_y   = 1;
			cur_input_cursor.max   = 16; 
			ultmp = ocomm_bcdbuf2hex(cur_sysinfo.gas_total, 6);
                     ocomm_printf(CH_DISP,2,1,"%ld.%02d",ultmp/100,ultmp%100);
			return;
		default:
			//oparas_basic_pro(0);
			return;
	}
	ocomm_printf(CH_DISP,1,5,"[%ld]",ultmp);
}

void oparas_basic_menuok_event(void)
{
	cur_setbasic_id = odisp_menu_getid();
/*	switch(cur_setbasic_id)
	{
		case 0: //ǹ��
		case 1: //���
		case 2: //�����Ʋ�����
		case 3: //��̨ͨѶ������
		case 4: //����վ���
		case 5://����С���
		case 6: //���ÿ�����
		case 7;
		
			oparas_basic_set_pro();
			break;
		
			break;
		default:
			okernel_putevent(KEY_RET_EV,0);	
			break;
	} */
	if(cur_setbasic_id <= 14)
	{
		oparas_basic_set_pro();
	}
	else
	{
		okernel_putevent(KEY_RET_EV,0);	
	}
}
/*
Task_Event  _BASIC_PARAS_Pro[]  = 
{
  {KEY_RET_EV,			oset_paras}, 
  {KEY_OK_EV,			oparas_basic_menuok_event}, 	  
};
*/
/////////����������������/////////////////////
void oparas_basic_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_BASIC_PARAS_MENU)/sizeof(_BASIC_PARAS_MENU[0]);
	okernel_clreventpro();
    
       okernel_addeventpro(KEY_RET_EV, oset_paras);		
	okernel_addeventpro(KEY_OK_EV, oparas_basic_menuok_event);
	
	if(id>=maxid) 
		okernel_putevent(KEY_RET_EV,0);	
	else          
		odisp_menu_pro(_BASIC_PARAS_MENU, maxid,id);
}
