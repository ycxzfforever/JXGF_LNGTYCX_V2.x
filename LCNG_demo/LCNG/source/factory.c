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
  ocomm_printf(CH_DISP,1,3,"���FLASH ����...");
  ocomm_printf(CH_DISP,2,3,"���Ժ�");
  oflash_eraseall(128);//128); 

  ocomm_printf(CH_DISP,1,3,"���EEPROM����...");
  memset(&cur_sysinfo.g_uid, 0 , sizeof(struct def_SysInfo));
  //////////���۳�ʼ����
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

  ocomm_printf(CH_DISP,1,3,"����Ĭ�ϲ���");
  
  cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] = MODBUS_TYPE_RM;            //EH//��˹����������
  ///��׼���߼�����
  cur_sysparas.b_basic_p[BASIC_P_VCTRL_NUM] = 0x37;
  cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] = DISP_TYPE_B;                  //����
  
  cur_sysparas.b_basic_p[BASIC_P_G_ID] /*b_g_id*/ = 1;						//ǹ��
  cur_sysparas.b_basic_p[BASIC_P_SHIFT_ID] /*b_g_shift_id*/ = 1;		    //���
  cur_sysparas.b_basic_p[BASIC_P_FLOW_BAUDRATE] /*b_flow_baudrate*/ = 16;   //�����Ʋ�����;
  cur_sysparas.b_basic_p[BASIC_P_PC_BAUDRATE] /*b_pc_baudrate*/ = 32;       //��̨������
  cur_sysparas.b_basic_p[BASIC_P_STATION_ID] /*b_pc_baudrate*/ = 1;       //����վ���
  cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY] = 10;                      //����С��� 10Ԫ
  
  cur_sysparas.f_price[0] = 4.00;
  cur_sysparas.f_price[1] = 4.00;
  cur_sysparas.f_price[2] = 4.00;
  //{15/*2005*/,17,19, 22, 29, 32, 35, 40, 42}; 
  memcpy(&cur_sysparas.c_p_stop_arr[0] , p_stop_arr, sizeof(p_stop_arr));
  
  cur_sysparas.c_rate[0] /*c_l2mv_rate*/ = 3.5; //��ѹת��ѹ����  2001
  cur_sysparas.c_rate[1] /*c_m2hv_rate*/ = 3.2; //��ѹת��ѹ����  2002 
  cur_sysparas.c_rate[2] /*c_max_rate*/ = 25;   //��߰�ȫ����   2019
  cur_sysparas.c_rate[3] /*c_hend_rate*/ = 1;   //��ѹ����ֹ���� 2020	  
  //{1/*2021*/,10,1, 10, 1, 10};
  cur_sysparas.c_dly[0]/*c_hprotec_dly*/ = 1;//1*100; //����  //�����ٱ�����ʱ 2021 HV_safeMaxValDly  �����ٳ�����߰�ȫ���ٺ� ��ʱ��������ʱͣ��
  cur_sysparas.c_dly[1]/*c_l2m_dly*/     = 10;//10*100;  //��ѹ���л���ʱ 2022
  cur_sysparas.c_dly[2]/*c_lclose_dly*/  = 1;//1*100;   //����ѹ������ʱ�ضϵ�ѹ�� 2023
  cur_sysparas.c_dly[3]/*c_m2h_dly*/     = 10;//10*100;  //��ѹ���л���ʱ 2024
  cur_sysparas.c_dly[4]/*c_mclose_dly*/  = 1;//1*100;   //��ѹ���л���ʱ 2025
  cur_sysparas.c_dly[5]/*c_hend_dly*/    = 10;//10*100;  //��ѹ���ض���ʱ 2026      �����ٵ��ڸ�ѹ����ֹ���ٺ� ��ʱ��������ʱͣ��
  
  ////
  cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY] /*b_densty*/ = 1.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT] /*b_coeff*/  = 1.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO] /*b_p_zero*/ = 146; //146.0;//194.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT] /*b_p_coeff*/ = 228;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO] /*b_p_zero*/ = 147; //146.0;//194.0;
  cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT] /*b_p_coeff*/ = 227;
  
  cur_sysparas.c_lcng_ctrl_paras[0] = -130;//Ԥ���ٽ��¶�
  cur_sysparas.c_lcng_ctrl_paras[1] = 5;//Ԥ���ٽ�����
  cur_sysparas.c_lcng_ctrl_paras[2] = 180; //Ԥ�䳬ʱʱ��
  ///LNG
  cur_sysparas.c_lcng_ctrl_paras[3] = 30; //ͣ����ʱ
  cur_sysparas.c_lcng_ctrl_paras[4] = 300; //ͣ������
  cur_sysparas.c_lcng_ctrl_paras[5] = 180; //ͣ����ʱʱ��------��Һ��ʱͣ��
  cur_sysparas.c_lcng_ctrl_paras[6] = 1.4;//Сѭ������ѹ��
  cur_sysparas.c_lcng_ctrl_paras[7] = 1.2;//Сѭ������ѹ��
  cur_sysparas.c_lcng_ctrl_paras[8] = 0; //������ֹ����
  ///
  //����ǰǹ��
  gas_total_bak = 0;
 //����ǰ���۽��
  sum_total_bak = 0;
  //������ǹ��
  cur_gas_total = 0;
  //��������۽��
  cur_sum_total = 0;

  cur_gas_num = 0;
  
  cur_gas_sum = 0;
  
  eepaddr = EEPROM_SYSPARAS_ADDR;
  eeprom_busy_wait();
  eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
  ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
  ///////��������/////////
  eepaddr = SUPER_PSW_ADDR;
  //eeprom_busy_wait();
  //eeprom_write_block((unsigned char*)&default_psw , (unsigned int*)eepaddr ,7);

eeprom_write_block((unsigned char*)&default_psw , (unsigned int*)eepaddr ,default_psw[0]+1);
ee24lc_write(eepaddr, default_psw[0]+1 ,(unsigned char*)&default_psw);  //2012-11-06
ocomm_message(4,1,"���óɹ�",10, NULL);
}

void ofactory_paras_pro(void)
{
	unsigned  int  m,n,eepaddr;
	unsigned  char rdeepbuf[32];
	unsigned  int  maxlen = 256; //1024*8/32 = 256
	olcm_clrscr();
	ocomm_printf(CH_DISP,3, 1, "  EEPROM����"); 
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
	ocomm_message(4,1,"���óɹ�",20,oui_main_ui);
}

void ofactory_set_paras_pro(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,1,"���ɻָ��Ĳ�����");
  ocomm_printf(CH_DISP,2,1,"���ȹػ�����");
  ocomm_printf(CH_DISP,3,1,"��ִ�д˲���");
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
  okernel_addeventpro(KEY_OK_EV,  ofactory_paras_pro);//ofactory_set_menu_pro);	
}

void ofactory_clear_pro(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,3,"���FLASH ����...");
  ocomm_printf(CH_DISP,2,3,"���Ժ�");
  oflash_eraseall(128);	
  ocomm_message(4,1,"����ɹ�",10,NULL);
}

void ofactory_set_menu_pro(void)
{
	ocomm_printf(CH_DISP,1,1,"1.�ָ���������");
	ocomm_printf(CH_DISP,2,1,"2.�ϴ�����");
	ocomm_printf(CH_DISP,3,1,"3.�������");
    okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
    okernel_addeventpro(KEY_1_EV, ofactory_set_pro);			
	okernel_addeventpro(KEY_2_EV, ofactory_set_paras_pro);//ofactory_paras_pro);			
	okernel_addeventpro(KEY_3_EV, ofactory_clear_pro);			
}

void ofactory_set_chk_pro(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,1,"���ɻָ��Ĳ�����");
  ocomm_printf(CH_DISP,2,1,"�����밴��ȷ�ϡ�");
  ocomm_printf(CH_DISP,3,1,"ȡ���밴�����ء�");
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oset_main_pro);		
  okernel_addeventpro(KEY_OK_EV,  ofactory_set_menu_pro);	
}