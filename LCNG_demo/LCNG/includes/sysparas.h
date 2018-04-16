/*
 * sysparas.h
 *
 * Created: 2012-8-9 10:05:53
 *  Author: luom
 */ 


#ifndef SYSPARAS_H_
#define SYSPARAS_H_

	#define  BASIC_P_G_ID				0  //ǹ��
	#define  BASIC_P_SHIFT_ID			1  //���
	#define  BASIC_P_FLOW_BAUDRATE		2//�����Ʋ�����
	#define  BASIC_P_PC_BAUDRATE		3//��̨������
	#define  BASIC_P_STATION_ID			4//����վ���
	#define  BASIC_P_MODBUS_TYPE		5//����������
	#define  BASIC_K_DISP_TYPE			6//Һ��������
	#define  BASIC_P_VCTRL_NUM			7//����������

       //��һ�õ�������˫��
	#define BASIC_PUMP_TO_MECH              8
	#define BASIC_PUMP_INTERLINE_A       9
	#define BASIC_PUMP_INTERLINE_B       10
	//Ԥ��11
	//Ԥ��12
	//Ԥ��13
	#define  BASIC_ICCARD_MIN_MOMEY		14  //����С���
	#define  BASIC_ICCARD_MAX_MOMEY		50000
	
	#define  BASIC_P_FACTOR_DENSTY		0
	#define  BASIC_P_FACTOR_CORRECT		1
	#define  BASIC_P_FACTOR_P_ZERO		2
	#define  BASIC_P_FACTOR_P_CORRECT	3
	#define  BASIC_P_FACTOR_P_L_ZERO	4 //Һ��
	#define  BASIC_P_FACTOR_P_L_CORRECT	5

	#define  BASIC_P_CARD_CHK		1//�Ƿ��ÿ�����
	#define  BASIC_P_AUTO_PRT		2//�Ƿ��Զ���ӡ
	#define  BASIC_P_BACK_GAS		4//�Ƿ��������
	#define  BASIC_P_TIME_CHK		8//�Ƿ�ʱ��У��
	#define  BASIC_P_REC_CHK		16//�Ƿ��¼У��
	#define  BASIC_P_RPT_FIRST		32//�Ƿ��������
	#define  BASIC_P_CYLINDER_TEST	64//64//�Ƿ�����ƿ����
	#define  BASIC_P_INPUT_CARNUM     128//�Ƿ����복�ƺ�
	//
	
	#define CNG_V_EX_DLY_TIME_ID		0  //�з���ʱ
	#define CNG_P_MAX_STOP_ID		    1  //��߰�ȫѹ��
	
	
	typedef struct def_SysParas
	{
		////////////���۲���///////
		float		  f_price[8]; //32
		////////////��������////////
		/*=======================
		float          b_densty;        //�ܶ�   
		float          b_coeff;         //����ϵ��
		float          b_p_zero;        //ѹ�����
		float          b_p_coeff;       //ѹ��ϵ��
		=======================*/
		float          b_factor[6]; //56
		float          b_vf_other[4];//72
		/*=============
		//ǹ�� unsigned char  b_g_id; 
		//���unsigned char  b_g_shift_id; 
		//������unsigned char  b_flow_baudrate; ///1:1200 4:4800  8:9600  16:19200 32: 38400///
		//������unsigned char  b_pc_baudrate;
		//����վ���
		//����������
		//����������
		//����������
		//����������: ����  ˫��  ����
		=====================*/
		unsigned char  b_basic_p[16];//ycx 88
		//���복�� bit0   �ÿ�����, �Զ���ӡ  ����ѹ��������  ������ʵʱ���  ��ˮ��У��
		unsigned char  b_y_or_n_operate; //ycx 89
		unsigned char  b_bak[16];//105
		////////////����ʱ����////////
		unsigned char r_comp_id;//106
		unsigned long r_cur_shift_id;  //��ǰ����ˮ��110
		unsigned long r_unupld_shift_id;//114
		unsigned long r_cur_g_id;      //��ǰǹ��ˮ�� 118
		unsigned long r_unupld_g_id;   //��ǰδ�ϴ���ǹ��ˮ��122
		unsigned char r_bak[16];//138
		////////////���Ʋ���////////
		/*================================================
		float		  c_l2mv_rate;//"��ת��ѹ����",
		float		  c_m2hv_rate;//"��ת��ѹ����",
		float		  c_hend_rate;//"�߽�ֹ����",
		float		  c_max_rate;//"��߰�ȫ����",
		================================================*/
		float         c_rate[4]; //154
		/*================================================
		unsigned int  c_hprotec_dly;//"�����ٱ�����ʱ",
		unsigned int  c_l2m_dly;//"������ѹ��ʱ",
		unsigned int  c_lclose_dly;//"���йص�ѹ��ʱ",
		unsigned int  c_m2h_dly;//"���и�ѹ��ʱ",
		unsigned int  c_mclose_dly;//"���߹���ѹ��ʱ",
		unsigned int  c_hend_dly;//"��ѹ�ض���ʱ",
		=================================================*/
		unsigned int  c_dly[6];//166
		unsigned char c_p_stop_arr[10];//176
		/*================================================
		LNG:  
		0.Ԥ���ٽ��¶�
		1.Ԥ���ٽ�����
		2.Ԥ�䳬ʱʱ��-----����ʱ������Ԥ��
		3.ͣ����ʱ----------��������ʱͣ��
		4.ͣ������
		5.ͣ����ʱʱ��-----��Һ��ʱͣ��
		6.Сѭ������ѹ��
		7.Сѭ������ѹ��
		8.������ֹ����
		CNG�� �з���ʱʱ��
		���ͣ��ѹ��
		=================================================*/
		float         c_lcng_ctrl_paras[10]; //216
		unsigned char c_bak[16];//232
		/////end
		unsigned char  paras_crc16_hi;//233
		unsigned char  paras_crc16_low;//234
	}Sysparas; 

	
	
	typedef struct def_SysInfo       
	{
	unsigned long  g_uid;   		  //ǹ��ˮ��
	unsigned char g_sum[6]; //��ǹ��   	
	unsigned char  m_amount[6];   //���ۼƽ��
	unsigned long  gas_count;       //�ܼ�������
	unsigned char gas_total[6];  //���ۼƼ�����
	unsigned char  shift_id;  	  //���
	unsigned char shift_cash[4];//���ֽ�
	unsigned long  shift_uid;       //����ˮ��
	unsigned long  shift_gascount;  //���������
	unsigned  char shift_m_amount[4];//���ۼƽ��
	unsigned char shift_s_gsum[4];//�࿪ʼǹ��
	unsigned char shift_s_amount[4];//�࿪ʼ���
	unsigned char shift_gas_total[4];//�������
	unsigned char  shift_s_date[3];  //�࿪ʼ����
	unsigned char  shift_s_time[3];  //�࿪ʼʱ��(ת��)
	unsigned long  unupld_rec_id;	  //δ�ϴ��ɹ���С��ˮ��¼
	unsigned long  unupld_rpt_id;	  //δ�ϴ��ɹ���С���ۼ�¼
	unsigned long  bak_err_recid;       //����洢����ID
	unsigned long  bak_err_rptid;
	unsigned char  powerdown_type;   //����
	unsigned long  upload_cards_id;   //�ϴ��Ĺ�ʧ������
	float          flow_last_total;   //��������һ��������Լ�ȣ�
	unsigned long  vctrl_l_times;
	unsigned long  vctrl_m_times;
	unsigned long  vctrl_h_times;     //��ŷ�����
	float			st_flow_rate;       //������������
	double		gas_total_total;    //������������������������������
	double		m_total_total;      //�ܽ��
	unsigned char  info_crc16_hi;
	unsigned char  info_crc16_low;
	}Sysinfo; 


	enum{
		TIPS_SETOK  = 0,
		TIPS_SETERR ,
		TIPS_INPUTPSW,
		TIPS_INPUTTIME,
		TIPS_DATE,
		TIPS_TIME,
		TIPS_INPUTERR,
		TIPS_TSTEND,
		TIPS_PLEASE_INPUT,
		TIPS_INPUT_OVERAGE,
	};

	enum{
		TIPS_CARD_OK      = 1,
		TIPS_CARD_RD_ERR  ,
		TIPS_CARD_WT_ERR  ,
		TIPS_CARD_GRAY ,
		TIPS_CARD_LOSS,
		TIPS_CARD_TYPE_ERR,
		TIPS_UNMACH_2_10,
		TIPS_UNMACH_1_9,
		TIPS_CASH_NOT_ENOUGH,
		TIPS_CASH_NOT_MATCH,
		TIPS_CARD_NOT_MATCH,
		TIP_GRAY_ERR,
		TIP_UNGRAY_ERR,
	};
	
	enum e_modbus_
	{
		MODBUS_TYPE_NO = 0,
		MODBUS_TYPE_RM,
		MODBUS_TYPE_EH,
	};	
	
	enum e_disp_
	{
		DISP_TYPE_NO = 0,
		DISP_TYPE_Y,
		DISP_TYPE_B,
		DISP_TYPE_N,
	};

	enum
	{
		REC_TYPE_NORMAL = 0,
		REC_TYPE_POWERDOWN,
		REC_TYPE_SIMULATION,
		REC_TYPE_SIM_POWERDOWN,
	};	
	//EEPROM�д������к���Ϣ���ϵ��ȡȷ���ⲿEEPROM�Ƿ�OK��	
	#define            EEPROM_SYSINFO_LEN		0x100
	#define            EEPROM_SYSPARAS_LEN		0x180 //len; 0x100
	#define            SUPER_PSW_LEN		    7
	#define            LAST_END_TIME_LEN		6
	#define            EEPROM_NOT_USE_LEN		51  //32

	#define            CARD_INFO_LEN			192 //160

	#define            POWER_DOWN_FLAG_LEN		1
	#define            CARD_PSW_LEN				6
	#define            SERIAL_NUMBER_LEN		10
	#define            EEPROM_BAK_USE_LEN		47  //32	

	#define            CARD_LOSTNUM_LEN			2*64 //64��վ
    #define            EEPROM_POWERDOWNREC_LEN  sizeof(struct def_LCNG_Record) //��ˮ���ȡ�

	//#define            POWERON_COPY_MAXLEN      SUPER_PSW_LEN+LAST_END_TIME_LEN+POWER_DOWN_FLAG_LEN+POWER_DOWN_REC_LEN
	#define            POWERON_COPY_MAXLEN      SUPER_PSW_LEN+LAST_END_TIME_LEN+EEPROM_NOT_USE_LEN+CARD_INFO_LEN
	/*enum _CARD_PARAS_ADDR
	{
		EEPROM_SYSINFO_ADDR   = 0x00,										
		EEPROM_SYSPARAS_ADDR  = EEPROM_SYSINFO_ADDR + EEPROM_SYSINFO_LEN,   
		SUPER_PSW_ADDR	      = EEPROM_SYSPARAS_ADDR + EEPROM_SYSPARAS_LEN,
		LAST_END_TIME_ADDR	  = SUPER_PSW_ADDR + SUPER_PSW_LEN,
		EEPROM_NOT_USE_ADDR   = LAST_END_TIME_ADDR+EEPROM_NOT_USE_LEN, //Ԥ��
		CARD_INFO_ADDR        = EEPROM_NOT_USE_ADDR + CARD_INFO_LEN,
		POWER_DOWN_FLAG_ADDR  = CARD_INFO_ADDR +POWER_DOWN_FLAG_LEN,
	};*/
	enum _CARD_PARAS_ADDR
	{
		EEPROM_SYSINFO_ADDR   = 0x00,										
		EEPROM_SYSPARAS_ADDR  = EEPROM_SYSINFO_ADDR + EEPROM_SYSINFO_LEN,   
		SUPER_PSW_ADDR	      = EEPROM_SYSPARAS_ADDR + EEPROM_SYSPARAS_LEN,
		LAST_END_TIME_ADDR	  = SUPER_PSW_ADDR + SUPER_PSW_LEN,
		EEPROM_NOT_USE_ADDR   = LAST_END_TIME_ADDR+LAST_END_TIME_LEN, //Ԥ��
		CARD_INFO_ADDR        = EEPROM_NOT_USE_ADDR + EEPROM_NOT_USE_LEN,
		POWER_DOWN_FLAG_ADDR  = CARD_INFO_ADDR +CARD_INFO_LEN,
		CARD_PSW_ADDR		  = POWER_DOWN_FLAG_ADDR + POWER_DOWN_FLAG_LEN,
		SERIAL_NUM_ADDR		  = CARD_PSW_ADDR + CARD_PSW_LEN,	
		EEPROM_BAK_USE_ADDR	  = SERIAL_NUM_ADDR + SERIAL_NUMBER_LEN, //CARD_PSW_ADDR + CARD_PSW_LEN,	
		CARD_LOSTNUM_ADDR	  = EEPROM_BAK_USE_ADDR  + EEPROM_BAK_USE_LEN, //CARD_PSW_ADDR + CARD_PSW_LEN,	
		//����ʱ������ˮ�����ڲ�EEPROM�У������ϵ�ʱ��������ˮд��flash����������-20170515yck��
		EEPROM_POWERDOWN_REC_ADDR  = CARD_LOSTNUM_ADDR + CARD_LOSTNUM_LEN,
	};		
   /*
		SYS_INFO:  ϵͳ��Ϣ    cur_sysinfo
		SYS_PARAS: ϵͳ����    cur_sysparas
   */    
	
    #define          EX_EEPROM_SYSINFO_ADDR		EEPROM_SYSINFO_ADDR
	#define          EX_EEPROM_SYSPARAS_ADDR	EEPROM_SYSPARAS_ADDR
	//#define          EX_POWER_DOWN_REC_ADDR     POWER_DOWN_REC_ADDR
	
   
    ex 	Sysinfo      cur_sysinfo; // __attribute__((section(".eeprom"))); 
	ex 	Sysparas     cur_sysparas;


	void oparas_factor_pro(void);  //����ϵ��
	void oparas_basic_pro(unsigned char id);
	unsigned int oparas_get_cardlost_num(unsigned char z_id);
	bool         oparas_set_cardlost_num(unsigned char z_id, unsigned int num);	
#endif /* SYSPARAS_H_ */