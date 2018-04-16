/*
 * modbus.c
 *
 * Created: 2012-8-13 15:09:40
 *  Author: luom
 */ 
#include "global_def.h"
/*************************************************************
器件地址
  RM:  
		250  温度
		248  密度
		290  增益
  E+H:	
		PRESSURE  5185 
*************************************************************/
void omodbus_init(unsigned int baudrate)
{
  unsigned int UBRR = ((F_CPU>>4)/baudrate -1); 
  //气相(对应CNG加气机)
  SET_PORT_BIT(DDRB, DDB5);						//DDRB |= 0x20;//DDRB |= 0x40;  			//用于设置485收发控制引脚  PB.5
  UCSR3B = 0x00; 								//disable while setting baud rate
  UBRR3H = (unsigned char) ((UBRR >>8) &0xFF);	//0x00;
  UBRR3L = (unsigned char) (UBRR & 0xFF);		//23;   			//23--19200, 47--9600|(1<<USBS0)
  UCSR3B |= 1<<TXEN3;							//UCSR3B_TXEN3 = 1;//_BV(TXEN3) | _BV(RXEN3);// | _BV(RXCIE3); //| _BV(TXCIE3);    //UCSR3B_TXEN3 = 1;// 发送接收使能，使用中断方式接收数据
  UCSR3B |= 1<<RXEN3;							//UCSR3B_RXEN3 = 1;
  UCSR3C = (1 << UCSZ30) | (1 << UCSZ31) | (1 << UPM31); 				//0x06 8位数据，1位停止位，偶校验
  _delay_loop_2(18432);//_delay_ms(10);
  //液相	
  SET_PORT_BIT(DDRB, DDB6);						//DDRB |= 0x40;  			//用于设置485收发控制引脚  PB.6
  UCSR1B = 0x00; 								//disable while setting baud rate
  UBRR1H = (unsigned char) ((UBRR >>8) &0xFF); //0x00;
  UBRR1L = (unsigned char) (UBRR & 0xFF);      //23;   			//23--19200, 47--9600|(1<<USBS0)
  UCSR1B |= 1<<TXEN1;							//UCSR3B_TXEN3 = 1;//_BV(TXEN3) | _BV(RXEN3);// | _BV(RXCIE3); //| _BV(TXCIE3);    //UCSR3B_TXEN3 = 1;// 发送接收使能，使用中断方式接收数据
  UCSR1B |= 1<<RXEN1;							//UCSR3B_RXEN3 = 1;
  UCSR1C = (1 << UCSZ10) | (1 << UCSZ11) | (1 << UPM11); 				//0x06 8位数据，1位停止位，偶校验
  _delay_loop_2(18432);//_delay_ms(10);
  MODBUS_G_CLR_RXIRPT(); 
  MODBUS_L_CLR_RXIRPT(); 
}

unsigned int omodbus_crc16(unsigned char *buf, unsigned int Len)
{
  unsigned int IX, IY, CRC;
  CRC = 0xFFFF; 
  if (Len <= 0)    return 0;
  for (IX = 0; IX < Len; IX++)
  {
      CRC = CRC ^ (unsigned int)(buf[IX]);
      for (IY = 0; IY < 8; IY++)
      {
        if ((CRC &1) != 0)
          CRC = (CRC >> 1) ^ 0xA001;
        else
          CRC = CRC >> 1;
      }
  }
  return CRC;
}

void omodbus_g_sendchar(unsigned char c)
{
   	while (!(UCSR3A &(1 << UDRE3)))   ;
   	UDR3 = c;
	while (!(UCSR3A &(1 << UDRE3)))   ;   
}
void omodbus_g_sendstr(unsigned char len, unsigned char *buf)
{
  unsigned int n;
  SET_PORT_BIT(PORTB, PB5);
  __delay_cycles(5);  
  for (n = 0; n < len; n++)
    omodbus_g_sendchar(*buf++);
  for (n = 0; n < 1000; n++) asm("nop"); //600 _delay_ms(1);  //__delay_cycles(100); 
  CLR_PORT_BIT(PORTB, PB5);
  __delay_cycles(5);  
}

void omodbus_l_sendchar(unsigned char c)
{
	while (!(UCSR1A &(1 << UDRE1)))   ;
	UDR1 = c;
	while (!(UCSR1A &(1 << UDRE1)))   ;
}
void omodbus_l_sendstr(unsigned char len, unsigned char *buf)
{
  unsigned int n;
  SET_PORT_BIT(PORTB, PB6); //L
  __delay_cycles(5);  
  for (n = 0; n < len; n++) omodbus_l_sendchar(*buf++);
  for (n = 0; n < 1000; n++) asm("nop"); //600
  CLR_PORT_BIT(PORTB, PB6);
  __delay_cycles(5);  
}
// ,unsigned char *vfrd ,unsigned int timeout
unsigned char omodbus_read_reg(unsigned char sw ,unsigned int reg_id, unsigned char regnum )
{
  unsigned char _modbus_buf[10]    = {0};	
  unsigned char addr     = 0;
  unsigned char regcoil  = 0;
  unsigned char ret_len = 0;
  unsigned char ret = 0;
  unsigned int  crc = 0;
  int			timeout = MODBUS_TIMEOUT_MAX*2;
  if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM){
		addr = 1;
		regcoil = 0x03;//0x03;
  }		
  else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH){
  	    addr = 247;
		regcoil = 0x03;
  }	  
  else
  {
	 return MODBUS_ERR;
  } 

  _modbus_buf[0] = addr ; //247;           //DEFAULT_ADDR;     //247
  _modbus_buf[1] = regcoil ; //0x03; 		  //写寄存器的功能代码
  
  _modbus_buf[2] = reg_id >> 8;     //寄存器高位
  _modbus_buf[3] = reg_id & 0xFF;   //寄存器低位
  
  _modbus_buf[4] = 0;                //number of registers
  _modbus_buf[5] = regnum;		   //2;//regnum; 

  crc = omodbus_crc16(_modbus_buf, 6);//FLOW_CRC16//进行CRC校验
	 
  _modbus_buf[6] = (unsigned char)(crc &0x00FF);//校验值低位
  _modbus_buf[7] = crc >>8;	
  ret_len =  3+2*regnum+2;	

  
  _modbus_response_g_len = 0;
  _modbus_response_l_len = 0;
  memset((unsigned char*)_modbus_g_buf,0,sizeof(_modbus_g_buf));
  memset((unsigned char*)_modbus_l_buf,0,sizeof(_modbus_l_buf));  
  
  if( _MODBUS_G_ == sw)
  {
	  //_modbus_response_g_len = 0;
	  MODBUS_G_SET_RXIRPT(); 	  
      omodbus_g_sendstr(8, &_modbus_buf[0]);   	
  }
  else if( _MODBUS_L_ == sw)
  {
	  //_modbus_response_l_len = 0;
	  MODBUS_L_SET_RXIRPT(); 	  
      omodbus_l_sendstr(8, &_modbus_buf[0]);   	
  }
  else
  {
	 return MODBUS_ERR;
  } 
  //_delay_ms(10);
  while(timeout--)
  {
	  osys_opower_chk(); //掉电检测
	 _delay_loop_2(1843);//_delay_ms(1);
	 if(_MODBUS_G_ == sw && _modbus_response_g_len >= ret_len) 
	 {
			if(_modbus_g_buf[0] != addr || _modbus_g_buf[1] != regcoil) 
			{
				ret = MODBUS_ERR;
				break;	
			}
			crc =omodbus_crc16((unsigned char*)&_modbus_g_buf[0],ret_len-2);	
			if(_modbus_g_buf[ret_len-2] != (unsigned char)(crc &0x00FF) || _modbus_g_buf[ret_len-1] != crc >>8 )
			{
				ret = MODBUS_ERR_CRC;
				break;
			}
			ret = MODBUS_OK;
			break;		 
	 }	
	 else if(_MODBUS_L_ == sw && _modbus_response_l_len >= ret_len) 
	 {
			if(_modbus_l_buf[0] != addr || _modbus_l_buf[1] != regcoil) 
			{
				ret = MODBUS_ERR;
				break;	
			}
			crc =omodbus_crc16((unsigned char*)&_modbus_l_buf[0],ret_len-2);	
			if(_modbus_l_buf[ret_len-2] != (unsigned char)(crc &0x00FF) || _modbus_l_buf[ret_len-1] != crc >>8 )
			{
				ret = MODBUS_ERR_CRC;
				break;
			}
			ret = MODBUS_OK;
			break;		 
	 }		 	 
  }
  if(_MODBUS_G_ == sw)		   { MODBUS_G_CLR_RXIRPT(); }
  else if(_MODBUS_L_ == sw)    { MODBUS_L_CLR_RXIRPT(); }
  if(timeout <= 0)				return MODBUS_ERR_TIMEOUT;
  return ret;
} 

/**
 ** 液相流量计通信模块测试:
 ** 串口给主板发送八个字节，如果首字节为0xaa，主板返回该帧数据；
 ** 如果首字节为0xbb，结束测试。-20180108yck
 **/
void omodbus_l_test(void)
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"液相流量计测试");  
	MODBUS_L_SET_RXIRPT(); 
	while(1)
	{	__WATCH_DOG();
		_delay_ms(500);
		if(_modbus_l_buf[0]==0xaa)
		{
			omodbus_l_sendstr(8, &_modbus_l_buf[0]);
		}
		else if(_modbus_l_buf[0]==0xbb)
		{
			_modbus_l_buf[0]=0;
			break;
		}
		_modbus_l_buf[0]=0;
		_modbus_response_l_len = 0;
	}
	MODBUS_L_CLR_RXIRPT();
	oui_main_ui();	
}

/**
 ** 气相流量计通信模块测试:
 ** 串口给主板发送八个字节，如果首字节为0xaa，主板返回该帧数据；
 ** 如果首字节为0xbb，结束测试。-20180108yck
 **/
void omodbus_g_test(void)
{
	olcm_clrscr();
 	ocomm_printf(CH_DISP,1,1,"气相流量计测试");  
	MODBUS_G_SET_RXIRPT(); 
	while(1)
	{   __WATCH_DOG();
	    _delay_ms(500);
		if(_modbus_g_buf[0]==0xaa)
		{
			omodbus_g_sendstr(8, &_modbus_g_buf[0]);
		}
		else if(_modbus_g_buf[0]==0xbb)
		{
			_modbus_g_buf[0]=0;
			break;
		}
		_modbus_g_buf[0]=0;
		_modbus_response_g_len = 0;
	}
	MODBUS_G_CLR_RXIRPT();
	oui_main_ui();	
}



void omodbus_write_reg_float(unsigned char sw , unsigned char modbus_addr, unsigned char modbus_coil, unsigned int reg_id, float v_value)
{
//	unsigned char _modbus_buf[14]    = {0xF7,0x10,0x14,0x11,0x00, 0x02,0x04,0x00,0x00,0x40,0xD0,0xE1,0xB8};	
   _union_float  unvfrd;
  unsigned char _modbus_buf[13]    = {0};	
  unsigned int   crc = 0;
  unvfrd.dt_float  = v_value;
  _modbus_buf[0] = modbus_addr;
  _modbus_buf[1] = modbus_coil;
  _modbus_buf[2] = reg_id >> 8;     //寄存器高位
  _modbus_buf[3] = reg_id & 0xFF;   //寄存器低位
  _modbus_buf[4] = 0;
  _modbus_buf[5] = 2;
  _modbus_buf[6] = 4;
  _modbus_buf[7] = unvfrd.dt_uc[1] ;
  _modbus_buf[8] = unvfrd.dt_uc[0] ;
  _modbus_buf[9] = unvfrd.dt_uc[3] ;
  _modbus_buf[10] = unvfrd.dt_uc[2] ;

  crc = omodbus_crc16(_modbus_buf, 11);//FLOW_CRC16//进行CRC校验
	 
  _modbus_buf[11] = (unsigned char)(crc &0x00FF);//校验值低位
  _modbus_buf[12] = crc >>8; 
  /*
return: F7   06    08 13   00 01   AF 39 
       addr  code   reg     val    crc
PC_SendStr(CNGMASS_TRANS,index);
  */  

  
  _modbus_response_g_len = 0;
  _modbus_response_l_len = 0;
  memset((unsigned char*)_modbus_g_buf,0,sizeof(_modbus_g_buf));
  memset((unsigned char*)_modbus_l_buf,0,sizeof(_modbus_l_buf));
  if( _MODBUS_G_ == sw)
  {
	  MODBUS_G_SET_RXIRPT(); 	  
      omodbus_g_sendstr(13, &_modbus_buf[0]); 
	  return;  	
  }
  if( _MODBUS_L_ == sw)
  {
	  MODBUS_L_SET_RXIRPT(); 	  
      omodbus_l_sendstr(13, &_modbus_buf[0]);   	
  }
} 

unsigned char omodbus_write_reg_vavle_float(unsigned char sw ,unsigned int reg_id, float v_value)
{
  unsigned char  addr     = 0;
  unsigned char  regcoil  = 0;
  unsigned char  ret = 0;
  unsigned int    crc = 0;
  int            timeout = 0;
  if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM){
		addr = 1;
		regcoil = 0x05;
  }		
  else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH){
  	    addr = 247;
		regcoil = 0x10;//0x06;
  }	  
  else
  {
	 return MODBUS_ERR;
  } 

	
	omodbus_write_reg_float(sw,addr, regcoil,reg_id,v_value);
	timeout = MODBUS_TIMEOUT_MAX*2;
	while(timeout--)
	{
		osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);	_modbus_response_g_len >= 10 ||	
		if(_modbus_response_g_len >= 8 || _modbus_response_l_len >= 8)
		{
			break;
		}
	}
	MODBUS_G_CLR_RXIRPT();
	MODBUS_L_CLR_RXIRPT();
	ret = MODBUS_OK;
	if(timeout <= 0)
	{
			return MODBUS_ERR_TIMEOUT;
	}	
	if(sw == _MODBUS_G_)
	{
		if(_modbus_g_buf[0] != addr || _modbus_g_buf[1] != regcoil) 
		{
				return MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_g_buf[0],6);	
		if(_modbus_g_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_g_buf[7] != crc >>8 )
		{
				return MODBUS_ERR_CRC;
		}	
		return MODBUS_OK;
	}	
	if(sw == _MODBUS_L_)
	{
		if(_modbus_l_buf[0] != addr || _modbus_l_buf[1] != regcoil) 
		{
				return MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_l_buf[0],6);	
		if(_modbus_l_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_l_buf[7] != crc >>8 )
		{
				return MODBUS_ERR_CRC;
		}	
		return MODBUS_OK;
	}	
	return MODBUS_ERR;	
} 

void omodbus_write_reg(unsigned char sw , unsigned char modbus_addr, unsigned char modbus_coil, unsigned int reg_id, unsigned int v_value)
{
  unsigned char _modbus_buf[10]    = {0};	
  unsigned int   crc = 0;

  _modbus_buf[0] = modbus_addr;
  _modbus_buf[1] = modbus_coil;
  _modbus_buf[2] = reg_id >> 8;     //寄存器高位
  _modbus_buf[3] = reg_id & 0xFF;   //寄存器低位
  
  _modbus_buf[4] = v_value >> 8; 	//值高位
  _modbus_buf[5] = v_value &0x00FF; //值低位

  crc = omodbus_crc16(_modbus_buf, 6);//FLOW_CRC16//进行CRC校验
	 
  _modbus_buf[6] = (unsigned char)(crc &0x00FF);//校验值低位
  _modbus_buf[7] = crc >>8;	
  /*
return: F7   06    08 13   00 01   AF 39 
       addr  code   reg     val    crc
PC_SendStr(CNGMASS_TRANS,index);
  */  
  _modbus_response_g_len = 0;
  _modbus_response_l_len = 0;
  memset((unsigned char*)_modbus_g_buf,0,sizeof(_modbus_g_buf));
  memset((unsigned char*)_modbus_l_buf,0,sizeof(_modbus_l_buf));
  if( _MODBUS_G_ == sw)
  {
	  MODBUS_G_SET_RXIRPT(); 	  
      omodbus_g_sendstr(8, &_modbus_buf[0]); 
	  return;  	
  }
  if( _MODBUS_L_ == sw)
  {
	  MODBUS_L_SET_RXIRPT(); 	  
      omodbus_l_sendstr(8, &_modbus_buf[0]);   	
  }
} 
#if 0
/////////////////
void omodbus_write_reg_ex(unsigned char sw , unsigned char modbus_addr, unsigned char modbus_coil, unsigned int reg_id, unsigned int v_value)
{
  unsigned char _modbus_buf[10]    = {0};	
  unsigned int   crc = 0;

  _modbus_buf[0] = modbus_addr;
  _modbus_buf[1] = modbus_coil;
  _modbus_buf[2] = reg_id >> 8;     //寄存器高位
  _modbus_buf[3] = reg_id & 0xFF;   //寄存器低位

  _modbus_buf[4] = 0; 	//值高位
  _modbus_buf[5] = 0; //值低位
    
  _modbus_buf[6] = v_value >> 8; 	//值高位
  _modbus_buf[7] = v_value &0x00FF; //值低位

  crc = omodbus_crc16(_modbus_buf, 8);//FLOW_CRC16//进行CRC校验
	 
  _modbus_buf[8] = (unsigned char)(crc &0x00FF);//校验值低位
  _modbus_buf[9] = crc >>8;	
  /*
return: F7   06    08 13   00 01   AF 39 
       addr  code   reg     val    crc
PC_SendStr(CNGMASS_TRANS,index);
  */  
  _modbus_response_g_len = 0;
  _modbus_response_l_len = 0;
  memset((unsigned char*)_modbus_g_buf,0,sizeof(_modbus_g_buf));
  memset((unsigned char*)_modbus_l_buf,0,sizeof(_modbus_l_buf));
  if( _MODBUS_G_ == sw)
  {
	  MODBUS_G_SET_RXIRPT(); 	  
      omodbus_g_sendstr(10, &_modbus_buf[0]); 
	  return;  	
  }
  if( _MODBUS_L_ == sw)
  {
	  MODBUS_L_SET_RXIRPT(); 	  
      omodbus_l_sendstr(10, &_modbus_buf[0]);   	
  }
} 
unsigned char omodbus_write_reg_vavle_ex(unsigned char sw ,unsigned int reg_id, unsigned int v_value)
{
  unsigned char  addr     = 0;
  unsigned char  regcoil  = 0;
  unsigned char  ret = 0;
  unsigned int   crc = 0;
  int            timeout;
  if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM){
		addr = 1;
		regcoil = 0x05;
  }		
  else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH){
  	    addr = 247;
		regcoil = 0x06;
  }	  
  else
  {
	 return MODBUS_ERR;
  } 

	
	omodbus_write_reg_ex(sw,addr, regcoil,reg_id,v_value);
	timeout = MODBUS_TIMEOUT_MAX*2;
	while(timeout--)
	{
		osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);	
		if(sw == _MODBUS_G_ && _modbus_response_g_len >= 8)	 break;
		if(sw == _MODBUS_L_ && _modbus_response_l_len >= 8)	 break;
	}
	MODBUS_G_CLR_RXIRPT();
	MODBUS_L_CLR_RXIRPT();
	ret = MODBUS_OK;
	if(timeout <=0 )
	{
		return MODBUS_ERR_TIMEOUT;
	}		
	if(sw == _MODBUS_G_)
	{
		if(_modbus_g_buf[0] != addr || _modbus_g_buf[1] != regcoil) 
		{
				return MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_g_buf[0],6);	
		if(_modbus_g_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_g_buf[7] != crc >>8 )
		{
				return MODBUS_ERR_CRC;
		}	
		return MODBUS_OK;
	}	
	if(sw == _MODBUS_L_)
	{
		if(_modbus_l_buf[0] != addr || _modbus_l_buf[1] != regcoil) 
		{
				return MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_l_buf[0],6);	
		if(_modbus_l_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_l_buf[7] != crc >>8 )
		{
				return MODBUS_ERR_CRC;
		}	
		return MODBUS_OK;
	}	
	return MODBUS_ERR;		
} 
/////////////////
#endif
unsigned char omodbus_write_reg_vavle(unsigned char sw ,unsigned int reg_id, unsigned char regcoil ,unsigned int v_value)
{
  unsigned char  addr     = 0;
 // unsigned char  regcoil  = 0;
  unsigned char  ret = 0;
  unsigned int   crc = 0;
  int            timeout;
  if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM){
		addr = 1;
		//regcoil = 0x05;
  }		
  else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH){
  	    addr = 247;
		regcoil = 0x06;
  }	  
  else
  {
	 return MODBUS_ERR;
  } 

	
	omodbus_write_reg(sw,addr, regcoil,reg_id,v_value);
	timeout = MODBUS_TIMEOUT_MAX*2;
	while(timeout--)
	{
		osys_opower_chk(); //掉电检测
		_delay_loop_2(1843);//_delay_ms(1);	
		if(sw == _MODBUS_G_ && _modbus_response_g_len >= 8)	 break;
		if(sw == _MODBUS_L_ && _modbus_response_l_len >= 8)	 break;
	}
	MODBUS_G_CLR_RXIRPT();
	MODBUS_L_CLR_RXIRPT();
	ret = MODBUS_OK;
	if(timeout <=0 )
	{
		return MODBUS_ERR_TIMEOUT;
	}		
	if(sw == _MODBUS_G_)
	{
		if(_modbus_g_buf[0] != addr || _modbus_g_buf[1] != regcoil) 
		{
				return MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_g_buf[0],6);	
		if(_modbus_g_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_g_buf[7] != crc >>8 )
		{
				return MODBUS_ERR_CRC;
		}	
		return MODBUS_OK;
	}	
	if(sw == _MODBUS_L_)
	{
		if(_modbus_l_buf[0] != addr || _modbus_l_buf[1] != regcoil) 
		{
				return MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_l_buf[0],6);	
		if(_modbus_l_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_l_buf[7] != crc >>8 )
		{
				return MODBUS_ERR_CRC;
		}	
		return MODBUS_OK;
	}	
	return MODBUS_ERR;		
/*	if(_modbus_response_l_len>=8){
			  //memcpy((unsigned char*)_modbus_l_buf, (unsigned char*)_modbus_g_buf, 8); //2012-11-20
			  memcpy((unsigned char*)_modbus_g_buf, (unsigned char*)_modbus_l_buf, 8); //2012-11-20
		}
		if(_modbus_g_buf[0] != addr || _modbus_g_buf[1] != regcoil) 
		{
				ret = MODBUS_ERR;
		}
		crc =omodbus_crc16((unsigned char*)&_modbus_g_buf[0],6);	
		if(_modbus_g_buf[6] != (unsigned char)(crc &0x00FF) || _modbus_g_buf[7] != crc >>8 )
		{
				ret = MODBUS_ERR_CRC;
		}	
	} */		
	//OS_ENABLE_KEYSCAN();	
	//return ret;
} 

unsigned char omodbus_clrtotalreg_test(unsigned char sw)
{
	/*
	unsigned int clr_timeout = MODBUS_TIMEOUT_MAX;
	unsigned int  reg  = 0;	
	unsigned int  v_value = 0;
	unsigned char ret = 0;
	_union_float  vfrd;
	OS_DISABLE_KEYSCAN();
	vfrd.dt_float = 0.0;
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM){
		reg = 55;
		v_value = 0xFF00;
	}		
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH){
	  	
	  reg = 2607;//CNG:  LNG: 2608 
	  v_value = 1;
	}	  
	else{
	  OS_ENABLE_KEYSCAN();	
	  return 10;
	}
	ret = omodbus_write_reg(sw,reg,v_value,1,clr_timeout);// != MODBUS_OK)
	OS_ENABLE_KEYSCAN();
	*/
	return 0;//ret;	
}	
/**/
//bool omodbus_g_clrtotalreg(void)
bool omodbus_clrtotalreg(unsigned char sw)
{
	unsigned int  reg1, reg2  = 0;	
	unsigned int  v_value = 0;
	bool		  ret = true;
	float         vfrd = 0;
	OS_DISABLE_KEYSCAN();
	ret = true;
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)
	{
		reg1 = 55;
		reg2 = 258;
		v_value = 0xFF00;
	}		
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)
	{
		  reg1 = 2607;//CNG:  LNG: 2608 
		  reg2 = 2609;
		  v_value = 1;
	}	  
	else
	{
	  ret =  false;
	}
	if( true == ret) 
	{
		ret = false;
		if(omodbus_write_reg_vavle(sw,reg1,5,v_value) == MODBUS_OK) 
		{
			ocomm_delay_ms(10);
			if(omodbus_read_float(sw, reg2,&vfrd))
			{
				if(vfrd <0.01 )
				{
					ret = true;
				}
			}			
		}
	}
	OS_ENABLE_KEYSCAN();	
	return ret;	
}
bool omodbus_read_int_ex(unsigned char sw, unsigned int reg, unsigned char* vrd)
{
	bool		  ret;
	OS_DISABLE_KEYSCAN();	
	ret  = false;
	if(omodbus_read_reg(sw, reg,1) == MODBUS_OK)
	{
		if(sw == _MODBUS_G_)
		{
			vrd[0] = _modbus_g_buf[3];
			vrd[1] = _modbus_g_buf[4];
			ret = true;
		}
		else if(sw == _MODBUS_L_)
		{
			vrd[0] = _modbus_l_buf[3];
			vrd[1] = _modbus_l_buf[4];
			ret = true;
		}
		
	}	
	OS_ENABLE_KEYSCAN();	
	return ret;	
}
bool omodbus_read_int(unsigned char sw, unsigned int reg, unsigned int* vrd)
{
	unsigned int  temp = 0;
	bool		  ret;
	OS_DISABLE_KEYSCAN();	
	ret  = false;
	if(omodbus_read_reg(sw, reg,1) == MODBUS_OK)
	{
		if(sw == _MODBUS_G_)
		{
			temp  = _modbus_g_buf[3];
			temp  = temp*0x100;
			*vrd  = temp + _modbus_g_buf[4];
			ret = true;
		}
		else if(sw == _MODBUS_L_)
		{
			temp  = _modbus_l_buf[3];
			temp  = temp*0x100;
			*vrd  = temp + _modbus_l_buf[4];
			ret = true;
		}
		
	}	
	OS_ENABLE_KEYSCAN();	
	return ret;	
}

bool omodbus_write_float(unsigned char sw, unsigned int reg, float vfrd)
{
	bool           ret = false;
	//_union_float  unvfrd;
	OS_DISABLE_KEYSCAN();	
	if(omodbus_write_reg_vavle_float(sw, reg,vfrd) == MODBUS_OK)
	{
		ret = true;
	}	
	OS_ENABLE_KEYSCAN();	
	return ret;
}

bool omodbus_read_float(unsigned char sw, unsigned int reg, float* vfrd)
{
	bool           ret = false;
	_union_float  unvfrd;
	OS_DISABLE_KEYSCAN();	
	unvfrd.dt_float  = 0.0;
	if(omodbus_read_reg(sw, reg,2) == MODBUS_OK)
	{
			unvfrd.dt_uc[0] = _modbus_g_buf[4];
			unvfrd.dt_uc[1] = _modbus_g_buf[3];
			unvfrd.dt_uc[2] = _modbus_g_buf[6];
			unvfrd.dt_uc[3] = _modbus_g_buf[5];		
			*vfrd = unvfrd.dt_float;	
			ret = true;
	}	
	OS_ENABLE_KEYSCAN();	
	return ret;
}

bool omodbus_write_multi_reg(unsigned char sw, unsigned int regaddr, unsigned int reg)
{
	bool ret = false;
	OS_DISABLE_KEYSCAN();	
	if(omodbus_write_reg_vavle(sw,regaddr,6,reg) == MODBUS_OK) ret = true;
	OS_ENABLE_KEYSCAN();	
	return ret;
}

/*
@读流量计
@sw:定义液态和气态,regnum寄存器号
*/

bool omodbus_read_auto_buffer(unsigned char sw, unsigned char regnum)
{
	bool ret = false;
	unsigned int reg = 0;
	OS_DISABLE_KEYSCAN();
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)
	{
		reg = 782;
	}
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)//MODBUS_TYPE_RM)
	{
		reg = 5050;
	}
	
	if(omodbus_read_reg(sw,reg,regnum*2) == MODBUS_OK)
	{
			ret = true;
	}
	OS_ENABLE_KEYSCAN();	
	return ret;	 
}
/*void omodbus_read_auto_value(unsigned char sw, unsigned char id, unsigned char *rdbuf)
{
	
}*/
float omodbus_read_auto_float_value(unsigned char sw, unsigned char id)
{
	_union_float  unvfrd;
	OS_DISABLE_KEYSCAN();
	unvfrd.dt_float  = 0.0;
	if( _MODBUS_G_ == sw)
	{
		unvfrd.dt_uc[0] = _modbus_g_buf[4+id*4];
		unvfrd.dt_uc[1] = _modbus_g_buf[3+id*4];
		unvfrd.dt_uc[2] = _modbus_g_buf[6+id*4];
		unvfrd.dt_uc[3] = _modbus_g_buf[5+id*4];	
	}
	else if( _MODBUS_L_ == sw)
	{
		unvfrd.dt_uc[0] = _modbus_l_buf[4+id*4];
		unvfrd.dt_uc[1] = _modbus_l_buf[3+id*4];
		unvfrd.dt_uc[2] = _modbus_l_buf[6+id*4];
		unvfrd.dt_uc[3] = _modbus_l_buf[5+id*4];	
	}
	OS_ENABLE_KEYSCAN();
	return 	unvfrd.dt_float;
}

unsigned int omodbus_read_auto_uint_value(unsigned char sw,unsigned char id)
{
	unsigned int  tmp = 0;;
	OS_DISABLE_KEYSCAN();
	tmp = 0;
	if( _MODBUS_G_ == sw)
	{
		tmp = _modbus_g_buf[3+id*4];
		tmp *= 0x100;
		tmp += _modbus_g_buf[4+id*4];		
	}
	else if( _MODBUS_L_ == sw)
	{
		tmp = _modbus_l_buf[3+id*4];
		tmp *= 0x100;
		tmp += _modbus_l_buf[4+id*4];		
	}
	OS_ENABLE_KEYSCAN();
	return 	tmp;
}

SIGNAL(USART1_RX_vect)
{
	unsigned char recv;
	recv = UDR1;	
    if(_modbus_response_l_len >= MODBUS_MAX_RESPONSE_LEN)
	{
	  _modbus_response_l_len = 0;	//++
	  return;
	}	  
    _modbus_l_buf[_modbus_response_l_len++] = recv;

}

SIGNAL(USART3_RX_vect)
{
	unsigned char recv;
	recv = UDR3;	
    if(_modbus_response_g_len >= MODBUS_MAX_RESPONSE_LEN){
	  _modbus_response_g_len = 0; //++	
	  return;
	}	  
    _modbus_g_buf[_modbus_response_g_len++] = recv;
}
