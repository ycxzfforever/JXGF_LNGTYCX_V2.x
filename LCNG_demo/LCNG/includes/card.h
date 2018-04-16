/*
 * card.h
 *
 * Created: 2012-8-15 14:55:25
 *  Author: luom
 */ 


#ifndef CARD_H_
#define CARD_H_

   typedef union _def_unlong_
   {  
		unsigned long  dt_ulong;
		unsigned char  dt_uc[4];
  }_union_ulong;

  typedef union _def_long_
  {  
	    long		   dt_long;
		unsigned char  dt_uc[4];
  }_union_long;


#define USART_SEL_S0H()       SET_PORT_BIT(PORTB,PB7)//PORTB |= 0x80
#define USART_SEL_S0L()       CLR_PORT_BIT(PORTB,PB7)//PORTB &=~0x80
#define USART_SEL_S0OUT()     SET_PORT_BIT(DDRB,DDB7)//DDRB  |= 0x80
#define USART_SEL_S0IN()      CLR_PORT_BIT(DDRB,DDB7)//DDRB  &=~0x80
#define USART_SEL_S1H()       SET_PORT_BIT(PORTH,PH7)//PORTH |= 0x80
#define USART_SEL_S1L()       CLR_PORT_BIT(PORTH,PH7)//PORTH &=~0x80
#define USART_SEL_S1OUT()     SET_PORT_BIT(DDRH,DDH7)//DDRH  |= 0x80
#define USART_SEL_S1IN()      CLR_PORT_BIT(DDRH,DDH7)//DDRH  &=~0x80


#define SELECT_CARD()		  USART_SEL_S1H(); USART_SEL_S0H()		 
#define SELECT_PRINT()		  USART_SEL_S1H(); USART_SEL_S0L()
#define SELECT_UART0()		  USART_SEL_S1L(); USART_SEL_S0L()		 

/*void sel_usart(unsigned char USARTdev)
{
  USART_SEL_S0OUT();
  USART_SEL_S1OUT();
  switch(USARTdev)
  {
    case USART_BANK:
      USART_SEL_S1L();
      USART_SEL_S0L();break;
    case USART_TFT:
      USART_SEL_S1L();
      USART_SEL_S0H();break;
    case USART_PRINT:
      USART_SEL_S1H();
      USART_SEL_S0L();break;
    case USART_CARD:
      USART_SEL_S1H();
      USART_SEL_S0H();break;
  }
}*/

  enum _ICCARD_OP_TYPE_
  {
	  ICCARD_NO_OPT = 1,
	  ICCARD_MASTER,
	  ICCARD_READ,
	  ICCARD_WRITE,
  };

/*
  #define		ICCARD_MASTER   1
  #define		ICCARD_READ		2
  #define		ICCARD_WRITE	3
*/
  #define      CARD_TIMEOUT		50  //10000 //cnt

  #define ICCARD_SET_RXIRPT()   SET_PORT_BIT(UCSR0B, RXCIE0)
  #define ICCARD_CLR_RXIRPT()   CLR_PORT_BIT(UCSR0B, RXCIE0)
  ex    volatile unsigned char icard_err_;
  ex    volatile unsigned char icard_err_code_;
  ex    volatile unsigned char icard_chk_step;
  ex    volatile unsigned char icard_getgray_step;////置灰步进20140219
  ex    volatile unsigned int  icard_step_cnt;
 // ex    volatile unsigned char icard_opt_type;
  //ex    volatile unsigned char icard_cur_block;
  ex 	volatile unsigned char oicard_recvlen;
 #if OLD_ICCARD_MODULE
  ex 	volatile unsigned char oicard_recvbuf[36]; //32
 #else
      volatile unsigned char oicard_recvbuf[140];
 #endif

 	void oicard_turnoff(void);
 #if  ICCARD_M1_MINGTE  
	  void oicard_init(unsigned int baudrate);
	  bool oicard_read(bool bnewpsw, unsigned char blocks, unsigned char* rdbuf);
	  bool oicard_write(bool bnewpsw, unsigned char blocks, unsigned char *wtbuf);
#else
	//void ICCARD_CPU_Oper(void);
	void PsamCardSelect_Oper(void);//选择PSAM卡------复位PSAM卡
	bool PsamCardMF_Oper(void);//PSAM卡选择MF
	bool PsamCardDF_Oper(void);//PSAM卡选择DF
	bool PsamCardDesInit_Oper(void);//PSAM-DES初始化
	bool PsamCardDes_Oper(void);//PSAM-DES加密
	bool PsamCardApdu_oper(unsigned char psamnumber,unsigned char *cmdarray,unsigned char cmdlength);//PSAM_apdu命令操作
	bool CpuCardSelect_Oper(void);//-------CPU卡上电复位
	bool CpuCardMF_Oper(void);//Cpu卡选择MF操作
	bool CpuCardDF_Oper(void);//Cpu卡选择DF操作
	bool CpuCardGetrand_oper(void);//Cpu获取随机数
	bool CpuCardExternVerity_Oper(void);//Cpu发送外部认证命令
	bool CpuCardApdu_oper(unsigned char *cmdarray,unsigned char cmdlength);//CPU-APDU命令操作
	bool oicard_read(unsigned char file,unsigned char number,unsigned char *rdbuf);
	bool oicard_write(unsigned char file,unsigned char number,unsigned char *wtbuf,unsigned char offset);
	//void oicard_read_oper(void);
	//void oicard_write_oper(void);
 #endif
  void oapp_iccard_check_imfor(void);

  /*
@恢复正常卡标志
@
*/
void oapp_iccard_back_write_flag(void);
  
#endif /* CARD_H_ */