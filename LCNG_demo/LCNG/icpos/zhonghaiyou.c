/*
�к���������POS��������������������壬��������������Ӧ����
*/
#undef   ZHONGHAIYOU
#ifdef   ZHONGHAIYOU
#include "kernel.h"

void main(void)
{
  unsigned int deviceaddr;
  FYD12864Init(); 
  I2cInit();
  StateMachineInit();
  bmainui = false;
  KeyBoardInit();
  JS28F128FlashInit();
  Uart0Init();
  Uart0Select(USART0_CARD);
//  Rdm208InitPsamcard();
//  Timer1Init();
  Timer2Init();//�ڼ���������ʹ��
//  Timer1CountCLr();
  MassFlowModbusInit(19200);//�����ƶ�ȡ�˿�
  AdcInit();//ADC��ʼ��
  ValveInit();
  JS28F128FlashInit();
  SecDisplayInit();
  SecDisplayShow(0,0,0,0,0);
  FYD12864DispPrintf(1,1,"��ӭ������ƽ���");
  DDRK_Bit7=1;
  PORTK_Bit7=0;
  /*
  if(MassFlowReadDeviceAddress(&deviceaddr))
  {
    if(deviceaddr!=247)
    {
      FYD12864ClearLine(4);
      FYD12864DispPrintf(4,1,"�����ƴ���");
      KeyBoardWait(KEY_RET_EV);
    }
  }
  else
  {
    FYD12864ClearLine(4);
    FYD12864DispPrintf(4,1,"�����ƴ���");
    KeyBoardWait(KEY_RET_EV);
  }
  */
  while(1)
  {
    if(uart0.intflag==0xa1)//�����ж�
    {
      PORTK_Bit7=0;
      ZHYComFenXi();
      uart0.intflag=0xa2;
      uart0.reclen=0;
      PORTK_Bit7=1;
    }
    if(ZHYworkstatus.workstatus==0x02)
    {
      ZHYWorkReadScanBuffer();
      ZHYWork();
    }
    if(uart0.intflag==0xa2)
    {
      if(ZHYworkstatus.workstatus==0x03)
      {
        ZHYWorkFinish();
        ZHYWorkFinishStore();
        ZHYworkstatus.workstatus=0x04;
      }
    }
  }
}



















    /*
    if(ZHYcngworkstatus==0x02)
    {
       ZhongHaiYouWork();
    }
    else if(ZHYcngworkstatus==0x03)
    {
      ZhongHaiYouWorkFinish();
    }
    flag=false;
    flag=ZhengXingComJudge();
    if(flag)
    {  
      framenumber=uart0receivebuffer[3];
      subcom=uart0receivebuffer[7];
      switch(uart0receivebuffer[6])
      {      
        case 0x01:
          ZhengXingStartWork(framenumber,subcom);//�����������
          ZhongHaiYouWorkReady();
          break;
 //      case 0x02:  
 //        if(ZHYcngworkstatus==0x03)
 //        {
 //          ZhengXingAnswerCom(uart0receivebuffer[3],&RealData.ZhengXingCmd,20);
 //        }
 //        break;
//          ZhongHaiYouWork();
//          ZhengXingAnswerCom(framenumber,&RealData.ZhengXingCmd,20);
//         ZhengXingGetStaus(framenumber);// ��ȡ״̬
//          break;
        case 0x03:
          ZhengXingGetRecord(framenumber,subcom);//��ȡ���׼�¼
          break;
        case 0x04:
          ZhengXingGetRecordOK(framenumber,subcom);//��ȡ�������ݳɹ�
          break;
        case 0x05:
          ZhengXingReadPara(framenumber,subcom);//������
          break;
        case 0x06:
          ZhengXingWritePara(framenumber,subcom);//д����
          break;
        case 0x0d:
          ZhengXingPassword(framenumber,subcom);//����ȶ�
          break;
        case 0x09:
          ZhengXingStopWork(framenumber,subcom);//ֹͣ����
          if(ZHYcngworkstatus==0x02)
          {
            ZHYcngworkstatus=0x03;
          }
          if(ZHYcngworkstatus==0x03)
          {
            ZHYStopCount=0;
            ValveAllOff();//�ر����е�ŷ�
          }
          break;
        case 0x10:
          ZhengXingQueryRecord(framenumber,subcom);
          break;
        default:
          break;
      }
      memset(uart0receivebuffer,0,sizeof(uart0receivebuffer));
    }
    */

#endif
