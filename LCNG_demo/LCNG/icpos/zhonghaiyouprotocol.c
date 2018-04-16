#include "kernel.h"

void ZHYStartWork(void)
/*
�к�������ģ��
����0x01--�������
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
�к�������ģ��
����0x02--ȡ�õ�ǰ����״̬
*/
{

}

void ZHYGetRecord(void)
/*
�к�������ģ��
����0x03--��ȡ������¼
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
�к�������ģ��
����0x04--��ȡ���׼�¼�ɹ�
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
�к�������ģ��
����0x05--����������
*/
{
  unsigned long readvalue;
  unsigned char array[10];
  unsigned char len;
  array[0]=0x05;
  array[1]=subcom;
  switch(subcom)
  {
  case 0x00://������
    readvalue=EepromReadIDValue(6002);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x01://�汾��
    readvalue=EepromReadIDValue(6003);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x02://ȡ�ô�ӡ����
    readvalue=EepromReadIDValue(6004);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x03://������λ
    readvalue=EepromReadIDValue(6005);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x04://ȡ���ۼ�
    readvalue=EepromReadIDValue(6006);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x05://ȡ��ѹ��ϵ��
    readvalue=EepromReadIDValue(6007);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x06://ȡ���Ǳ�ϵ��
    readvalue=EepromReadIDValue(6008);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x07://�ܶ�
    readvalue=EepromReadIDValue(6009);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x08://��ѹ����ֵ
    readvalue=EepromReadIDValue(6010);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x09://���嵱��
    readvalue=EepromReadIDValue(6011);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0a://ȡ�û����ӳ�ʱ��
    readvalue=EepromReadIDValue(6012);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x0b://��������ֵ
    readvalue=EepromReadIDValue(6013);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0c://��ѹ����ֵ
    readvalue=EepromReadIDValue(6014);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0d://��ѹ����ֵ
    readvalue=EepromReadIDValue(6015);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0e://��ѹ����ֵ
    readvalue=EepromReadIDValue(6016);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x0f://���Źض��ж�ֵ
    readvalue=EepromReadIDValue(6017);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x10://��ѹ����ֵ
    readvalue=EepromReadIDValue(6018);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x11://ʧѹ����ֵ
    readvalue=EepromReadIDValue(6019);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x12://ʹ������
    readvalue=EepromReadIDValue(6020);
    LongToCharBuffer(readvalue,&array[2],1);
    len=3;
    break;
  case 0x13://ȡ�ü�����ʽ
    readvalue=EepromReadIDValue(6021);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x14://ȡ�ùط�����
    readvalue=EepromReadIDValue(6022);
    LongToCharBuffer(readvalue,&array[2],3);
    len=5;
    break;
  case 0x15://ȡ�÷�����
    readvalue=EepromReadIDValue(6023);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x16://ȡ��Ŀ��ѹ��
    readvalue=EepromReadIDValue(6024);
    LongToCharBuffer(readvalue,&array[2],2);
    len=4;
    break;
  case 0x17://ȡ��ѹ��
    readvalue=EepromReadIDValue(6025);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x18://����ǹ��
//    readvalue=CharBufferToLong(&uart0.recbuf[8],4);//
//    EepromWriteIDValue(6026,readvalue);
    readvalue=EepromReadIDValue(6026);
    LongToCharBuffer(readvalue,&array[2],4);
    len=6;
    break;
  case 0x1d://�������
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
�к�������ģ��
����0x06--д��������
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
  case 0x02://ȡ�ô�ӡ����
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6004,readvalue);
    break;
  case 0x03://������λ
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6005,readvalue);
    break;
  case 0x05://ȡ��ѹ��ϵ��
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6007,readvalue);
    break;
  case 0x06://ȡ���Ǳ�ϵ��
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6008,readvalue);
    break;
  case 0x07://�ܶ�
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6009,readvalue);
    break;
  case 0x08://��ѹ����ֵ
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6010,readvalue);
    break;
  case 0x09://���嵱��
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6011,readvalue);
    break;
  case 0x0a://ȡ�û����ӳ�ʱ��
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6012,readvalue);
    break;
  case 0x0b://��������ֵ
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6013,readvalue);
    break;
  case 0x0c://��ѹ����ֵ
    readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6014,readvalue);
    break;
  case 0x0d://��ѹ����ֵ
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6015,readvalue);
    break;
  case 0x0e://��ѹ����ֵ
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6016,readvalue);
    break;
  case 0x0f://���Źض��ж�ֵ
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6017,readvalue);
    break;
  case 0x10://��ѹ����ֵ
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6018,readvalue);
    break;
  case 0x11://ʧѹ����ֵ
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6019,readvalue);
    break;
  case 0x12://ʹ������
    readvalue=CharBufferToLong(&uart0.recbuf[8],1);
    EepromWriteIDValue(6020,readvalue);
    break;
  case 0x13://ȡ�ü�����ʽ
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6021,readvalue);
    break;
  case 0x14://ȡ�ùط�����
     readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6022,readvalue);
    break;
  case 0x15://ȡ�÷�����
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6023,readvalue);
    break;
  case 0x16://ȡ��Ŀ��ѹ��
     readvalue=CharBufferToLong(&uart0.recbuf[8],2);
    EepromWriteIDValue(6024,readvalue);
    break;
  case 0x17://ȡ��ѹ��
    readvalue=CharBufferToLong(&uart0.recbuf[8],4);
    EepromWriteIDValue(6025,readvalue);
    break;
  case 0x18://����ǹ��
    readvalue=CharBufferToLong(&uart0.recbuf[8],4);//
    EepromWriteIDValue(6026,readvalue);
    break;
  case 0x19://�û�����
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6027,readvalue);
    break;
  case 0x1a://��������
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6028,readvalue);
    break;
  case 0x1b://��������
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6029,readvalue);
    break;
  case 0x1c://��������
    readvalue=CharBufferToLong(&uart0.recbuf[8],3);
    EepromWriteIDValue(6030,readvalue);
    break;
  case 0x1d://�������
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
�к�������ģ��
����0x09--ֹͣ����
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
�к�������ģ��
����0x0d--���������ж�
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
�к�������ģ��
0x10--��ʷ��¼��ѯ
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
  array[4]=(unsigned char)rdvalue;//����
  
  rdnumber=CharBufferToLong(&temprecord.RecMoneyTotal[0],4);
  FYD12864ClearLine(3);
  FYD12864DispPrintf(3,1,"M:%ld",rdnumber);
  rdvalue=Hex2Bcd(rdnumber);
  array[5]=(unsigned char)(rdvalue>>16); 
  array[6]=(unsigned char)(rdvalue>>8);
  array[7]=(unsigned char)rdvalue;//���
  
  rdnumber=CharBufferToLong(&temprecord.RecUnitPrice[0],4);
  rdvalue=Hex2Bcd(rdnumber);
  array[8]=(unsigned char)(rdvalue>>8); 
  array[9]=(unsigned char)rdvalue;
  ZHYResCom(array,10);    
}

void ZHYComFenXi(void)
/*
�к�������ģ��
�����ж
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
        ZHYStartWork();//�����������
        ZHYWorkReady();
//        ZhongHaiYouWorkReady();
        break;
      case 0x02://ʵʱ����
        ZHYResCom(&ZHYrealdata.ZHYCmd,20);
        break;
      case 0x03:
        ZHYGetRecord();//��ȡ���׼�¼
        break;
      case 0x04:
        ZHYGetRecordOK();//��ȡ�������ݳɹ�
        break;
      case 0x05:
        ZHYReadPara(subcom);//������
        break;
      case 0x06:
        ZHYWritePara(subcom);//д����
        break;
      case 0x09:
        ZHYStopWork();//ֹͣ����
        if(ZHYworkstatus.workstatus==0x02)
        {
          ZHYworkstatus.workstatus=0x03;
          ValveAllOff();//�ر����е�ŷ�
        }
        ZHYworkstatus.workfixtype=0;
        break;
      case 0x0d:
        ZHYPassword(subcom);//����ȶ�
        break;  
      case 0x10:
        ZHYQueryRecord(subcom);
        break;
      default:
        break;
    }//switch
  }//if(flag==true)
}