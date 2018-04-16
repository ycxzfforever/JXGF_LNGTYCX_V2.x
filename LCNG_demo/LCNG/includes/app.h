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
		unsigned char select_buf[16]; //0   2~9：卡号
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
		//文件0x15   占19个地址
		unsigned char CpuCardNum[8]; //CPU卡卡号
		unsigned char CpuCardType;//卡类型
		unsigned char CpuCompanyNum[2];//公司编号
		unsigned char CpuStationNum[2];//发卡站编号
		unsigned char CpuCardPassword[3];//密码
		unsigned char CpuSteelDate[3]; //钢瓶到检日期
		//文件0x16  占42个地址
		unsigned char CpuLastMon[4]; //最后加气金额
		unsigned char CpuLastVol[4]; //最后加气气量
		unsigned char CpuLastTime[6]; //最后使用时间
		unsigned char CpuLastStation[2]; //最后加气加气站编号
		unsigned char CpuGunNum;//枪号
		unsigned char CpuSerialNum[4];//流水号
		unsigned char CpuValidity; //卡状态		0x01正常卡，0x02灰卡
		unsigned char CpuBalance[4]; //卡余额
		unsigned char CpuLastBalance[4];//上次余额
		unsigned char CpuLastOper;//上次操作方式1加气，2充值，3硬件解灰，4后台解灰，5发卡
		unsigned char CpuLastOperTime[6]; //上次操作时间
		unsigned char CpuOper;//最后操作流程1加气，2充值，3硬件解灰，4后台解灰，5发卡
		unsigned char CpuRechargeMon[4];//最后充值金额
	  }_cpucard_data;

	  enum _card_rderr_type_
	  {
		  _rd_chk_ok = 1,
		  _rd_chk_h_card,
		  _rd_chk_g_card,
		  _rd_chk_type_err,
		  _rd_chk_b_less, //余额不足
		  _rd_chk_b_excess, //记账超额-------只针对记账卡
		  _rd_chk_b_unm,  //余额不一致
		  _rd_chk_b_err,  //余额异常
		  _rd_chk_gnum_err,  //枪号不匹配
		  _rd_chk_serialnum_err,  //序列号不匹配 
		  _rd_chk_cylinder_err,  //钢瓶到期
		  _rd_chk_cardnum_err,  //卡号错误
		  _rd_chk_jizhangka_err,  //记账卡不能加气
	  };

	  enum _cpu_ready_wterr_type_
	  {
		  _wirte_ready_ok = 1,//准备OK 
		  _wirte_ready_cpumf_err,//CPU-MF失败
		  _wirte_ready_cpudf_err,//CPU-DF失败
		  _wirte_ready_cpugetrand_err,//CPU获取随机数失败
		  _wirte_ready_psammf_err,//PSAM-MF失败
		  _wirte_ready_psamdf_err,//PSAM-DF失败
		  _wirte_ready_psamdeslnit_err,//PSAM-DES初始化失败
		  _wirte_ready_psamdes_err,//PSAM-DES失败
		  _wirte_ready_cpuverity_err,//CPU外部认证失败
	  };

	typedef struct def_CNG_RunSRec    
	{
	  unsigned char   runRFCardNum[4]; //IC卡卡号
	  unsigned char   runRFCardType;   //IC卡类型
	  unsigned char   runRFCardPreM[4];//IC卡卡前余额
	  unsigned char   runCarNum[2];    //车号后三位
	  unsigned char   runPressure[2];  //当前压力
	  unsigned char   runSpeed[2];     //当前流速
	  unsigned char   runCNGAmount[6]; //当前枪累(最大9999999999.99)
	  unsigned char   brunRFCard;      //卡状态（1插入、0拨出）
	  unsigned char   brunLV;          //低压阀开启状态（1开、0关）
	  unsigned char   brunMV;          //中压阀开启状态（1开、0关）
	  unsigned char   brunHV;          //高压阀开启状态（1开、0关）
	  unsigned char   brunCNG;         //加气状态（1正在加气、0加气结束）
	  unsigned char   brunPrice[2];    //当前加气单价
	  unsigned char   brunCNGNum[4];   //当前加气量
	  unsigned char   brunCNGCash[4];  //当前加气金额
	  unsigned char   brunRDCardM[4];  //当前余额

	 //zl add 2013-8-21
	  unsigned char  CalutUnit;  //计量单位
	  unsigned char  MechType; //加气机类型
	  unsigned char  StationNum;  //加气站编号
	  //zl 
	}_lcng_runs_rec;//19命令

	typedef struct def_LCNG_TRANS_REC
	{
	  unsigned char  UID[6];             //UID加气记录枪流水号
	  unsigned char  UsrID;              //班号
	  unsigned char  CUID[6];            //UID班记录枪流水号
	  unsigned char  RecStartTime[6];    //加气开始时间
	  unsigned char  RecEndTime[6];      //加气结束时间
	  unsigned char  RecICNum[4];        //IC卡号
	  unsigned char  RecICType;          //类型
	  unsigned char  RecICPreMoney[4];   //加气前余额 
	  unsigned char  RecCNGNum[4];       //本次加气量
	  //#if VISION_EN                 
	  //unsigned char  RecPrice[4];	     //本次加气单价	
	  //#else
	  unsigned char  RecPrice[2];	       //本次加气单价	
	  //#endif  
	  unsigned char  RecCNGMoney[4];     //本次加金额  
	  unsigned char  RecICCurMoney[4];   //本次加气后余额
	  unsigned char  RecICAccFTimes[3];  //累计充值次数
	  unsigned char  RecICAccFMoney[6];  //累计充值金额
	  unsigned char  RecICAccConNum[3];  //累计消费次数 *	
	  unsigned char  RecICAccConCNG[6];  //累计消费气量 *	
	  unsigned char  RecICAccConMoney[6];//累计消费金额 *
	  unsigned char  RecICPreOverage[4]; //期初余额
	  unsigned char  RecICEndOverage[4]; //计算余额 *  	
	  unsigned char  RecCarNum[2];       //车号后4位
	  unsigned char  RecPrePressure[2];  //加气前压力
	  unsigned char  RecEndPressure[2];  //加气后压力
	  unsigned char  RecStartAccCNG[6];  //加气前枪累
	  unsigned char  RecEndAccCNG[6];    //加气后枪累
	  unsigned char  bRecCardOK;         //是否灰卡了
	  unsigned char  bRecPrint;          //是否打印了
	  unsigned char  lstCardType;        //bRecTransOK;//卡上次记录(改为加气充值)
         //zl add 2013-8-21
	  unsigned char  CalutUnit;  //计量单位
	  unsigned char  MechType; //加气机类型
	  unsigned char  StationNum;  //加气站编号
	  //unsigned char  bak[2];
	}_lcng_trans_rec;//流水记录


	typedef struct def_LCNG_SHIFT_TRANS_Rpt
	{
	  unsigned char  UID[6];                //班流水号
	  unsigned char  UsrID;                 //班号
	  unsigned char  RptStartTime[6];       //起始时间
	  unsigned char  RptEndTime[6];         //结束时间
	  unsigned char  RptStartCNG[6];        //开始枪累
	  unsigned char  RptEndCNG[6];          //结束枪累
	  unsigned char  RptTotalCNG[4];        
	  unsigned char  RptUseCNG[4];          //班累
	  unsigned char  RptMoney[4];           //金额
	  unsigned char  RptCash[4];            //现金
	  unsigned char  RptCNGNum[2];          //次数
	  unsigned char  bRptTrans;
	  //zl add 2013-8-21
	   unsigned char  CalutUnit;  //计量单位
	  unsigned char  MechType; //加气机类型
	  unsigned char  StationNum;  //加气站编号
	}_lcng_shift_trans_Rpt;//班累记录

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
  void oapp_iccard_ungray_pro(void);    //现场解灰
  void oapp_iccard_tip_msg(unsigned char xpos, unsigned char ypos, unsigned char id);
#endif /* APP_H_ */