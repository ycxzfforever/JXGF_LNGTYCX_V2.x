#include "kernel.h"

void ZHYStartWork(void)
/*
中海油正星模块
命令0x01--请求加气
*/
{
  unsigned char array[5];
  array[0]=0x01;
  array[1]=0x77;
  array[2]=0x88;
  array[3]=0x99;
  ZHYResCom(array,4);   
}

void ZHYGetStaus(void)
/*
中海油正星模块
命令0x02--取得当前加气状态
*/
{

}

void ZHYGetRecord(void)
/*
中海油正星模块
命令0x03--获取加气记录
*/
{
  unsigned char array[20];
  array[0]=0x03;
  array[1]=0x01;
  
  array[2]=0x00;
  array[3]=0x00;
  array[4]=0x00;
  array[5]=0x00;
  
  array[6]=0x00;
  array[7]=0x00;
  array[8]=0x00;
  array[9]=0x00;
  
  array[10]=0x00;
  array[11]=0x00;
  array[12]=0x00;
  array[13]=0x00;
  
  array[14]=0x00;
  array[15]=0x00;
  
  ZHYResCom(array,16);  
}

void ZHYGetRecordOK(void)
/*
中海油正星模块
命令0x04--获取交易记录成功
*/
{
  unsigned char array[5];
  array[0]=0x04;
  array[1]=0x77;
  array[2]=0x88;
  array[3]=0x99;
  ZHYResCom(array,4);   
}

void ZHYReadPara(unsigned char subcom)
/*
中海油正星模块
命令0x05--读参数函数
*/
{
  unsigned long readvalue;
  unsigned char array[10];
  unsigned char len;
  array[0]=0x05;
  array[1]=subcom;
  switch(subcom)
  {
  case 0x00://读单价
    readvalue=EepromReadIDValue(6002);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x01://版本号
    readvalue=EepromReadIDValue(6003);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x02://取得打印设置
    readvalue=EepromReadIDValue(6004);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x03://计量单位
    readvalue=EepromReadIDValue(6005);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x04://取得累计
    readvalue=EepromReadIDValue(6006);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x05://取得压力系数
    readvalue=EepromReadIDValue(6007);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x06://取得仪表系数
    readvalue=EepromReadIDValue(6008);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x07://密度
    readvalue=EepromReadIDValue(6009);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x08://高压保护值
    readvalue=EepromReadIDValue(6010);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x09://脉冲当量
    readvalue=EepromReadIDValue(6011);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0a://取得换阀延迟时间
    readvalue=EepromReadIDValue(6012);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x0b://过流保护值
    readvalue=EepromReadIDValue(6013);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0c://低压阀阀值
    readvalue=EepromReadIDValue(6014);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0d://中压阀阀值
    readvalue=EepromReadIDValue(6015);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0e://高压阀阀值
    readvalue=EepromReadIDValue(6016);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0f://阀门关断判断值
    readvalue=EepromReadIDValue(6017);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x10://超压保护值
    readvalue=EepromReadIDValue(6018);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x11://失压保护值
    readvalue=EepromReadIDValue(6019);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x12://使用限制
    readvalue=EepromReadIDValue(6020);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x13://取得加气方式
    readvalue=EepromReadIDValue(6021);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x14://取得关阀参数
    readvalue=EepromReadIDValue(6022);
    LongToCharBuffer(readvalue,&array[2],3);
    len=5;
    break;
  case 0x15://取得阀数量
    readvalue=EepromReadIDValue(6023);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x16://取得目标压力
    readvalue=EepromReadIDValue(6024);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x17://取得压力
    readvalue=EepromReadIDValue(6025);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x18://设置枪号
//    readvalue=CharBufferToLong(&uart0.recbuf[8],4);//
//    EepromWriteIDValue(6026,readvalue);
    readvalue=EepromReadIDValue(6026);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x1d://班号设置
//    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
//    EepromWriteIDValue(6030,readvalue);
    readvalue=EepromReadIDValue(6031);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break; 
  default:
    break;
  }
  ZHYResCom(array,len);
}

void ZHYWritePara(unsigned char subcom)
/*
中海油正星模块
命令0x06--写参数函数
*/
{
  unsigned long readvalue;
  unsigned char array[5];
  switch(subcom)
  {
  case 0x00:
    readvalue=HexToLong(&uart0.recbuf[8]);
    EepromWriteIDValue(6002,readvalue);
    break;
  case 0x02://取得打印设置
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6004,readvalue);
    break;
  case 0x03://计量单位
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6005,readvalue);
    break;
  case 0x05://取得压力系数
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6007,readvalue);
    break;
  case 0x06://取得仪表系数
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6008,readvalue);
    break;
  case 0x07://密度
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6009,readvalue);
    break;
  case 0x08://高压保护值
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6010,readvalue);
    break;
  case 0x09://脉冲当量
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6011,readvalue);
    break;
  case 0x0a://取得换阀延迟时间
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6012,readvalue);
    break;
  case 0x0b://过流保护值
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6013,readvalue);
    break;
  case 0x0c://低压阀阀值
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6014,readvalue);
    break;
  case 0x0d://中压阀阀值
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6015,readvalue);
    break;
  case 0x0e://高压阀阀值
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6016,readvalue);
    break;
  case 0x0f://阀门关断判断值
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6017,readvalue);
    break;
  case 0x10://超压保护值
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6018,readvalue);
    break;
  case 0x11://失压保护值
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6019,readvalue);
    break;
  case 0x12://使用限制
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6020,readvalue);
    break;
  case 0x13://取得加气方式
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6021,readvalue);
    break;
  case 0x14://取得关阀参数
     readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6022,readvalue);
    break;
  case 0x15://取得阀数量
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6023,readvalue);
    break;
  case 0x16://取得目标压力
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6024,readvalue);
    break;
  case 0x17://取得压力
    readvalue=CharBufferToLong(&uart0.recbuf[8],4);
    EepromWriteIDValue(6025,readvalue);
    break;
  case 0x18://设置枪号
    readvalue=CharBufferToLong(&uart0.recbuf[8],4);//
    EepromWriteIDValue(6026,readvalue);
    break;
  case 0x19://用户密码
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6027,readvalue);
    break;
  case 0x1a://管理密码
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6028,readvalue);
    break;
  case 0x1b://计量密码
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6029,readvalue);
    break;
  case 0x1c://出厂密码
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6030,readvalue);
    break;
  case 0x1d://班号设置
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6031,readvalue);
    break;
  default:
    break;
  }
  array[0]=0x06;
  array[1]=0x77;
  array[2]=0x88;
  array[3]=0x99;
  ZHYResCom(array,4);
}

void ZHYStopWork(void)
/*
中海油正星模块
命令0x09--停止加气
*/
{
  unsigned char array[5];
  array[0]=0x09;
  array[1]=0x77;
  array[2]=0x88;
  array[3]=0x99;
  Timer2Off();
  ZHYResCom(array,4);  
}

void ZHYPassword(unsigned char subcom)
/*
中海油正星模块
命令0x0d--参数密码判断
*/
{
  unsigned long readvalue;
  unsigned long initvalue;
  unsigned char array[6];
  unsigned char result=0x55;
  switch(subcom)
  {
  case 0x00:
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    initvalue=EepromReadIDValue(6027);
    if(readvalue==initvalue)
    {
      result=0xaa;
    }
    break;
  case 0x01:
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    initvalue=EepromReadIDValue(6028);
    if(readvalue==initvalue)
    {
      result=0xaa;
    }
    break;
  case 0x02:
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    initvalue=EepromReadIDValue(6029);
    if(readvalue==initvalue)
    {
      result=0xaa;
    }
    break;
  case 0x03:
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    initvalue=EepromReadIDValue(6030);
    if(readvalue==initvalue)
    {
      result=0xaa;
    }
    break;
  default:
    break;
  } 
  array[0]=0x0d;
  if(result==0xaa)
  {
    array[1]=0x77;
    array[2]=0x88;
    array[3]=0x99;
  }
  else
  {
    array[1]=0x55;
    array[2]=0x55;
    array[3]=0x55;
  }
  ZHYResCom(array,4);
}

void ZHYQueryRecord(unsigned char subcom)
/*
中海油正星模块
0x10--历史记录查询
*/
{
  unsigned char array[20];
  struct ZHYStoreRecord temprecord;
  unsigned long rdnumber;
  unsigned long rdvalue;
  array[0]=0x10;
  array[1]=subcom;
  rdnumber=EepromReadSystemPara(6031);
  rdnumber=rdnumber-subcom+1;
  ZhongHaiYouReadFlowRec(rdnumber,&temprecord.RecUID[0]);
  rdnumber=CharBufferToLong(&temprecord.RecMassTotal[0],4);
  FYD12864ClearLine(4);
  FYD12864DispPrintf(2,1,"M:%ld",rdnumber);
  rdvalue=Hex2Bcd(rdnumber);
  array[2]=(unsigned char)(rdvalue>>16);
  array[3]=(unsigned char)(rdvalue>>8);
  array[4]=(unsigned char)rdvalue;//气量
  
  rdnumber=CharBufferToLong(&temprecord.RecMoneyTotal[0],4);
  FYD12864ClearLine(3);
  FYD12864DispPrintf(3,1,"M:%ld",rdnumber);
  rdvalue=Hex2Bcd(rdnumber);
  array[5]=(unsigned char)(rdvalue>>16); 
  array[6]=(unsigned char)(rdvalue>>8);
  array[7]=(unsigned char)rdvalue;//金额
  
  rdnumber=CharBufferToLong(&temprecord.RecUnitPrice[0],4);
  rdvalue=Hex2Bcd(rdnumber);
  array[8]=(unsigned char)(rdvalue>>8); 
  array[9]=(unsigned char)rdvalue;
  ZHYResCom(array,10);    
}

void ZHYComFenXi(void)
/*
中海油正星模块
命令接卸
*/
{
  bool flag;
  unsigned char com;
  unsigned char subcom;
  flag=false;
  flag=ZHYComRec();
  if(flag==true)
  {
    uart0.sendbuf[3]=uart0.recbuf[3];
    subcom=uart0.recbuf[7];
    com=uart0.recbuf[6];
    switch(com)
    {      
      case 0x01:
        ZHYStartWork();//请求加气命令
        ZHYWorkReady();
//        ZhongHaiYouWorkReady();
        break;
      case 0x02://实时数据
        ZHYResCom(&ZHYrealdata.ZHYCmd,20);
        break;
      case 0x03:
        ZHYGetRecord();//获取交易记录
        break;
      case 0x04:
        ZHYGetRecordOK();//获取交易数据成功
        break;
      case 0x05:
        ZHYReadPara(subcom);//读参数
        break;
      case 0x06:
        ZHYWritePara(subcom);//写参数
        break;
      case 0x09:
        ZHYStopWork();//停止加气
        if(ZHYworkstatus.workstatus==0x02)
        {
          ZHYworkstatus.workstatus=0x03;
          ValveAllOff();//关闭所有电磁阀
        }
        ZHYworkstatus.workfixtype=0;
        break;
      case 0x0d:
        ZHYPassword(subcom);//密码比对
        break;  
      case 0x10:
        ZHYQueryRecord(subcom);
        break;
      default:
        break;
    }//switch
  }//if(flag==true)
}