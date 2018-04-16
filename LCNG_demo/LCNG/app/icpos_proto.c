/*
@中海油卡POS协议
@zhouliang
*/
#include "global_def.h"

/*
@响应正量模块命令
@SendToIcpos
*/
void SendToIcpos(unsigned char *Senddata,unsigned char len)
{
  unsigned char bcdlen;
  unsigned int crcvalue;
  unsigned char i;
  bcdlen = (unsigned char)ocomm_hex2bcd((unsigned long)len);
  
  
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

/*
@中海油正星模块
@命令0x01--请求加气
@Icpos_StartWork
*/
void Icpos_StartWork(void)
{
   unsigned char CmdOne_buf[5];
   CmdOne_buf[0]=0x01;
   CmdOne_buf[1]=0x77;
   CmdOne_buf[2]=0x88;
   CmdOne_buf[3]=0x99;
   	
}

