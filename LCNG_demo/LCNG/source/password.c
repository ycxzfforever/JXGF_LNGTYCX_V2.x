/*
 * password.c
 *
 * Created: 2012-8-23 10:43:11
 *  Author: luom
 */ 
#include "global_def.h"

extern void opsw_set_pro(unsigned char  nstep);
static unsigned char newpswstep = 0;
enum _MIDIFYPSW_SW_
{
	_MIDIFYPSW_SW_CHK_ = 1,
	_MIDIFYPSW_SW_NEW_,
	_MIDIFYPSW_SW_RENEW_
};
bool opsw_set_pswchk(void)
{
	unsigned char len = 0;
	unsigned int eepaddr = SUPER_PSW_ADDR;
    	eeprom_busy_wait();
   	eeprom_read_block((unsigned char*)&psw ,(unsigned int*)eepaddr  ,7);
	len = strlen((char*)cur_input_cursor.str_input);
	if( len > 0 ) 
	{
		if(memcmp((unsigned char*)&psw[1],cur_input_cursor.str_input,len)== 0 || memcmp(cur_input_cursor.str_input,ADMIN_PSW,6) ==0 ) //验证旧密码
		{
			return true;
		}		
	}
	return false;
}

bool opsw_set_renewpswchk(void)
{
	if(strlen((char*)cur_input_cursor.str_input) == psw[0] && memcmp(&psw[1],(char*)cur_input_cursor.str_input,psw[0])== 0)
	{
		return true;		
	}
	return false;
}
void opsw_setok_event(void)
{
	unsigned int eepaddr = SUPER_PSW_ADDR;
	if(newpswstep == _MIDIFYPSW_SW_CHK_)
	{
		if(!opsw_set_pswchk())
		{
			ocomm_message(4,1,"密码错误",10, NULL);
			return;
		}
		newpswstep = _MIDIFYPSW_SW_NEW_;
		opsw_set_pro(newpswstep);
	}
	else if(newpswstep == _MIDIFYPSW_SW_NEW_)
	{
		memset(psw,0,7);
		psw[0] = strlen((char*)cur_input_cursor.str_input);
		if( psw[0] > 0 ) 
		{
			memcpy(&psw[1],(char*)cur_input_cursor.str_input,psw[0] );
			newpswstep = _MIDIFYPSW_SW_RENEW_;
			opsw_set_pro(newpswstep);
		}
		else
		{
			okernel_putevent(KEY_RET_EV,0);
		}
	}	
	else if(newpswstep == _MIDIFYPSW_SW_RENEW_)
	{
		if(!opsw_set_renewpswchk())
		{
			ocomm_message(4,1,"密码不一致",10, NULL);
			return;
		}
		eeprom_write_block((unsigned char*)&psw , (unsigned int*)eepaddr ,psw[0]+1);
		ee24lc_write(eepaddr, psw[0]+1 ,(unsigned char*)&psw);  //2012-11-06
		ocomm_message(4,1,"设置成功",10, NULL);
	}
}

void opsw_set_pro(unsigned char  nstep)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = true;
	cur_input_cursor.b_pt  = false;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oset_paras);
    okernel_addeventpro(KEY_OK_EV, opsw_setok_event);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	switch(nstep)
	{
		case _MIDIFYPSW_SW_CHK_:
			ocomm_printf(CH_DISP,2,1,"请输入旧密码:");
			break;
		case _MIDIFYPSW_SW_NEW_:
			ocomm_printf(CH_DISP,2,1,"请输入新密码:");
			break;
		case _MIDIFYPSW_SW_RENEW_:
			ocomm_printf(CH_DISP,2,1,"请输入重新密码:");
			break;
		default:
			okernel_putevent(KEY_RET_EV,0);
			return;
	}		
}


void opsw_modify_pro(void)
{
	newpswstep = _MIDIFYPSW_SW_CHK_;
	opsw_set_pro(newpswstep);
}