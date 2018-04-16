/*
 * flash.c
 *
 * Created: 2012-8-10 9:45:09
 *  Author: luom
 */ 
#include "global_def.h"

//static
void oflash_init(void)
{

  DDRJ  |= 0xBC;//AT14  1011 1100
  //DDRJ  |= 0xFC; //J0,J0 为串口3
  DDRL  |= 0x40;//0100 0000  
  DDRD  |= 0xF0;
  XMCRA |= 0x80;
  XMCRB  = 0x00;
  PORTJ |= 0x80; //将Flash置为正常工作模式

/* SET_PORT_BIT(DDRJ,DDJ7);  //NRST
 SET_PORT_BIT(DDRJ,DDJ5);  //AT15
 SET_PORT_BIT(DDRJ,DDJ4);  //AT16
 SET_PORT_BIT(DDRJ,DDJ3);  //AT17
 SET_PORT_BIT(DDRJ,DDJ2);  //AT18
 
 SET_PORT_BIT(DDRL,DDL6);  //AT19
 
 SET_PORT_BIT(DDRD,DDD7);  //AT23
 SET_PORT_BIT(DDRD,DDD6);  //AT22
 SET_PORT_BIT(DDRD,DDD5);  //AT21
 SET_PORT_BIT(DDRD,DDD4);  //AT20
 
 XMCRA |= 0x80;
 XMCRB  = 0x00;

 SET_PORT_BIT(PORTJ,PJ7);  //NRST 
 __delay_cycles(OFLASH_DLY_CYCLES);
 CLR_PORT_BIT(PORTJ,PJ7); 
 __delay_cycles(OFLASH_DLY_CYCLES);
 SET_PORT_BIT(PORTJ,PJ7);   */
}

bool oflash_checkid(void)
{
  volatile unsigned char mCode = 0, dCode = 0; //// 
  //OS_ENTER_CRITICAL();
  OS_ENTER_CRITICAL();//cli();
  OFlash_Enable();
  
  __delay_cycles(OFLASH_DLY_CYCLES);
  IODATA(FLASHCHIPADDR) = CMD_RDID;
  __delay_cycles(OFLASH_DLY_CYCLES);
  mCode = (unsigned char)IODATA(FLASHCHIPADDR);
  dCode = (unsigned char)IODATA(FLASHCHIPADDR+2);          
  if( INTEL_ID_28F128J3A == dCode && INTEL_MANUFACT == mCode)
  {
    IODATA(FLASHCHIPADDR) = 0xFF;
    __delay_cycles(OFLASH_DLY_CYCLES*2);
    OFlash_Disable();
	OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
    return true;
  }  
  IODATA(FLASHCHIPADDR) = 0xFF;  
  __delay_cycles(OFLASH_DLY_CYCLES);
  OFlash_Disable();
  OS_EXIT_CRITICAL();//sei();////OS_EXIT_CRITICAL();
  return false;
}

bool oflash_checksr(void)
{
  volatile   unsigned char rd;
  volatile   unsigned int  nCnt = 1000;//1000;
  while (nCnt--)
  {
	__WATCH_DOG();   
    OFlash_Enable();
    IODATA(FLASHCHIPADDR) = 0x70;
    __delay_cycles(OFLASH_DLY_CYCLES);
    rd = (unsigned char)IODATA(FLASHCHIPADDR);
    if(rd & (1<<7))
    {
      IODATA(FLASHCHIPADDR) = 0x70;
      __delay_cycles(OFLASH_DLY_CYCLES);
      rd = (unsigned char)IODATA(FLASHCHIPADDR);
      OFlash_Disable();
      if (rd == 0x80) return true;  //return false; //04-27
    }
	__delay_cycles(8000);//_delay_ms(1);
	__WATCH_DOG();  
  }
  OFlash_Disable();
  return false;
}

void oflash_setpage(unsigned long ulAddr)
{
  var_Bitctrl vBctrl ;
  vBctrl.TVar= (ulAddr &0xFF8000)>>15; 
  PORTJ &= 0xC3;									//PORTJ_BIT2/3/4/5 =>AT15/16/17/18
  
  /*CLR_PORT_BIT(PORTJ, PJ2);
  CLR_PORT_BIT(PORTJ, PJ3);
  CLR_PORT_BIT(PORTJ, PJ4);
  CLR_PORT_BIT(PORTJ, PJ5);*/
  
  if(vBctrl.TVar_Bit0 == 1)  {SET_PORT_BIT(PORTJ, PJ5);}	//PORTJ_PORTJ5 = 1;
  if(vBctrl.TVar_Bit1 == 1)  {SET_PORT_BIT(PORTJ, PJ4);}	//PORTJ_PORTJ4 = 1;
  if(vBctrl.TVar_Bit2 == 1)  {SET_PORT_BIT(PORTJ, PJ3);}	//PORTJ_PORTJ3 = 1;
  if(vBctrl.TVar_Bit3 == 1)  {SET_PORT_BIT(PORTJ, PJ2);}	//PORTJ_PORTJ2 = 1;

  if(vBctrl.TVar_Bit4 == 1){  
  	SET_PORT_BIT(PORTL, PL6);//PORTL_PORTL6 = 1;
  }
  else{
    CLR_PORT_BIT(PORTL, PL6);//PORTL_PORTL6 = 0;
  }	  

  PORTD &= 0x0F; //PORTD_BIT4/5/6/7 => AT20/21/22/23
  
  /*CLR_PORT_BIT(PORTD, PJ4);
  CLR_PORT_BIT(PORTD, PJ5);
  CLR_PORT_BIT(PORTD, PJ6);
  CLR_PORT_BIT(PORTD, PJ7); */ 
  if(vBctrl.TVar_Bit5 == 1)  {SET_PORT_BIT(PORTD, PD4);}//PORTD_PORTD4 = 1;
  if(vBctrl.TVar_Bit6 == 1)  {SET_PORT_BIT(PORTD, PD5);}//PORTD_PORTD5 = 1;
  if(vBctrl.TVar_Bit7 == 1)  {SET_PORT_BIT(PORTD, PD6);}//PORTD_PORTD6 = 1;
  if(vBctrl.TVar_Bit8 == 1)  {SET_PORT_BIT(PORTD, PD7);}//PORTD_PORTD7 = 1;
}

bool oflash_unprotect(unsigned long ulAddr)
{
  volatile unsigned int offsetaddr;
  if (ulAddr > 0xFFFFFF)  return false;
  OFlash_Enable();
  oflash_setpage(ulAddr);
  offsetaddr = ulAddr & 0x7FFF; //取得地址的高位
  offsetaddr |= FLASHCHIPADDR;
  IODATA(offsetaddr) = 0x50; //Clear Status Register
  __delay_cycles(OFLASH_DLY_CYCLES);
  IODATA(offsetaddr) = 0x60;
  __delay_cycles(OFLASH_DLY_CYCLES);
  IODATA(offsetaddr) = 0xD0;
  __delay_cycles(OFLASH_DLY_CYCLES);
  if(oflash_checksr() == false)	
  {
    IODATA(FLASHCHIPADDR) = 0xB0;
    __delay_cycles(OFLASH_DLY_CYCLES);
    OFlash_Disable();
    return false;
  }      
  IODATA(FLASHCHIPADDR) = 0xFF;
  __delay_cycles(OFLASH_DLY_CYCLES);
  OFlash_Disable();
  return true; 
}

bool oflash_eraseblock(unsigned long ulAddr)
{
  unsigned int n = 100;	
  volatile unsigned int offsetaddr;
  if(!oflash_unprotect(ulAddr)) 
  {
    return false;
  }  
  if(!oflash_checksr()){
	 return false;
  }   	 
  OFlash_Enable();
  oflash_setpage(ulAddr);
  offsetaddr = ulAddr&0x7FFF;
  offsetaddr |= FLASHCHIPADDR;
  
  IODATA(offsetaddr) = 0x50;  //clear status reg
  __delay_cycles(OFLASH_DLY_CYCLES);
  IODATA(offsetaddr) = 0x20;            
  __delay_cycles(OFLASH_DLY_CYCLES);
  IODATA(offsetaddr) = 0xD0; //确认擦除
  n = 100;
  while(n--) 
  {
	  __WATCH_DOG();
      _delay_loop_2(18432);//_delay_ms(25);//__delay_cycles(8000000);////_delay_ms(1000);//__delay_cycles(8000000);//
  }   
  
  if(!oflash_checksr())	
  {
    IODATA(offsetaddr) = 0xB0;
    __delay_cycles(OFLASH_DLY_CYCLES);
    OFlash_Disable();
    return false;
  }      
  IODATA(offsetaddr) = 0xFF;
  __delay_cycles(OFLASH_DLY_CYCLES);
  OFlash_Disable();
  return true;
}

bool oflash_writechar(unsigned long ulAddr, unsigned char ucData)
{
  volatile unsigned int offsetaddr;
  volatile bool ret;
  OFlash_Enable();
  oflash_setpage(ulAddr); 
  offsetaddr = ulAddr &0x7FFF; 
  offsetaddr |= FLASHCHIPADDR; 

  IODATA(offsetaddr) = 0x40; 
  __delay_cycles(OFLASH_DLY_CYCLES);
  IODATA(offsetaddr) = ucData; 
  __delay_cycles(OFLASH_DLY_CYCLES);
  ret = oflash_checksr(); 
  IODATA(offsetaddr) = 0xFF;
  __delay_cycles(OFLASH_DLY_CYCLES);
  OFlash_Disable();
  return ret;
}


unsigned char oflash_readchar(unsigned long ulAddr)
{
   unsigned char rd = 0;
   unsigned int  offsetaddr = 0;
   OFlash_Enable();
   oflash_setpage(ulAddr);
   offsetaddr = ulAddr &0x7FFF;
   offsetaddr |= FLASHCHIPADDR; 
   IODATA(offsetaddr) = 0xFF;
   __delay_cycles(OFLASH_DLY_CYCLES);
   rd = (unsigned char)IODATA(offsetaddr);
   OFlash_Disable();
   return rd;
}

bool   oflash_readbuf(unsigned long rdAddr, unsigned long Cnt, unsigned char *buf)
{
  unsigned int  i = 0;
  unsigned char chknum = 0;
  while(1)
  {
	if(oflash_checksr()) break;
	if( ++chknum > 10 )  return false;
  }  
  
  for (i = 0; i < Cnt; i++)
  {
	  __WATCH_DOG();
      *(buf + i) = oflash_readchar(rdAddr + i);
  }  	
  return true;
}

bool oflash_wrbuf(unsigned long wtAddr, unsigned long Cnt, unsigned char *buf) //成功返回1, 写失败返回0;
{
  unsigned long i, j;
  unsigned char chknum = 0;
  bool			ret = false;
  while(1)
  {
	if(oflash_checksr()) break;
	if( ++chknum > 10 )  return false;
  }  

  if ((wtAddr &0x01FFFF) == 0) 
  {
      if(!oflash_eraseblock(wtAddr))
      {
        return false;
      }
  }
  for (i = 0; i < Cnt; i++)
  {
	  __WATCH_DOG();
      for (j = 0; j < 3; j++) 
     {
	   ret = false;
	   oflash_writechar(wtAddr + i, *(buf + i));
       if (oflash_readchar(wtAddr + i) == *(buf + i))
       {
          ret = true; 
          break;
       }
     } 
     if (ret == false)
     {
       return false;
     }  
  }
  return true; 
}

bool oflash_eraseall(unsigned char blocks)
{
  unsigned char i;
  OS_ENTER_CRITICAL();//cli();//OS_ENTER_CRITICAL();
  for(i = 0; i<blocks; i++)	
  {
	__WATCH_DOG();  
    if(!oflash_eraseblock(i*0x20000))
	{
		OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
		return false;
	}		
    ocomm_printf(CH_DISP,3,1,"擦除数据块:%02d", i);
  }
  OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
  return true;
}

bool oflash_erase_blocks(unsigned char blocks)
{
  OS_ENTER_CRITICAL();//cli();
  if(!oflash_eraseblock(blocks*0x20000))
  {
	OS_EXIT_CRITICAL();//sei();
	return false;
  }		
  ocomm_printf(CH_DISP,3,1,"擦除数据块:%02d", blocks);
  OS_EXIT_CRITICAL();//sei();
  return true;
}
