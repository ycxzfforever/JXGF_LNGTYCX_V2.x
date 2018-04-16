/*
 * key.c
 *
 * Created: 2012-8-6 16:54:55
 *  Author: luom
 */ 
#include "global_def.h"
extern unsigned char Precool_Flag;
/*
CLK:   PE2
EN :   PE3
DATA:  PE4
*/
void okey_portinit(void)
{
  SET_PORT_BIT(PORTE, PE2);
  SET_PORT_BIT(PORTE, PE3);
  CLR_PORT_BIT(PORTE, PE4);

  SET_PORT_BIT(DDRE, DDE2);
  SET_PORT_BIT(DDRE, DDE3);
  CLR_PORT_BIT(DDRE, DDE4);
  
  SET_PORT_BIT(PORTE, PE2);
  SET_PORT_BIT(PORTE, PE3);
  CLR_PORT_BIT(PORTE, PE4);
}
/*************************************************************************
功能：  串行键盘读当前按键键值
参数:	
返回值：按键键值
备注：  
*************************************************************************/
unsigned char okey_get(void)////////////////////////////////////////////扫描按键
//unsigned int okey_get(void)////////////////////////////////////////////扫描按键
{
  volatile  unsigned long rdv = 0;
  volatile  unsigned char i; 
  volatile  unsigned char rdp = 0;

  CLR_PORT_BIT(DDRE, PE4);
  
  CLR_PORT_BIT(PORTE, PE3);
  __delay_cycles(10);
  CLR_PORT_BIT(PORTE, PE2);
  __delay_cycles(30);
  SET_PORT_BIT(PORTE, PE2);
  __delay_cycles(10);
  SET_PORT_BIT(PORTE, PE3);
  __delay_cycles(30); 
  rdv = 0;
  rdp = 0;
  for (i = 0; i < 24; i++) ////读数据
  {
      rdp = PINE;
      CLR_PORT_BIT(PORTE, PE2);
      __delay_cycles(80); 
      SET_PORT_BIT(PORTE, PE2);
      __delay_cycles(20);
      rdv  = rdv << 1;
      if(rdp &0x10) rdv |= 0x01;
  }
  rdv = rdv&0x000FFFFF;
  switch(rdv)
  {
    case KEY_0_MAP:       return KEY_0_EV;
    case KEY_1_MAP:       return KEY_1_EV;
    case KEY_2_MAP:       return KEY_2_EV;
    case KEY_3_MAP:       return KEY_3_EV;
    case KEY_4_MAP:       return KEY_4_EV;
    case KEY_5_MAP:       return KEY_5_EV;
    case KEY_6_MAP:       return KEY_6_EV;
    case KEY_7_MAP:       return KEY_7_EV;
    case KEY_8_MAP:       return KEY_8_EV;
    case KEY_9_MAP:       return KEY_9_EV;

    case KEY_QRY_MAP:     return KEY_QRY_EV;
    case KEY_SET_MAP:     return KEY_SET_EV;
    case KEY_FIX_MAP:     return KEY_FIX_EV; 
    case KEY_BACK_MAP:    return KEY_BACK_EV;   
    case KEY_CLR_MAP:     return KEY_CLR_EV;   
    case KEY_CNG_MAP:     return KEY_LCNG_EV; 
    case KEY_STOP_MAP:    return KEY_STOP_EV;   
    case KEY_OK_MAP:      return KEY_OK_EV;   
    case KEY_RET_MAP:     return KEY_RET_EV;     
    case KEY_PT_MAP:      return KEY_PT_EV;
    default: 				return 0;
  }
  return 0;
}
volatile  unsigned char currdkey = 0;
volatile  unsigned char prekey   = 0;


int Press_Counter = 0;


void CloseMidVctrl(void)
{
     VCTRL_MID_CLOSE();
	 otimer_release(LCNG_COMM_EVENT1_EV);
}

void okey_scan(void)
{
	volatile unsigned char rdkey1 = 0,rdkey2 = 0;

    rdkey1 = okey_get();
    if(rdkey1 != 0)
    {
        __delay_cycles(5000);
        rdkey2 = okey_get();
        if(rdkey1 == rdkey2) 
            currdkey=rdkey2;
        else
            currdkey=0;        
    }	
    else
        currdkey=0;
    //ocomm_printf(CH_DISP,4,1,"键值:%d",currdkey);
	if( currdkey == 0 && prekey == 0)
	{
		return;
	}
	if(prekey !=0 && currdkey == 0)
	{
		if(prekey >= KEY_0_EV && prekey<KEY_END)
		{
			okernel_putevent(prekey,0);	
		}
		prekey = 0;
		return;
	}
	prekey = currdkey;
}


