#include "kernel.h"

#define TOTAL_MAX1       4200000000//�������������������
struct ZHYRealRecord   ZHYrealdata;//����ʵʱ��¼
struct ZHYStoreRecord  ZHYstorerecord;//������¼
struct ZHYWorkStatus   ZHYworkstatus;//����״̬
float  cngaccumass=0;//�����ۼ�����
float  unitprice;//����
float  cngaccumassbak=0;  
double cngaccmoneybak=0;
double cngaccmoney=0;//�������
double calfactor;//�������̱���ϵ��
unsigned long maxonevolume;//���μ���������
unsigned long maxonemoney;// ���μ����������

#define  F_CPU    7372800

#define _delay_us(x)\
__delay_cycles( (unsigned long) ( (double)(F_CPU)*((x)/1000000.0) + 0.5))  

#define _delay_ms(x)\
__delay_cycles( (unsigned long) ( (double)(F_CPU)*((x)/1000.0) + 0.5))  

#define _delay_s(x) \
__delay_cycles( (unsigned long) ( (double)(F_CPU)*((x)/1.0) + 0.5))   
//�����������ʵ����
#if 0

void ZHYWorkReady(void)
//����ǰ׼��
{
  unsigned long temp;
  unsigned long rd;
  unsigned long currentpressure;//��ǰѹ��
//  modbusTReg rdvalue;
  FYD12864ClrScreen();
  FYD12864DispPrintf(1,1,"״̬: ����ǰ���");
  memset(&ZHYrealdata.ZHYWorkStatus,0,sizeof(struct ZHYRealRecord));
  memset(&ZHYstorerecord.RecUID[0],0,sizeof(struct ZHYStoreRecord));
  ZHYrealdata.ZHYCmd=0x02;
  ZHYrealdata.ZHYWorkStatus=0x01;
  cngaccumass=0;
  cngaccmoney=0;
  cngaccumassbak=0;
  cngaccmoneybak=0;
  
  ZHYworkstatus.workstatus=0x02;
  uart0.waitflag=0;
  if(uart0.recbuf[11]==0x00)
  {
    ZHYworkstatus.workfixtype=0x00;
  }
  else if(uart0.recbuf[11]==0x01)//������
  {
    ZHYworkstatus.workfixtype=0x01;
    
    rd=CharBufferToLong(&uart0.recbuf[12],4);
    ZHYworkstatus.workfixvolume=(float)rd/100.00;
    rd=EepromReadSystemPara(5007);
    ZHYworkstatus.workCofficient=(float)rd/100.00;
//    Uart0Select(USART0_BANK);
//    Uart0Printf("the Cofficient is:%.02f\n\r",ZHYworkstatus.workCofficient);
//    Uart0Select(USART0_CARD);
    /*
    rdvalue.rdcv[0]=uart0.recbuf[12];
    rdvalue.rdcv[1]=uart0.recbuf[13];
    rdvalue.rdcv[2]=uart0.recbuf[14];
    rdvalue.rdcv[3]=uart0.recbuf[15];
    ZHYworkstatus.workfixvolume=rdvalue.rdfv;
    Uart0Select(USART0_BANK);
    Uart0Printf("the fix volume1 is:%ld\n\r",rd);
    Uart0Printf("the fix volume is:%.02f\n\r",ZHYworkstatus.workfixvolume);
    Uart0Select(USART0_CARD); */
    
  }
  else if(uart0.recbuf[11]==0x02)//�����
  {
    ZHYworkstatus.workfixtype=0x02; 
    rd=CharBufferToLong(&uart0.recbuf[12],4);
    ZHYworkstatus.workfixmoney=(float)rd/100.00;
    ZHYworkstatus.workfixvolume=(float)ZHYworkstatus.workfixmoney/unitprice; 
    rd=EepromReadSystemPara(5007);
    ZHYworkstatus.workCofficient=(float)rd/100.00;
//    Uart0Select(USART0_BANK);
//    Uart0Printf("the Cofficient is:%.02f\n\r",ZHYworkstatus.workCofficient);
//    Uart0Select(USART0_CARD);
    /*
    rdvalue.rdcv[0]=uart0.recbuf[15];
    rdvalue.rdcv[1]=uart0.recbuf[14];
    rdvalue.rdcv[2]=uart0.recbuf[13];
    rdvalue.rdcv[3]=uart0.recbuf[12];
    ZHYworkstatus.workfixvolume=rdvalue.rdfv;
    Uart0Select(USART0_BANK);
    Uart0Printf("the fix money1 is:%ld\n\r",rd);
    Uart0Printf("the fix money is:%.02f\n\r",ZHYworkstatus.workfixvolume);
    Uart0Select(USART0_CARD); */
    
  }
  //�����ۼƽ��ȼ��
  //����ǰ׼��
  ZhongHaiYouInit();
  //��ȡ����ϵ��
  temp=EepromReadSystemPara(5001);
  rd=EepromReadSystemPara(5002);
  calfactor=(double)rd/temp;
  //�����������ۼƼ�������
  if(!MassFlowClearTotal())
  {
    FYD12864DispPrintf(1,1,"�������������");
    return;
  }
  //����������������뵥��
  maxonevolume=EepromReadSystemPara(5003);
  maxonemoney=EepromReadSystemPara(5004);
  rd=EepromReadSystemPara(4001);//�޿�����
  unitprice=(double)rd/100.00;
  
  unitprice=10.0;
  calfactor=1;
  LongToCharBuffer((unsigned long)(unitprice*100),&ZHYstorerecord.RecUnitPrice[0],4); 
  LongToCharBuffer((unsigned long)(unitprice*100),&ZHYrealdata.ZHYUnitPrice[0],4);
  //����ǰѹ��
  if(!PressureGetValue(true,&currentpressure))
  {   
  }
  else
  {
  }
  FYD12864ClearLine(1);
  FYD12864DispPrintf(1,1,"״̬: ���ڼ���");
//  FYD12864ClearLine(1);
//  FYD12864DispPrintf(1,1,"״̬: ���ڼ���");
  Timer2ClcTick(0);//��ģ�ⶨʱ��  
}

void ZHYWorkReadScanBuffer(void)
//������������
{
//  UCSR0B_RXCIE0=0;//disable uart0;
  if(!MassFlowReadScanBuffer())
  {
    FYD12864DispPrintf(1,1,"�������ƴ���");
    ValveAllOff();//�ر����е�ŷ�
    Timer2Off();
    ZHYworkstatus.workstatus=0x03;
  }
//  UCSR0B_RXCIE0=1;//enable uart0
}
void ZHYWork(void)
//������������������
{
  float  rdmasstotal;//���������ۼƼ�������
  float  rdmassflow;//������������
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  /*************************
  //���ۼƼ�������,�������ܽ��
  **************************/
  rdmasstotal=MassFlowReadTotal();
  cngaccumass=rdmasstotal*calfactor;
  cngaccumass=(unsigned long)(cngaccumass*100);
  cngaccumass=(double)cngaccumass/100.00;
  
  cngaccmoney = cngaccumass*unitprice;  
  cngaccmoney = (unsigned long)(cngaccmoney*100+0.5);
  cngaccmoney = (double)cngaccmoney/100.00;
  /****************************
  //˲ʱ�����쳣
  *****************************/ 
  if(cngaccmoney>(cngaccmoneybak+(unitprice/2)))     
  {
    FYD12864DispPrintf(1,1,"�쳣����");
    cngaccumass=cngaccumassbak;
    cngaccmoney=cngaccmoneybak;
    Timer2Off();
    ValveAllOff();//�ر����е�ŷ�
    ZHYworkstatus.workstatus=0x03;
  }
  else   
  {
    cngaccmoneybak=cngaccmoney;
    cngaccumassbak=cngaccumass;
  }
  /****************************
  //����SPLC
  *****************************/
  SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  /****************************    
  //��󵥴���
  *****************************/
  /****************************
  //����
  *****************************/
  //������
  if(ZHYworkstatus.workfixtype==0x01)
  {
    if((cngaccumass+ZHYworkstatus.workCofficient)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      Timer2Off();
      ValveAllOff();//�ر����е�ŷ�
      return;
    }
  }
  //�����
  if(ZHYworkstatus.workfixtype==0x02)
  {
    if((cngaccumass+ZHYworkstatus.workCofficient)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      Timer2Off();
      ValveAllOff();//�ر����е�ŷ�
      return;
    }
  }
  /****************************
  //ͣ�� �з�
  *****************************/
  rdmassflow=MassFlowReadFlowMass();
  rdmassflow=fabs(rdmassflow);
  FYD12864DispPrintf(4,1,"V:%.02f",rdmassflow);
  LongToCharBuffer((unsigned int)rdmassflow,&ZHYrealdata.ZHYMassFlow[0],2);
//  Uart0Printf("the flow:%f\r\n",rdmassflow);
  rdmassflow = (unsigned int)(rdmassflow*100+0.5);
  if(rdmassflow>maxsafemassflow)//���ڰ�ȫ����
  {
    if(maxsafeflowstarttime==0)
    {
      ticks=0;//huanghao add
      maxsafeflowstarttime=Timer2GetTick();
    }
    if((Timer2GetTick()-maxsafeflowstarttime)>maxsafeflowdelaytime)
    {
      ValveAllOff();//�ر����е�ŷ�
      Timer2Off();
      ZHYworkstatus.workstatus=0x03;
    }
  }
  else
  {
    maxsafeflowstarttime=0;
  }
  if(!MassFlowAndPressure((unsigned int)rdmassflow))
  {
    FYD12864DispPrintf(1,1,"ѹ������");
    ValveAllOff();//�ر����е�ŷ�
    Timer2Off();
    ZHYworkstatus.workstatus=0x03;
  }
  if(!valveswitchbymassflow((unsigned int)rdmassflow))
  {
    FYD12864DispPrintf(1,1,"���ٴ���");
    ValveAllOff();//�ر����е�ŷ�
    Timer2Off();
    ZHYworkstatus.workstatus=0x03;
  }
  
  /****************************
  //�ϴ�ʵʱ״̬
  *****************************/
  FYD12864DispPrintf(2,1,"����:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"���:%.02f",cngaccmoney);
  cngaccumassrealtime=(unsigned long)(cngaccumass*100+0.5);
  cngaccumoneyrealtime=(unsigned long)(cngaccmoney*100+0.5);
  LongToCharBuffer(cngaccumassrealtime,&ZHYrealdata.ZHYMass[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYrealdata.ZHYSum[0],4);  
}

void ZHYWorkFinish(void)
//��������
{
  float  rdmasstotal;//���������ۼƼ�������
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  unsigned char rdtime;
  unsigned char counttime;
  
  unsigned long fixdata;
  unsigned long tmpdata;
  unsigned long count;
//  unsigned long rd;
  if(ZHYworkstatus.workfixtype==0x01)//������ 
  {
    fixdata=(unsigned long)(ZHYworkstatus.workfixvolume*100)-1;
    tmpdata=(unsigned long)(cngaccumass*100);
    if(fixdata>tmpdata)
    {
      tmpdata=fixdata-tmpdata;
      for(count=0;count<tmpdata;count++)
      {
        cngaccumass += 0.01;
        cngaccmoney=cngaccumass*unitprice;
        SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
        _delay_ms(20);
      }
    }
    cngaccumass=ZHYworkstatus.workfixvolume;
    cngaccmoney= cngaccumass*unitprice;
    SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  }
  else if(ZHYworkstatus.workfixtype==0x02)//�����
  {
    fixdata=(unsigned long)(ZHYworkstatus.workfixvolume*100)-1;
    tmpdata=(unsigned long)(cngaccumass*100);
    if(fixdata>tmpdata)
    {
      tmpdata=fixdata-tmpdata;
      for(count=0;count<tmpdata;count++)
      {
        cngaccumass += 0.01;
        cngaccmoney=cngaccumass*unitprice;
        SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
        _delay_ms(20);
      }
    }
    cngaccmoney=ZHYworkstatus.workfixmoney;
    cngaccumass=cngaccmoney/unitprice;
    SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  }
  else if(ZHYworkstatus.workfixtype==0)
  {
    rdtime=20;
    counttime=0;
    cngaccumassbak=cngaccumass;
    cngaccmoneybak=cngaccmoney;
    while(rdtime--)
    {  
      _delay_ms(100);
      if(!MassFlowModbusReadTotal(&rdmasstotal)) 
      {
        cngaccumass=cngaccumassbak;
        break;
      }
      cngaccumass = rdmasstotal*calfactor;
      cngaccumass = (unsigned long)(cngaccumass*100);
      cngaccumass = (double)cngaccumass/100.00;
//    Uart0Select(USART0_BANK);
//    Uart0Printf("the read value is:%f\n\r",cngaccumass);
//    Uart0Printf("the bake value is:%f\n\r",cngaccumassbak);
//    Uart0Select(USART0_CARD);    
      if(cngaccumass==cngaccumassbak) 
      {  
        if(++counttime>3) 
        {
          break;
        }
        continue;
      }
      else
      {
        counttime=0;
      }
      cngaccumassbak=cngaccumass;
    } 
    cngaccmoney = cngaccumass*unitprice;
    cngaccmoney = (unsigned long)(cngaccmoney*100+0.5);
    cngaccmoney = (double)cngaccmoney/100.00;
  }
  FYD12864ClearLine(1);
  FYD12864DispPrintf(1,1,"״̬: ��������");
  FYD12864DispPrintf(2,1,"����:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"���:%.02f",cngaccmoney);
  SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  cngaccumassrealtime=(unsigned long)(cngaccumass*100+0.5);
  cngaccumoneyrealtime=(unsigned long)(cngaccmoney*100+0.5);
  LongToCharBuffer(cngaccumassrealtime,&ZHYrealdata.ZHYMass[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYrealdata.ZHYSum[0],4);
  LongToCharBuffer(cngaccumassrealtime,&ZHYstorerecord.RecMassTotal[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYstorerecord.RecMoneyTotal[0],4);
  ZHYworkstatus.workstatus=0x04;   
}
void ZHYWorkFinishStore(void)
//�洢
{
  unsigned long storetime;
  //�洢
  storetime=EepromReadSystemPara(6031);
  storetime+=1;
  __disable_interrupt();
  ZhongHaiYouWriteFlowRec(storetime,&ZHYstorerecord.RecUID[0]);
  __enable_interrupt();
  ZHYrealdata.ZHYWorkStatus=0x00;
  EepromWriteSystemPara(6031,&storetime); 
}
#endif

//���������ģ�����
#if 1
unsigned int simulatortime=0;//ģ��ʱ��
unsigned long currentsimulatortime;//��ȡʱ��

void ZHYWorkReady(void)
{
  unsigned long temp;
  unsigned long rd;
  FYD12864ClrScreen();
  FYD12864DispPrintf(1,1,"״̬: ����ǰ���");
  memset(&ZHYrealdata.ZHYWorkStatus,0,sizeof(struct ZHYRealRecord));
  memset(&ZHYstorerecord.RecUID[0],0,sizeof(struct ZHYStoreRecord));
  ZHYrealdata.ZHYCmd=0x02;
  ZHYrealdata.ZHYWorkStatus=0x01;
  cngaccumass=0;
  cngaccmoney=0;
  cngaccumassbak=0;
  cngaccmoneybak=0;
  
  simulatortime=0;
  currentsimulatortime=1;
  ZHYworkstatus.workstatus=0x02;
  uart0.waitflag=0;
  
    //��ȡ����ϵ��
  temp=EepromReadSystemPara(5001);
  rd=EepromReadSystemPara(5002);
  calfactor=(double)rd/temp;
   //����������������뵥��
  maxonevolume=EepromReadSystemPara(5003);
  maxonemoney=EepromReadSystemPara(5004);
  rd=EepromReadSystemPara(4001);//�޿�����
  unitprice=(double)rd/100.00;
  
  unitprice=10.0;
  calfactor=1;
  
  if(uart0.recbuf[11]==0x00)
  {
    ZHYworkstatus.workfixtype=0x00;
  }
  else if(uart0.recbuf[11]==0x01)//������
  {
    ZHYworkstatus.workfixtype=0x01;
    
    rd=CharBufferToLong(&uart0.recbuf[12],4);
    ZHYworkstatus.workfixvolume=(float)rd/100.00;
      
  }
  else if(uart0.recbuf[11]==0x02)//�����
  {
    ZHYworkstatus.workfixtype=0x02;
    
    rd=CharBufferToLong(&uart0.recbuf[12],4);
    ZHYworkstatus.workfixmoney=(float)rd/100.00;
    ZHYworkstatus.workfixvolume=(float)ZHYworkstatus.workfixmoney/unitprice; 
    
    Uart0Select(USART0_BANK);
    Uart0Printf("the fix volume is:%.02f\n\r",ZHYworkstatus.workfixvolume);
    Uart0Printf("the fix money is:%.02f\n\r",ZHYworkstatus.workfixmoney);
    Uart0Select(USART0_CARD);
    
    /*
    rdvalue.rdcv[0]=uart0.recbuf[15];
    rdvalue.rdcv[1]=uart0.recbuf[14];
    rdvalue.rdcv[2]=uart0.recbuf[13];
    rdvalue.rdcv[3]=uart0.recbuf[12];
    ZHYworkstatus.workfixvolume=rdvalue.rdfv;
    Uart0Select(USART0_BANK);
    Uart0Printf("the fix money1 is:%ld\n\r",rd);
    Uart0Printf("the fix money is:%.02f\n\r",ZHYworkstatus.workfixmoney);
    Uart0Select(USART0_CARD); */
    
  }

  LongToCharBuffer((unsigned long)(unitprice*100),&ZHYstorerecord.RecUnitPrice[0],4); 
  LongToCharBuffer((unsigned long)(unitprice*100),&ZHYrealdata.ZHYUnitPrice[0],4);
  
  FYD12864ClearLine(1);
  FYD12864DispPrintf(1,1,"״̬: ���ڼ���");
  Timer2ClcTick(0);//��ģ�ⶨʱ��  
}

void ZHYWorkReadScanBuffer(void)
{
}

void ZHYWork(void)
{
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  /*************************
  //���ۼƼ�������
  **************************/
//  currentsimulatortime=Timer2GetTick()/100;
  if(currentsimulatortime>simulatortime)
  {
    cngaccumass += 0.03;
    cngaccumass =cngaccumass*calfactor;
    cngaccumass =(unsigned long)(cngaccumass*100+0.5);
    cngaccumass =(double)cngaccumass/100.00;
    simulatortime++;
    currentsimulatortime=currentsimulatortime+1;
  }
  
  //������
  if(ZHYworkstatus.workfixtype==0x01)
  {
    if((cngaccumass+0.06)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      return;
    }
  }
  //�����
  if(ZHYworkstatus.workfixtype==0x02)
  {
    if((cngaccumass+0.06)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      return;
    }
  }
  if(uart0.intflag==0xa1)
    return;
  cngaccmoney = cngaccumass*unitprice;  
  cngaccmoney = (unsigned long)(cngaccmoney*100+0.5);
  cngaccmoney = (double)cngaccmoney/100.00;
  SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);  
  FYD12864DispPrintf(2,1,"����:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"���:%.02f",cngaccmoney);
  cngaccumassrealtime=(unsigned long)(cngaccumass*100+0.5);
  cngaccumoneyrealtime=(unsigned long)(cngaccmoney*100+0.5);
  LongToCharBuffer(cngaccumassrealtime,&ZHYrealdata.ZHYMass[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYrealdata.ZHYSum[0],4);  
}
void ZHYWorkFinish(void)
{
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  unsigned long fixdata;
  unsigned long tmpdata;
  unsigned long count;
//  unsigned long rd;
  if(ZHYworkstatus.workfixtype==0x01)//������ 
  {
    fixdata=(unsigned long)(ZHYworkstatus.workfixvolume*100)-1;
    tmpdata=(unsigned long)(cngaccumass*100);
    if(fixdata>tmpdata)
    {
      tmpdata=fixdata-tmpdata;
      for(count=0;count<tmpdata;count++)
      {
        cngaccumass += 0.01;
        cngaccmoney=cngaccumass*unitprice;
        SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
      }
    }
    cngaccumass=ZHYworkstatus.workfixvolume;
    cngaccmoney= cngaccumass*unitprice;
    SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  }
  else if(ZHYworkstatus.workfixtype==0x02)//�����
  {
    fixdata=(unsigned long)(ZHYworkstatus.workfixvolume*100)-1;
    tmpdata=(unsigned long)(cngaccumass*100);
    if(fixdata>tmpdata)
    {
      tmpdata=fixdata-tmpdata;
      for(count=0;count<tmpdata;count++)
      {
        cngaccumass += 0.01;
        cngaccmoney=cngaccumass*unitprice;
        SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
      }
    }
    cngaccmoney=ZHYworkstatus.workfixmoney;
    cngaccumass=cngaccmoney/unitprice;
    SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  }
  FYD12864ClearLine(1);
  FYD12864DispPrintf(1,1,"״̬: ��������");
  FYD12864DispPrintf(2,1,"����:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"���:%.02f",cngaccmoney);
  SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  cngaccumassrealtime=(unsigned long)(cngaccumass*100+0.5);
  cngaccumoneyrealtime=(unsigned long)(cngaccmoney*100+0.5);
  LongToCharBuffer(cngaccumassrealtime,&ZHYrealdata.ZHYMass[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYrealdata.ZHYSum[0],4);
  LongToCharBuffer(cngaccumassrealtime,&ZHYstorerecord.RecMassTotal[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYstorerecord.RecMoneyTotal[0],4);
  //�洢
  ZHYrealdata.ZHYWorkStatus=0x00; 
}
void ZHYWorkFinishStore(void)
{
}
#endif

bool ZhongHaiYouWriteFlowRec(unsigned long recNum, unsigned char *pstr)
//����:�洢��ˮ��¼
{
  unsigned long addr;
  unsigned long recid; 

  recid = recNum%FlashFlowRecMaxID;  //119808  //��������--long add
  if(recid==0) recid = FlashFlowRecMaxID;
  addr = FlashFlowRecSADDR + (recid-1)*FlashFlowRecLen;  
  return JS28F128FlashWriteBuf(addr,sizeof(struct ZHYStoreRecord),pstr);
}

bool ZhongHaiYouReadFlowRec(unsigned long recNum,unsigned char *pstr)
//���ã�����ˮ��¼
{
  unsigned long addr;
  recNum = recNum%FlashFlowRecMaxID;	
  if(recNum==0) recNum = FlashFlowRecMaxID;
  addr = FlashFlowRecSADDR + (recNum-1)*FlashFlowRecLen;
  JS28F128FlashReadBuf(addr,sizeof(struct ZHYStoreRecord),pstr); 
  if(pstr[0] == 0xFF)
  {
    return true;
  }
  return true;
}