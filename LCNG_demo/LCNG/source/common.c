/*
 * common.c
 *
 * Created: 2012-8-6 16:21:10
 *  Author: luom
 */ 

#include "global_def.h"

void ocomm_delay_ms(unsigned int ms)
{
	OS_ENTER_CRITICAL();
	while(--ms)
	{
		osys_opower_chk(); //掉电检测 06-20 luom  
		_delay_loop_2(1843);//_delay_ms(1);
	}
	OS_EXIT_CRITICAL();	
}

unsigned int ocomm_crc16_chksum(unsigned char* buf, unsigned char len)
{
	unsigned char n = 0;
	unsigned int  crc16 = 0;
	for(n=0; n<len; n++)
	{
		crc16 += *buf++;
	}
	return crc16;
}

void ocomm_printf(unsigned char ch, unsigned char posx, unsigned posy, char *fmt,...)
{
	OS_ENTER_CRITICAL();
    va_list ap;
    char str[64];
    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
	switch(ch)
	{
		case CH_DISP:
			olcm_wrtstr(posx, posy, str, 1);
			break;
		case CH_UART:
			ouart2_sendstr((unsigned char*)&str[0], strlen(str));
			break;
		case CH_PRINT:
			oprt_sendstrext(str);
			break;		
		default:
			break;			
	}		
    va_end(ap);
	__WATCH_DOG();
	OS_EXIT_CRITICAL();	
}

void ocomm_disp_str(unsigned char posx, unsigned posy, unsigned long dispvalue)
{
        unsigned char dispbuf[20];
	 unsigned long tmp = 0;
	 unsigned char tmp1 = 0;
	 tmp = dispvalue/100;
	 tmp1 = dispvalue%100;
	 memset(dispbuf,0,20);	
        sprintf((char *)dispbuf,"%ld.%d",tmp,tmp1);
	 
	 olcm_wrtstr(posx, posy,(char *)dispbuf, 1);
}


bool ocomm_checkdate(unsigned char * str)
{
    unsigned char DayOfMon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    unsigned char y,m,d;
    y=str[0];
    m=str[1];
    d=str[2];
   
    if (((y%4) == 0 &&(y%100 != 0))||(y%400 == 0)) DayOfMon[1]+=1;//年
    if ((m>12)||(m < 1))  return false;//月
    if (d>DayOfMon[m-1])  return false;//日
    return true;
}
bool ocomm_checktime(unsigned char * str)
{
  if(*str++>23 || *str++>=60 || *str++>=60)
    return false;
  return true;
}

unsigned char ocomm_asc2bcd(unsigned char* asc)
{
  return (asc[0] -0x30)*0x10 + (asc[1]-0x30);
}

void ocomm_bcd2asc(unsigned char* dest,unsigned char bcd)
{
  *dest     = bcd/10 +0x30;
  *(dest+1) = bcd%10 +0x30;
}

char* ltostr (char *str,unsigned long val, unsigned char base) 
{
    ldiv_t r; 
    r = ldiv (labs(val), base);
    if (r.quot > 0)  str = ltostr (str, r.quot, base);
    *str++ = "0123456789abcdefghijklmnopqrstuvwxyz"[(int)r.rem];
    *str   = '\0';
    return str;
}
unsigned long ocomm_hex2bcd(unsigned long sdt)
{
  char buf[12];
  memset(buf,0,sizeof(buf));
  ltostr(buf,sdt, 10);
  return strtoul((char*)&buf[0],NULL,16);
}

unsigned long ocomm_bcd2hex(unsigned long sdt)
{
  unsigned long rd_h = 0;
  unsigned long rd_l = 0;  
  char buf[12];
  memset(buf,0,sizeof(buf));
  rd_h = sdt/0x10000;
  rd_l = sdt%0x10000;
  memset(buf,0,sizeof(buf));
  ltostr(buf, rd_h, 16);
  rd_h = strtoul((char*)&buf[0],NULL,10)*10000;
  memset(buf,0,sizeof(buf));
  ltostr(buf, rd_l, 16);
  return rd_h+strtoul((char*)&buf[0],NULL,10);
}

long  long  ocomm_bcdbuf2hex(unsigned char* buf,unsigned char len)
{
  _union_ulong tmp;
  unsigned char n;
  tmp.dt_ulong = 0;
  long long ull = 0;
  if(len == 2)
  {
    tmp.dt_uc[0] = buf[1];
	tmp.dt_uc[1] = buf[0];
    return ocomm_bcd2hex(tmp.dt_ulong);
  }
  
  if(len == 4)
  {
    for(n=0; n<4; n++) tmp.dt_uc[n] = buf[3-n];
    return ocomm_bcd2hex(tmp.dt_ulong);
  }
  if(len == 6)
  {
	tmp.dt_uc[0] = buf[1];
	tmp.dt_uc[1] = buf[0];    
	ull  = ocomm_bcd2hex(tmp.dt_ulong);
	ull *= 100000000; 
	for(n=0; n<4; n++) tmp.dt_uc[n] = buf[5-n];  
	ull += ocomm_bcd2hex(tmp.dt_ulong);
  }
  return ull;
} 

bool ocomm_cmp2float(double vcmp1, double vcmp2)
{
	char   buf1[12] = {0};
	char   buf2[12] = {0};
	unsigned char len1, len2 = 0;
	sprintf((char*)&buf1[0],"%.02f", vcmp1);
	sprintf((char*)&buf2[0],"%.02f", vcmp2);	
	len1 = strlen(buf1);
	len2 = strlen(buf2);
	len1 = (len1>=len2) ? len1 : len2;
	if(memcmp(buf1,buf2,len1) == 0) return true;
	return false;
}



#if 0
void ocomm_hex2bcdbuf(unsigned long sdt, char* buf, unsigned char len)
{
  unsigned long  tmpul;
  unsigned char  n;
  union un_ex
  {
    unsigned long uldt;
    unsigned char ucdt[4];
  };
  union un_ex  tmp;
  tmpul = ocomm_hex2bcd(sdt);
  tmp.uldt = tmpul;
  for(n=0; n<len; n++) buf[len-1-n] = tmp.ucdt[n];
}
#endif


void ocomm_hex2bcdbuf_ex(unsigned long vdt, char* buf, unsigned char len) //_ex
{
    char str[16]={0};
	unsigned char g,n,m,k,l = 0;	
	if(len == 0) return;
	OS_ENTER_CRITICAL();//cli();
	g = len-1;
	l = 11; //strlen(str);
	m = 6;
	k = 0;
	sprintf(str,"%012ld",vdt);
	for(n=0;n<m;n++)
	{
		buf[g] = ((str[l-1]-'0')<<4)+(str[l]-'0');
		if(g == 0) break;
		l -= 2;
		g -= 1;
	}
	OS_EXIT_CRITICAL();//sei();
}

void ocomm_hex2bcdbuf(unsigned long sdt, char* buf, unsigned char len)
{
    char str[16]={0};
	unsigned char g,n,m,k,l = 0;	
	if(len == 0) return;
	OS_ENTER_CRITICAL();//cli();
	g = len-1;
	l = 11; 
	m = 6;
	k = 0;
	sprintf(str,"%012ld",sdt);
	for(n=0;n<m;n++)
	{
		buf[g] = ((str[l-1]-'0')<<4)+(str[l]-'0');
		if(g == 0) break;
		l -= 2;
		g -= 1;
	}
	OS_EXIT_CRITICAL();//sei();	
}

void ocomm_float2bcdbuf(double fdt, char* buf, unsigned char len)
{
    char str[16]={0};
	unsigned char g,n,m,k,l = 0;	
	if(len == 0) return;
	OS_ENTER_CRITICAL();//cli();
	sprintf(str,"%013.02f",fdt);
	buf[len-1] = ((str[11]-'0')<<4)+(str[12]-'0');
	g = len-2;
	l = 9; 
	m = 5;
	k = 0;
	
	for(n=0;n<m;n++)
	{
		buf[g] = ((str[l-1]-'0')<<4)+(str[l]-'0');
		if(g == 0) break;
		l -= 2;
		g -= 1;
	}
	OS_EXIT_CRITICAL();//sei();	
}

void ocomm_float2asciibuf(double fdt, char* buf, unsigned char len)
{
    char str[16]={0};
	unsigned char g,n = 0;	
	if(len < 3)   return;
	OS_ENTER_CRITICAL();//cli();
	sprintf(str,"%013.02f",fdt);
	buf[len-1] = str[12];
	buf[len-2] = str[11];
	g = len-3;
	for(n=0; n<len-2; n++)
	{
		buf[g] = str[9-n];
		if(g==0) break; 
		g--;
	}
	OS_EXIT_CRITICAL();//sei();	
}

void ocomm_hex2asciibuf(unsigned long fdt,char *buf,unsigned char len)
{
         char str[16]={0};
	unsigned char n = 0;	
	if(len < 3)   return;
	OS_ENTER_CRITICAL();//cli();
	memset(str,0,16);
	sprintf(str,"%012ld",fdt);
	for(n = 0; n < len; n ++)
	{
	     buf[len -1-n] = str[11-n];
	}
	OS_EXIT_CRITICAL();//sei();	
}

prog_char  _LCNG_CTRLEND_TIPS[][16]={
	"30Mpa 停机",
	"24.5Mpa 停机",
	"流量计异常",
	"IC卡异常",
	"正常停机",
	"RAM 数据错",
};

void ocomm_otip_ctrlend_msg(unsigned char xpos, unsigned char ypos, unsigned char id)
{
   //ocomm_printf(CH_DISP,xpos, ypos, &_LCNG_CTRLEND_TIPS[id][0], 1);    
   olcm_dispprogconst(xpos, ypos, &_LCNG_CTRLEND_TIPS[id][0], 1);
}
/*
bool ocomm_input_password(void (*cmd_func)(void))
{
	ocomm_key_input("请输入密码",1,2,1,false,true,pswlen,cmd_func);
}
*/
void ocomm_msg_ret_event(void)
{
	otimer_release(LCNG_COMM_EVENT1_EV); 
	okernel_putevent(KEY_RET_EV,0); 
}
Task_Event  _OCOMM_TIPS_MSG_Pro[]  = 
{
  {LCNG_COMM_EVENT1_EV,			ocomm_msg_ret_event}, 	
};
void ocomm_message(unsigned char x, unsigned char y, char* str, int dlyms, void (*cmd_func)(void))
{
	okernel_seteventpro(_OCOMM_TIPS_MSG_Pro, sizeof(_OCOMM_TIPS_MSG_Pro)/sizeof(_OCOMM_TIPS_MSG_Pro[0]));	
	if(cmd_func !=NULL){
		 okernel_modifyeventpro(_OCOMM_TIPS_MSG_Pro,0,cmd_func);
	}
	if(dlyms > 0)
	{
		otimer_reqire(LCNG_COMM_EVENT1_EV, dlyms, 0, 1); 	//dlyms*400ms
		ocomm_printf(CH_DISP,x, y, "%s",str);	
	}
	else
	{
		okernel_putevent(KEY_RET_EV,0); 				 
	}	 
}
