/*
 * iccard.c
 *
 * Created: 2012-8-15 14:43:34
 *  Author: luom
 */ 
#include "global_def.h"

#define CARD_TIP_MSG_TIME		50  //50*100 ms
#define CARD_RESPONSE_MAX_TIME  100 // ms
#define CARD_READ_MAX_CNT		5

unsigned char iccardnum[8],randdata[8],randverify[8];

//long cur_card_serial_num = 0;
void oapp_iccard_read_pro(void);
void oapp_iccard_select_pro(void);
void oapp_iccard_update_pro(void);

unsigned long hexTolong(unsigned char *array,unsigned char len)
{
	unsigned long tmp =0,sum=0;
	unsigned char i;
	for(i=0;i<len;i++)
	{
		sum = sum*100;
		tmp  =((unsigned long)*(array+i))/16*10+((unsigned long)*(array+i))%16;
		sum = sum +tmp;
	}
	return sum;
}

bool LongTohex(unsigned long sdt,unsigned char *buffer,unsigned char len)
{
	unsigned long data = 0;
	unsigned char i;
	data = sdt;
	for(i=1;i<=len;i++)
	{
		buffer[len-i]=(data%100)/10*16+(data%100)%10;
		data = data/100;
	}
	return true;
}

unsigned long charTolong(unsigned char *array,unsigned char len)
{
	unsigned long tmp =0,sum=0;
	unsigned char i;
	for(i=0;i<len;i++)
	{
		sum = sum*10;
		tmp  =((unsigned long)*(array+i))%16;
		sum = sum +tmp;
	}
	return sum;
}

bool LongTochar(unsigned long sdt,unsigned char *buffer,unsigned char len)
{
	unsigned long data = 0;
	unsigned char i;
	data = sdt;
	for(i=1;i<=len;i++)
	{
		buffer[len-i]=48 + data%10;
		data = data/10;
	}
	return true;
}

bool charTohex(unsigned char *buffer,unsigned char len)
{
	unsigned char i;
	unsigned char data;
	for(i=1;i<=len;i++)
	{
		data = buffer[len-i];
		buffer[len-i]=data/10*16+data%10;
	}
	return true;
}

void oicard_sendchar(unsigned char c) 
{
    while(!(UCSR0A &(1<<UDRE0))) ;
    UDR0 = c;
}

void oicard_sendstr(unsigned char *buf, unsigned char Len) 
{
	oicard_recvlen = 0;
    while(Len--) oicard_sendchar(*buf++);
}

#if OLD_ICCARD_MODULE
void oicard_turnoff(void)
{
	unsigned char i;
	unsigned char str[4] = {0xAA, 0xFF, 0x40,0x00};
	for(i=0;i<3;i++) str[3] ^= str[i];
	oicard_sendstr(str,4);
}

bool oicard_select(void)  //寻卡
{
	unsigned char i, str[5] = {0xAA, 0xFF, 0x70, 0x52,0x00};
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;		
	bool		  ret = true;		
	ICCARD_CLR_RXIRPT();
	OS_DISABLE_KEYSCAN();
	str[0] = 0xAA;
	str[1] = 0xFF;
	str[2] = 0x70;
	str[3] = 0x52;
	str[4] = 0x00;
	
	ret = true;
	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	for(i=0;i<4;i++) str[4] ^= str[i];
	ICCARD_SET_RXIRPT();
	oicard_recvlen = 0;
	icard_err_code_ = 0;
	oicard_sendstr(str,5);
	timeout =  CARD_RESPONSE_MAX_TIME*2;//*4;  CARD_RESPONSE_MAX_TIME*4
	 while(timeout--)
	 {
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);	
		if( oicard_recvbuf[2] == 0xA0 || oicard_recvbuf[2] == 0xA1 )
		{
			icard_err_code_ = oicard_recvbuf[2];
			ret = false;
			break;
		}			
		if( oicard_recvlen>=8 )
		{
			ret = true;
			break;
		}
	 }
	 oicard_recvlen = 0; 	 
	 ICCARD_CLR_RXIRPT();	  
	 OS_ENABLE_KEYSCAN(); 
	 if(timeout == 0) ret = false;
	 return ret;	
}

bool oicard_read(bool bnewpsw,unsigned char blocks, unsigned char* rdbuf)
{
	unsigned char i , str[] = {0xAA,0xFF,0x10,0x04,0x61,'2','2','2','2','2','2',0x00};	
	//unsigned char i , str[] = {0xAA,0xFF,0x10,0x04,0x61,'0','0','0','0','0','0',0x00};		
	
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;		
	bool		  ret = true;		
	ICCARD_CLR_RXIRPT();
	//OS_DISABLE_KEYSCAN();  //2013-03-11
	timeout =  CARD_RESPONSE_MAX_TIME;	
	ret = true;
	//ocomm_printf(CH_DISP,2,1,"密码:%c%c%c%c%c%c",str[5],str[6],str[7],str[8],str[9],str[10]);
	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	ee24lc_read(CARD_PSW_ADDR, 6 ,(unsigned char *)&str[5]);
	//ocomm_printf(CH_DISP,3,1,"密码:%c%c%c%c%c%c",str[5],str[6],str[7],str[8],str[9],str[10]);
    //ocomm_printf(CH_DISP,4,1,"密码:%s",CARD_PSW_ADDR);
    
	str[3] = blocks;
	if(bnewpsw)
	{
		memset(&str[5],0xFF,6);
	}
	for(i=0;i<11;i++) str[11] ^= str[i];
	ICCARD_SET_RXIRPT();
	oicard_recvlen = 0;
	icard_err_code_= 0; 
	oicard_sendstr(str,12);
	 while(timeout--)
	 {
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);		
		if( oicard_recvbuf[2] == 0xA0 || oicard_recvbuf[2] == 0xA1 )
		{
			icard_err_code_ = oicard_recvbuf[2];
			ret = false;
			break;
		}
		if(oicard_recvlen>=20)
		{
			ret = true;	
			break;
		}				
	 }
	 oicard_recvlen = 0; 
	 ICCARD_CLR_RXIRPT();	  
	 //OS_ENABLE_KEYSCAN();  //2013-03-11
	 if(timeout == 0){
		 
		 ret = false;
		 return false;
	 }	  	 
	 memcpy(rdbuf, (unsigned char*)&oicard_recvbuf[3], 16);
	 return ret;	 	
}

bool oicard_write(bool bnewpsw,unsigned char blocks, unsigned char *wtbuf)
{
	unsigned char i, str[28] = {0xAA,0xFF,0x20,0x00, 0x61,'2','2','2','2','2','2'};
	//unsigned char i, str[28] = {0xAA,0xFF,0x20,0x00, 0x61,'0','0','0','0','0','0'};		
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;		
	bool		  ret = true;
	ICCARD_CLR_RXIRPT();
	//OS_DISABLE_KEYSCAN();  //2013-03-11
	timeout =  CARD_RESPONSE_MAX_TIME;	
	ret = true;		
	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	ee24lc_read(CARD_PSW_ADDR, 6 ,(unsigned char *)&str[5]);
	str[3] = blocks;
	if(bnewpsw)
	{
		memset(&str[5],0xFF,6);
	}
	memcpy(&str[11], wtbuf, 16);
	for(i=0;i<27;i++) str[27] ^= str[i];
	ICCARD_SET_RXIRPT();
	icard_err_code_ = 0;
	oicard_recvlen = 0;
	oicard_sendstr(str,28);
	 while(timeout--)
	 {
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);		
		if( oicard_recvbuf[2] == 0xA0 || oicard_recvbuf[2] == 0xA1 )
		{
			icard_err_ = 1;
			icard_err_code_ = oicard_recvbuf[2];
			ret = false;
			break;
		}
		if(oicard_recvlen>=4)
		{
			ret = true;
			break;
		}				
	 }
	 oicard_recvlen = 0; 	 
	 ICCARD_CLR_RXIRPT();	  
	 //OS_ENABLE_KEYSCAN();  //2013-03-11
	 if(timeout == 0)
	 {
		 icard_err_ = 2; 
		 ret = false;
	 }	  	 
	 return ret;	 
}
#else 

 /*
@MT318 BCC校验
@Mt318CrcVerify
*/
unsigned char Mt318CrcVerify(unsigned char *array,unsigned char length)
{
	  unsigned char crcresult=0;
	  unsigned char tmp;//tmp varible
	  crcresult=*array;
	  for(tmp=1;tmp<length;tmp++)
	    crcresult^=*(array+tmp);
	  return(crcresult); 
}

 void oicard_turnoff(void)
 {
         return;
 }
#if  ICCARD_M1_MINGTE
  bool oicard_select(void)  //寻卡获取卡序列号
  {
	unsigned char str[6] = {0x02, 0x00, 0x02, 0x34,0x31,0x03};
	//unsigned char str[6] = {0x02, 0x00, 0x02, 0x34,0x31,0x03};
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;	
	unsigned char crcvalue = 0;
	bool		  ret = false;
	ICCARD_CLR_RXIRPT();
	OS_DISABLE_KEYSCAN();

	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	crcvalue=Mt318CrcVerify(str,6);

	ICCARD_SET_RXIRPT();
	oicard_recvlen = 0;
	icard_err_code_ = 0;
	oicard_sendstr(str,6);
	oicard_sendchar(crcvalue);
	
	timeout =  CARD_RESPONSE_MAX_TIME*4;//*4;  CARD_RESPONSE_MAX_TIME*4 
	while(timeout--)
	{
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);
		if(oicard_recvbuf[5] == 0x59 &&oicard_recvbuf[10] == 0x03)
		{
			ret = true;
			//ocomm_message(3,1,"寻卡成功",10,oui_main_ui);
			//ocomm_printf(CH_DISP,3,1,"序列号1:%x %x %x %x",oicard_recvbuf[6],oicard_recvbuf[7],oicard_recvbuf[8],oicard_recvbuf[9]);
			break;   
		}
		else if(oicard_recvbuf[5] == 0x4e)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[5];
			ocomm_printf(CH_DISP,2,1,"寻卡不成功:%x",oicard_recvbuf[5]);	
			break;	   
		}
		else if(oicard_recvbuf[5] == 0x45)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[5];	   
			ocomm_printf(CH_DISP,2,1,"机内无卡:%x",oicard_recvbuf[5]);		
			break;	 
		}
	icard_err_code_ = oicard_recvbuf[5];
	}
	oicard_recvlen = 0; 	 
	ICCARD_CLR_RXIRPT();
	OS_ENABLE_KEYSCAN(); 
	if(timeout == 0) ret = false; 
	return  ret;
  }

  bool authen_keya(unsigned char secter_num)
  {
	unsigned char  str[16] = {0x02, 0x00, 0x09, 0x34,0x39};
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;	
	unsigned char crcvalue = 0;
	bool		  ret = false;
	ICCARD_CLR_RXIRPT();
	OS_DISABLE_KEYSCAN();

	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	str[5] = secter_num; 
	ee24lc_read(CARD_PSW_ADDR, 6 ,(unsigned char *)&str[6]);
	str[12]= 0x03;
	crcvalue=Mt318CrcVerify(str,13);

	ICCARD_SET_RXIRPT();
	oicard_recvlen = 0;
	icard_err_code_ = 0;
	oicard_sendstr(str,13);
	oicard_sendchar(crcvalue);
	
	timeout =  CARD_RESPONSE_MAX_TIME*4;
	while(timeout--)
	{
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);
		if(oicard_recvbuf[6] == 0x59)
		{
			ret = true;
			//ocomm_printf(CH_DISP,4,1,"下载密码成功");
			_delay_loop_2(1843);//_delay_ms(1)
			break;
		}
		else if(oicard_recvbuf[6] == 0x30)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[6];	
			ocomm_printf(CH_DISP,4,1,"寻不到射频卡");	
			break;
		}
		else if(oicard_recvbuf[6] == 0x33)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[6];
			ocomm_printf(CH_DISP,4,1,"密码验证错误%x,%x",oicard_recvbuf[5],oicard_recvbuf[6]);	
			break;
		}
		else if(oicard_recvbuf[6] == 0x45)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[6];
			ocomm_printf(CH_DISP,4,1,"机内无卡");
			break;
		}
		icard_err_code_ = oicard_recvbuf[6];
	}
	 oicard_recvlen = 0; 	 
	 ICCARD_CLR_RXIRPT();
	 OS_ENABLE_KEYSCAN(); 
	 if(timeout == 0) ret = false; 
        return  ret;
  }

  bool oicard_read(bool bnewpsw,unsigned char blocks, unsigned char* rdbuf)
  {
	unsigned char secter = 0,crcvalue = 0;
	unsigned char str[10] = {0x02, 0x00, 0x04, 0x34,0x33};
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;		
	bool		  ret = false;

	secter = blocks/4;
	if(authen_keya(secter) == false)
	return false;
	_delay_loop_2(1843);//_delay_ms(1);

	ICCARD_CLR_RXIRPT();
	OS_DISABLE_KEYSCAN();
	timeout =  CARD_RESPONSE_MAX_TIME;	

	str[5] = secter;
	str[6] = blocks-4*secter;
	str[7] = 0x03;
	crcvalue=Mt318CrcVerify(str,8);

	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	ICCARD_SET_RXIRPT();
	oicard_recvlen = 0;
	icard_err_code_ = 0;
	oicard_sendstr(str,8);
	oicard_sendchar(crcvalue);
		
        timeout =  CARD_RESPONSE_MAX_TIME*6;//*4;  CARD_RESPONSE_MAX_TIME*4 
	while(timeout--)
	{
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);
		if((oicard_recvbuf[7] == 0x59)&&(oicard_recvbuf[24] == 0x03))
		{
			ret = true;
			memcpy(rdbuf, (unsigned char*)&oicard_recvbuf[8], 16);
			//ocomm_printf(CH_DISP,4,1,"读块成功:%d,%x",blocks,oicard_recvbuf[11]);
			break;
		}
		else if(oicard_recvbuf[7] == 0x30)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];	
			ocomm_printf(CH_DISP,4,1,"寻不到射频卡");	
			break;
		}
		else if(oicard_recvbuf[7] == 0x33)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];
			ocomm_printf(CH_DISP,4,1,"密码验证错误%x,%x",oicard_recvbuf[5],oicard_recvbuf[6]);	
			break;
		}
		else if(oicard_recvbuf[7] == 0x34)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];	
			ocomm_printf(CH_DISP,4,1,"读数据错");	
			break;
		}
		else if(oicard_recvbuf[7] == 0x45)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];
			ocomm_printf(CH_DISP,4,1,"机内无卡");
			break;
		}
		icard_err_code_ = oicard_recvbuf[7];
	}
	oicard_recvlen = 0;
	ICCARD_CLR_RXIRPT();
	OS_ENABLE_KEYSCAN(); 
	if(timeout == 0) ret = false; 
	return  ret;
  }

  bool oicard_write(bool bnewpsw,unsigned char blocks, unsigned char *wtbuf)
  {
	unsigned char secter = 0,crcvalue = 0;
	unsigned char str[26] = {0x02, 0x00, 0x14, 0x34,0x34};
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;		
	bool		  ret = false;

	secter = blocks/4;
	if(authen_keya(secter) == false)
	return false;
	_delay_loop_2(1843);//_delay_ms(1)

	ICCARD_CLR_RXIRPT();
	str[5] = secter;
	str[6] = blocks-4*secter;
	memcpy((unsigned char *)&str[7], wtbuf, 16);
	str[23] =  0x03;
	crcvalue=Mt318CrcVerify(str,24);

	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	ICCARD_SET_RXIRPT();
	icard_err_code_ = 0;
	oicard_recvlen = 0;
	oicard_sendstr(str,24);
	oicard_sendchar(crcvalue);

	timeout =  CARD_RESPONSE_MAX_TIME*6;//*4;  CARD_RESPONSE_MAX_TIME*4 
	while(timeout--)
	{
		//osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);
		if(oicard_recvbuf[7] == 0x59)
		{
			ret = true;
			//ocomm_printf(CH_DISP,4,1,"写块成功:%d",blocks);
			break;
		}
		else if(oicard_recvbuf[7] == 0x30)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];	
			ocomm_printf(CH_DISP,4,1,"寻不到射频卡");	
			break;
		}
		else if(oicard_recvbuf[7] == 0x31)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];	
			ocomm_printf(CH_DISP,4,1,"操作扇区号错");	
			break;
		}
		else if(oicard_recvbuf[7] == 0x33)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];
			ocomm_printf(CH_DISP,4,1,"密码验证错%x,%x",oicard_recvbuf[5],oicard_recvbuf[6]);	
			break;
		}
		else if(oicard_recvbuf[7] == 0x34)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];	
			ocomm_printf(CH_DISP,4,1,"校验写入块数据错");	
			break;
		}
		else if(oicard_recvbuf[7] == 0x45)
		{
			ret = false;
			icard_err_code_ = oicard_recvbuf[7];
			ocomm_printf(CH_DISP,4,1,"机内无卡");
			break;
		}
		icard_err_code_ = oicard_recvbuf[7];
	}
	oicard_recvlen = 0;
	ICCARD_CLR_RXIRPT();
	// OS_ENABLE_KEYSCAN(); 
	if(timeout == 0) ret = false; 
	return  ret;
}
#else

/*void ICCARD_CPU_Oper(void)
{
	okernel_clreventpro();
	okernel_addeventpro(KEY_0_EV,CpuCardSelect_Oper);//CPU复位
	okernel_addeventpro(KEY_1_EV,CpuCardMF_Oper);//CPU主目录
	okernel_addeventpro(KEY_2_EV,CpuCardDF_Oper);//CPU应用目录
	okernel_addeventpro(KEY_3_EV,CpuCardGetrand_oper);//CPU取随机数
	okernel_addeventpro(KEY_4_EV,PsamCardMF_Oper);//PSAM主目录
	okernel_addeventpro(KEY_5_EV,PsamCardDF_Oper);//PSAM应用目录
	okernel_addeventpro(KEY_6_EV,PsamCardDesInit_Oper);//PSAM DES初始化命令
	okernel_addeventpro(KEY_7_EV,PsamCardDes_Oper);//PSAM DES命令
	okernel_addeventpro(KEY_8_EV,CpuCardExternVerity_Oper);//CPU外部认证
	okernel_addeventpro(KEY_9_EV,oicard_read_oper);//PSAM DES命令
	okernel_addeventpro(KEY_PT_EV,oicard_write_oper);//CPU外部认证
	okernel_addeventpro(KEY_RET_EV,oui_main_ui);
	okernel_addeventpro(KEY_OK_EV,oprt_command_info);
}*/

//////////////////////PSAM操作//////////////////////
void PsamCardSelect_Oper(void)//选择PSAM卡------复位PSAM卡
{
   unsigned char str[7]={0x02,0x00,0x03,0x3d,0x40,0x30,0x03};
   unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;	
   unsigned char len,crcvalue = 0;
   ICCARD_CLR_RXIRPT();
   OS_DISABLE_KEYSCAN();
   memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
    crcvalue=Mt318CrcVerify(str,7);

    ICCARD_SET_RXIRPT();
   oicard_recvlen = 0;
    icard_err_code_ = 0;
   oicard_sendstr(str,7);
   oicard_sendchar(crcvalue);

   timeout =  CARD_RESPONSE_MAX_TIME*10;//*4;  CARD_RESPONSE_MAX_TIME*4 
    while(timeout--)
    {
	       //osys_opower_chk(); //掉电检测
		     _delay_loop_2(1843);//_delay_ms(1);
		   //ocomm_printf(CH_DISP,3,1,"%x %x %x",oicard_recvbuf[6],5+oicard_recvbuf[2],oicard_recvlen);
		   //ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",oicard_recvbuf[6],oicard_recvbuf[7],oicard_recvbuf[8],oicard_recvbuf[9],oicard_recvbuf[10],oicard_recvbuf[11]);
		   //ocomm_printf(CH_DISP,1,1,"%x %x %x %x %x %x",oicard_recvbuf[0],oicard_recvbuf[1],oicard_recvbuf[2],oicard_recvbuf[3],oicard_recvbuf[4],oicard_recvbuf[5]);
		   len=oicard_recvbuf[8]+8;
		   if(oicard_recvbuf[6] == 0x59&&oicard_recvbuf[len+1] == 0x03)
		   {
			ocomm_printf(CH_DISP,4,1,"PSAM复位成功:%x",oicard_recvbuf[6]);
			break;   
		   }
		   else if(oicard_recvbuf[6] == 0x4e)
		   {
			icard_err_code_ = oicard_recvbuf[6];
			ocomm_printf(CH_DISP,4,1,"PSAM复位失败:%x",oicard_recvbuf[6]);
			break;	   
		   }
		   icard_err_code_ = oicard_recvbuf[6];
       }
	oicard_recvlen = 0; 	 
	ICCARD_CLR_RXIRPT();
	OS_ENABLE_KEYSCAN(); 
}

bool PsamCardMF_Oper(void)//PSAM卡选择MF
{
	unsigned char cmdarray[7]={0x00,0xa4,0x00,0x00,0x02,0x3f,0x00};
	if(PsamCardApdu_oper(0x30,cmdarray, 7))
		{
		//ocomm_printf(CH_DISP,4,1,"成功PSAM-MF:%x",oicard_recvbuf[6]);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"失败PSAM-MF:%x",oicard_recvbuf[6]);
		return false;}
}
bool PsamCardDF_Oper(void)//PSAM卡选择DF
{
	unsigned char cmdarray[7]={0x00,0xa4,0x00,0x00,0x02,0xdf,0x01};
	if(PsamCardApdu_oper(0x30,cmdarray, 7))
		{
		//ocomm_printf(CH_DISP,4,1,"成功PSAM-DF:%x",oicard_recvbuf[6]);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"失败PSAM-DF:%x",oicard_recvbuf[6]);
		return false;}
}

bool PsamCardDesInit_Oper(void)//PSAM-DES初始化
{
	unsigned char cmdarray[5]={0x80,0x1a,0x07,0x01,0x00};
	if(PsamCardApdu_oper(0x30, cmdarray, 5))
		{
		//ocomm_printf(CH_DISP,4,1,"初始化成功DES:%x",oicard_recvbuf[6]);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"初始化失败DES:%x",oicard_recvbuf[6]);
		return false;}
}

bool PsamCardDes_Oper(void)//PSAM-DES加密
{
	unsigned char cmdarray[13]={0x80,0xfa,0x00,0x00,0x08};
	memcpy(cmdarray+5,randdata,8);
	memset((unsigned char*)cmdarray+9,0,4);
	//ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",cmdarray[0],cmdarray[1],cmdarray[2],cmdarray[3],cmdarray[4],cmdarray[5]);
	//ocomm_printf(CH_DISP,3,1,"%x %x %x %x %x %x",cmdarray[6],cmdarray[7],cmdarray[8],cmdarray[9],cmdarray[10],cmdarray[11]);
	//ocomm_printf(CH_DISP,1,1,"%x %x %x %x %x %x",randdata[0],randdata[1],randdata[2],randdata[3],randdata[4],randdata[5]);
	if(PsamCardApdu_oper(0x30, cmdarray, 13))
		{
		//ocomm_printf(CH_DISP,4,1,"加密成功DES:%x",oicard_recvbuf[6]);
		memset(randverify,0,8);	
		memcpy((unsigned char*)randverify,(unsigned char*)(oicard_recvbuf+9),8);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"加密失败DES:%x",oicard_recvbuf[6]);
		return false;}
}

bool PsamCardApdu_oper(unsigned char psamnumber,unsigned char *cmdarray,unsigned char cmdlength)//PSAM_apdu命令操作
{
	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;
   	bool		  ret = false;
	unsigned char len,crcvalue = 0;
	unsigned char  str[50];  
	memset((unsigned char*)str,0,sizeof(str));
	str[0]=0x02;
	str[1]=0x00;
	str[2]=5+cmdlength;
	str[3]=0x3d;
	str[4]=0x43;
	str[5]=psamnumber;//PSAM卡座号
	str[6]=0x00;
	str[7]=cmdlength;
	memcpy(str+8,cmdarray,cmdlength);
	str[8+cmdlength]=0x03;  /////得到发送命令的内容

ICCARD_CLR_RXIRPT();
OS_DISABLE_KEYSCAN();
memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
crcvalue=Mt318CrcVerify(str,9+cmdlength);

ICCARD_SET_RXIRPT();
oicard_recvlen = 0;
icard_err_code_ = 0;
oicard_sendstr(str,9+cmdlength);
oicard_sendchar(crcvalue);

   timeout =  CARD_RESPONSE_MAX_TIME*10;
    while(timeout--)
    {
	           //osys_opower_chk(); //掉电检测
		   _delay_loop_2(1843);//_delay_ms(1);
		   //ocomm_printf(CH_DISP,3,1,"%x %x",oicard_recvbuf[6],oicard_recvbuf[len+1]);
		   //ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",oicard_recvbuf[6],oicard_recvbuf[7],oicard_recvbuf[8],oicard_recvbuf[9],oicard_recvbuf[10],oicard_recvbuf[11]);
		   //ocomm_printf(CH_DISP,1,1,"%x %x %x %x %x %x",oicard_recvbuf[0],oicard_recvbuf[1],oicard_recvbuf[2],oicard_recvbuf[3],oicard_recvbuf[4],oicard_recvbuf[5]);
		   len=oicard_recvbuf[8]+8;
		   if(oicard_recvbuf[6] == 0x59&&oicard_recvbuf[len-1] == 0x90&&oicard_recvbuf[len] == 0x00&&oicard_recvbuf[len+1] == 0x03)
		   {
		           ret = true;
			  break;   
		   }
		   else if(oicard_recvbuf[6] == 0x4e)
		   {
		           ret = false;
			  icard_err_code_ = oicard_recvbuf[6];
			  break;	   
		   }
		   icard_err_code_ = oicard_recvbuf[6];
       }
	 oicard_recvlen = 0; 	 
	 ICCARD_CLR_RXIRPT();
	 OS_ENABLE_KEYSCAN(); 
	 if(timeout == 0) ret = false; 
        return  ret;
}

///////////////////////////CPU卡操作//////////////////////
bool CpuCardSelect_Oper(void)//-------CPU卡上电复位
{
   //unsigned char str[6]={0x02,0x00,0x02,0x37,0x40,0x03};
   unsigned char str[6]={0x02,0x00,0x02,0x34,0x40,0x03};
   unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;	
   unsigned char len,crcvalue = 0;
   bool		  ret = false;
   ICCARD_CLR_RXIRPT();
   OS_DISABLE_KEYSCAN();
   memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
    crcvalue=Mt318CrcVerify(str,6);

    ICCARD_SET_RXIRPT();
   oicard_recvlen = 0;
    icard_err_code_ = 0;
   oicard_sendstr(str,6);
   oicard_sendchar(crcvalue);

   timeout =  CARD_RESPONSE_MAX_TIME*10;//*4;  CARD_RESPONSE_MAX_TIME*4 
    while(timeout--)
    {
	           //osys_opower_chk(); //掉电检测
		     _delay_loop_2(1843);//_delay_ms(1);
		   //ocomm_printf(CH_DISP,3,1,"%x %x %x",oicard_recvbuf[5],5+oicard_recvbuf[2],oicard_recvlen);
		   //ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",oicard_recvbuf[6],oicard_recvbuf[7],oicard_recvbuf[8],oicard_recvbuf[9],oicard_recvbuf[10],oicard_recvbuf[11]);
		   //ocomm_printf(CH_DISP,1,1,"%x %x %x %x %x %x",oicard_recvbuf[0],oicard_recvbuf[1],oicard_recvbuf[2],oicard_recvbuf[3],oicard_recvbuf[4],oicard_recvbuf[5]);
		   len=oicard_recvbuf[7]+7;
		   if(oicard_recvbuf[5] == 0x59&&oicard_recvbuf[len+1] == 0x03)
		   {
		           ret = true;
			  //ocomm_printf(CH_DISP,4,1,"复位成功CPU:%x",oicard_recvbuf[5]);
			  break;   
		   }
		   else if(oicard_recvbuf[5] == 0x4e)
		   {
		           ret = false;
			  icard_err_code_ = oicard_recvbuf[5];
			  ocomm_printf(CH_DISP,4,1,"复位失败CPU:%x",oicard_recvbuf[5]);	
			  break;	   
		   }
		   else if(oicard_recvbuf[5] == 0x45)
		   {
			ret = false;
			icard_err_code_ = oicard_recvbuf[5];
			ocomm_printf(CH_DISP,4,1,"卡机无卡:%x",oicard_recvbuf[5]);
			break;
		   }
		   icard_err_code_ = oicard_recvbuf[5];
       }
	 oicard_recvlen = 0; 	 
	 ICCARD_CLR_RXIRPT();
	 OS_ENABLE_KEYSCAN(); 
	 if(timeout == 0) ret = false; 
        return  ret;
}

bool CpuCardMF_Oper(void)//Cpu卡选择MF操作
{
	unsigned char cmdarray[7]={0x00,0xa4,0x00,0x00,0x02,0x3f,0x00};
	if(CpuCardApdu_oper(cmdarray, 7))
		{
		//ocomm_printf(CH_DISP,4,1,"成功Cpu-MF:%x",oicard_recvbuf[5]);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"失败Cpu-MF:%x",oicard_recvbuf[5]);
		return false;}
}

bool CpuCardDF_Oper(void)//Cpu卡选择DF操作
{
	unsigned char cmdarray[7]={0x00,0xa4,0x00,0x00,0x02,0x3f,0x01};
	if(CpuCardApdu_oper(cmdarray, 7))
		{
		//ocomm_printf(CH_DISP,4,1,"成功Cpu-DF:%x",oicard_recvbuf[5]);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"失败Cpu-DF:%x",oicard_recvbuf[5]);
		return false;}
}

bool CpuCardGetrand_oper(void)//Cpu获取随机数
{
	unsigned char cmdarray[5]={0x00,0x84,0x00,0x00,0x04};
	if(CpuCardApdu_oper(cmdarray, 5))
		{
		memset(randdata,0,8);	
		memcpy((unsigned char*)randdata,(unsigned char*)(oicard_recvbuf+8),8);
		//ocomm_printf(CH_DISP,3,1,"随机数:%x",randdata);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"取数失败Cpu:%x",oicard_recvbuf[5]);
		return false;}
}

bool CpuCardExternVerity_Oper(void)//Cpu发送外部认证命令
{	
	unsigned char externalarray[13]={0x00,0x82,0x00,0x00,0x08};
	memcpy(externalarray+5,randverify,8);
	//ocomm_printf(CH_DISP,1,1,"%x %x %x %x %x %x",randverify[0],randverify[1],randverify[2],randverify[3],randverify[4],randverify[5]);
	//ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",randverify[6],randverify[7],externalarray[4],externalarray[5],externalarray[6],externalarray[7]);
	//ocomm_printf(CH_DISP,3,1,"%x %x %x %x %x %x",externalarray[8],externalarray[9],externalarray[10],externalarray[11],externalarray[12],externalarray[13]);
	if(CpuCardApdu_oper(externalarray, 13))
		{
		//ocomm_printf(CH_DISP,4,1,"认证成功Cpu:%x",oicard_recvbuf[5]);
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"认证失败Cpu:%x",oicard_recvbuf[5]);
		return false;}
}

bool CpuCardApdu_oper(unsigned char *cmdarray,unsigned char cmdlength)//CPU-APDU命令操作
{
  	unsigned int  timeout =  CARD_RESPONSE_MAX_TIME;
   	bool		  ret = false;
	unsigned char len=0,crcvalue = 0;
	unsigned char  str[50];
	memset((unsigned char*)str,0,sizeof(str));
	str[0]=0x02;
	str[1]=0x00;
	str[2]=4+cmdlength;
	str[3]=0x34;
	str[4]=0x41;
	str[5]=0x00;
	str[6]=cmdlength;
	memcpy(str+7,cmdarray,cmdlength);
	str[7+cmdlength]=0x03;  // /////得到发送命令的内容

	ICCARD_CLR_RXIRPT();
	OS_DISABLE_KEYSCAN();
	memset((unsigned char*)oicard_recvbuf,0,sizeof(oicard_recvbuf));
	crcvalue=Mt318CrcVerify(str,8+cmdlength);

	ICCARD_SET_RXIRPT();
	oicard_recvlen = 0;
	icard_err_code_ = 0;
	oicard_sendstr(str,8+cmdlength);
	oicard_sendchar(crcvalue);

   timeout =  CARD_RESPONSE_MAX_TIME*10;//*4;  CARD_RESPONSE_MAX_TIME*4 
    while(timeout--)
    {
	          // osys_opower_chk(); //掉电检测
		   _delay_loop_2(1843);//_delay_ms(1);
		   //ocomm_printf(CH_DISP,3,1,"%x %x %x",oicard_recvbuf[5],5+oicard_recvbuf[2],oicard_recvlen);
		   //ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",oicard_recvbuf[6],oicard_recvbuf[7],oicard_recvbuf[8],oicard_recvbuf[9],oicard_recvbuf[10],oicard_recvbuf[11]);
		   //ocomm_printf(CH_DISP,1,1,"%x %x %x %x %x %x",oicard_recvbuf[0],oicard_recvbuf[1],oicard_recvbuf[2],oicard_recvbuf[3],oicard_recvbuf[4],oicard_recvbuf[5]);
		   len=oicard_recvbuf[7]+7;
		   if(oicard_recvbuf[5] == 0x59&&oicard_recvbuf[len-1] == 0x90&&oicard_recvbuf[len] == 0x00&&oicard_recvbuf[len+1] == 0x03)
		   {
		           ret = true;
			  break;   
		   }
		   else if(oicard_recvbuf[5] == 0x4e)
		   {
		           ret = false;
			  icard_err_code_ = oicard_recvbuf[5];
			  break;	   
		   }
		   else if(oicard_recvbuf[5] == 0x45)
		   {
			ret = false;
			icard_err_code_ = oicard_recvbuf[5];
			break;
		   }
		   icard_err_code_ = oicard_recvbuf[5];
       }
	 oicard_recvlen = 0; 	 
	 ICCARD_CLR_RXIRPT();
	 OS_ENABLE_KEYSCAN(); 
	 if(timeout == 0) ret = false; 
        return  ret;
}

unsigned char Cpucard_Write_Ready(void)
{
	if(CpuCardMF_Oper()  == false)
		return _wirte_ready_cpumf_err;
	if(CpuCardDF_Oper() == false)
		return _wirte_ready_cpudf_err;
	if(CpuCardGetrand_oper() == false)
		return  _wirte_ready_cpugetrand_err;
	if(PsamCardMF_Oper()  == false)
		return  _wirte_ready_psammf_err;
	if(PsamCardDF_Oper() == false)
		return _wirte_ready_psamdf_err;
	if(PsamCardDesInit_Oper() == false)
		return _wirte_ready_psamdeslnit_err;
	if(PsamCardDes_Oper() == false)
		return _wirte_ready_psamdes_err;
	if(CpuCardExternVerity_Oper() == false)
		return _wirte_ready_cpuverity_err;
	return _wirte_ready_ok;
}

bool oicard_read(unsigned char file,unsigned char number,unsigned char *rdbuf)
{	
	unsigned char cmdarray[5]={0x00,0xb0,0xff,0xff,0xff};
	cmdarray[2] = 0x80+file;
	cmdarray[3] = 0x00;
	cmdarray[4] = number;
	if(CpuCardApdu_oper(cmdarray, 5))
		{
			memset(rdbuf,0,number);
			memcpy((unsigned char*)rdbuf,(unsigned char*)&oicard_recvbuf[8],number);
			//ocomm_printf(CH_DISP,4,1,"读文件成功:%x",filenumber);
			//oprt_command_info();
			return true;
		}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"读文件失败:%x",file);
		return false;}
}

bool oicard_write(unsigned char file,unsigned char number,unsigned char *wtbuf,unsigned char offset)
{	
	unsigned char cmdarray[50]={0x00,0xd6,0xff,0xff,0xff};
	cmdarray[2] = 0x80+file;
	cmdarray[3] = offset;
	cmdarray[4] = number;
	memcpy((unsigned char*)(cmdarray+5),(unsigned char*)wtbuf,number);

	if(CpuCardApdu_oper(cmdarray, 5+number))
		{
		//ocomm_printf(CH_DISP,2,1,"余额:%x %x %x %x",oicard_recvbuf[9],oicard_recvbuf[10],oicard_recvbuf[11],oicard_recvbuf[12]);
		//ocomm_printf(CH_DISP,3,1,"余额:%x %x %x %x",oicard_recvbuf[13],oicard_recvbuf[14],oicard_recvbuf[15],oicard_recvbuf[16]);
		//ocomm_printf(CH_DISP,4,1,"写:%ld %ld %ld %ld %ld",arraybuf[0],arraybuf[1],arraybuf[2],arraybuf[3],arraybuf[4]);
		//ocomm_printf(CH_DISP,4,1,"写成功");
		return true;}
	else
		{
		//ocomm_printf(CH_DISP,4,1,"写文件失败:%x",file);
		return false;}
}

/*void oicard_read_oper(void)
{
	oicard_read(0x15,19,cpu_card_data.CpuCardNum);
	oicard_read(0x16,42,cpu_card_data.CpuLastMon);
	oprt_Cpufeil_info();
	//_delay_ms(100);
}

void oicard_write_oper(void)
{
	cpu_card_data.CpuValidity  =  0x01;
	oicard_write(0x16,1,cpu_card_data.CpuValidity,21);
}*/

#endif
#endif

prog_char  _ICCARD_ERR_TIPS[][16]={
	"读卡OK",
	"灰卡",
	"挂失卡",
	"卡类型错",
	"余额不足",
	"记账超额",//只针对记账卡
	"余额不一致",
	"余额异常",
	"枪号不匹配",
	"卡序列号不一致",	
	"钢瓶到期",
	"卡号错误",
	"记账卡暂不能加气",
};
void oapp_iccard_tip_msg(unsigned char xpos, unsigned char ypos, unsigned char id)
{
    olcm_dispprogconst(xpos, ypos, &_ICCARD_ERR_TIPS[id][0], 1);
}

#if OLD_ICCARD_MODULE
	SIGNAL(USART0_RX_vect)
	{
	  unsigned char recv;
	  recv = UDR0;
	  if(oicard_recvlen == 0 && recv == 0xBB)
	  {
		  oicard_recvbuf[oicard_recvlen++] = 0xBB;
		  return;
	  }
	  if(oicard_recvlen == 1)
	  {
		  if( recv == 0xFF )  oicard_recvbuf[oicard_recvlen++] = 0xFF;
		  else                oicard_recvlen = 0;     
		  return;
	  }
	  oicard_recvbuf[oicard_recvlen++] = recv;
	  if(oicard_recvlen >= 22) oicard_recvlen = 0;
	}
#else 
     SIGNAL(USART0_RX_vect)
     {
	  unsigned char recv;
	  recv = UDR0;
	  if(oicard_recvlen == 0 && recv == 0x02)
	  {
		  oicard_recvbuf[oicard_recvlen++] = 0x02;
		  return;
	  }
	  else if(oicard_recvlen == 0 && recv == 0x06)
	  {
	         oicard_sendchar(0x05);
		   return;
	  }
	  
	  oicard_recvbuf[oicard_recvlen++] = recv;
	  if(oicard_recvlen >= 140) oicard_recvlen = 0;
	}
#endif

//#if ICCARD_M1_MINGTE
#if 1
void oapp_iccard_ungray_updateok_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);	
	ocomm_message(4,1,"解灰成功",10,NULL);
}

void oapp_iccard_ungray_readok_pro(void)
{
	unsigned long gid = 0;
	unsigned char gnum = 0,snum = 0;//枪号，加气站编号
	unsigned char tmpbuf[16] = {0};	
	gnum = cur_card_data.block14_buf[6];//枪号
	gid=cur_card_data.block14_buf[7];
	gid<<=8;
	gid |=cur_card_data.block14_buf[8];
	gid<<=8;
	gid |=cur_card_data.block14_buf[9];
	gid<<=8;
	gid |=cur_card_data.block14_buf[10];  //流水号
	//snum = cur_card_data.block14_buf[11];
	snum = hexTolong((unsigned char*)&cur_card_data.block14_buf[11],1);//加气站编号
	ocomm_printf(CH_DISP,2,1,"加气站编号：%d",snum); 
	ocomm_printf(CH_DISP,2,1,"枪号：%d",gnum); 
	ocomm_printf(CH_DISP,3,1,"流水号:%ld", gid);
	if(cur_card_data.block2_buf[7] == '1')		
	{
		ocomm_printf(CH_DISP,4,1,"正常卡");
		return;			
	}
	if(cur_sysinfo.g_uid < gid)		
	{
		ocomm_printf(CH_DISP,4,1,"流水号有误");
		return;		
	}
	if(!ostore_rec_read((gid -1),NULL,128))
	{
		ocomm_printf(CH_DISP,4,1,"读流水错误");
		return;
	} 		
	if(snum != cur_sysparas.b_basic_p[4])
	{
		ocomm_printf(CH_DISP,2,1,"请从%02d站解灰",snum);
		return;
	}
	if(gnum != cur_sysparas.b_basic_p[0])
	{
		ocomm_printf(CH_DISP,2,1,"请从%02d枪解灰",gnum);
		return;
	}
	memset(tmpbuf,0,16);
	memcpy(tmpbuf,&cur_card_data.block1_buf[2]  ,8);
	if(cur_rec.rec_cardnum != atol((char*)tmpbuf))
	{
		ocomm_printf(CH_DISP,2,1,"卡号不匹配:%ld",cur_rec.rec_cardnum);
		return;	
	}
	if(cur_rec.rec_id != gid)
	{
		ocomm_printf(CH_DISP,2,1,"流水不匹配:%ld",cur_rec.rec_id);
		return;	
	}
	oapp_update_card_info();  //解灰
	icard_step_cnt = 0;
	icard_chk_step = 0;
	olcm_clrscr();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_update_pro);	
	okernel_addeventpro(KEY_RET_EV,			 oapp_iccard_ungray_updateok_pro);			
	okernel_putevent(LCNG_COMM_EVENT1_EV,0); 
}

void oapp_iccard_ungray_selectok_pro(void)
{
	olcm_clrscr();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_read_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_ungray_readok_pro);		
	okernel_addeventpro(KEY_RET_EV,			 oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);	
}

void oapp_iccard_ungray_pro(void) //现场解灰
{
	icard_step_cnt = 0;
	icard_chk_step = 0;
	olcm_clrscr();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_ungray_selectok_pro);		
	okernel_addeventpro(KEY_RET_EV,			 oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

//更新数据
void oapp_update_card_info(void) //(bool bungray, unsigned long recid)
{
	unsigned char  tmp= 0, n;
	unsigned char tmpbuf[6];
	unsigned long tmp_long = 0;
	//将第4块拷贝至第5块
	memcpy((unsigned char *)&cur_card_data.block5_buf[0], (unsigned char *)&cur_card_data.block4_buf[0], 16);
	////更新第4块数据
	tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_M, 4);
	memset((char*)&cur_card_data.block4_buf[2],0,8);
	ocomm_hex2asciibuf(tmp_long,(char*)&cur_card_data.block4_buf[2],8);

	for(n=0; n<8; n++)
	{
		tmp ^= cur_card_data.block4_buf[2+n];
	}

	cur_card_data.block4_buf[0] = tmp/0x10 +0x30;
	cur_card_data.block4_buf[1] = tmp%0x10 +0x30;  

	ds3231_read_date((unsigned char*)&cur_time[0],true);
	ds3231_read_time((unsigned char*)&cur_time[3],true);
	ocomm_bcd2asc((unsigned char *)&cur_card_data.block4_buf[10],cur_time[0]); //年
	ocomm_bcd2asc((unsigned char *)&cur_card_data.block4_buf[12],cur_time[1]); //月
	ocomm_bcd2asc((unsigned char *)&cur_card_data.block4_buf[14],cur_time[2]); //日

	////更新第6块数据
	if(cur_rec.rec_lcngsum < 0)
	{
		//tmp_long = cur_rec.rec_lcngsum&0x7F;
		//sprintf((char*)&cur_card_data.block6_buf[2],"%ld",abs(cur_rec.rec_lcngsum));
		//LongTohex(unsigned long sdt,unsigned char * buffer,unsigned char len);
		//tmp_long = cur_rec.rec_lcngnum &0x7F;
		//tmp_long = (unsigned long)(cur_rec.rec_lcngnum);
		//ocomm_hex2asciibuf(tmp_long,(char*)&cur_card_data.block6_buf[2],8);
		LongTochar(abs(cur_rec.rec_lcngsum),(unsigned char*)&cur_card_data.block6_buf[2],8);
		cur_card_data.block6_buf[10] = '0'; 
		cur_card_data.block6_buf[11] = '7';  //回气 
	}
	else
	{
		//sprintf((char*)&cur_card_data.block6_buf[2],"%ld",cur_rec.rec_lcngsum);
		LongTochar(cur_rec.rec_lcngsum,(unsigned char*)&cur_card_data.block6_buf[2],8);
		cur_card_data.block6_buf[10] = '0'; 
		cur_card_data.block6_buf[11] = '1';  //加气
	}

	tmp = 0;
	for(n=0; n<4; n++)  tmp ^= ocomm_asc2bcd(&cur_card_data.block6_buf[2+n*2]);
	cur_card_data.block6_buf[0] = tmp/0x10 +0x30;
	cur_card_data.block6_buf[1] = tmp%0x10 +0x30;  

	//zl add 2013-12-5
	//block13
	memset(tmpbuf,'\0',6);
	memcpy((unsigned char *)tmpbuf,(unsigned char*)&cur_card_data.block13_buf[6],4);
	cur_rec.rec_CarNum = atoi((char *)tmpbuf);
	////更新第17块数据 
	tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_M, 4);
	memset((char*)&cur_card_data.block17_buf[11],0,5);
	ocomm_hex2bcdbuf(tmp_long,(char*)&cur_card_data.block17_buf[11],5); 
	//ocomm_float2bcdbuf(cur_rec.rec_card_M, (char*)&cur_card_data.block17_buf[11], 5);

	////更新第16块数据
	//ocomm_float2bcdbuf(cur_rec.rec_card_accConM, (char*)&cur_card_data.block16_buf[4], 6);  //累计消费金额
	//ocomm_float2bcdbuf(cur_rec.rec_card_accConlcng,(char*)&cur_card_data.block16_buf[12],4);

	//memcpy((unsigned char*)&cur_card_data.block16_buf[4],cur_rec.rec_card_accConM,6);
	tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_accConM, 6);
	memset((char*)&cur_card_data.block16_buf[4],0,6);
	ocomm_hex2bcdbuf(tmp_long,(char*)&cur_card_data.block16_buf[4],6);

	//memcpy((unsigned char*)&cur_card_data.block16_buf[12],cur_rec.rec_card_accConlcng,4);
	tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_accConlcng, 6);
	memset((char*)&cur_card_data.block16_buf[12],0,4);
	ocomm_hex2bcdbuf(tmp_long, (char*)&cur_card_data.block16_buf[12],4);

	ocomm_hex2bcdbuf(cur_rec.rec_card_connum,(char*)&cur_card_data.block16_buf[0],4);	  	//累计消费次数
	cur_card_data.block2_buf[6] = '0';
	cur_card_data.block2_buf[7] = '1';
}

void oapp_iccard_select_pro(void)
{
	otimer_release(LCNG_COMM_EVENT1_EV);
	memset(&cur_card_data.select_buf[0],0,sizeof(_icard_data));
	if(oicard_select() == false)
	{
		icard_step_cnt++; 
		ocomm_printf(CH_DISP,1,1,"寻卡:%d %d",icard_step_cnt, icard_err_code_);
		oicard_turnoff();
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//500ms		
		return;
	}
	memcpy(&cur_card_data.select_buf[0], (unsigned char*)&oicard_recvbuf[6], 4);
	icard_chk_step = 0;
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
}

unsigned char oapp_iccard_datachk(void)
{
	unsigned char tmpbuf[16] = {0};
	unsigned long tmp_long = 0,tmp_long1 = 0,tmp_long2 = 0;
	//float tmp_f = 0;
	////////block1	
	if(memcmp((unsigned char*)&cur_card_data.block1_buf[2],(unsigned char*)&cur_card_data.block9_buf[2],8) != 0)
	{
		return _rd_chk_cardnum_err;
	}
	memcpy(tmpbuf,&cur_card_data.block1_buf[2]  ,8);
	cur_rec.rec_cardnum = atol((char*)tmpbuf);  //卡号
	if(ostore_losscard_chk(cur_rec.rec_cardnum))
	{
		return _rd_chk_g_card;////挂失卡判定
	}		
    ////////block2
    if(cur_card_data.block2_buf[7] == 0x32)
    {    
		return _rd_chk_h_card;
    }
    else if(cur_card_data.block2_buf[9] == 0x32) 
    { 
		return _rd_chk_g_card;
    }       
	else
	{
		if(cur_card_data.block2_buf[3] > '6' || cur_card_data.block2_buf[3] < '1')
		{
			return _rd_chk_type_err;
		}
		else
		{
			cur_price = cur_sysparas.f_price[cur_card_data.block2_buf[3]-'0']; 
			cur_rec.rec_lcngprice = cur_price;
			ocomm_float2bcdbuf(cur_price, (char*)&cur_lcng_r_rec.brunPrice[0],2 );	
			//重新计算单价 金额
			if(cur_lcng_opt_type == _CNG_FIX_N) cur_fix_money = cur_fix_num*cur_price;

			switch(cur_card_data.block2_buf[3])
			{
			       case '1':
				   	  ocomm_printf(CH_DISP,2,1,"用户卡");
				   	  break;
			      case '2':
				   	  ocomm_printf(CH_DISP,2,1,"员工卡");
				   	  break;
			       case '3':
				   	  ocomm_printf(CH_DISP,2,1,"记帐卡");
				   	  break;	
				case '4':
				   	  ocomm_printf(CH_DISP,2,1,"维修卡");
				   	  break;
			       case '5':
				   	  ocomm_printf(CH_DISP,2,1,"内部卡");
				   	  break;
			        case '6':
				   	  ocomm_printf(CH_DISP,2,1,"积分卡");
				   	  break;	  
				default:
					break;	  
			}
		}
	}    
	
	////////block4
	//加气前余额
	memset(tmpbuf,'\0',16);//余额
	memcpy(tmpbuf,(unsigned char *)&cur_card_data.block4_buf[2],8);
	tmp_long = atol((char*)tmpbuf);
	memset(cur_rec.rec_card_preM,0,4);
	ocomm_hex2bcdbuf(tmp_long, (char *)cur_rec.rec_card_preM, 4);

	if(cur_card_data.block2_buf[3]  == '3')//记账卡
	{
		if(tmp_long >= BASIC_ICCARD_MAX_MOMEY)	
			return _rd_chk_b_excess;//记账超额----只针对记账卡
	}
	else//非记账卡
	{
		if(tmp_long <= cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY]*100) //10
			return _rd_chk_b_less;//余额不足
	}
	
	/////block13
	memset(tmpbuf,0,6);
	if(cur_sysparas.b_y_or_n_operate&BASIC_P_CYLINDER_TEST)
	{
		ds3231_read_date((unsigned char*)&cur_time[0],true);
		ocomm_bcd2asc((unsigned char *)&tmpbuf[0],cur_time[0]); 
		ocomm_bcd2asc((unsigned char *)&tmpbuf[2],cur_time[1]); 
		ocomm_bcd2asc((unsigned char *)&tmpbuf[4],cur_time[2]); 
		if(memcmp((unsigned char*)&tmpbuf[0],(unsigned char *)&cur_card_data.block13_buf[0],6) > 0)
		{
			//omm_printf(CH_DISP,2,1,"钢瓶到期，请检验");
			//ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",tmpbuf[0],tmpbuf[1],tmpbuf[2],tmpbuf[3],tmpbuf[4],tmpbuf[5]);
			//ocomm_printf(CH_DISP,3,1,"%x %x %x %x %x %x",cur_card_data.block13_buf[0],cur_card_data.block13_buf[1],cur_card_data.block13_buf[2],cur_card_data.block13_buf[3],cur_card_data.block13_buf[4],cur_card_data.block13_buf[5]);
			return  _rd_chk_cylinder_err;
		}
	}
	
	//////block16
	//累计消费次数
	memset(tmpbuf,0,16);
	memcpy(&tmpbuf[1], &cur_card_data.block16_buf[1], 3);
	cur_rec.rec_card_connum =  ocomm_bcdbuf2hex((unsigned char *)&tmpbuf[0], 4);												  //累计消费次数
	//累计消费金额
	tmp_long = ocomm_bcdbuf2hex((unsigned char *)&cur_card_data.block16_buf[4], 6);
	memset(cur_rec.rec_card_accConM,0,6);
	ocomm_hex2bcdbuf(tmp_long, (char *)cur_rec.rec_card_accConM,6);
	//累计消费气量
	tmp_long = ocomm_bcdbuf2hex((unsigned char *)&cur_card_data.block16_buf[12], 4);
	memset(cur_rec.rec_card_accConlcng,0,6);
	ocomm_hex2bcdbuf(tmp_long, (char *)cur_rec.rec_card_accConlcng,6);

	//////block17
	memset(tmpbuf,0,16);
	memcpy((unsigned char *)&tmpbuf[1],(unsigned char *)&cur_card_data.block17_buf[11], 5);
	tmp_long = ocomm_bcdbuf2hex((unsigned char *)&tmpbuf[0], 6);
	//计算余额
	memset(cur_rec.rec_card_EndOverage,0,4);
	ocomm_hex2bcdbuf(tmp_long, (char *)cur_rec.rec_card_EndOverage, 4);
      
	tmp_long1 = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
	tmp_long2 = 0;
	if(tmp_long != tmp_long1)
	{
		ocomm_printf(CH_DISP,1,1,"%ld",tmp_long2);
		ocomm_printf(CH_DISP,2,1,"%ld.%02d",tmp_long/100,tmp_long%100);//计算余额
		ocomm_printf(CH_DISP,3,1,"%ld.%02d",tmp_long1/100,tmp_long1%100);//加气前余额
		return _rd_chk_b_unm;//余额不一致
	}
	
	//////block18
	//累计充值金额
	memcpy(cur_rec.rec_card_accfM,(unsigned char *)&cur_card_data.block18_buf[0],6);

	//cur_rec.rec_card_accfM = (float)(((double)ocomm_bcdbuf2hex(&cur_card_data.block18_buf[0], 6))/100); 	//累计充值金额
	memcpy(&tmpbuf[1], &cur_card_data.block18_buf[7], 3);
	cur_rec.rec_card_accftimes = ocomm_bcdbuf2hex(&tmpbuf[0], 4);		 							//累计充值次数

	tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_preM,4);//加气前余额
	tmp_long1 =  ocomm_bcdbuf2hex(cur_rec.rec_card_accConM, 6);//累计消费金额
	tmp_long2 = ocomm_bcdbuf2hex(cur_rec.rec_card_accfM, 6);//累计充值金额

	if(cur_card_data.block2_buf[3]  == '3')//记账卡
	{
		if((tmp_long1 + tmp_long2) != tmp_long)
		{
			ocomm_printf(CH_DISP,1,1,"%ld.%02d",tmp_long/100,tmp_long%100);//加气前余额
			ocomm_printf(CH_DISP,2,1,"%ld.%02d",tmp_long1/100,tmp_long1%100); //累计消费金额
			ocomm_printf(CH_DISP,3,1,"%ld.%02d",tmp_long2/100,tmp_long2%100); //累计充值金额
			return _rd_chk_b_err; //余额异常
		}
	}
	else//非记账卡
	{
		if((tmp_long + tmp_long1) != tmp_long2)
		{
			ocomm_printf(CH_DISP,1,1,"%ld.%02d",tmp_long/100,tmp_long%100);//加气前余额
			ocomm_printf(CH_DISP,2,1,"%ld.%02d",tmp_long1/100,tmp_long1%100); //累计消费金额
			ocomm_printf(CH_DISP,3,1,"%ld.%02d",tmp_long2/100,tmp_long2%100); //累计充值金额
			return _rd_chk_b_err; //余额异常
		}
	}
	return _rd_chk_ok;	
}


prog_char _ICCARD_GETGRAY_BLOCKS[]     = {14,2};
prog_char _ICCARD_GETGRAY_BLOCKS_PT[]  = {8,2};
void oapp_iccard_gray_pro(void)
{
	unsigned long tmp = 0;
	unsigned char buf[4];
	unsigned char snum;
	unsigned char cur_blocks = 0;
	unsigned char buf_pt     = 0;
	unsigned char *wtbuf  = NULL;
	OS_DISABLE_KEYSCAN();  //2013-03-11
	cur_blocks = pgm_read_byte(&_ICCARD_GETGRAY_BLOCKS[icard_chk_step]);
	buf_pt     = pgm_read_byte(&_ICCARD_GETGRAY_BLOCKS_PT[icard_chk_step]);
	ocomm_printf(CH_DISP,1,1,"写灰: %d %d",cur_blocks, buf_pt);	
	wtbuf = cur_card_data.select_buf+buf_pt*16;
	if(!oicard_write(false, cur_blocks, wtbuf) )
	{
		ocomm_printf(CH_DISP,2,1,"err: %d", icard_chk_step);	
		otimer_reqire(LCNG_COMM_EVENT2_EV, 3, 0, 1); 
		OS_ENABLE_KEYSCAN();  //2013-03-11
		return;
	}
	otimer_release(LCNG_COMM_EVENT2_EV);
	icard_chk_step++;
	if(icard_chk_step >= 2 && cur_blocks == 2)
	{
		/////判断写灰是否成功
		if(cur_card_data.block2_buf[7] != '2')
		{
			ocomm_printf(CH_DISP,1,1,"写灰错误");	
			icard_chk_step = 0;
			cur_rec.rec_b_CardOK = false;
			otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
			OS_ENABLE_KEYSCAN();
			return;
		}
		/////判断枪号是否写入
		if(cur_card_data.block14_buf[6] != cur_sysparas.b_basic_p[0])
		{
			ocomm_printf(CH_DISP,1,1,"枪号写入失败");	
			icard_chk_step = 0;
			cur_rec.rec_b_CardOK = false;
			otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
			OS_ENABLE_KEYSCAN();
			return;
		}
		/////判断流水号是否写入
		buf[0] = (unsigned char)((cur_sysinfo.g_uid +1)>>24); 		
		buf[1] = (unsigned char)((cur_sysinfo.g_uid +1)>>16); 		
		buf[2] = (unsigned char)((cur_sysinfo.g_uid +1)>>8); 		
		buf[3] = (unsigned char)((cur_sysinfo.g_uid +1)); 	
		if(memcmp((unsigned char*)&cur_card_data.block14_buf[7],buf,4) != 0)
		{
			ocomm_printf(CH_DISP,1,1,"流水号写入失败");
			icard_chk_step = 0;
			cur_rec.rec_b_CardOK = false;
			otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
			OS_ENABLE_KEYSCAN();
			return;
		}
		/////判断加气站编号是否写入
		snum = hexTolong((unsigned char*)&cur_card_data.block14_buf[11],1);//加气站编号
		if(snum != cur_sysparas.b_basic_p[4])
		{
			ocomm_printf(CH_DISP,1,1,"加气站编号写入失败");	
			icard_chk_step = 0;
			cur_rec.rec_b_CardOK = false;
			otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
			OS_ENABLE_KEYSCAN();
			return;
		}
		
		icard_chk_step = 0;
		cur_rec.rec_b_CardOK = true;  //2013-02-26
		tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
		ocomm_printf(CH_DISP,1,1,"余额:%ld.%02d",tmp/100,tmp%100);
		otimer_reqire(LCNG_COMM_EVENT1_EV, 10, 0, 1); 	//1000ms	
	}
	else
	{
		okernel_putevent(LCNG_COMM_EVENT2_EV,0);
	}
	OS_ENABLE_KEYSCAN();  //2013-03-11 
}

void oapp_iccard_lcngctrl_start_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, olngctrl_start_pro);		
#ifdef LNG_COEFFI_SET
	unsigned int tmp = (unsigned int)(cur_sysparas.b_factor[1]);
	if(tmp != 0)
	{
		ocomm_printf(CH_DISP,2,1,"加液等待中...");
		olngctrl_start_init();	
		otimer_reqire(LCNG_COMM_EVENT2_EV,tmp,0,1);
	}
	else
	{
		olngctrl_start_init(); 
		okernel_putevent(LCNG_COMM_EVENT2_EV,1);   
	}
#else
		okernel_putevent(LCNG_COMM_EVENT2_EV,1);
#endif	   
}

void oapp_iccard_gray_execure(void)
{
	icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_lcngctrl_start_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_gray_pro);	
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);		
	
	cur_card_data.block2_buf[6] = '0';
	cur_card_data.block2_buf[7] = '2';//////置灰卡
	cur_card_data.block14_buf[6] = cur_sysparas.b_basic_p[0];//枪号		
	cur_card_data.block14_buf[7] = (unsigned char)((cur_sysinfo.g_uid + 1)>>24); 		
	cur_card_data.block14_buf[8] = (unsigned char)((cur_sysinfo.g_uid + 1)>>16); 		
	cur_card_data.block14_buf[9] = (unsigned char)((cur_sysinfo.g_uid + 1)>>8); 		
	cur_card_data.block14_buf[10] = (unsigned char)((cur_sysinfo.g_uid + 1));//流水号
	LongTohex(cur_sysparas.b_basic_p[4],(unsigned char*)&cur_card_data.block14_buf[11],1);
	//cur_card_data.block14_buf[11] = cur_sysparas.b_basic_p[4];//加气站编号
	icard_chk_step = 0;
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
}

void oapp_iccard_getpsw_pro(void)
{
	unsigned char len;
	unsigned char buf[4] = {'0','0','0','0'};
	len = strlen((char*)cur_input_cursor.str_input);
	memcpy((unsigned char *)&buf[0],cur_input_cursor.str_input,len); 
	if(memcmp((unsigned char*)&buf[0],(unsigned char *)&cur_card_data.block13_buf[6],4) != 0)
	{
		ocomm_printf(CH_DISP,4,1,"车牌号错误");
		//ocomm_printf(CH_DISP,2,1,"%c %c %c %c",cur_card_data.block13_buf[6],cur_card_data.block13_buf[7],cur_card_data.block13_buf[8],cur_card_data.block13_buf[9]);
		//ocomm_printf(CH_DISP,3,1,"%c %c %c %c",buf[0],buf[1],buf[2],buf[3]);
		//okernel_putevent(KEY_RET_EV,0);
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1);
		return;
	} 
	oapp_iccard_gray_execure();
}
void oapp_iccard_getpsw_execure(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = true;
	cur_input_cursor.p_x   = 2;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 4;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_getpsw_execure);	
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);
	okernel_addeventpro(KEY_OK_EV,  oapp_iccard_getpsw_pro);
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"请输入车牌号:"); 
}

void oapp_iccard_readok_pro(void)
{
	unsigned char ret = 0;
	unsigned char buf[4] = {'0','0','0','0'};
	OS_DISABLE_KEYSCAN();  //2013-03-11
	ocomm_printf(CH_DISP,1,1,"卡数据校验");	
	ret = oapp_iccard_datachk();
	if(_rd_chk_ok == ret) 
	{
		OS_ENABLE_KEYSCAN();  //2013-03-11
#if 0	//luom 2014-06-25	
		if(memcmp((unsigned char*)&buf[0],(unsigned char *)&cur_card_data.block13_buf[6],4) != 0)
			oapp_iccard_getpsw_execure();
		else
#else
		if(cur_sysparas.b_y_or_n_operate&BASIC_P_INPUT_CARNUM )
			oapp_iccard_getpsw_execure();
		else
#endif
			oapp_iccard_gray_execure();
		return;
	}		
	oapp_iccard_tip_msg(4,1,ret-1); ///提示
	OS_ENABLE_KEYSCAN();  //2013-03-11
}

prog_char _ICCARD_READ_BLOCKS[15]   = {1,2,4,5,6,9,13,14,16,17,18};
prog_char _ICCARD_STEPS_BLOCKS[15]  = {0,1,2,3,4,5, 6, 7,8,9,10,11};
void oapp_iccard_read_pro(void)
{
	unsigned char cur_blocks = 0;
	unsigned char nstep      = 0;
	unsigned char *rdbuf = NULL;
	OS_DISABLE_KEYSCAN();  //2013-03-11
	ocomm_printf(CH_DISP,1,1,"读卡:%d",cur_blocks);	
	cur_blocks = pgm_read_byte(&_ICCARD_READ_BLOCKS[icard_chk_step]);
	nstep      = pgm_read_byte(&_ICCARD_STEPS_BLOCKS[icard_chk_step]);
	
	rdbuf = cur_card_data.block1_buf+icard_chk_step*16;
	if(!oicard_read(false, cur_blocks, rdbuf) || nstep != icard_chk_step)
	{
		ocomm_printf(CH_DISP,1,5,"读卡:%d %d %",cur_blocks, icard_chk_step, nstep);	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//300ms	
		OS_ENABLE_KEYSCAN();  //2013-03-11	
		return;
	}

	otimer_release(LCNG_COMM_EVENT1_EV);
	icard_chk_step++;
	if(icard_chk_step >= 9 && cur_blocks == 18)
	{
		okernel_putevent(LCNG_COMM_EVENT2_EV,0);
	}
	else
	{
		okernel_putevent(LCNG_COMM_EVENT1_EV,0);
	}
	OS_ENABLE_KEYSCAN();  //2013-03-11
}

void oapp_iccard_readblocks_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_read_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_readok_pro);	
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);	

}

void oapp_iccard_select_ex_pro(void)
{
	unsigned char str[4] = {0xAA, 0xFF, 0x40,0x00};
	bool          rdok = false;
	if(true == oicard_select())
	{
		//oprt_ctrl_paras_info();
		if(memcmp((unsigned char*)&oicard_recvbuf[6],(unsigned char*)&cur_card_data.select_buf[0],4) != 0) 
		{
			ocomm_printf(CH_DISP,2,1,"卡号不匹配");
			rdok = false;
//			ocomm_printf(CH_DISP,3,1,"%d %d %d %d",oicard_recvbuf[3],oicard_recvbuf[4],oicard_recvbuf[5],oicard_recvbuf[6]);
//			ocomm_printf(CH_DISP,4,1,"%d %d %d %d",cur_card_data.select_buf[0],cur_card_data.select_buf[1],cur_card_data.select_buf[2],cur_card_data.select_buf[3]);
		}			 
		else
		{
			 rdok = true;
		} 
	}
	if(rdok == false)
	{
		str[3] ^= str[0];
		str[3] ^= str[1];
		str[3] ^= str[2];
		oicard_sendstr(str,4);			
		icard_step_cnt++;
		ocomm_printf(CH_DISP,1,1,"寻卡:%d",icard_step_cnt);
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//3300ms		
		return;
	}
	//memcpy(cur_card_data.select_buf, &oicard_recvbuf[3], 16);
	otimer_release(LCNG_COMM_EVENT1_EV);
	icard_chk_step = 0;
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
}

prog_char _ICCARD_UPDATE_BLOCKS[]     = {5,4,6,16,17,2};
prog_char _ICCARD_UPDATE_BLOCKS_PT[]  = {4,3,5,9, 10 ,2};	
void oapp_iccard_update_pro(void)
{
	unsigned char cur_blocks = 0;//pgm_read_byte(&_ICCARD_UPDATE_BLOCKS[icard_chk_step]);
	unsigned char buf_pt     = 0;//pgm_read_byte(&_ICCARD_UPDATE_BLOCKS_PT[icard_chk_step]);
	unsigned char nstep      = 0;
	//unsigned char wtbuf[16]  = {0};//cur_card_data.select_buf;;
	unsigned char *wtbuf  = NULL;//cur_card_data.select_buf;;
	OS_DISABLE_KEYSCAN();  //2013-03-11
	cur_blocks = pgm_read_byte(&_ICCARD_UPDATE_BLOCKS[icard_chk_step]);
	buf_pt     = pgm_read_byte(&_ICCARD_UPDATE_BLOCKS_PT[icard_chk_step]);
	nstep      = pgm_read_byte(&_ICCARD_STEPS_BLOCKS[icard_chk_step]);
	ocomm_printf(CH_DISP,1,1,"写卡: %d %d",cur_blocks, buf_pt);	
	wtbuf = cur_card_data.select_buf+buf_pt*16;
	//memcpy(wtbuf, cur_card_data.select_buf+buf_pt*16, 16);
	
	if(!oicard_write(false, cur_blocks, wtbuf) || nstep != icard_chk_step)
	{
		ocomm_printf(CH_DISP,2,1,"err: %d %d",nstep, icard_chk_step);	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 3, 0, 1); 	//300ms	 //ocomm_printf(CH_DISP,2,1,"失败:%d %d %d %d",icard_err_, oicard_recvbuf[0], oicard_recvbuf[1], oicard_recvbuf[2] );
		OS_ENABLE_KEYSCAN();  //2013-03-11
		return;
	}
	otimer_release(LCNG_COMM_EVENT1_EV);
	icard_chk_step++;
	if(icard_chk_step >= 6 && cur_blocks == 2)
	{
		icard_chk_step = 0;
		cur_rec.rec_b_CardOK = false;  //2013-02-26
		okernel_putevent(KEY_RET_EV,0);
	}
	else
	{
		okernel_putevent(LCNG_COMM_EVENT1_EV,0);
	}
	OS_ENABLE_KEYSCAN();  //2013-03-11 
}

void oapp_iccard_updateblocks_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_update_pro);	
	okernel_addeventpro(KEY_RET_EV,          olngctrl_stop_disp);	
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_update_execute(void)
{
	icard_step_cnt = 0;
	icard_chk_step = 0;
	oapp_update_card_info();
	olcm_clrscr();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_ex_pro);
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_updateblocks_pro);	
	okernel_addeventpro(KEY_RET_EV,          olngctrl_stop_disp);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0); 
}

void oapp_iccard_ready_execute(void)
{
	icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);//寻卡
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_readblocks_pro);	//读卡
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);	
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_read_imfor_disp(void)
{
	unsigned char tmpbuf[16] = {0};
	unsigned long tmp_long = 0;
       olcm_clrscr();
	//okernel_putevent(LCNG_COMM_EVENT3_EV,0);
	memset(tmpbuf,0,16);
	memcpy(tmpbuf,&cur_card_data.block1_buf[2]  ,8);
       tmp_long = atol((char*)tmpbuf);  //卡号
       ocomm_printf(CH_DISP, 1, 1,"卡号:%ld",tmp_long);
       memset(tmpbuf,0,16);
	memcpy(tmpbuf,(unsigned char *)&cur_card_data.block4_buf[2],8);
       tmp_long = atol((char*)tmpbuf);  
       ocomm_printf(CH_DISP,2,1,"余额:%ld.%02d",tmp_long/100,tmp_long%100);
           memset(tmpbuf,0,16);
	memcpy(tmpbuf,(unsigned char *)&cur_card_data.block18_buf[0],6);
	tmp_long = ocomm_bcdbuf2hex(tmpbuf, 6);
	ocomm_printf(CH_DISP,3,1,"充值金额:");
	ocomm_printf(CH_DISP,4,1,"%ld.%02d",tmp_long/100,tmp_long%100);

}

void oapp_iccard_check_readblocks_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_read_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_read_imfor_disp);	
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_check_imfor(void)
{
       icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);//寻卡
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_check_readblocks_pro);
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);	
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_write_flag_return(void)
{
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);	
	ocomm_message(4,1,"正常卡恢复",10,NULL);	
}

/*
@写标志
*/
void iccard_write_flag(void)
{
   	cur_card_data.block2_buf[6] = '0'; 
	cur_card_data.block2_buf[7] = '1';
	OS_DISABLE_KEYSCAN();  //2013-03-11
	if(!oicard_write(false, 2, cur_card_data.block2_buf))
	{
		ocomm_printf(CH_DISP,1,1,"置正常卡失败",icard_step_cnt++);		
		otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1); 	//300ms	
		OS_ENABLE_KEYSCAN();  //2013-03-11
		return;
	}
	cur_rec.rec_b_CardOK = false;  //2013-02-26
	otimer_release(LCNG_COMM_EVENT2_EV);
	okernel_putevent(KEY_RET_EV, 0);
	OS_ENABLE_KEYSCAN();  //2013-03-11	
}
/*
@恢复正常卡标志
@
*/
void oapp_iccard_back_write_flag(void)
{
    icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);//寻卡
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, iccard_write_flag);
	okernel_addeventpro(KEY_RET_EV, oapp_iccard_write_flag_return);
	okernel_putevent(LCNG_COMM_EVENT1_EV, 0);
}

#else

void oapp_iccard_ungray_updateok_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);	
	ocomm_message(4,1,"解灰成功",10,NULL);
}

void oapp_iccard_ungray_readok_pro(void)
{
	unsigned long gid = 0;
	unsigned char gnum = 0,snum = 0;
	gnum = cpu_card_data.CpuGunNum;
	gid = hexTolong(cpu_card_data.CpuSerialNum, 4);
	snum = cpu_card_data.CpuLastStation[1];
	ocomm_printf(CH_DISP,1,1,"加气站编号：%d",snum); 
	ocomm_printf(CH_DISP,2,1,"枪号：%d",gnum); 
	ocomm_printf(CH_DISP,3,1,"流水号:%ld", gid);
	if(cpu_card_data.CpuValidity == 0x01)		
	{
		ocomm_printf(CH_DISP,4,1,"正常卡");
		return;			
	}
	if(cur_sysinfo.g_uid < gid)		
	{
		ocomm_printf(CH_DISP,4,1,"流水号有误");
		return;		
	}
	if(!ostore_rec_read((gid -1),NULL,128))
	{
		ocomm_printf(CH_DISP,4,1,"读流水错误");
		return;
	} 	

	if(snum != cur_sysparas.b_basic_p[4])
	{
		ocomm_printf(CH_DISP,2,1,"请从%02d站解灰",snum);
		return;
	}
	
	if(gnum != cur_sysparas.b_basic_p[0])
	{
		ocomm_printf(CH_DISP,2,1,"请从%02d枪解灰",gnum);
		return;
	}
	if(cur_rec.rec_cardnum != hexTolong((unsigned char*)&cpu_card_data.CpuCardNum[4],4))
	{
		ocomm_printf(CH_DISP,2,1,"卡号不匹配:%ld",cur_rec.rec_cardnum);
		return;	
	}
	if(cur_rec.rec_id != gid)
	{
		ocomm_printf(CH_DISP,2,1,"流水不匹配:%ld",cur_rec.rec_id);
		return;	
	}
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"解灰......");	
	oapp_update_card_info();  //解灰
	icard_step_cnt = 0;
	icard_chk_step = 0;
	//最后操作方式
	cpu_card_data.CpuOper = 0x03;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_update_pro);	
	okernel_addeventpro(KEY_RET_EV,			 oapp_iccard_ungray_updateok_pro);			
	okernel_putevent(LCNG_COMM_EVENT1_EV,0); 
}

void oapp_iccard_ungray_selectok_pro(void)
{
	olcm_clrscr();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_read_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_ungray_readok_pro);		
	okernel_addeventpro(KEY_RET_EV,			 oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);	
}

void oapp_iccard_ungray_pro(void) //现场解灰
{
	icard_step_cnt = 0;
	icard_chk_step = 0;
	olcm_clrscr();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_ungray_selectok_pro);		
	okernel_addeventpro(KEY_RET_EV,			 oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

//更新数据
void oapp_update_card_info(void) //(bool bungray, unsigned long recid)
{
	unsigned long  tmp= 0;
	//上次余额、上次操作方式、上次操作时间
	memcpy((unsigned char *)&cpu_card_data.CpuLastBalance[0], (unsigned char *)&cpu_card_data.CpuBalance[0], 4);
	cpu_card_data.CpuLastOper = cpu_card_data.CpuOper;
	memcpy((unsigned char *)&cpu_card_data.CpuLastOperTime[0], (unsigned char *)&cpu_card_data.CpuLastTime[0], 6);
	//最后加气金额
	tmp = cur_rec.rec_lcngsum;
	LongTohex(tmp, cpu_card_data.CpuLastMon, 4);
	//最后加气气量
	tmp = (unsigned long)cur_rec.rec_lcngnum;
	LongTohex(tmp, cpu_card_data.CpuLastVol, 4);
	//最后使用时间
	ds3231_read_date((unsigned char*)&cur_time[0],true);
	ds3231_read_time((unsigned char*)&cur_time[3],true);
	charTohex(cur_time,6);
	memcpy((unsigned char *)&cpu_card_data.CpuLastTime[0], (unsigned char *)&cur_time[0], 6);
	//卡余额
	memcpy((unsigned char *)&cpu_card_data.CpuBalance[0], cur_rec.rec_card_M, 4);
	//最后操作方式
	cpu_card_data.CpuOper = 0x01;
	//更改灰标志
	cpu_card_data.CpuValidity = 0x01;
}

void oapp_iccard_select_pro(void)
{
	otimer_release(LCNG_COMM_EVENT1_EV);
	if(CpuCardSelect_Oper() == false)
	{
		icard_step_cnt++; 
		ocomm_printf(CH_DISP,1,1,"寻卡:%d",icard_step_cnt);
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//500ms		
		return;
	}
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
}

unsigned char oapp_iccard_datachk(void)
{
	unsigned long tmp_long = 0,tmp_long1 = 0,tmp_long2 = 0,tmp_long3 = 0;
	
	cur_rec.rec_cardnum = hexTolong((unsigned char*)&cpu_card_data.CpuCardNum[4],4);
	if(ostore_losscard_chk(cur_rec.rec_cardnum))
	{
		return _rd_chk_g_card;////挂失卡判定
	}		
	if(cpu_card_data.CpuValidity == 0x02)
	{    
		return _rd_chk_h_card;//灰卡
	}
	if(cpu_card_data.CpuCardType > 0x06 || cpu_card_data.CpuCardType < 0x01)
	{
		return _rd_chk_type_err;//卡类型错误
	}
	else
	{
		cur_price = cur_sysparas.f_price[cpu_card_data.CpuCardType -0x00]; 
		cur_rec.rec_lcngprice = cur_price;
		ocomm_float2bcdbuf(cur_price, (char*)&cur_lcng_r_rec.brunPrice[0],2 );	
		//重新计算单价 金额
		if(cur_lcng_opt_type == _CNG_FIX_N) cur_fix_money = cur_fix_num*cur_price;
		switch(cpu_card_data.CpuCardType)
		{
		       case 0x01:
			   	  ocomm_printf(CH_DISP,2,1,"用户卡");
			   	  break;
		      case 0x02:
			   	  ocomm_printf(CH_DISP,2,1,"员工卡");
			   	  break;
		       case 0x03:
			   	  ocomm_printf(CH_DISP,2,1,"记帐卡");
			   	  break;	
			case 0x04:
			   	  ocomm_printf(CH_DISP,2,1,"维修卡");
			   	  break;
		       case 0x05:
			   	  ocomm_printf(CH_DISP,2,1,"内部卡");
			   	  break;
		        case 0x06:
			   	  ocomm_printf(CH_DISP,2,1,"积分卡");
			   	  break;	  
			default:
				break;	  
		}
	}
	
	tmp_long = hexTolong(cpu_card_data.CpuBalance,4);//卡余额
	memcpy(cur_rec.rec_card_preM,cpu_card_data.CpuBalance,4);

	if(cpu_card_data.CpuCardType  == 0x03)//记账卡
	{
		if(tmp_long >= BASIC_ICCARD_MAX_MOMEY)	
			return _rd_chk_b_excess;//记账超额----只针对记账卡
	}
	else//非记账卡
	{
		if(tmp_long <= cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY]*100) //10
		{
			ocomm_printf(CH_DISP,1,1,"%ld.%02d",tmp_long/100,tmp_long%100);//卡余额
			return _rd_chk_b_less;//余额不足
		}
	}
	
	if(cur_sysparas.b_y_or_n_operate&BASIC_P_CYLINDER_TEST)
	{
		ds3231_read_date((unsigned char*)&cur_time[0],true);
		//ocomm_bcd2asc((unsigned char *)&tmpbuf[0],cur_time[0]); 
		//ocomm_bcd2asc((unsigned char *)&tmpbuf[2],cur_time[1]); 
		//ocomm_bcd2asc((unsigned char *)&tmpbuf[4],cur_time[2]); 
		if(memcmp((unsigned char*)&cur_time[0],(unsigned char *)&cpu_card_data.CpuSteelDate[0],3) > 0)
		{
			//omm_printf(CH_DISP,2,1,"钢瓶到期，请检验");
			//ocomm_printf(CH_DISP,2,1,"%x %x %x %x %x %x",tmpbuf[0],tmpbuf[1],tmpbuf[2],tmpbuf[3],tmpbuf[4],tmpbuf[5]);
			//ocomm_printf(CH_DISP,3,1,"%x %x %x %x %x %x",cur_card_data.block13_buf[0],cur_card_data.block13_buf[1],cur_card_data.block13_buf[2],cur_card_data.block13_buf[3],cur_card_data.block13_buf[4],cur_card_data.block13_buf[5]);
			return  _rd_chk_cylinder_err;
		}
	}
	
	tmp_long = ocomm_bcdbuf2hex(cpu_card_data.CpuBalance,4);//卡余额
	tmp_long1 =  ocomm_bcdbuf2hex(cpu_card_data.CpuLastBalance, 4);//上次余额
	tmp_long2 =  ocomm_bcdbuf2hex(cpu_card_data.CpuRechargeMon, 4);//最后充值金额
	tmp_long3 =  ocomm_bcdbuf2hex(cpu_card_data.CpuLastMon, 4);//最后加气金额
	if(cpu_card_data.CpuCardType  == 0x03)//记账卡
	{
		if((tmp_long1 + tmp_long3) != tmp_long)
		{
			ocomm_printf(CH_DISP,1,1,"%ld.%02d",tmp_long/100,tmp_long%100);//卡余额
			ocomm_printf(CH_DISP,2,1,"%ld.%02d",tmp_long1/100,tmp_long1%100); //上次余额
			ocomm_printf(CH_DISP,3,1,"%ld.%02d",tmp_long3/100,tmp_long3%100); //最后加气金额
			return _rd_chk_b_err; //余额异常
		}
	}
	else//非记账卡
	{
		if(cpu_card_data.CpuOper ==0x02 )//最后操作为充值
		{
			if(tmp_long != (tmp_long1 + tmp_long2))
			{
				ocomm_printf(CH_DISP,1,1,"%ld.%02d",tmp_long/100,tmp_long%100);//卡余额
				ocomm_printf(CH_DISP,2,1,"%ld.%02d",tmp_long1/100,tmp_long1%100); //上次余额
				ocomm_printf(CH_DISP,3,1,"%ld.%02d",tmp_long2/100,tmp_long2%100); //最后充值金额
				return _rd_chk_b_err; //余额异常
			}	
		}
		else
		{
			if(tmp_long != (tmp_long1 - tmp_long3))
			{
				ocomm_printf(CH_DISP,1,1,"%ld.%02d",tmp_long/100,tmp_long%100);//卡余额
				ocomm_printf(CH_DISP,2,1,"%ld.%02d",tmp_long1/100,tmp_long1%100); //上次余额
				ocomm_printf(CH_DISP,3,1,"%ld.%02d",tmp_long3/100,tmp_long3%100); //最后加气金额
				return _rd_chk_b_err; //余额异常
			}
		}
	}
	return _rd_chk_ok;	
}

void oapp_iccard_gray_pro(void)
{
	unsigned long tmp = 0;
	unsigned char buf[4];
	OS_DISABLE_KEYSCAN(); 
	ocomm_printf(CH_DISP,1,1,"写灰......");	
	if(!oicard_write(0x16,6,(unsigned char*)&cpu_card_data.CpuGunNum,16))
	{
		ocomm_printf(CH_DISP,2,1,"写灰失败");	
		otimer_reqire(LCNG_COMM_EVENT2_EV, 3, 0, 1); 
		OS_ENABLE_KEYSCAN();
		return;
	}

	if(!oicard_read(0x16,22,cpu_card_data.CpuLastMon))
	{
		ocomm_printf(CH_DISP,2,1,"写灰失败");	
		otimer_reqire(LCNG_COMM_EVENT2_EV, 3, 0, 1); 
		OS_ENABLE_KEYSCAN();
		return;
	}
	/////判断写灰是否成功
	if(cpu_card_data.CpuValidity != 0x02)
	{
		ocomm_printf(CH_DISP,1,1,"写灰错误");	
		cur_rec.rec_b_CardOK = false;
		otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN();
		return;
	}

	/////判断加气站编号是否写入
	if(cpu_card_data.CpuLastStation[1] != cur_sysparas.b_basic_p[4])
	{
		ocomm_printf(CH_DISP,1,1,"加气站编号写入失败");	
		cur_rec.rec_b_CardOK = false;
		otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN();
		return;
	}
	
	/////判断枪号是否写入
	if(cpu_card_data.CpuGunNum != cur_sysparas.b_basic_p[0])
	{
		ocomm_printf(CH_DISP,1,1,"枪号写入失败");	
		cur_rec.rec_b_CardOK = false;
		otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN();
		return;
	}
	/////判断流水号是否写入
	LongTohex((cur_sysinfo.g_uid +1),buf,4);
	if(memcmp((unsigned char*)&cpu_card_data.CpuSerialNum[0],buf,4) != 0)
	{
		ocomm_printf(CH_DISP,1,1,"流水号写入失败");
		cur_rec.rec_b_CardOK = false;
		otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN();
		return;
	}
	//oprt_Cpufeil_info();
	otimer_release(LCNG_COMM_EVENT2_EV);
	cur_rec.rec_b_CardOK = true;
	tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
	ocomm_printf(CH_DISP,1,1,"余额:%ld.%02d",tmp/100,tmp%100);
	otimer_reqire(LCNG_COMM_EVENT1_EV, 10, 0, 1);
	OS_ENABLE_KEYSCAN();
}

void oapp_iccard_lcngctrl_start_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, olngctrl_start_pro);		
#ifdef LNG_COEFFI_SET
	unsigned int tmp = (unsigned int)(cur_sysparas.b_factor[1]);
	if(tmp != 0)
	{
		ocomm_printf(CH_DISP,2,1,"加液等待中...");
		olngctrl_start_init();	
		otimer_reqire(LCNG_COMM_EVENT2_EV,tmp,0,1);
	}
	else
	{
		olngctrl_start_init(); 
		okernel_putevent(LCNG_COMM_EVENT2_EV,1);   
	}
#else
		okernel_putevent(LCNG_COMM_EVENT2_EV,1);
#endif	   
}

void oapp_iccard_gray_execure(void)
{
	icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_lcngctrl_start_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_gray_pro);	
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);		
	//////置灰卡
	cpu_card_data.CpuValidity = 0x02;
	cpu_card_data.CpuGunNum = cur_sysparas.b_basic_p[0]; 	//枪号
	LongTohex((cur_sysinfo.g_uid +1), cpu_card_data.CpuSerialNum, 4);//流水号
	cpu_card_data.CpuLastStation[0] = 0x00;
	cpu_card_data.CpuLastStation[1] = cur_sysparas.b_basic_p[4]; 	//加气站编号
	
	icard_chk_step = 0;
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
}
void oapp_iccard_getpsw_pro(void)
{
	unsigned char len;
	unsigned char buf[6] = {'0','0','0','0','0','0'},buffer[6],data;
	len = strlen((char*)cur_input_cursor.str_input);
	memcpy((unsigned char *)&buf[0],cur_input_cursor.str_input,len); 
	data = cpu_card_data.CpuCardPassword[0];
	buffer[0] = data/16 +48;
	buffer[1] = data%16 +48;
	data = cpu_card_data.CpuCardPassword[1];
	buffer[2] = data/16 +48;
	buffer[3] = data%16 +48;
	data = cpu_card_data.CpuCardPassword[2];
	buffer[4] = data/16 +48;
	buffer[5] = data%16 +48;
	
	if(memcmp((unsigned char*)&buf[0],(unsigned char *)&buffer[0],6) != 0)
	{
		ocomm_printf(CH_DISP,4,1,"车牌号错误");
		//ocomm_printf(CH_DISP,2,1,"%c %c %c",cpu_card_data.CpuCardPassword[0],cpu_card_data.CpuCardPassword[1],cpu_card_data.CpuCardPassword[2]);
		//ocomm_printf(CH_DISP,2,1,"%c %c %c %c %c %c",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
		//ocomm_printf(CH_DISP,3,1,"%c %c %c %c %c %c",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);
		//ocomm_printf(CH_DISP,4,1,"%c %c %c",cpu_card_data.CpuCardPassword[0],cpu_card_data.CpuCardPassword[1],cpu_card_data.CpuCardPassword[2]);
		//okernel_putevent(KEY_RET_EV,0);
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1);
		return;
	} 
	oapp_iccard_gray_execure();
}
void oapp_iccard_getpsw_execure(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = true;
	cur_input_cursor.p_x   = 2;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_getpsw_execure);	
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);
	okernel_addeventpro(KEY_OK_EV,  oapp_iccard_getpsw_pro);
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"请输入卡密码:"); 
}

void oapp_iccard_readok_pro(void)
{
	unsigned char ret = 0;
	unsigned char buf[3] = {0x00,0x00,0x00};
	OS_DISABLE_KEYSCAN();  //2013-03-11
	ocomm_printf(CH_DISP,1,1,"卡数据校验");	
	ret = oapp_iccard_datachk();
	if(_rd_chk_ok == ret) 
	{
		OS_ENABLE_KEYSCAN();  //2013-03-11
		if(memcmp((unsigned char*)&buf[0],(unsigned char *)&cpu_card_data.CpuCardPassword[0],3) != 0)
			oapp_iccard_getpsw_execure();
		else
			oapp_iccard_gray_execure();
		return;
	}		
	oapp_iccard_tip_msg(4,1,ret-1); ///提示
	OS_ENABLE_KEYSCAN();  //2013-03-11
}

//prog_char _CPUCARD_READ_FILE[5]   = {0x15,0x16};
//prog_char _CPUCARD_STEPS_FILE[5]  = {19,42};
void oapp_iccard_read_pro(void)
{
	//unsigned char files = 0,nstep = 0,ret,i;
	//unsigned char *rdbuf = cpu_card_data.CpuCardNum;
	unsigned char ret;
	OS_DISABLE_KEYSCAN();
	ocomm_printf(CH_DISP,1,1,"读卡......");	
	ret =  Cpucard_Write_Ready();
	if(ret != _wirte_ready_ok)
	{	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN(); 
		return;
	}
	if(!oicard_read(0x15,19,cpu_card_data.CpuCardNum))
	{	
		ocomm_printf(CH_DISP,2,1,"读卡:0x15");	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN(); 
		return;
	}
	if(!oicard_read(0x16,42,cpu_card_data.CpuLastMon))
	{	
		ocomm_printf(CH_DISP,2,1,"读卡:0x16");	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 1, 0, 1);
		OS_ENABLE_KEYSCAN(); 
		return;
	}
	//oprt_Cpufeil_info();
	/*files = pgm_read_byte(&_CPUCARD_READ_FILE[icard_chk_step]);
	nstep      = pgm_read_byte(&_CPUCARD_STEPS_FILE[icard_chk_step]);
	if(!oicard_read(files,nstep,rdbuf))
	{
		ocomm_printf(CH_DISP,1,5,"读卡1:%x %d",files, icard_chk_step);	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1);
		OS_ENABLE_KEYSCAN();
		return;
	}
	rdbuf = cpu_card_data.CpuCardNum + nstep;
	otimer_release(LCNG_COMM_EVENT1_EV);
	icard_chk_step++;
	if(icard_chk_step >= 2 && files == 0x16)
	{
		oprt_Cpufeil_info();
		okernel_putevent(LCNG_COMM_EVENT2_EV,0);
	}
	else
	{
		okernel_putevent(LCNG_COMM_EVENT1_EV,0);
	}*/
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
	OS_ENABLE_KEYSCAN();  //2013-03-11
}

void oapp_iccard_readblocks_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_read_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_readok_pro);	
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);
	icard_chk_step = 0;
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);	

}

void oapp_iccard_select_ex_pro(void)
{
	unsigned char str[4] = {0xAA, 0xFF, 0x40,0x00};
	unsigned char buf[10];
	bool          rdok = false;
	
	memset(buf,0,10);
	if(true == oicard_read(0x15,8,buf))
	{
		if(memcmp((unsigned char*)&buf[0],&cpu_card_data.CpuCardNum[0],8) != 0) 
		{
			ocomm_printf(CH_DISP,2,1,"卡号不匹配");
			ocomm_printf(CH_DISP,3,1,"%c %c %c %c %c %c",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
			ocomm_printf(CH_DISP,4,1,"%c %c %c %c %c %c",cpu_card_data.CpuCardNum[0],cpu_card_data.CpuCardNum[1],cpu_card_data.CpuCardNum[2],cpu_card_data.CpuCardNum[3],cpu_card_data.CpuCardNum[4],cpu_card_data.CpuCardNum[5]);
		}			 
		else
		{
			 rdok = true;
		} 
	}
	if(rdok == false)
	{
		str[3] ^= str[0];
		str[3] ^= str[1];
		str[3] ^= str[2];
		oicard_sendstr(str,4);			
		icard_step_cnt++;
		ocomm_printf(CH_DISP,1,1,"寻卡:%d",icard_step_cnt);
		otimer_reqire(LCNG_COMM_EVENT1_EV, 5, 0, 1); 	//3300ms		
		return;
	}
	otimer_release(LCNG_COMM_EVENT1_EV);
	icard_chk_step = 0;
	okernel_putevent(LCNG_COMM_EVENT2_EV,0);
}
	
void oapp_iccard_update_pro(void)
{
	unsigned char *wtbuf = 0;
	OS_DISABLE_KEYSCAN();
	
	wtbuf = cpu_card_data.CpuLastMon;
	if(!oicard_write(0x16,38,wtbuf,0))
	{
		icard_chk_step++;
		ocomm_printf(CH_DISP,2,1,"err: %d",icard_chk_step);	
		otimer_reqire(LCNG_COMM_EVENT1_EV, 3, 0, 1);
		OS_ENABLE_KEYSCAN();
		return;
	}
	//oprt_Cpufeil_info();
	cur_rec.rec_b_CardOK = false;
	okernel_putevent(KEY_RET_EV,0);
	OS_ENABLE_KEYSCAN();//2013-3-11
}

void oapp_iccard_updateblocks_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_update_pro);	
	okernel_addeventpro(KEY_RET_EV,          olngctrl_stop_disp);	
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_update_execute(void)
{
	icard_step_cnt = 0;
	icard_chk_step = 0;
	oapp_update_card_info();
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"写卡......");	
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_ex_pro);
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_updateblocks_pro);	
	okernel_addeventpro(KEY_RET_EV,          olngctrl_stop_disp);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0); 
}

void oapp_iccard_ready_execute(void)
{
	icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);//寻卡
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_readblocks_pro);	//读卡
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);	
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_read_imfor_disp(void)
{
	unsigned char tmpbuf[16] = {0};
	unsigned long tmp_long = 0;
	olcm_clrscr();
	tmp_long = hexTolong((unsigned char*)&cpu_card_data.CpuCardNum[4],4);  //卡号
	ocomm_printf(CH_DISP, 1, 1,"卡号:%ld",tmp_long);
	memset(tmpbuf,0,16);
	memcpy(tmpbuf,(unsigned char *)&cpu_card_data.CpuBalance[0],4);
	tmp_long = hexTolong((unsigned char*)tmpbuf,4);  
	ocomm_printf(CH_DISP,2,1,"余额:%ld.%02d",tmp_long/100,tmp_long%100);
	memset(tmpbuf,0,16);
	memcpy(tmpbuf,(unsigned char *)&cpu_card_data.CpuRechargeMon[0],4);
	tmp_long = ocomm_bcdbuf2hex(tmpbuf, 4);
	ocomm_printf(CH_DISP,3,1,"充值金额:");
	ocomm_printf(CH_DISP,4,1,"%ld.%02d",tmp_long/100,tmp_long%100);
}

void oapp_iccard_check_readblocks_pro(void)
{
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_read_pro);	
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_read_imfor_disp);	
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);		
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_check_imfor(void)
{
       icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);//寻卡
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, oapp_iccard_check_readblocks_pro);
	okernel_addeventpro(KEY_RET_EV,          oui_main_ui);	
	okernel_putevent(LCNG_COMM_EVENT1_EV,0);
}

void oapp_iccard_write_flag_return(void)
{
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);	
	ocomm_message(4,1,"正常卡恢复",10,NULL);	
}

/*
@写标志
*/
void iccard_write_flag(void)
{
	cpu_card_data.CpuValidity = 0x01;
	OS_DISABLE_KEYSCAN();  //2013-03-11
	if(!oicard_write(0x16,1,(unsigned char*)&cpu_card_data.CpuValidity,21))
	{
		ocomm_printf(CH_DISP,1,1,"置正常卡失败",icard_step_cnt++);		
		otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1); 	//300ms	
		OS_ENABLE_KEYSCAN();  //2013-03-11
		return;
	}
	cur_rec.rec_b_CardOK = false;  //2013-02-26
	otimer_release(LCNG_COMM_EVENT2_EV);
	okernel_putevent(KEY_RET_EV, 0);
	OS_ENABLE_KEYSCAN();  //2013-03-11	
}
/*
@恢复正常卡标志
@
*/
void oapp_iccard_back_write_flag(void)
{
    icard_step_cnt = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, oapp_iccard_select_pro);//寻卡
	okernel_addeventpro(LCNG_COMM_EVENT2_EV, iccard_write_flag);
	okernel_addeventpro(KEY_RET_EV, oapp_iccard_write_flag_return);
	okernel_putevent(LCNG_COMM_EVENT1_EV, 0);
}
#endif

