/*
 * factory.c
 *
 * Created: 2012-8-14 11:40:59
 *  Author: luom
 */ 
#include "global_def.h"

void ofactory_set_pro(void)
{
  unsigned char p_stop_arr[] =  {15/*2005*/,17,19, 22, 29, 32, 35, 40, 42}; 	
  //unsigned char default_psw[7] = {6,'0','0','0','0','0','0'};	
  unsigned char default_psw[7] = {6,'2','2','2','2','2','2'};	
  //char psw[7] = {'7','6','0','1','0','3'};	
  unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,3,"清除FLASH 数据...");
  ocomm_printf(CH_DISP,2,3,"请稍后");
  oflash_eraseall(128);//128); 

  ocomm_printf(CH_DISP,1,3,"清除EEPROM数据...");
  memset(&cur_sysinfo.g_uid, 0 , sizeof(struct def_SysInfo));
  //////////班累初始内容
  cur_sysinfo.shift_id = 1;
  ds3231_read_date((unsigned char*)&cur_sysinfo.shift_s_date[0],true);
  ds3231_read_time((unsigned char*)&cur_sysinfo.shift_s_time[0],true);  
  //////////
  eeprom_busy_wait();
  eeprom_write_block((unsigned char*)&cur_sysinfo.g_uid , EEPROM_SYSINFO_ADDR ,sizeof(struct def_SysInfo));
  ee24lc_write(EX_EEPROM_SYSINFO_ADDR, sizeof(struct def_SysInfo) ,(unsigned char*)&cur_sysinfo.g_uid);
  
  memset(&cur_sysparas.f_price, 0 , sizeof(struct def_SysParas));
  
  eepaddr = EEPROM_SYSPARAS_ADDR;
  eeprom_busy_wait();
  eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr  ,sizeof(struct def_SysParas));
  ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);

  ocomm_printf(CH_DISP,1,3,"设置默认参数");
  
  cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] = MODBUS_TYPE_RM;            //EH//罗斯蒙特流量计
  ///标准三线加气机
  cur_sysparas.b_basic_p[BASIC_P_VCTRL_NUM] = 0x37;
  cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] = DISP_TYPE_B;                  //大屏
  
  cur_sysparas.b_basic_p[BASIC_P_G_ID] /*b_g_id*/ = 1;						//枪号
  cur_sysparas.b_basic_p[BASIC_P_SHIFT_ID] /*b_g_shift_id*/ = 1;		    //班号
  cur_sysparas.b_basic_p[BASIC_P_FLOW_BAUDRATE] /*b_flow_baudrate*/ = 16;   //流量计波特率;
  cur_sysparas.b_basic_p[BASIC_P_PC_BAUDRATE] /*b_pc_baudrate*/ = 32;       //后台波特率
  cur_sysparas.b_basic_p[BASIC_P_STATION_ID] /*b_pc_baudrate*/ = 1;       //加气站编号
  cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY] = 10;                      //卡最小余额 10元
  
  cur_sysparas.f_price[0] = 4.00;
  cur_sysparas.f_price[1] = 4.00;
  cur_sysparas.f_price[2] = 4.00;
  //{15/*2005*/,17,19, 22, 29, 32, 35, 40, 42}; 
  memcpy(&cur_sysparas.c_p_stop_arr[0] , p_stop_arr, sizeof(p_stop_arr));
  
  cur_sysparas.c_rate[0] /*c_l2mv_rate*/ = 3.5; //低压转中压流速  2001
  cur_sysparas.c_rate[1] /*c_m2hv_rate*/ = 3.2; //中压转高压流速  2002 
  cur_sysparas.c_rate[2] /*c_max_rate*/ = 25;   //最高安全流速   2019
  cur_sysparas.c_rate[3] /*c_hend_rate*/ = 1;   //高压阀截止流速 2020	  
  //{1/*2021*/,10,1, 10, 1, 10};
  cur_sysparas.c_dly[0]/*c_hprotec_dly*/ = 1;//1*100; //毫秒  //高流速保护延时 2021 HV_safeMaxValDly  当流速超过最高安全流速后 延时超过该延时停机
  cur_sysparas.c_dly[1]/*c_l2m_dly*/     = 10;//10*100;  //低压阀切换延时 2022
  cur_sysparas.c_dly[2]/*c_lclose_dly*/  = 1;//1*100;   //开中压阀后延时关断低压阀 2023
  cur_sysparas.c_dly[3]/*c_m2h_dly*/     = 10;//10*100;  //中压阀切换延时 2024
  cur_sysparas.c_dly[4]/*c_mclose_dly*/  = 1;//1*100;   //中压阀切换延时 2025
  cur_sysparas.c_dly[5]/*c_hend_dly*/    = 10;//10*100;  //高压阀关断延时 2026      当流速低于高压阀截止流速后 延时超过该延时停机
  
  ////
  cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY] /*b_densty*/ = 1.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT] /*b_coeff*/  = 1.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO] /*b_p_zero*/ = 146; //146.0;//194.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT] /*b_p_coeff*/ = 228;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO] /*b_p_zero*/ = 147; //146.0;//194.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT] /*b_p_coeff*/ = 227;
  
  cur_sysparas.c_lcng_ctrl_paras[0] = -130;//预冷临界温度
  cur_sysparas.c_lcng_ctrl_paras[1] = 5;//预冷临界增益
  cur_sysparas.c_lcng_ctrl_paras[2] = 180; //预冷超时时间
  ///LNG
  cur_sysparas.c_lcng_ctrl_paras[3] = 30; //停机延时
  cur_sysparas.c_lcng_ctrl_paras[4] = 300; //停机流速
  cur_sysparas.c_lcng_ctrl_paras[5] = 180; //停机超时时间------加液后超时停泵
  cur_sysparas.c_lcng_ctrl_paras[6] = 1.4;//小循环阀开压力
  cur_sysparas.c_lcng_ctrl_paras[7] = 1.2;//小循环阀关压力
  cur_sysparas.c_lcng_ctrl_paras[8] = 0; //回气截止流速
  ///
  //加气前枪累
  gas_total_bak = 0;
 //加气前班累金额
  sum_total_bak = 0;
  //加气后枪累
  cur_gas_total = 0;
  //加气后班累金额
  cur_sum_total = 0;

  cur_gas_num = 0;
  
  cur_gas_sum = 0;
  
  eepaddr = EEPROM_SYSPARAS_ADDR;
  eeprom_busy_wait();
  eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
  ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
  ///////设置密码/////////
  eepaddr = SUPER_PSW_ADDR;
  //eeprom_busy_wait();
  //eeprom_write_block((unsigned char*)&default_psw , (unsigned int*)eepaddr ,7);

eeprom_write_block((unsigned char*)&default_psw , (unsigned int*)eepaddr ,default_psw[0]+1);
ee24lc_write(eepaddr, default_psw[0]+1 ,(unsigned char*)&default_psw);  //2012-11-06
ocomm_message(4,1,"设置成功",10, NULL);
}

void ofactory_paras_pro(void)
{
	unsigned  int  m,n,eepaddr;
	unsigned  char rdeepbuf[32];
	unsigned  int  maxlen = 256; //1024*8/32 = 256
	olcm_clrscr();
	ocomm_printf(CH_DISP,3, 1, "  EEPROM复制"); 
	__WATCH_DOG();
	eepaddr = EEPROM_SYSINFO_ADDR;
	m = 0;
	for(n=0; n<maxlen; n++)
	{
		/*
		__WATCH_DOG();
		ee24lc_read(eepaddr, 32 ,(unsigned char*)&rdeepbuf[0]);
		eeprom_busy_wait();
		eeprom_write_block((unsigned char*)&rdeepbuf ,   (unsigned int*)eepaddr ,32);
		eepaddr += 32;
		*/
		__WATCH_DOG();
		eeprom_busy_wait();
		eeprom_read_block((unsigned char*)&rdeepbuf , (unsigned int*)eepaddr ,32);
		ee24lc_write(eepaddr, 32 ,(unsigned char*)&rdeepbuf[0]);
		eepaddr += 32;
		if(n%16 == 0)
		{
			ocomm_printf(CH_DISP,4, 1+m, "");
			m++;
			if(m>=8) m = 0; 		
		}
		
	}	
	ocomm_message(4,1,"设置成功",20,oui_main_ui);
}

void ofactory_set_paras_pro(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,1,"不可恢复的操作！");
  ocomm_printf(CH_DISP,2,1,"请先关机重启");
  ocomm_printf(CH_DISP,3,1,"再执行此操作");
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
  okernel_addeventpro(KEY_OK_EV,  ofactory_paras_pro);//ofactory_set_menu_pro);	
}

void ofactory_clear_pro(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,3,"清除FLASH 数据...");
  ocomm_printf(CH_DISP,2,3,"请稍后");
  oflash_eraseall(128);	
  ocomm_message(4,1,"清除成功",10,NULL);
}

void ofactory_set_menu_pro(void)
{
	ocomm_printf(CH_DISP,1,1,"1.恢复出厂设置");
	ocomm_printf(CH_DISP,2,1,"2.上传参数");
	ocomm_printf(CH_DISP,3,1,"3.清除数据");
    okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
    okernel_addeventpro(KEY_1_EV, ofactory_set_pro);			
	okernel_addeventpro(KEY_2_EV, ofactory_set_paras_pro);//ofactory_paras_pro);			
	okernel_addeventpro(KEY_3_EV, ofactory_clear_pro);			
}

void ofactory_set_chk_pro(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,1,"不可恢复的操作！");
  ocomm_printf(CH_DISP,2,1,"继续请按【确认】");
  ocomm_printf(CH_DISP,3,1,"取消请按【返回】");
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
  okernel_addeventpro(KEY_OK_EV,  ofactory_set_menu_pro);	
}