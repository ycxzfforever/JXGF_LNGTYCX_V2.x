/*
 * iic.c
 *
 * Created: 2012-8-6 16:31:58
 *  Author: luom
 */ 

#include "global_def.h"

static unsigned char twst;

void oiic_init(void)
{
  DDRD  |= 0x03;
  PORTD |= 0x03; 
  //TWBR = 0x20;					//位速率（CLKcpu/(16+2TWBR*(4的TWPS方))）
  TWBR = 0x40;
  EEPROM_WP_OUT();
}

//signed char oiic_read_bytes(unsigned char eeaddr,signed char len, unsigned char *buf)//20110812
signed char oiic_read_bytes(unsigned char DVaddr,unsigned char eeaddr,signed char len, unsigned char *buf)
{
  unsigned char sla, twcr, n = 0;
  int   rv = 0; 
  sla = DVaddr;//20110812 TWI_SLA_DS;//0xD0
  
restart:
  if (n++ >= MAX_ITER) return -1;//20
  
begin:
  TWI_ACT_START();//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
  switch ((twst = TW_STATUS))//(TWSR & 0xF8)
  {
    case TW_REP_START://0x10
    case TW_START:break;//0x08
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:return -1;
  }
  /* send SLA+W */
  TWDR = sla | TW_WRITE;//0
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
  switch ((twst = TW_STATUS))//(TWSR & 0xF8)
  {
    case TW_MT_SLA_ACK:break;//0x18
    case TW_MT_SLA_NACK:goto restart;//0x20
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;	
  }
  TWDR = eeaddr;//地址	
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
  switch ((twst = TW_STATUS))//(TWSR & 0xF8)
  {
    case TW_MT_DATA_ACK:break;//0x28
    case TW_MT_DATA_NACK:goto quit;//0x30
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;  
  }
  /*
   * Next cycle(s): master receiver mode
   */
  TWI_ACT_START();//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
  switch ((twst = TW_STATUS))//(TWSR & 0xF8)
  {
    case TW_REP_START://0x10
    case TW_START:break;//0x08                                                  /* OK, but should not happen */
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;
  }
  /* send SLA+R */
  TWDR = sla | TW_READ;//1
  TWCR = (1<<TWINT) | (1<<TWEN);                                                /* clear interrupt to start transmission */
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)
  switch ((twst = TW_STATUS))//(TWSR & 0xF8)
  {
    case TW_MR_SLA_ACK:break;//0x40
    case TW_MR_SLA_NACK:goto quit;//0x48
    case TW_MR_ARB_LOST:goto begin;//0x38
    default:goto error;
  }
  for (twcr = (1<<TWINT) | (1<<TWEN) | (1<<TWEA) ;len > 0;len--)  
  {
      if (len == 1) twcr = (1<<TWINT) | (1<<TWEN);
      TWCR = twcr;		
      while ((TWCR & (1<<TWINT)) == 0) ;
      switch ((twst = TW_STATUS))
      {
	case TW_MR_DATA_NACK:len = 0;
	case TW_MR_DATA_ACK:*buf++ = TWDR;
                            rv++;
                            break;
	default:goto error;  
      }
  }
quit:
  TWI_ACT_STOP();//TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
  return rv;
error:
  rv = -1;
  goto quit;
}

//signed char oiic_write_bytes(unsigned char eeaddr, signed char len, unsigned char *buf)
signed char oiic_write_bytes(unsigned char DVaddr,unsigned char eeaddr, signed char len, unsigned char *buf)
{
  unsigned char sla, n = 0;
  int   rv = 0;
  sla = DVaddr;//TWI_SLA_DS;

restart:
  if (n++ >= MAX_ITER) return -1;
begin:
  TWI_ACT_START();
  TWI_ACT_WAIT();
  switch ((twst = TW_STATUS))
  {
    case TW_REP_START:                                                          /* OK, but should not happen */
    case TW_START:break;
    case TW_MT_ARB_LOST:goto begin;
    default:return -1; 
  }
  /* send SLA+W */
  TWDR = sla | TW_WRITE;
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();
  switch ((twst = TW_STATUS))
  {
    case TW_MT_SLA_ACK:break;  
    case TW_MT_SLA_NACK:goto restart;
    case TW_MT_ARB_LOST:goto begin;
    default:goto error; 	
  }
  TWDR = eeaddr;
  TWCR = (1<<TWINT) | (1<<TWEN);                                                /* clear interrupt to start transmission */
  TWI_ACT_WAIT();
  switch ((twst = TW_STATUS))
  {
    case TW_MT_DATA_ACK:break;
    case TW_MT_DATA_NACK:goto quit; 
    case TW_MT_ARB_LOST:goto begin;
    default:goto error;	
  }
  for (; len > 0; len--)
  {
      TWDR = *buf++;
      TWCR = (1<<TWINT) | (1<<TWEN);
      while ((TWCR & (1<<TWINT)) == 0) ;
      switch ((twst = TW_STATUS))
      {
	case TW_MT_DATA_NACK:goto error;	
	case TW_MT_DATA_ACK:rv++;break;
	default:goto error;
      }
  }
quit:
  TWI_ACT_STOP();
  return rv;
error:
  rv = -1;
  goto quit;
}

bool ds3231_set_time(unsigned char *buf)
{
  unsigned char timebuf[3];
  if(!ocomm_checktime(buf)) return false;
  timebuf[0] = ocomm_hex2bcd(buf[2]);
  timebuf[1] = ocomm_hex2bcd(buf[1]);
  timebuf[2] = ocomm_hex2bcd(buf[0]);  
  if(oiic_write_bytes(TWI_SLA_DS_RT,0x00, 3, timebuf) != 3) return false; 
  return true;
}

bool ds3231_set_date(unsigned char *buf)
{
  unsigned char datebuf[3];
  if(!ocomm_checkdate(buf)) return false;
  datebuf[0] = ocomm_hex2bcd(buf[2]);
  datebuf[1] = ocomm_hex2bcd(buf[1]) & 0x7F;
  datebuf[2] = ocomm_hex2bcd(buf[0]);  
  if(oiic_write_bytes(TWI_SLA_DS_RT,0x04, 3, datebuf) != 3) return false; 
  return true;
}

bool ds3231_read_time(unsigned char *buf, bool bhex)
{
  unsigned char timebuf[3];
  OS_ENTER_CRITICAL();
  if(oiic_read_bytes(TWI_SLA_DS_RT,0x00, 3, timebuf) != 3) // return false; //if(oiic_read_bytes(0x00, 3, timebuf) != 3)  return false; 
  {
	  OS_EXIT_CRITICAL();
	  return false;
  }  
  if(bhex)
  {  
	*buf++ = ocomm_bcd2hex(timebuf[2]);
	*buf++ = ocomm_bcd2hex(timebuf[1]);
	*buf   = ocomm_bcd2hex(timebuf[0]);
  }
  else
  {
	*buf++ = timebuf[2];
	*buf++ = timebuf[1];
	*buf   = timebuf[0];	  
  }   		
  OS_EXIT_CRITICAL();
  return true;
}

bool ds3231_read_date(unsigned char *buf, bool bhex)
{
  unsigned char datebuf[3];
  OS_ENTER_CRITICAL();
  if(oiic_read_bytes(TWI_SLA_DS_RT,0x04, 3, datebuf) != 3) // return false;//if(oiic_read_bytes(0x04, 3, datebuf) != 3)  return false; 
  {
	  OS_EXIT_CRITICAL();
	  return false;
  }
  if(bhex)
  {
	*buf++ = ocomm_bcd2hex(datebuf[2]);
	*buf++ = ocomm_bcd2hex(datebuf[1]);
	*buf   = ocomm_bcd2hex(datebuf[0]);  
  }
  else
  {
	*buf++ = datebuf[2];
	*buf++ = datebuf[1];
	*buf   = datebuf[0];  
  }
  OS_EXIT_CRITICAL();
  return true;
}
#if 0
bool ds3231_hardware_chk(void)
{
    unsigned char buf[6] = {0,0};
		
    if(oiic_read_bytes(TWI_SLA_DS_RT,0x0E, 2, &buf[0]) != 2) 
    {
      olcm_dispprintf(1,1, "读时钟错");
      return false;
    }
    if(buf[0] != 28 || (buf[1]&0xF8) != 136)
    {  
      olcm_dispprintf(1,1, "时钟REG:%d %d", buf[0], buf[1]);
      return false;
    } 
	
	ds3231_read_date((unsigned char*)&cur_time[0],true);
	ds3231_read_time((unsigned char*)&cur_time[3],true);
	//上次关机时间 检测
	eeprom_read_block((unsigned char*)&buf[0] , LAST_END_TIME_ADDR ,LAST_END_TIME_LEN);//ee24cxxx_read_bytes(SLA_W_24CXXX,LAST_END_TIME  ,6,&buf[0]);
	/*
	olcm_clrscr();
	olcm_dispprintf(1,1, "上次关机时间");
	olcm_dispprintf(2,1,"%02d-%02d %02d %02d %02d",buf[1],buf[2],buf[3],buf[4],buf[5]);  
	ocomm_delay_ms(3000);*/
	/////////////////////////////////
	if(cur_time[0] < buf[0])
    {
      return false;
    }
	else
	{
		if(cur_time[1] < buf[1])
		{
		  return false;
		}
		else if(cur_time[2] < buf[2])
		{
		  return false;
		}
		
	}
    return true;    
}
#endif

///////////////////////////外扩EEPROM////////////////////
int ee24cxxx_read_bytes(unsigned char Dvaddr,unsigned int addr, int len, unsigned char *buf)
{   
  unsigned char sla, twcr, n = 0;
  int   rv = 0; 
  sla = Dvaddr;
  
restart:
  if (n++ >= MAX_ITER) return -1;

begin:
  TWI_ACT_START();
  TWI_ACT_WAIT(); 

  switch (TW_STATUS)
  {   
    case TW_REP_START://0x10
    case TW_START:break;//0x08
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:return -1;
  }
  /* send SLA+W */
  TWDR = sla | TW_WRITE;//0
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();

  switch (TW_STATUS)
  { 
    case TW_MT_SLA_ACK:break;//0x18
    case TW_MT_SLA_NACK:goto restart;//0x20
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;	
  }
  
  TWDR = addr>>8;
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

  switch (TW_STATUS)//(TWSR & 0xF8)
  {      
    case TW_MT_DATA_ACK:break;//0x28
    case TW_MT_DATA_NACK:goto quit;//0x30
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;  
  }
  
  TWDR = addr&0x00ff;
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();

  switch (TW_STATUS)//(TWSR & 0xF8)
  {      
    case TW_MT_DATA_ACK:break;//0x28
    case TW_MT_DATA_NACK:goto quit;//0x30
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;  
  }
  /*
   * Next cycle(s): master receiver mode
   */
  TWI_ACT_START();//TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

  switch (TW_STATUS)//(TWSR & 0xF8)
  { 
    case TW_REP_START://0x10
    case TW_START:break;//0x08                                                  /* OK, but should not happen */
    case TW_MT_ARB_LOST:goto begin;//0x38
    default:goto error;
  }
  /* send SLA+R */
  TWDR = sla | TW_READ;//1
  TWCR = (1<<TWINT) | (1<<TWEN);                                                /* clear interrupt to start transmission */
  TWI_ACT_WAIT();//while ((TWCR & (1<<TWINT)) == 0)

  switch (TW_STATUS)//(TWSR & 0xF8)
  {      
    case TW_MR_SLA_ACK:break;//0x40
    case TW_MR_SLA_NACK:goto quit;//0x48
    case TW_MR_ARB_LOST:goto begin;//0x38
    default:goto error;
  }
  for (twcr = (1<<TWINT) | (1<<TWEN) | (1<<TWEA) ;len > 0;len--)  
  {
      if (len == 1) twcr = (1<<TWINT) | (1<<TWEN);
      TWCR = twcr;		
      while ((TWCR & (1<<TWINT)) == 0) ;
 
      switch (TW_STATUS)
      { 	
        case TW_MR_DATA_NACK:len = 0;//0x58
	   case TW_MR_DATA_ACK:
							*buf++ = TWDR;//0x50
                            rv++;
                            break;
		default:goto error;  
      }
  }
quit:
  TWI_ACT_STOP();//TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
  return rv;
error:
  rv = -1;
  goto quit;
}


int ee24cxxx_write_bytes(unsigned char Dvaddr,unsigned int addr, int len, unsigned char *buf)
{
  unsigned char sla, n = 0;
  int   rv = 0;
  sla = Dvaddr;
  
restart:
  if (n++ >= MAX_ITER) return -1;
begin:
  TWI_ACT_START();
  TWI_ACT_WAIT();

  switch (TW_STATUS)
  {
    case TW_REP_START:                                                          /* OK, but should not happen */
    case TW_START:break;
    case TW_MT_ARB_LOST:goto begin;
    default:return -1; 
  }
  /* send SLA+W */
  TWDR = sla | TW_WRITE;
  TWCR = (1<<TWINT) | (1<<TWEN);
  TWI_ACT_WAIT();
 
  switch (TW_STATUS)
  {
    case TW_MT_SLA_ACK:break;  
    case TW_MT_SLA_NACK:goto restart;
    case TW_MT_ARB_LOST:goto begin;
    default:goto error; 	
  }
  TWDR = addr>>8;
  TWCR = (1<<TWINT) | (1<<TWEN);                                                /* clear interrupt to start transmission */
  TWI_ACT_WAIT();

  switch (TW_STATUS)
  {
    case TW_MT_DATA_ACK:break;
    case TW_MT_DATA_NACK:goto quit; 
    case TW_MT_ARB_LOST:goto begin;
    default:goto error;	
  }
  
  TWDR = addr&0x00ff;
  TWCR = (1<<TWINT) | (1<<TWEN);                                                /* clear interrupt to start transmission */
  TWI_ACT_WAIT();

  switch (TW_STATUS)
  {
    case TW_MT_DATA_ACK:break;
    case TW_MT_DATA_NACK:goto quit; 
    case TW_MT_ARB_LOST:goto begin;
    default:goto error;	
  }
  
  for (; len > 0; len--)
  {
      TWDR = *buf++;
      TWCR = (1<<TWINT) | (1<<TWEN);
      while ((TWCR & (1<<TWINT)) == 0) ;
 
      switch (TW_STATUS)
      {
	case TW_MT_DATA_NACK:goto error;	
	case TW_MT_DATA_ACK:rv++;break;
	default:goto error;
      }
  }
quit:
  TWI_ACT_STOP();
  //_delay_loop_2(9216);//_delay_ms(5); //ocomm_delay_ms(5);//
  _delay_loop_2(12000);//_delay_ms(5); //ocomm_delay_ms(5);//
  return rv;
error:
  rv = -1;
  goto quit;
}



void ee24lc_ex_crc(unsigned int e_addr)
{
	unsigned int crc = 0;
	unsigned int addr = 0;
	unsigned char crc_hi, crc_low;
	unsigned char read_crc_hi,read_crc_low;
	unsigned char timeout =5;
	if((EX_EEPROM_SYSPARAS_ADDR <= e_addr) && (e_addr <= EX_EEPROM_SYSPARAS_ADDR+sizeof(struct def_SysParas)) )		
	{
		crc = ocomm_crc16_chksum((unsigned char*)&cur_sysparas.f_price[0],  sizeof(struct def_SysParas)-2);
		addr = EX_EEPROM_SYSPARAS_ADDR+offsetof(Sysparas,paras_crc16_hi);
	}
	if((EX_EEPROM_SYSINFO_ADDR <= e_addr) && (e_addr <= EX_EEPROM_SYSINFO_ADDR+sizeof(struct def_SysInfo)))		
	{
		crc = ocomm_crc16_chksum((unsigned char*)&cur_sysinfo.g_uid,  sizeof(struct def_SysInfo)-2);
		addr = EX_EEPROM_SYSINFO_ADDR+offsetof(Sysinfo,info_crc16_hi);
	}
	if(addr != 0)
	{
		while(timeout--)
		{
			crc_hi  = crc/0x100;
			crc_low = crc%0x100; 
			cur_sysparas.paras_crc16_hi = crc_hi;
			cur_sysparas.paras_crc16_low = crc_low;
			ee24cxxx_write_bytes(SLA_W_24CXXX, addr,  1,(unsigned char*)&crc_hi);
			ee24cxxx_write_bytes(SLA_W_24CXXX, addr+1,1,(unsigned char*)&crc_low);

			ee24cxxx_read_bytes(SLA_W_24CXXX, addr,  1,(unsigned char*)&read_crc_hi);
			ee24cxxx_read_bytes(SLA_W_24CXXX, addr+1,  1,(unsigned char*)&read_crc_low);
			if((memcmp((unsigned char*)&crc_hi,(unsigned char*)&read_crc_hi,1) == 0)&&(memcmp((unsigned char*)&crc_low,(unsigned char*)&read_crc_low,1) == 0))
				timeout = 0;
		}
	}		
}

//bool ee24lc_write(unsigned char Dvaddr,unsigned int addr, int len, unsigned char *buf)
bool ee24lc_write(unsigned int addr, int len, unsigned char *buf)
{
	unsigned char  cnt, nblocks, n  = 0;
	unsigned char rdbuf[64];
	unsigned int timeout = 5;
	OS_ENTER_CRITICAL();
	OS_DISABLE_KEYSCAN();
	nblocks = len/64;
	cnt    = len%64;
	for(n = 0; n < nblocks; n++)
	{
		__WATCH_DOG();
		timeout = 5;
		while(timeout--)
		{
			if( ee24cxxx_write_bytes(SLA_W_24CXXX,addr,64,&buf[n*64]) != 64)
			{
				OS_ENABLE_KEYSCAN();
				OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
			    	return false;
			}		
			////0901wen add
			if( ee24cxxx_read_bytes(SLA_W_24CXXX,addr,64,&rdbuf[0]) != 64)
			{
				OS_ENABLE_KEYSCAN();
				OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
			    	return false;
			}
			if(memcmp((unsigned char*)&rdbuf[0],(unsigned char*)&buf[n*64],64) == 0)
			{
				timeout = 0;
				addr += 64;
			}
			/////add end
		}
	}
	for(n = 0; n < cnt; n++)
	{
		__WATCH_DOG();
		timeout = 5;
		while(timeout--)
		{
			if( ee24cxxx_write_bytes(SLA_W_24CXXX,addr,cnt,&buf[nblocks*64]) != cnt)
			{
				OS_ENABLE_KEYSCAN();
				OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
			    return false;
			}
			////0901wen add
			if( ee24cxxx_read_bytes(SLA_W_24CXXX,addr,cnt,(unsigned char*)&rdbuf[0]) != cnt)
			{
				OS_ENABLE_KEYSCAN();
				OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
			    	return false;
			}
			if(memcmp((unsigned char*)&rdbuf[0],(unsigned char*)&buf[nblocks*64],cnt) == 0)
				timeout = 0;
			/////add end
		}
	}
	//////crc
	ee24lc_ex_crc(addr);	
	
	OS_ENABLE_KEYSCAN();
	OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
	return true;
}

bool ee24lc_read(unsigned int addr, int len, unsigned char *buf)
{
	unsigned char cnt, nblocks, n  = 0;
	OS_ENTER_CRITICAL();//cli();//OS_ENTER_CRITICAL();
	OS_DISABLE_KEYSCAN();	
	nblocks = len/64;
	cnt     = len%64;
	for(n = 0; n < nblocks; n++)
	{
		__WATCH_DOG();
		if( ee24cxxx_read_bytes(SLA_W_24CXXX,addr,64,&buf[n*64]) != 64)
		{
			OS_ENABLE_KEYSCAN();	
			OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
		    return false;
		}		 
		addr += 64;
	}
	for(n = 0; n < cnt; n++)
	{
		__WATCH_DOG();
		if( ee24cxxx_read_bytes(SLA_W_24CXXX,addr,cnt,&buf[nblocks*64]) != cnt)
		{
			OS_ENABLE_KEYSCAN();
			OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
		    return false;
		}		 
		
	}
	OS_ENABLE_KEYSCAN();
	OS_EXIT_CRITICAL();//sei();//OS_EXIT_CRITICAL();
	return true;
}