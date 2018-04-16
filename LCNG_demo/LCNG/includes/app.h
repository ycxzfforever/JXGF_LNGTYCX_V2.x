/*
 * app.h
 *
 * Created: 2012-8-16 14:11:27
 *  Author: luom
 */ 


#ifndef APP_H_
#define APP_H_

	  typedef struct _def_icardinfo_       
	  {
		unsigned char select_buf[16]; //0   2~9������
		unsigned char block1_buf[16]; //1
		unsigned char block2_buf[16]; //2
		unsigned char block4_buf[16]; //3
		unsigned char block5_buf[16]; //4
		unsigned char block6_buf[16]; //5
		unsigned char block9_buf[16]; //5
		unsigned char block13_buf[16]; //6
		unsigned char block14_buf[16]; //6
		unsigned char block16_buf[16]; //6
		unsigned char block17_buf[16]; //7
		unsigned char block18_buf[16]; //8
	  }_icard_data;	

	  typedef struct _def_cpucardinfo_       
	  {
		//�ļ�0x15   ռ19����ַ
		unsigned char CpuCardNum[8]; //CPU������
		unsigned char CpuCardType;//������
		unsigned char CpuCompanyNum[2];//��˾���
		unsigned char CpuStationNum[2];//����վ���
		unsigned char CpuCardPassword[3];//����
		unsigned char CpuSteelDate[3]; //��ƿ��������
		//�ļ�0x16  ռ42����ַ
		unsigned char CpuLastMon[4]; //���������
		unsigned char CpuLastVol[4]; //����������
		unsigned char CpuLastTime[6]; //���ʹ��ʱ��
		unsigned char CpuLastStation[2]; //����������վ���
		unsigned char CpuGunNum;//ǹ��
		unsigned char CpuSerialNum[4];//��ˮ��
		unsigned char CpuValidity; //��״̬		0x01��������0x02�ҿ�
		unsigned char CpuBalance[4]; //�����
		unsigned char CpuLastBalance[4];//�ϴ����
		unsigned char CpuLastOper;//�ϴβ�����ʽ1������2��ֵ��3Ӳ����ң�4��̨��ң�5����
		unsigned char CpuLastOperTime[6]; //�ϴβ���ʱ��
		unsigned char CpuOper;//����������1������2��ֵ��3Ӳ����ң�4��̨��ң�5����
		unsigned char CpuRechargeMon[4];//����ֵ���
	  }_cpucard_data;

	  enum _card_rderr_type_
	  {
		  _rd_chk_ok = 1,
		  _rd_chk_h_card,
		  _rd_chk_g_card,
		  _rd_chk_type_err,
		  _rd_chk_b_less, //����
		  _rd_chk_b_excess, //���˳���-------ֻ��Լ��˿�
		  _rd_chk_b_unm,  //��һ��
		  _rd_chk_b_err,  //����쳣
		  _rd_chk_gnum_err,  //ǹ�Ų�ƥ��
		  _rd_chk_serialnum_err,  //���кŲ�ƥ�� 
		  _rd_chk_cylinder_err,  //��ƿ����
		  _rd_chk_cardnum_err,  //���Ŵ���
		  _rd_chk_jizhangka_err,  //���˿����ܼ���
	  };

	  enum _cpu_ready_wterr_type_
	  {
		  _wirte_ready_ok = 1,//׼��OK 
		  _wirte_ready_cpumf_err,//CPU-MFʧ��
		  _wirte_ready_cpudf_err,//CPU-DFʧ��
		  _wirte_ready_cpugetrand_err,//CPU��ȡ�����ʧ��
		  _wirte_ready_psammf_err,//PSAM-MFʧ��
		  _wirte_ready_psamdf_err,//PSAM-DFʧ��
		  _wirte_ready_psamdeslnit_err,//PSAM-DES��ʼ��ʧ��
		  _wirte_ready_psamdes_err,//PSAM-DESʧ��
		  _wirte_ready_cpuverity_err,//CPU�ⲿ��֤ʧ��
	  };

	typedef struct def_CNG_RunSRec    
	{
	  unsigned char   runRFCardNum[4]; //IC������
	  unsigned char   runRFCardType;   //IC������
	  unsigned char   runRFCardPreM[4];//IC����ǰ���
	  unsigned char   runCarNum[2];    //���ź���λ
	  unsigned char   runPressure[2];  //��ǰѹ��
	  unsigned char   runSpeed[2];     //��ǰ����
	  unsigned char   runCNGAmount[6]; //��ǰǹ��(���9999999999.99)
	  unsigned char   brunRFCard;      //��״̬��1���롢0������
	  unsigned char   brunLV;          //��ѹ������״̬��1����0�أ�
	  unsigned char   brunMV;          //��ѹ������״̬��1����0�أ�
	  unsigned char   brunHV;          //��ѹ������״̬��1����0�أ�
	  unsigned char   brunCNG;         //����״̬��1���ڼ�����0����������
	  unsigned char   brunPrice[2];    //��ǰ��������
	  unsigned char   brunCNGNum[4];   //��ǰ������
	  unsigned char   brunCNGCash[4];  //��ǰ�������
	  unsigned char   brunRDCardM[4];  //��ǰ���

	 //zl add 2013-8-21
	  unsigned char  CalutUnit;  //������λ
	  unsigned char  MechType; //����������
	  unsigned char  StationNum;  //����վ���
	  //zl 
	}_lcng_runs_rec;//19����

	typedef struct def_LCNG_TRANS_REC
	{
	  unsigned char  UID[6];             //UID������¼ǹ��ˮ��
	  unsigned char  UsrID;              //���
	  unsigned char  CUID[6];            //UID���¼ǹ��ˮ��
	  unsigned char  RecStartTime[6];    //������ʼʱ��
	  unsigned char  RecEndTime[6];      //��������ʱ��
	  unsigned char  RecICNum[4];        //IC����
	  unsigned char  RecICType;          //����
	  unsigned char  RecICPreMoney[4];   //����ǰ��� 
	  unsigned char  RecCNGNum[4];       //���μ�����
	  //#if VISION_EN                 
	  //unsigned char  RecPrice[4];	     //���μ�������	
	  //#else
	  unsigned char  RecPrice[2];	       //���μ�������	
	  //#endif  
	  unsigned char  RecCNGMoney[4];     //���μӽ��  
	  unsigned char  RecICCurMoney[4];   //���μ��������
	  unsigned char  RecICAccFTimes[3];  //�ۼƳ�ֵ����
	  unsigned char  RecICAccFMoney[6];  //�ۼƳ�ֵ���
	  unsigned char  RecICAccConNum[3];  //�ۼ����Ѵ��� *	
	  unsigned char  RecICAccConCNG[6];  //�ۼ��������� *	
	  unsigned char  RecICAccConMoney[6];//�ۼ����ѽ�� *
	  unsigned char  RecICPreOverage[4]; //�ڳ����
	  unsigned char  RecICEndOverage[4]; //������� *  	
	  unsigned char  RecCarNum[2];       //���ź�4λ
	  unsigned char  RecPrePressure[2];  //����ǰѹ��
	  unsigned char  RecEndPressure[2];  //������ѹ��
	  unsigned char  RecStartAccCNG[6];  //����ǰǹ��
	  unsigned char  RecEndAccCNG[6];    //������ǹ��
	  unsigned char  bRecCardOK;         //�Ƿ�ҿ���
	  unsigned char  bRecPrint;          //�Ƿ��ӡ��
	  unsigned char  lstCardType;        //bRecTransOK;//���ϴμ�¼(��Ϊ������ֵ)
         //zl add 2013-8-21
	  unsigned char  CalutUnit;  //������λ
	  unsigned char  MechType; //����������
	  unsigned char  StationNum;  //����վ���
	  //unsigned char  bak[2];
	}_lcng_trans_rec;//��ˮ��¼


	typedef struct def_LCNG_SHIFT_TRANS_Rpt
	{
	  unsigned char  UID[6];                //����ˮ��
	  unsigned char  UsrID;                 //���
	  unsigned char  RptStartTime[6];       //��ʼʱ��
	  unsigned char  RptEndTime[6];         //����ʱ��
	  unsigned char  RptStartCNG[6];        //��ʼǹ��
	  unsigned char  RptEndCNG[6];          //����ǹ��
	  unsigned char  RptTotalCNG[4];        
	  unsigned char  RptUseCNG[4];          //����
	  unsigned char  RptMoney[4];           //���
	  unsigned char  RptCash[4];            //�ֽ�
	  unsigned char  RptCNGNum[2];          //����
	  unsigned char  bRptTrans;
	  //zl add 2013-8-21
	   unsigned char  CalutUnit;  //������λ
	  unsigned char  MechType; //����������
	  unsigned char  StationNum;  //����վ���
	}_lcng_shift_trans_Rpt;//���ۼ�¼

  ex   unsigned long    vulpsw;
  ex   _icard_data      cur_card_data; 
  ex  _cpucard_data  cpu_card_data;
  //ex   _lcng_trans_rec  cur_lcng_t_rec;   
  ex   _lcng_runs_rec   cur_lcng_r_rec;   
  void oapp_card_chk_pro(void);
  //void oapp_card_update_pro(void);
  void oapp_iccard_select_pro(void);
  //void oapp_update_card_info(bool bsave, bool bungray);
  void oapp_update_card_info(void);
  void oapp_card_serialnumchk_pro(void);
  void oapp_iccard_ready_execute(void);
  void oapp_iccard_update_execute(void);
  void oapp_iccard_ungray_pro(void);    //�ֳ����
  void oapp_iccard_tip_msg(unsigned char xpos, unsigned char ypos, unsigned char id);
#endif /* APP_H_ */