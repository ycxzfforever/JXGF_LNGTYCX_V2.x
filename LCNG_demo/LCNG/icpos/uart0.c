#include "kernel.h"

struct UART0 uart0;

void Uart0Init(void)
{
  //这段代码是为调试正星代码准备
// desired baud rate: 9600
// actual: baud rate:9600 (0.0%)
// char size: 8 bit
// parity: Even
  uart0receivelength=0;
  uart0receiveflag=0;
  memset(uart0receivebuffer,0,sizeof(uart0receivebuffer));
  memset(&uart0.reclen,0,sizeof(struct UART0));
  uart0.sendbuf[0]=0xFA;
  uart0.sendbuf[1]=0x10;
  uart0.sendbuf[2]=0x20;
  uart0.sendbuf[4]=0x00;
  uart0.faflag=0x55;
  UCSR0B = 0x00; //disable while setting baud rate
  UCSR0A = 0x00;
  UCSR0C = 0x26;
  UBRR0L = 0x2F; //set baud rate lo
  UBRR0H = 0x00; //set baud rate hi
  UCSR0B = (1<<RXCIE0)|(1<<TXEN0) |(1<<RXEN0);//受和发送 ,接受中断使能 <中断方式>
  __enable_interrupt();//中断方式
}

void Uart0SendChar(char ch)
//Explain:Send a char from usart.
{
  while ( UCSR0A_UDRE0 == 0); 
  UDR0 = ch;
  while (UCSR0A_UDRE0 == 0);
}

void Uart0PutString(char* string)
{
 while(*string!='\0')
 {
  Uart0SendChar(*string);
  string++;
 }
}

void Uart0Printf(char *fmt,...)
{ 
  va_list ap;
  char str[40];
  va_start(ap,fmt);
  vsprintf(str,fmt,ap);
  Uart0PutString(str);
  va_end(ap);
}

int Uart0ReciveByte(void)
{
  unsigned int tick485dly = 0;
  while((UCSR0A_RXC0==0)&&(++tick485dly < 10000));//RXCn
  if(tick485dly <10000) return UDR0;
  return -1;
}

void Uart0Select(unsigned char uart0device)
//function:串口0通过4052，选择不同的设备
//USART0_BANK:串口0(J6)
//USART0_PRINT:打印机
//USART0_CARD:IC卡
{
  UART0S0_OUT();
  UART0S1_OUT();
  switch(uart0device)
  {
  case USART0_BANK:
    UART0S0_L();
    UART0S1_L();
    __delay_cycles(10);
    break;
  case USART0_TFT:
    UART0S1_L();
    UART0S0_H();
    __delay_cycles(10);
    break;
  case USART0_PRINT:
    UART0S0_L();
    UART0S1_H();
    __delay_cycles(10);
    break;
  case USART0_CARD:
    UART0S0_H();
    UART0S1_H();
    __delay_cycles(10);
    break;   
  }
}

unsigned int ZHYCrc(unsigned char *buf,unsigned char length)
//中海油正星模块
/*计算buf里的CRC的值,length是要计算的长度,buf是要计算的数据,返回两字节crc值*/
{
   unsigned char  a,b,c;
   unsigned char i;
   unsigned int crc=0;
   unsigned char *ptr;
   for(i=0;i<length;i++)
   {
     a=buf[i];
     ptr=(unsigned char *)&crc;
     b=ptr[0];
     b=a^b;
     ptr[0]=b;
     for(c=0;c<8;c++)
     {
         b=ptr[0];
         crc>>=1;
         if((b&0x01))
         {
             crc=crc^0xa001;
         }
     }
   }
   return(crc);
}

bool ZHYComRec(void)
//中海油正星模块
//验证正星模块发送的命令
{
  unsigned int  crcvalue;
  unsigned char crclow=0x0;
//  unsigned char crchigh=0;
  unsigned char len=0;//接受命令长度
  unsigned int  timecount=0;
  unsigned char flag=0;
  
  while(timecount++<5000)
  {
    len=BcdToHex(uart0.recbuf[5]);
    if(uart0.reclen>=(8+len))
    {
      flag=0xaa;
      break;
    }
  }
  if(flag==0xaa)
  {
    crcvalue=ZHYCrc(&uart0.recbuf[1],5+len);
    crclow=(unsigned char)crcvalue;
//    crchigh=(unsigned char)(crcvalue>>8);
    if(crclow==uart0.recbuf[len+7])
//    if(crchigh==uart0.recbuf[len+6])
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}

void ZHYResCom(unsigned char *comarray,unsigned char len)
//中海油正星模块
//响应正星模块命令
{
  unsigned char bcdlen;
  unsigned int crcvalue;
  unsigned char i;
  bcdlen=HexToBcd(len);
  uart0.sendbuf[5]=bcdlen;
  for(i=0;i<len;i++)
  {
    uart0.sendbuf[6+i]=*(comarray+i);
  }
  crcvalue=ZHYCrc(&uart0.sendbuf[1],len+5);
  uart0.sendbuf[6+len]=(unsigned char)(crcvalue>>8);
  uart0.sendbuf[7+len]=(unsigned char)crcvalue;
  Uart0SendChar(uart0.sendbuf[0]);
  Uart0SendChar(uart0.sendbuf[1]);
  Uart0SendChar(uart0.sendbuf[2]);
  Uart0SendChar(uart0.sendbuf[3]);
  Uart0SendChar(uart0.sendbuf[4]);
  for(bcdlen=5;bcdlen<8+len;bcdlen++)
  {
    if(uart0.sendbuf[bcdlen]==0xFA)
    {
      Uart0SendChar(uart0.sendbuf[bcdlen]);
      Uart0SendChar(uart0.sendbuf[bcdlen]);
    }
    else
    {
       Uart0SendChar(uart0.sendbuf[bcdlen]);
    }
  }
}

#pragma vector = USART0_RX_vect
//中海油正星模块
//串口0接收中断处理 //huanghao 20130330 delete
__interrupt void uart0_ZHY(void)
{
  unsigned char recbyte;
  recbyte=UDR0;
  uart0.recbuf[uart0.reclen++]=recbyte;
  if(uart0.recbuf[0]==0xfa)
  {
    uart0.intflag=0xa1;//表示串口接受到中断
  }
  else
  {
    uart0.reclen=0;
  }
  if(uart0.reclen>4)
  {
    if((uart0.recbuf[uart0.reclen-1]==0xfa)&&(uart0.faflag==0x55))
    {
      uart0.faflag=0xaa;
      uart0.reclen--;
    }
    else if((uart0.recbuf[uart0.reclen-1]==0xfa)&&(uart0.faflag==0xaa))
    {
      uart0.faflag=0x55;
    }
  }
}