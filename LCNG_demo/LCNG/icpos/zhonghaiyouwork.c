#include "kernel.h"

#define TOTAL_MAX1       4200000000//最大加气量和最大加气金额
struct ZHYRealRecord   ZHYrealdata;//加气实时记录
struct ZHYStoreRecord  ZHYstorerecord;//加气记录
struct ZHYWorkStatus   ZHYworkstatus;//加气状态
float  cngaccumass=0;//加气累计气量
float  unitprice;//单价
float  cngaccumassbak=0;  
double cngaccmoneybak=0;
double cngaccmoney=0;//加气金额
double calfactor;//加气过程比例系数
unsigned long maxonevolume;//单次加气量限制
unsigned long maxonemoney;// 单次加气金额限制

#define  F_CPU    7372800

#define _delay_us(x)\
__delay_cycles( (unsigned long) ( (double)(F_CPU)*((x)/1000000.0) + 0.5))  

#define _delay_ms(x)\
__delay_cycles( (unsigned long) ( (double)(F_CPU)*((x)/1000.0) + 0.5))  

#define _delay_s(x) \
__delay_cycles( (unsigned long) ( (double)(F_CPU)*((x)/1.0) + 0.5))   
//下面程序是真实加气
#if 0

void ZHYWorkReady(void)
//加气前准备
{
  unsigned long temp;
  unsigned long rd;
  unsigned long currentpressure;//当前压力
//  modbusTReg rdvalue;
  FYD12864ClrScreen();
  FYD12864DispPrintf(1,1,"状态: 加气前检查");
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
  else if(uart0.recbuf[11]==0x01)//定气量
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
  else if(uart0.recbuf[11]==0x02)//定金额
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
  //加气累计金额等检查
  //加气前准备
  ZhongHaiYouInit();
  //获取比例系数
  temp=EepromReadSystemPara(5001);
  rd=EepromReadSystemPara(5002);
  calfactor=(double)rd/temp;
  //流量计清零累计加气气量
  if(!MassFlowClearTotal())
  {
    FYD12864DispPrintf(1,1,"流量计清零错误");
    return;
  }
  //最大金额与最大加气量与单价
  maxonevolume=EepromReadSystemPara(5003);
  maxonemoney=EepromReadSystemPara(5004);
  rd=EepromReadSystemPara(4001);//无卡单价
  unitprice=(double)rd/100.00;
  
  unitprice=10.0;
  calfactor=1;
  LongToCharBuffer((unsigned long)(unitprice*100),&ZHYstorerecord.RecUnitPrice[0],4); 
  LongToCharBuffer((unsigned long)(unitprice*100),&ZHYrealdata.ZHYUnitPrice[0],4);
  //加气前压力
  if(!PressureGetValue(true,&currentpressure))
  {   
  }
  else
  {
  }
  FYD12864ClearLine(1);
  FYD12864DispPrintf(1,1,"状态: 正在加气");
//  FYD12864ClearLine(1);
//  FYD12864DispPrintf(1,1,"状态: 正在加气");
  Timer2ClcTick(0);//打开模拟定时器  
}

void ZHYWorkReadScanBuffer(void)
//读流量、气量
{
//  UCSR0B_RXCIE0=0;//disable uart0;
  if(!MassFlowReadScanBuffer())
  {
    FYD12864DispPrintf(1,1,"读流量计错误");
    ValveAllOff();//关闭所有电磁阀
    Timer2Off();
    ZHYworkstatus.workstatus=0x03;
  }
//  UCSR0B_RXCIE0=1;//enable uart0
}
void ZHYWork(void)
//根据气量、流量计算
{
  float  rdmasstotal;//读加气机累计加气气量
  float  rdmassflow;//读加气机流速
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  /*************************
  //读累计加气气量,并计算总金额
  **************************/
  rdmasstotal=MassFlowReadTotal();
  cngaccumass=rdmasstotal*calfactor;
  cngaccumass=(unsigned long)(cngaccumass*100);
  cngaccumass=(double)cngaccumass/100.00;
  
  cngaccmoney = cngaccumass*unitprice;  
  cngaccmoney = (unsigned long)(cngaccmoney*100+0.5);
  cngaccmoney = (double)cngaccmoney/100.00;
  /****************************
  //瞬时数据异常
  *****************************/ 
  if(cngaccmoney>(cngaccmoneybak+(unitprice/2)))     
  {
    FYD12864DispPrintf(1,1,"异常错误");
    cngaccumass=cngaccumassbak;
    cngaccmoney=cngaccmoneybak;
    Timer2Off();
    ValveAllOff();//关闭所有电磁阀
    ZHYworkstatus.workstatus=0x03;
  }
  else   
  {
    cngaccmoneybak=cngaccmoney;
    cngaccumassbak=cngaccumass;
  }
  /****************************
  //更新SPLC
  *****************************/
  SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  /****************************    
  //最大单此量
  *****************************/
  /****************************
  //定量
  *****************************/
  //定气量
  if(ZHYworkstatus.workfixtype==0x01)
  {
    if((cngaccumass+ZHYworkstatus.workCofficient)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      Timer2Off();
      ValveAllOff();//关闭所有电磁阀
      return;
    }
  }
  //定金额
  if(ZHYworkstatus.workfixtype==0x02)
  {
    if((cngaccumass+ZHYworkstatus.workCofficient)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      Timer2Off();
      ValveAllOff();//关闭所有电磁阀
      return;
    }
  }
  /****************************
  //停机 切阀
  *****************************/
  rdmassflow=MassFlowReadFlowMass();
  rdmassflow=fabs(rdmassflow);
  FYD12864DispPrintf(4,1,"V:%.02f",rdmassflow);
  LongToCharBuffer((unsigned int)rdmassflow,&ZHYrealdata.ZHYMassFlow[0],2);
//  Uart0Printf("the flow:%f\r\n",rdmassflow);
  rdmassflow = (unsigned int)(rdmassflow*100+0.5);
  if(rdmassflow>maxsafemassflow)//大于安全流速
  {
    if(maxsafeflowstarttime==0)
    {
      ticks=0;//huanghao add
      maxsafeflowstarttime=Timer2GetTick();
    }
    if((Timer2GetTick()-maxsafeflowstarttime)>maxsafeflowdelaytime)
    {
      ValveAllOff();//关闭所有电磁阀
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
    FYD12864DispPrintf(1,1,"压力错误");
    ValveAllOff();//关闭所有电磁阀
    Timer2Off();
    ZHYworkstatus.workstatus=0x03;
  }
  if(!valveswitchbymassflow((unsigned int)rdmassflow))
  {
    FYD12864DispPrintf(1,1,"流速错误");
    ValveAllOff();//关闭所有电磁阀
    Timer2Off();
    ZHYworkstatus.workstatus=0x03;
  }
  
  /****************************
  //上传实时状态
  *****************************/
  FYD12864DispPrintf(2,1,"气量:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"金额:%.02f",cngaccmoney);
  cngaccumassrealtime=(unsigned long)(cngaccumass*100+0.5);
  cngaccumoneyrealtime=(unsigned long)(cngaccmoney*100+0.5);
  LongToCharBuffer(cngaccumassrealtime,&ZHYrealdata.ZHYMass[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYrealdata.ZHYSum[0],4);  
}

void ZHYWorkFinish(void)
//加气结束
{
  float  rdmasstotal;//读加气机累计加气气量
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  unsigned char rdtime;
  unsigned char counttime;
  
  unsigned long fixdata;
  unsigned long tmpdata;
  unsigned long count;
//  unsigned long rd;
  if(ZHYworkstatus.workfixtype==0x01)//定气量 
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
  else if(ZHYworkstatus.workfixtype==0x02)//定金额
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
  FYD12864DispPrintf(1,1,"状态: 加气结束");
  FYD12864DispPrintf(2,1,"气量:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"金额:%.02f",cngaccmoney);
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
//存储
{
  unsigned long storetime;
  //存储
  storetime=EepromReadSystemPara(6031);
  storetime+=1;
  __disable_interrupt();
  ZhongHaiYouWriteFlowRec(storetime,&ZHYstorerecord.RecUID[0]);
  __enable_interrupt();
  ZHYrealdata.ZHYWorkStatus=0x00;
  EepromWriteSystemPara(6031,&storetime); 
}
#endif

//下面程序是模拟程序
#if 1
unsigned int simulatortime=0;//模拟时间
unsigned long currentsimulatortime;//获取时间

void ZHYWorkReady(void)
{
  unsigned long temp;
  unsigned long rd;
  FYD12864ClrScreen();
  FYD12864DispPrintf(1,1,"状态: 加气前检查");
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
  
    //获取比例系数
  temp=EepromReadSystemPara(5001);
  rd=EepromReadSystemPara(5002);
  calfactor=(double)rd/temp;
   //最大金额与最大加气量与单价
  maxonevolume=EepromReadSystemPara(5003);
  maxonemoney=EepromReadSystemPara(5004);
  rd=EepromReadSystemPara(4001);//无卡单价
  unitprice=(double)rd/100.00;
  
  unitprice=10.0;
  calfactor=1;
  
  if(uart0.recbuf[11]==0x00)
  {
    ZHYworkstatus.workfixtype=0x00;
  }
  else if(uart0.recbuf[11]==0x01)//定气量
  {
    ZHYworkstatus.workfixtype=0x01;
    
    rd=CharBufferToLong(&uart0.recbuf[12],4);
    ZHYworkstatus.workfixvolume=(float)rd/100.00;
      
  }
  else if(uart0.recbuf[11]==0x02)//定金额
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
  FYD12864DispPrintf(1,1,"状态: 正在加气");
  Timer2ClcTick(0);//打开模拟定时器  
}

void ZHYWorkReadScanBuffer(void)
{
}

void ZHYWork(void)
{
  unsigned long cngaccumassrealtime;
  unsigned long cngaccumoneyrealtime;
  /*************************
  //读累计加气气量
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
  
  //定气量
  if(ZHYworkstatus.workfixtype==0x01)
  {
    if((cngaccumass+0.06)>ZHYworkstatus.workfixvolume)
    {
      ZHYworkstatus.workstatus=0x03;
      return;
    }
  }
  //定金额
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
  FYD12864DispPrintf(2,1,"气量:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"金额:%.02f",cngaccmoney);
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
  if(ZHYworkstatus.workfixtype==0x01)//定气量 
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
  else if(ZHYworkstatus.workfixtype==0x02)//定金额
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
  FYD12864DispPrintf(1,1,"状态: 加气结束");
  FYD12864DispPrintf(2,1,"气量:%.02f",cngaccumass);
  FYD12864DispPrintf(3,1,"金额:%.02f",cngaccmoney);
  SecDisplayShow(cngaccumass,cngaccmoney,unitprice,0,0);
  cngaccumassrealtime=(unsigned long)(cngaccumass*100+0.5);
  cngaccumoneyrealtime=(unsigned long)(cngaccmoney*100+0.5);
  LongToCharBuffer(cngaccumassrealtime,&ZHYrealdata.ZHYMass[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYrealdata.ZHYSum[0],4);
  LongToCharBuffer(cngaccumassrealtime,&ZHYstorerecord.RecMassTotal[0],4);
  LongToCharBuffer(cngaccumoneyrealtime,&ZHYstorerecord.RecMoneyTotal[0],4);
  //存储
  ZHYrealdata.ZHYWorkStatus=0x00; 
}
void ZHYWorkFinishStore(void)
{
}
#endif

bool ZhongHaiYouWriteFlowRec(unsigned long recNum, unsigned char *pstr)
//作用:存储流水记录
{
  unsigned long addr;
  unsigned long recid; 

  recid = recNum%FlashFlowRecMaxID;  //119808  //翻滚计算--long add
  if(recid==0) recid = FlashFlowRecMaxID;
  addr = FlashFlowRecSADDR + (recid-1)*FlashFlowRecLen;  
  return JS28F128FlashWriteBuf(addr,sizeof(struct ZHYStoreRecord),pstr);
}

bool ZhongHaiYouReadFlowRec(unsigned long recNum,unsigned char *pstr)
//作用：读流水记录
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