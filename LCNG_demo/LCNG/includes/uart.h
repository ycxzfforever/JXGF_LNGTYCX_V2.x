/*
 * uart.h
 *
 * Created: 2012-8-6 13:42:25
 *  Author: luom
 */ 


#ifndef UART_H_
#define UART_H_

  #define RS485EN_H()			SET_PORT_BIT(PORTB, PB4)
  #define RS485EN_L()			CLR_PORT_BIT(PORTB, PB4)

  #define RS485EN_OUT()			SET_PORT_BIT(DDRB, DDB4)
  #define RS485EN_IN()			CLR_PORT_BIT(DDRB, DDB4)


  #define RS485_SET_RXIRPT()    SET_PORT_BIT(UCSR2B, RXCIE2) //UCSR2B = UCSR2B |( (1<<RXCIE2))//SET_PORT_BIT(UCSR2B, RXCIE2) // UCSR2B &(1 << UDRE2)//UCSR2B_RXCIE2 = 1     
  #define RS485_CLR_RXIRPT()    CLR_PORT_BIT(UCSR2B, RXCIE2) //UCSR2B = UCSR2B &(~(1<<RXCIE2))//CLR_PORT_BIT(UCSR2B, RXCIE2) // UCSR2B_RXCIE2 = 0     

  ex  volatile unsigned char		 pctrans_recvlen;//nRs485BufLen;
  ex  volatile unsigned char		 pctrans_recvbuf[120];

// ex  volatile unsigned char uart_comm_recvlen;
 //ex  volatile unsigned char uart_comm_recvbuf[64];
  void ouart2_init(unsigned int baudrate);
  void ouart2_sendstr(unsigned char *buf, unsigned char Len);
  void ouart2_sendstr_ex(volatile unsigned char *buf, unsigned char Len);
  
#endif /* UART_H_ */