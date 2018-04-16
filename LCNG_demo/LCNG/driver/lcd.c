/*
 * lcd.c
 *
 * Created: 2012-8-6 16:12:33
 *  Author: luom
 */ 
#include "global_def.h"

void olcm_sendbyte(unsigned char B_yte)
{
 unsigned char i;
 OLcd_SID_OUT(); 	 //设置SID为输出
 for(i=0;i<8;i++)
 {
    OLcd_CLK_L();
    if(B_yte &0x80)	
	{
		OLcd_SID_H(); 
	}
	else
	{
	    OLcd_SID_L(); 
	}		
    B_yte = B_yte << 1;
    OLcd_CLK_H(); 
    __delay_cycles(2); 
 }
 OLcd_SID_IN(); 
}

void olcm_wrtdata(char Data, bool bCmd)
{
 OLcd_CS_H();
 if(bCmd)           olcm_sendbyte(0xF8);  //写命	  	
 else               olcm_sendbyte(0xFA);  //写数据
 olcm_sendbyte(0xF0 & Data);		      //高4位
 olcm_sendbyte(0xF0 &(Data<<4));	      //低4位
 OLcd_CS_L();
}

void olcm_clrscr(void)
{
  OS_ENTER_CRITICAL();//cli();	
  olcm_wrtdata(0,0x01);
  _delay_loop_2(9216);//5ms//_delay_ms(2);  //ocomm_delay_ms(5);//__delay_cycles(36864);//_delay_ms(5);
  OS_EXIT_CRITICAL();//sei();
}

static void olcm_locate(unsigned char row,unsigned char col)
{
  unsigned char addr[4] ={0x80,0x90,0x88,0x98};
  olcm_wrtdata(addr[row-1]+col-1, LCMCMD);  
}

void olcm_wrtstr(unsigned char row,unsigned char col,char *str, bool bClrLeft)
{
  unsigned char i,length;
  length = strlen(str);
  length = ((9-col)*2 < length) ? (9-col)*2 :length; //进行显示长度处理，如果超过单行，则截断不显示
  olcm_locate(row, col);
  for(i=0;i<length;i++)
     olcm_wrtdata(*str++,LCMDATA);//OLcd_WrtStr(str,length);
  if(bClrLeft)
  {
    for(i = (col-1)*2 + length; i<16;i++)
    {
      olcm_wrtdata(0x20,LCMDATA);
    }
  }  
}

void olcm_clrrow(unsigned char row)
{
  unsigned char i;
  OS_ENTER_CRITICAL();//cli();
  olcm_locate(row,1);
  for(i=0; i<16; i++)
  {
    olcm_wrtdata(0x20,LCMDATA);
  }
  OS_EXIT_CRITICAL();//sei();
}

void olcm_narrowup(unsigned char px, unsigned char py)
{
  olcm_locate(px,py);
  olcm_wrtdata(0x1E,LCMDATA);
}

void olcm_narrowdown(unsigned char px, unsigned char py)
{
  olcm_locate(px,py);
  olcm_wrtdata(0x1F,LCMDATA);
}
/**/
void olcm_dispprogconst(unsigned char row, unsigned char col, prog_char* str, bool bClrLeft)
{
  unsigned char i,length;
  OS_ENTER_CRITICAL();//cli();
  length = strlen_P(str);
  length = ((9-col)*2 < length) ? (9-col)*2 :length; 		
  olcm_locate(row, col);
  for(i=0;i<length;i++)
     olcm_wrtdata(pgm_read_byte(str++),LCMDATA);
  if(bClrLeft)
  {
    for(i = (col-1)*2 + length; i<16;i++)
    {
      olcm_wrtdata(0x20,LCMDATA);
    }
  }   
  OS_EXIT_CRITICAL();//sei();
}
/*
void olcm_dispprintf(unsigned char posx, unsigned posy, char *fmt,...)
{
    va_list ap;
    char str[30];
    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    olcm_wrtstr(posx, posy, str, 1);
    va_end(ap);
}
*/
void olcm_init(void)
{
  OLcd_CS_H();  
  OLcd_SID_H(); 
  OLcd_CLK_H(); 
  
  OLcd_CS_OUT();
  OLcd_SID_OUT();
  OLcd_CLK_OUT();
  
  olcm_wrtdata(0x03,LCMCMD);
  olcm_wrtdata(0x0C,LCMCMD);
  olcm_wrtdata(0x06,LCMCMD);
  olcm_wrtdata(0x01,LCMCMD);
  __delay_cycles(30000);//73728
  olcm_clrscr();
}



