/*
 * sysparas.h
 *
 * Created: 2012-8-9 10:05:53
 *  Author: luom
 */ 


#ifndef SYSPARAS_H_
#define SYSPARAS_H_

	#define  BASIC_P_G_ID				0  //枪号
	#define  BASIC_P_SHIFT_ID			1  //班号
	#define  BASIC_P_FLOW_BAUDRATE		2//流量计波特率
	#define  BASIC_P_PC_BAUDRATE		3//后台波特率
	#define  BASIC_P_STATION_ID			4//加气站编号
	#define  BASIC_P_MODBUS_TYPE		5//流量计类型
	#define  BASIC_K_DISP_TYPE			6//液晶屏类型
	#define  BASIC_P_VCTRL_NUM			7//加气机线数

       //是一泵单机还是双机
	#define BASIC_PUMP_TO_MECH              8
	#define BASIC_PUMP_INTERLINE_A       9
	#define BASIC_PUMP_INTERLINE_B       10
	//预冷11
	//预留12
	//预留13
	#define  BASIC_ICCARD_MIN_MOMEY		14  //卡最小余额
	#define  BASIC_ICCARD_MAX_MOMEY		50000
	
	#define  BASIC_P_FACTOR_DENSTY		0
	#define  BASIC_P_FACTOR_CORRECT		1
	#define  BASIC_P_FACTOR_P_ZERO		2
	#define  BASIC_P_FACTOR_P_CORRECT	3
	#define  BASIC_P_FACTOR_P_L_ZERO	4 //液相
	#define  BASIC_P_FACTOR_P_L_CORRECT	5

	#define  BASIC_P_CARD_CHK		1//是否用卡加气
	#define  BASIC_P_AUTO_PRT		2//是否自动打印
	#define  BASIC_P_BACK_GAS		4//是否回气计量
	#define  BASIC_P_TIME_CHK		8//是否时间校验
	#define  BASIC_P_REC_CHK		16//是否记录校验
	#define  BASIC_P_RPT_FIRST		32//是否班累优先
	#define  BASIC_P_CYLINDER_TEST	64//64//是否检验钢瓶到期
	#define  BASIC_P_INPUT_CARNUM     128//是否输入车牌号
	//
	
	#define CNG_V_EX_DLY_TIME_ID		0  //切阀延时
	#define CNG_P_MAX_STOP_ID		    1  //最高安全压力
	
	
	typedef struct def_SysParas
	{
		////////////单价参数///////
		float		  f_price[8]; //32
		////////////基本参数////////
		/*=======================
		float          b_densty;        //密度   
		float          b_coeff;         //修正系数
		float          b_p_zero;        //压力零点
		float          b_p_coeff;       //压力系数
		=======================*/
		float          b_factor[6]; //56
		float          b_vf_other[4];//72
		/*=============
		//枪号 unsigned char  b_g_id; 
		//班号unsigned char  b_g_shift_id; 
		//波特率unsigned char  b_flow_baudrate; ///1:1200 4:4800  8:9600  16:19200 32: 38400///
		//波特率unsigned char  b_pc_baudrate;
		//加气站编号
		//流量计类型
		//客显屏类型
		//加气机类型
		//加气机线数: 单线  双线  三线
		=====================*/
		unsigned char  b_basic_p[16];//ycx 88
		//输入车牌 bit0   用卡加气, 自动打印  禁用压力传感器  流量计实时检测  流水号校验
		unsigned char  b_y_or_n_operate; //ycx 89
		unsigned char  b_bak[16];//105
		////////////运行时参数////////
		unsigned char r_comp_id;//106
		unsigned long r_cur_shift_id;  //当前班流水号110
		unsigned long r_unupld_shift_id;//114
		unsigned long r_cur_g_id;      //当前枪流水号 118
		unsigned long r_unupld_g_id;   //当前未上传的枪流水号122
		unsigned char r_bak[16];//138
		////////////控制参数////////
		/*================================================
		float		  c_l2mv_rate;//"低转中压流速",
		float		  c_m2hv_rate;//"中转高压流速",
		float		  c_hend_rate;//"高截止流速",
		float		  c_max_rate;//"最高安全流速",
		================================================*/
		float         c_rate[4]; //154
		/*================================================
		unsigned int  c_hprotec_dly;//"高流速保护延时",
		unsigned int  c_l2m_dly;//"低切中压延时",
		unsigned int  c_lclose_dly;//"开中关低压延时",
		unsigned int  c_m2h_dly;//"中切高压延时",
		unsigned int  c_mclose_dly;//"开高关中压延时",
		unsigned int  c_hend_dly;//"高压关断延时",
		=================================================*/
		unsigned int  c_dly[6];//166
		unsigned char c_p_stop_arr[10];//176
		/*================================================
		LNG:  
		0.预冷临界温度
		1.预冷临界增益
		2.预冷超时时间-----超过时间重新预冷
		3.停机延时----------低流速延时停机
		4.停机流速
		5.停机超时时间-----加液后超时停泵
		6.小循环阀开压力
		7.小循环阀关压力
		8.回气截止流速
		CNG： 切阀延时时间
		最高停机压力
		=================================================*/
		float         c_lcng_ctrl_paras[10]; //216
		unsigned char c_bak[16];//232
		/////end
		unsigned char  paras_crc16_hi;//233
		unsigned char  paras_crc16_low;//234
	}Sysparas; 

	
	
	typedef struct def_SysInfo       
	{
	unsigned long  g_uid;   		  //枪流水号
	unsigned char g_sum[6]; //总枪累   	
	unsigned char  m_amount[6];   //总累计金额
	unsigned long  gas_count;       //总加气次数
	unsigned char gas_total[6];  //总累计加气量
	unsigned char  shift_id;  	  //班号
	unsigned char shift_cash[4];//班现金
	unsigned long  shift_uid;       //班流水号
	unsigned long  shift_gascount;  //班加气次数
	unsigned  char shift_m_amount[4];//班累计金额
	unsigned char shift_s_gsum[4];//班开始枪累
	unsigned char shift_s_amount[4];//班开始金额
	unsigned char shift_gas_total[4];//班加气量
	unsigned char  shift_s_date[3];  //班开始日期
	unsigned char  shift_s_time[3];  //班开始时间(转换)
	unsigned long  unupld_rec_id;	  //未上传成功最小流水记录
	unsigned long  unupld_rpt_id;	  //未上传成功最小班累记录
	unsigned long  bak_err_recid;       //错误存储备份ID
	unsigned long  bak_err_rptid;
	unsigned char  powerdown_type;   //掉电
	unsigned long  upload_cards_id;   //上传的挂失卡数量
	float          flow_last_total;   //流量计上一次总量（约等）
	unsigned long  vctrl_l_times;
	unsigned long  vctrl_m_times;
	unsigned long  vctrl_h_times;     //电磁阀次数
	float			st_flow_rate;       //加气启动流速
	double		gas_total_total;    //总气量（机子运行以来的总气量）
	double		m_total_total;      //总金额
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
	//EEPROM中存入序列号信息，上电读取确认外部EEPROM是否OK？	
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

	#define            CARD_LOSTNUM_LEN			2*64 //64个站
    #define            EEPROM_POWERDOWNREC_LEN  sizeof(struct def_LCNG_Record) //流水长度。

	//#define            POWERON_COPY_MAXLEN      SUPER_PSW_LEN+LAST_END_TIME_LEN+POWER_DOWN_FLAG_LEN+POWER_DOWN_REC_LEN
	#define            POWERON_COPY_MAXLEN      SUPER_PSW_LEN+LAST_END_TIME_LEN+EEPROM_NOT_USE_LEN+CARD_INFO_LEN
	/*enum _CARD_PARAS_ADDR
	{
		EEPROM_SYSINFO_ADDR   = 0x00,										
		EEPROM_SYSPARAS_ADDR  = EEPROM_SYSINFO_ADDR + EEPROM_SYSINFO_LEN,   
		SUPER_PSW_ADDR	      = EEPROM_SYSPARAS_ADDR + EEPROM_SYSPARAS_LEN,
		LAST_END_TIME_ADDR	  = SUPER_PSW_ADDR + SUPER_PSW_LEN,
		EEPROM_NOT_USE_ADDR   = LAST_END_TIME_ADDR+EEPROM_NOT_USE_LEN, //预留
		CARD_INFO_ADDR        = EEPROM_NOT_USE_ADDR + CARD_INFO_LEN,
		POWER_DOWN_FLAG_ADDR  = CARD_INFO_ADDR +POWER_DOWN_FLAG_LEN,
	};*/
	enum _CARD_PARAS_ADDR
	{
		EEPROM_SYSINFO_ADDR   = 0x00,										
		EEPROM_SYSPARAS_ADDR  = EEPROM_SYSINFO_ADDR + EEPROM_SYSINFO_LEN,   
		SUPER_PSW_ADDR	      = EEPROM_SYSPARAS_ADDR + EEPROM_SYSPARAS_LEN,
		LAST_END_TIME_ADDR	  = SUPER_PSW_ADDR + SUPER_PSW_LEN,
		EEPROM_NOT_USE_ADDR   = LAST_END_TIME_ADDR+LAST_END_TIME_LEN, //预留
		CARD_INFO_ADDR        = EEPROM_NOT_USE_ADDR + EEPROM_NOT_USE_LEN,
		POWER_DOWN_FLAG_ADDR  = CARD_INFO_ADDR +CARD_INFO_LEN,
		CARD_PSW_ADDR		  = POWER_DOWN_FLAG_ADDR + POWER_DOWN_FLAG_LEN,
		SERIAL_NUM_ADDR		  = CARD_PSW_ADDR + CARD_PSW_LEN,	
		EEPROM_BAK_USE_ADDR	  = SERIAL_NUM_ADDR + SERIAL_NUMBER_LEN, //CARD_PSW_ADDR + CARD_PSW_LEN,	
		CARD_LOSTNUM_ADDR	  = EEPROM_BAK_USE_ADDR  + EEPROM_BAK_USE_LEN, //CARD_PSW_ADDR + CARD_PSW_LEN,	
		//掉电时，将流水存入内部EEPROM中，重新上电时，将此流水写入flash的两个区域-20170515yck。
		EEPROM_POWERDOWN_REC_ADDR  = CARD_LOSTNUM_ADDR + CARD_LOSTNUM_LEN,
	};		
   /*
		SYS_INFO:  系统信息    cur_sysinfo
		SYS_PARAS: 系统参数    cur_sysparas
   */    
	
    #define          EX_EEPROM_SYSINFO_ADDR		EEPROM_SYSINFO_ADDR
	#define          EX_EEPROM_SYSPARAS_ADDR	EEPROM_SYSPARAS_ADDR
	//#define          EX_POWER_DOWN_REC_ADDR     POWER_DOWN_REC_ADDR
	
   
    ex 	Sysinfo      cur_sysinfo; // __attribute__((section(".eeprom"))); 
	ex 	Sysparas     cur_sysparas;


	void oparas_factor_pro(void);  //设置系数
	void oparas_basic_pro(unsigned char id);
	unsigned int oparas_get_cardlost_num(unsigned char z_id);
	bool         oparas_set_cardlost_num(unsigned char z_id, unsigned int num);	
#endif /* SYSPARAS_H_ */