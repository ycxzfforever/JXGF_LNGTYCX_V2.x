/*
 * uart.c
 *
 * Created: 2012-8-6 13:41:29
 *  Author: luom
 */ 
#include "global_def.h"

void ouart2_init(unsigned int baudrate)
{
   unsigned int UBRR;
   pctrans_recvlen = 0;
   //pctrans_flag = 0;
   //uart_comm_recvlen = 0;
   memset((unsigned char*)pctrans_recvbuf, 0, sizeof(pctrans_recvbuf));
  // memset((unsigned char *)uart_comm_recvbuf,0,64);
   
   RS485EN_L();
   RS485EN_OUT();	 

   UCSR2B = 0x00;           //disable while setting baud rate
   UCSR2A = 0x00;
   UCSR2C = (1<<2) |(1<<1); //8位数据位，无奇偶校验，1位停止位UCSZn1UCSZn0	 
   UBRR = ((F_CPU>>4)/baudrate -1);             //计算UBRR
   UBRR2L = (unsigned char) (UBRR & 0xFF); 	//set baud rate lo
   UBRR2H = (unsigned char) ((UBRR >>8) &0xFF); //set baud rate hi
   UCSR2B = (1<<3) |(1<<4); //使能接收发送，//TXENnRXENn
   
   //RS485_CLR_RXIRPT();	 
   RS485_SET_RXIRPT();	 
}

bool ouart2_recvbyte_ex(unsigned char *recv, int timeout)
{
	while (!(UCSR2A &(1<<RXC2))) 
	{
	  while(--timeout < 0) 
	  {
		_delay_us(1);  
		return false;   
	  }
	}
	*recv = UDR2;
	return true;
}

//static 
void ouart2_sendchar(unsigned char c)
{
  while (!(UCSR2A &(1<<UDRE2)))   ;//while ( UCSR2A_UDRE2 == 0); 
  UDR2 = c;
  //while (!(UCSR2A &(1 << UDRE2)))   ;//while (UCSR2A_UDRE2 == 0);
}


void ouart2_sendstr(unsigned char *buf, unsigned char Len)
{
  unsigned char i =0;
  unsigned int  temp =0;
  RS485EN_H();
  _delay_us(50);
  for (i =0; i <Len; i++)
  {
    ouart2_sendchar(*buf++);
  }
  for (temp = 0; temp < 600;  temp++)  asm("nop"); //38400
  //for (temp = 0; temp < 600*4;  temp++)  asm("nop"); //9600
  RS485EN_L();
  _delay_us(50);
}
#if 1
void ouart2_sendstr_ex(volatile unsigned char *buf, unsigned char Len)
{
  unsigned char i =0;
  unsigned int  temp =0;
  RS485EN_H();
  _delay_us(500);//50
  for (i =0; i <Len; i++)
  {
    ouart2_sendchar(*buf++);
  }
  for (temp = 0; temp < 600;  temp++)  asm("nop"); //38400
  //for (temp = 0; temp < 600*4;  temp++)  asm("nop"); //9600
  RS485EN_L();
  _delay_us(500);
}
#endif
//AA FF 00 00 00 04 A0 10 03 00 00 00 A0  //中油节能
//FC FC E0 01 04 00 00 //请求实时状态
/*
同步头:   2 Byte。 所有命令和应答均以2个0FCH为同步头。
地址：    1 Byte（下位机为机号，上位机为E0H，该地址表示数据发送方向）
机号：    1byte（机号01-FF）
命令:     1 Byte
数据长度：1Byte
数据:     根据命令的不同, 数据的长度是可变的。
校验和:   1Byte。为地址、命令、 數據長度、数据字节的异或值。
除同步头、命令号、状态、校验和以及特殊指定外，其它全部为BCD码。
地址、数据长度采用HEX。
*/
/***********************************************************************
中油节能                                                             
同步头:			2 Byte 0xAA 0xFF (PC->LCNG)   0xBB 0xEE (LCNG->PC)
目标设备标识:   1 Byte
命令ID:         1 Byte
枪号:           1 Byte
消息体内容长度: 1 Byte 
消息体:
CRC:            1 Byte
截止符:         2 Byte 0xCC 0xFF (PC->LCNG)   0xDD 0xEE (LCNG->PC)
***********************************************************************/

SIGNAL(USART2_RX_vect)
{
  unsigned char recv;
  recv = UDR2;
  if(pctrans_recvlen >= 120)  pctrans_recvlen = 0;
  pctrans_recvbuf[pctrans_recvlen++] =  recv;

  if(pctrans_recvlen == 1)
  {
	if( recv != 0xAA)  pctrans_recvlen = 0;
    return;	   
  } 
  if(pctrans_recvlen == 2)
  {
	if( recv != 0xFF){  
		pctrans_recvlen = 0;
	}		
	else{
		otimer_t0_off();
		RS485_CLR_RXIRPT();
		while(1)
		{
			if(false == ouart2_recvbyte_ex((unsigned char*)&pctrans_recvbuf[pctrans_recvlen++],1000)) break;
			if((pctrans_recvlen == 5) && (pctrans_recvbuf[4]!= cur_sysparas.b_basic_p[BASIC_P_G_ID])) break;
			__WATCH_DOG();
		}
		//消息体长度
		if((pctrans_recvlen>6 )&& (pctrans_recvlen >= pctrans_recvbuf[5] +9))
		{
		              okernel_addeventpro(LCNG_COMM_EVENT6_EV, opotocol_zyjn_analysis);
				okernel_putevent(LCNG_COMM_EVENT6_EV,0);
		}
		else
		{
			RS485_SET_RXIRPT();
			pctrans_recvlen = 0;
		}
		
		otimer_t0_on(); //03-12
	}	
  }
} 

/*
SIGNAL(USART2_RX_vect)
{
       unsigned char recv;
       recv = UDR2;
	 if((pctrans_recvlen == 0)&&(recv == 0xAA))
	 {
	       pctrans_recvbuf[pctrans_recvlen++] =  recv;
		return;
	 }

       
        if(pctrans_recvlen >= 1)
        {
               if(pctrans_recvlen == 2)
	        {
	              otimer_t0_off();
	        }
		
	        pctrans_recvbuf[pctrans_recvlen++] =  recv;

		if(pctrans_recvlen > (pctrans_recvbuf[5] +6))
		{
		        okernel_addeventpro(LCNG_COMM_EVENT6_EV, opotocol_zyjn_analysis);
			 okernel_putevent(LCNG_COMM_EVENT6_EV,0);
			 otimer_t0_on();
		}

		if(pctrans_recvlen > 120)
		{
		        pctrans_recvlen = 0;
		}
        }
        
}
*/

