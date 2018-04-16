/*
 * main.c
 *
 * Created: 2012-8-10 14:10:19
 *  Author: luom
 */ 
#define  _JKJN_APP_
#include "global_def.h"



extern void oui_main_ui(void);
extern void ofixctrl_powerdown_save(void);
extern volatile unsigned char			Rs485Buf[128];

void osys_owatchdog_enable(void)
{
	__WATCH_DOG();
	wdt_enable(WDTO_2S);
}

void osys_opower_chk(void)
{
	unsigned char nCnt = 10;
	__WATCH_DOG();
    if ( (PING&0x20) == 0x20)  
    {
		while(nCnt--)
		{
			__delay_cycles(10);
			if( (PING&0x20) == 0x00 ) return;  
		}
		//if(blcng_running)
		if(blcng_powersave)
		{
			//д�ڲ�EEPROM
			ds3231_read_date((unsigned char*)&cur_rec.rec_etime[0],true);
  			ds3231_read_time((unsigned char*)&cur_rec.rec_etime[3],true); /////20150301
			OS_DISABLE_KEYSCAN();
			ofixctrl_powerdown_save();
			eeprom_write_byte(POWER_DOWN_FLAG_ADDR, 1);
			eeprom_write_block((unsigned char*)&cur_time[0] , LAST_END_TIME_ADDR ,LAST_END_TIME_LEN); //����ػ�ʱ��
			eeprom_write_block((unsigned char*)&cur_rec.rec_etime[0] , LAST_END_TIME_ADDR ,LAST_END_TIME_LEN); //����ػ�ʱ��

			//����ʱ������ˮ�����ڲ�EEPROM�У������ϵ�ʱ��������ˮд��flash����������-20170515yck��
			eeprom_write_block((unsigned char*)&cur_rec.rec_id , EEPROM_POWERDOWN_REC_ADDR ,EEPROM_POWERDOWNREC_LEN); 
			
			OS_ENABLE_KEYSCAN();
		}
		else
		{
			OS_DISABLE_KEYSCAN();
			#if 0 //05-16
				#if 1
					eeprom_write_block((unsigned char*)&cur_sysinfo.g_uid , EEPROM_SYSINFO_ADDR ,sizeof(struct def_SysInfo));
					eeprom_write_block((unsigned char*)&cur_sysparas.r_comp_id, EEPROM_SYSPARAS_ADDR+offsetof(Sysparas, r_comp_id), 17);
				#else
					ee24lc_write(EX_EEPROM_SYSINFO_ADDR ,sizeof(struct def_SysInfo) ,(unsigned char*)&cur_sysinfo.g_uid);
					ee24lc_write(EX_EEPROM_SYSPARAS_ADDR+offsetof(Sysparas, r_comp_id) ,17,(unsigned char*)&cur_sysparas.r_comp_id);
				#endif
			#endif
			if( (sys_err&(1<<SYSERR_TIME)) == 0)
			{
				eeprom_write_block((unsigned char*)&cur_time[0] , LAST_END_TIME_ADDR ,LAST_END_TIME_LEN); //��һ�εĹػ�ʱ��	
			}
			OS_ENABLE_KEYSCAN();
		}
		olcm_clrscr();
		//if(blcng_running == true)
		if(blcng_powersave == true)
		{
			ocomm_printf(CH_DISP,2,1,"���籣��ɹ�");
		}
		else
		{
			ocomm_printf(CH_DISP,2,1,"�ػ�����ɹ�");	
		}
		
		while(1)
		{
			__WATCH_DOG();
			_delay_loop_2(18432);//asm("nop");
		}	
	}		
}

void osys_portinit(void)
{
   PORTA = 0x00;
   DDRA  = 0x00;
   PORTB = 0x00;
   DDRB  = 0x00;
   PORTC = 0x00;  //m103 output only
   DDRC  = 0x00;
   PORTD = 0x00;
   DDRD  = 0x00;
   PORTE = 0x00;
   DDRE  = 0x00;
   PORTF = 0x00;
   DDRF  = 0x00;  //0x00;             
   DDRG  = 0x00;//8; //PG3:S1 //DDRG  = 0x00;0x00;  //
   PORTG = 0x00;  //PORTG = 0x00;           
   DDRH  = 0x00; //0x80;  //0x00;  //
   PORTH = 0x00;  //PH7:S0           
   //DDRB &= 0xDF;  //���籣��	
}
prog_uint16_t _MODBUS_G_AUTO_BUF[]     = {2007/*����*/,2610/*����*/,2017/*�¶�*/,2013/*�ܶ�*/,6859/*״̬*/,5138/*С�����г�*/}; 
//prog_uint16_t _MODBUS_L_AUTO_BUF[]     = {2007/*����*/,2610/*����*/,2017/*�¶�*/,9503/*����*/,6859/*״̬*/,5138/*С�����г�*/}; 
prog_uint16_t _MODBUS_L_AUTO_BUF[]     = {2007/*����*/,2610/*����*/,2017/*�¶�*/,2013/*����*/,6859/*״̬*/,5138/*С�����г�*/}; 

#define       _MODBUS_G_AUTO_BUF_LEN		5 //6 	
#define       _MODBUS_L_AUTO_BUF_LEN		5 //6 

prog_uint16_t _MODBUS_G_AUTO_RM_BUF[]     = {0/*����*/,2/*����*/,1/*�¶�*/,3/*�ܶ�*/,9/*ѹ��*/}; 
prog_uint16_t _MODBUS_L_AUTO_RM_BUF[]     = {0/*����*/,2/*����*/,1/*�¶�*/,47/*����*/}; 

#define       _MODBUS_G_AUTO_RM_BUF_LEN		4 
#define       _MODBUS_L_AUTO_RM_BUF_LEN		4 
bool omobus_init_chk(unsigned char ntype, unsigned char mtype)
{
	//bool  ret = false;
	unsigned int  rdv = 0;
	if(MODBUS_TYPE_RM == ntype)
	{
		return omodbus_read_int(mtype, 312, &rdv);
	}
	else if(MODBUS_TYPE_EH == ntype)
	{
		return omodbus_read_int(mtype, 4909, &rdv);	//true;//
	}
	else
	{
		return false;	
	}
	
}
bool omobus_init_reg(void)
{
	unsigned char n = 0;
	unsigned int  rdv = 0;	
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)
	{
	//#ifdef LNG_G
		for(n=0;n<_MODBUS_G_AUTO_RM_BUF_LEN;n++)
		{
			rdv = pgm_read_word(&_MODBUS_G_AUTO_RM_BUF[n]);
			if(!omodbus_write_multi_reg(_MODBUS_G_, 750+n, rdv))
			{
				sys_err |= 1<<SYSERR_MODBUS_G_AUTO_ERR;
				return false;
			}
		} 
	//#endif	
		for(n=0;n<_MODBUS_G_AUTO_RM_BUF_LEN;n++)
		{
			rdv = pgm_read_word(&_MODBUS_G_AUTO_RM_BUF[n]);
			if(!omodbus_write_multi_reg(_MODBUS_L_, 750+n, rdv))
			{
				sys_err |= 1<<SYSERR_MODBUS_L_AUTO_ERR;
				break;
			}
		}		
	}
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH) 
	{
	__WATCH_DOG();
	  //#ifdef LNG_G
		for(n=0;n<_MODBUS_G_AUTO_BUF_LEN;n++)
		{
			rdv = pgm_read_word(&_MODBUS_G_AUTO_BUF[n]);
			if(!omodbus_write_multi_reg(_MODBUS_G_, 5000+n, rdv))
			{
				sys_err |= 1<<SYSERR_MODBUS_G_AUTO_ERR;
				return false;
			}
		} 
	  //#endif	
		for(n=0;n<_MODBUS_L_AUTO_BUF_LEN;n++)
		{
			rdv = pgm_read_word(&_MODBUS_L_AUTO_BUF[n]);
			if(!omodbus_write_multi_reg(_MODBUS_L_, 5000+n, rdv))
			{
				sys_err |= 1<<SYSERR_MODBUS_L_AUTO_ERR;
				return false;
			}
		}				
	}	
	else
	{
		return false;
	}
	return true;
}
void osys_hardware_chk(void)
{
	unsigned char nkeyid = 0;
	//unsigned char ret = 0;
	unsigned char nmtype = 0;
	//unsigned int  rdv = 0;	
	#if LNG_LIANDONG
		INTER_LINE_A_IN();
		INTER_LINE_A_LOW();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_INPUT;
		INTER_LINE_B_OUT();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_OUTPUT;
	#else
		INTER_LINE_A_OUT();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_OUTPUT;
		INTER_LINE_B_IN();
		INTER_LINE_B_LOW();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_INPUT;
	#endif
		eeprom_busy_wait();
		eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] ,EEPROM_SYSPARAS_ADDR,sizeof(struct def_SysParas));
		ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
		
	if(!oflash_checkid())
	{
		ocomm_printf(CH_DISP,4, 1, "FLASH ���ʧ��"); 
		sys_err |= 1<<SYSERR_FLASH_CHK;
	}	
	osplc_hardwarechk();
	/*-------------------------------------
	�ֳ����ߵ�ֵַ�� ��ַ 4910  ֵ 247
	--------------------------------------*/
	bmodbus_chk_ok = false;
	nmtype = _MODBUS_L_;
	ocomm_printf(CH_DISP,3, 1, "�����Ƽ��");
	while(1)
	{
		__WATCH_DOG();
		if(omobus_init_chk(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE], nmtype))
		{
			bmodbus_chk_ok = true;
			break;
		}
		//osys_opower_chk();
	#if ICCARD_M1_MINGTE
	#else
		PsamCardSelect_Oper();//PSAM����λ
	#endif
        		nkeyid = okernel_geteventid();
		_delay_ms(1000);
		if(nkeyid == KEY_RET_EV){
			 break;
		}			 
	} 
	if(bmodbus_chk_ok == false)  //����anto scan buffer
	{
		sys_err |= 1<<SYSERR_MODBUS_ERR;  
		return;
	}
	omobus_init_reg();
}

bool  osys_poweron_time_chk(void)
{
	//����һ����ˮ ���ϴιػ�ʱ��
	unsigned int  eepaddr = LAST_END_TIME_ADDR;
	//unsigned char rectime[6];
	unsigned char powerofftime[6];
	if(cur_sysinfo.g_uid == 0) return true;
	eeprom_read_block((unsigned char*)&powerofftime[0] , (unsigned int*)eepaddr ,LAST_END_TIME_LEN);
    ds3231_read_date((unsigned char*)&cur_time[0],true);
    ds3231_read_time((unsigned char*)&cur_time[3],true);	
	if(memcmp(cur_time, powerofftime, 3) ==0)  //��������ȲűȽ�ʱ����
	{
		if(cur_time[3] > powerofftime[3]) return true; //ʱ
		if(cur_time[3] == powerofftime[3])             //Сʱ���
		{
			if(cur_time[4] > powerofftime[4]) return true; //��
			if(cur_time[4] == powerofftime[4])
			{
				if(cur_time[5] > powerofftime[5]) return true; //��
			}
		}
	}
	else
	{
		if(cur_time[0] < powerofftime[0]) return false;
		if(cur_time[1] < powerofftime[1]) return false;
		if(cur_time[2] < powerofftime[2]) return false;
	}
	//sys_err = SYSERR_TIME;
	sys_err |= (1<<SYSERR_TIME);
	return false;
}

bool  osys_rec_continuity_chk(void)
{
	unsigned long id;
	bool ret = true;
	id = cur_sysinfo.g_uid %FlashRecMaxID;  
	if(id == 0){
		 return true;
	}			 
#if 0	
	if((cur_sysparas.b_y_or_n_operate&BASIC_P_REC_CHK) == 0) return true;//��ˮ��У��
	////////��ˮ����������/////////
	if(cur_sysparas.r_cur_g_id /* ��ǰǹ��ˮ��*/ == cur_sysinfo.g_uid)
	{
		if(ostore_rec_read(cur_sysparas.r_cur_g_id-1,NULL,128))
		{
			if(cur_rec.rec_id == cur_sysparas.r_cur_g_id )
			{
				osplc_show(cur_rec.rec_lcngnum ,cur_rec.rec_lcngsum,cur_rec.rec_lcngprice,0,false);	
				return true;
			}
		}
	}
	sys_err |= 1<<SYSERR_REC_NUM_NOMATCH;  
	return false;

	if(cur_sysparas.r_cur_g_id /* ��ǰǹ��ˮ��*/ == cur_sysinfo.g_uid)
	{
		if(ostore_rec_read(cur_sysparas.r_cur_g_id-1,NULL,128))
		{
			if(cur_rec.rec_id == cur_sysparas.r_cur_g_id )
			{
				return true;
			}
		}
	}
	if((cur_sysparas.b_y_or_n_operate&BASIC_P_REC_CHK) == 0) return true;	
	sys_err |= 1<<SYSERR_REC_NUM_NOMATCH;  
	return false;	
#else
	if(cur_sysparas.b_y_or_n_operate&BASIC_P_REC_CHK )
	{
		if(cur_sysparas.r_cur_g_id /* ��ǰǹ��ˮ��*/ == cur_sysinfo.g_uid)
		{
			if(ostore_rec_read(cur_sysparas.r_cur_g_id,NULL,128)) //����һ�ʼ�¼����֤��һ��������
			{
				if(cur_rec.rec_id == cur_sysparas.r_cur_g_id + 1)
					{
					//ocomm_printf(CH_DISP,1,1,"��һ������1:%ld",cur_rec.rec_id);
					sys_err |= 1<<SYSERR_REC_NUM_NOMATCH;  
					return false;
					}
			}
			
			if(ostore_rec_read(cur_sysparas.r_cur_g_id-1,NULL,128))//���ID�������
			{
				if(cur_rec.rec_id == cur_sysparas.r_cur_g_id )
					{
					//ocomm_printf(CH_DISP,1,1,"��ǰ����1:%ld",cur_rec.rec_id);
					return true;
					}
				else
					{
					//ocomm_printf(CH_DISP,2,1,"��ǰ����2:%ld",cur_rec.rec_id);
					sys_err |= 1<<SYSERR_REC_NUM_NOMATCH;  
					return false;
					}
			}
			else
				{
				//ocomm_printf(CH_DISP,3,1,"��ǰ����1:%ld",cur_rec.rec_id);
				sys_err |= 1<<SYSERR_REC_NUM_NOMATCH;  
				return false;
				}
		}
		else 
		{
			//ocomm_printf(CH_DISP,3,1,"����1:%ld %ld",cur_sysparas.r_cur_g_id,cur_sysinfo.g_uid);
			sys_err |= 1<<SYSERR_REC_NUM_NOMATCH;  
			return false;
		}
	}
	else
		return true;
#endif		
}


bool  osysparas_chk(void)
{
/*
	bool          bok = false;
	unsigned char rdeep;
	unsigned int  n;// = EEPROM_SYSPARAS_ADDR;
*/
	unsigned char nkeyid = 0;	
	unsigned int  eepaddr;
	Sysparas      inner_paras;
	ee24lc_read(EX_EEPROM_SYSINFO_ADDR, sizeof(struct def_SysInfo) ,(unsigned char*)&cur_sysinfo.g_uid);
	//gas_total_bak = (unsigned long)(cur_sysinfo.shift_gas_total*100);
    gas_total_bak = ocomm_bcdbuf2hex(cur_sysinfo.shift_gas_total, 4);
	   
	ee24lc_read(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
	sum_total_bak = ocomm_bcdbuf2hex(cur_sysinfo.shift_m_amount, 4);
	
      //sum_total_bak = (unsigned long)(cur_sysinfo.shift_m_amount*100);
	
	if( eeprom_read_byte(POWER_DOWN_FLAG_ADDR))/////�������̵���
	{		
		////��ӡ��������
		eeprom_write_block((unsigned char*)&cur_sysinfo.g_uid , EEPROM_SYSINFO_ADDR ,sizeof(struct def_SysInfo));
		eeprom_write_block((unsigned char*)&cur_sysparas.r_comp_id, EEPROM_SYSPARAS_ADDR+offsetof(Sysparas, r_comp_id), 17);
					
		//���ڲ�EEPROM�ж�����ˮ��Ȼ�����flash������������-20170515yck��
		eeprom_read_block((unsigned char*)&cur_rec.rec_id , EEPROM_POWERDOWN_REC_ADDR ,EEPROM_POWERDOWNREC_LEN);
         		
		//��������ˮ����flash�ڶ���-20170515yck��
		ostore_rec_write();	
		
		//��������ˮ����flash��һ��-20170515yck��
		opotocol_zyjn_data_exchange(cur_sysinfo.g_uid,true,true,true,NULL); //�������ݻָ�  //cur_sysinfo.g_uid-1
        //�洢�ɹ������������
        eeprom_write_byte(POWER_DOWN_FLAG_ADDR, 0);//������־��
        //��ӡ�õ�����ˮ��
		SELECT_PRINT();
		oprt_printheadcmd();
		ocomm_printf(CH_PRINT,0,0, "     <ϵͳ�������ݴ�ӡ>\r\n");
		oprt_lfrow(2);    		
		SELECT_CARD();    						
		oprt_rec(true,cur_sysinfo.g_uid-1);  
	}

	eepaddr = EEPROM_SYSPARAS_ADDR;
	eeprom_busy_wait();
	eeprom_read_block((unsigned char*)&inner_paras.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas)); 
#if 0		
	//if( memcmp(&cur_sysparas.f_price[0], &ex_paras.f_price[0], sizeof(struct def_SysParas)) != 0 ) 
	if( memcmp((unsigned char*)&cur_sysparas.f_price[0], (unsigned char*)&inner_paras.f_price[0], sizeof(struct def_SysParas)) != 0 ) 
	{
#if 0 		
		ocomm_printf(CH_DISP,4, 1, "EEPROM����......"); 
		__WATCH_DOG();
		eepaddr = EEPROM_SYSPARAS_ADDR;
		//memcpy(unsigned char*)&inner_paras.f_price[0],(unsigned char*)&cur_sysparas.f_price[0], ( sizeof(struct def_SysParas));
		eeprom_busy_wait();
		eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
		//2012-11-06
		__WATCH_DOG();
		eepaddr = EX_EEPROM_SYSINFO_ADDR;
		eeprom_write_block((unsigned char*)&cur_sysinfo.g_uid ,   (unsigned int*)eepaddr ,sizeof(struct def_SysInfo));
		//
		eepaddr = SUPER_PSW_ADDR;
		for(n=0; n<POWERON_COPY_MAXLEN; n++)
		{
			__WATCH_DOG();
			ee24lc_read(eepaddr, 1 ,(unsigned char*)&rdeep);
			eeprom_busy_wait();
			eeprom_write_block((unsigned char*)&rdeep ,   (unsigned int*)eepaddr ,1);
			eepaddr += 1;
		}
#else  //2013-05-13
		olcm_clrscr();	
		ocomm_printf(CH_DISP,1,1,"EEPROM���ݸ���");  
		ocomm_printf(CH_DISP,2,1,"1.ȡ������");
		ocomm_printf(CH_DISP,3,1,"2.�ڲ�����");
	
		while(1)
		{
			//osys_opower_chk();			
			nkeyid = okernel_geteventid();
			_delay_ms(100);
			if(KEY_1_EV == nkeyid)
			{
				sys_err |= 1<<SYSERR_EEPROM;
				return false;
			}		
			if(KEY_2_EV == nkeyid)
			{
				break;
			}					
		}

		olcm_clrscr();	
		ocomm_printf(CH_DISP,1,1,"���ɻָ��Ĳ�����");
		ocomm_printf(CH_DISP,2,1,"���ڳ����ۺ���Ա");
		ocomm_printf(CH_DISP,3,1,"�����ִ�д˲���");
	    
		nkeyid = 0;
		bok =  false;
		while(1)
		{
			//osys_opower_chk();			
			nkeyid = okernel_geteventid();
			_delay_ms(100);
			if(KEY_RET_EV == nkeyid)
			{
				sys_err |= 1<<SYSERR_EEPROM;
				bok =  false;
			}	
			if(KEY_OK_EV == nkeyid)
			{
				bok =  true;
				break;
			}						
		}
		if( bok ==  true )
		{
			ocomm_printf(CH_DISP,4, 1, "EEPROM����......"); 
			__WATCH_DOG();
			eepaddr = EEPROM_SYSPARAS_ADDR;
			//memcpy(unsigned char*)&inner_paras.f_price[0],(unsigned char*)&cur_sysparas.f_price[0], ( sizeof(struct def_SysParas));
			eeprom_busy_wait();
			eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
			//2012-11-06
			__WATCH_DOG();
			eepaddr = EX_EEPROM_SYSINFO_ADDR;
			eeprom_write_block((unsigned char*)&cur_sysinfo.g_uid ,   (unsigned int*)eepaddr ,sizeof(struct def_SysInfo));
			//
			eepaddr = SUPER_PSW_ADDR;
			for(n=0; n<POWERON_COPY_MAXLEN; n++)
			{
				__WATCH_DOG();
				ee24lc_read(eepaddr, 1 ,(unsigned char*)&rdeep);
				eeprom_busy_wait();
				eeprom_write_block((unsigned char*)&rdeep ,   (unsigned int*)eepaddr ,1);
				eepaddr += 1;
			}	
		}				
#endif		
	}
#endif
    unsigned int crc = 0;
    crc = ocomm_crc16_chksum((unsigned char*)&cur_sysparas.f_price[0],  sizeof(struct def_SysParas)-2);
	if((cur_sysparas.paras_crc16_hi != crc/0x100) || (cur_sysparas.paras_crc16_low != crc%0x100))	
	{
		olcm_clrscr();	
		ocomm_printf(CH_DISP,1,1,"�ⲿEE��������");
		nkeyid = 0;
		while(1)
		{
			//osys_opower_chk();			
			nkeyid = okernel_geteventid();
			_delay_ms(100);
			if(KEY_RET_EV == nkeyid)
			{
				sys_err |= 1<<SYSERR_EEPROM;
				break;
			}	
		}		
	}		
	omodbus_init((unsigned long)((unsigned long)cur_sysparas.b_basic_p[BASIC_P_FLOW_BAUDRATE]*1200));
	ouart2_init((unsigned long)((unsigned long)cur_sysparas.b_basic_p[BASIC_P_PC_BAUDRATE]*1200));
	return true;
}

void osys_init(void)
{
	osys_portinit();
	osys_owatchdog_enable();//wdt_disable();
	oflash_init();
	__WATCH_DOG();
	olcm_init();
	oiic_init();
	__WATCH_DOG();
	ocomm_printf(CH_DISP,1,3,"��ƽ���");
	ocomm_printf(CH_DISP,2,1,"%s",VERSION);  //,sizeof(cur_sysparas.b_basic_p)
	ocomm_printf(CH_DISP,3,1,"ϵͳ����......");
	__WATCH_DOG();
	
	//ouart2_init(38400);	 //pc 485 ͨѶ
	okey_portinit();
	__WATCH_DOG();
	ovctrl_init();
	omodbus_init(19200);
	oprt_init(9600);
	__WATCH_DOG();
	//////////
	b_Simulation  = false;
	bui_tick      = false;
	blcng_running = false;
	b_lcng_disp_shine = true;
	ui_tick_cnt = 0;
    //////////kernel///////	
	okernal_init();
	__WATCH_DOG();
    otimer_t0_init();
	__WATCH_DOG();	
}

void okernel_go(void);
extern unsigned char Precool_Flag;
//extern unsigned char Precool_Flag;
int main(void)
{
	OS_ENTER_CRITICAL();//cli();
	osys_init();
	OS_EXIT_CRITICAL();//sei();
	if ( (PING&0x20) == 0x20)  
    {
		while(1)
		{
			__delay_cycles(10);
			if( (PING&0x20) == 0x00 ) break;  
		}
    }
	osysparas_chk();
	__WATCH_DOG();
	osys_hardware_chk();
	__WATCH_DOG();	
	osys_rec_continuity_chk();


	if(cur_rec.rec_lcngnum < 0)
		osplc_show(fabs(cur_rec.rec_lcngnum)/100.00,fabs(cur_rec.rec_lcngsum)/100.00,cur_price,1,true);
	else
		osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_rec.rec_lcngprice,0,true);	
	__WATCH_DOG();	
	 //ʱ��У��
	if(cur_sysparas.b_y_or_n_operate&BASIC_P_TIME_CHK) //ʱ��У��
	{
		osys_poweron_time_chk();		
	}

/*	#if LNG_LIANDONG
		INTER_LINE_A_IN();
		INTER_LINE_A_LOW();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_INPUT;
		INTER_LINE_B_OUT();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_OUTPUT;
	#else
		INTER_LINE_A_OUT();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] = PUMP_INTERLINE_OUTPUT;
		INTER_LINE_B_IN();
		INTER_LINE_B_LOW();
		cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] = PUMP_INTERLINE_INPUT;
	#endif
		eeprom_busy_wait();
		eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] ,EEPROM_SYSPARAS_ADDR,sizeof(struct def_SysParas));
		ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
*/
	Precool_Flag = 0;

	VCTRL_KEY_OUT();  //08-01
	VCTRL_OPENKEY();
	  
	  	
	PUMPCTRL_HIGH_OUT();
	PUMPCTRL_LOW_OUT();

	VCTRL_MID_OUT();
	VCTRL_HIGH_OUT();

	VCTRL_MID_OPEN();  //����2 (��)
	VCTRL_HIGH_CLOSE();//�ط�1 (��)	
	//LNG_PUMP_IDLE();	
	//LNG_PUMP_STOP();	 //04-28
	LNG_PUMP_IDLE();	 //05-01
	ocomm_printf(CH_UART,0,0,"%s\r\n",VERSION);
	oui_main_ui();
	while(1)
	{
		okernel_go();
	    
		while(1)
		{
			////ϵͳ����
			ocomm_printf(CH_DISP,1,2,"�Ĵ���ƽ���");
			ocomm_printf(CH_DISP,2,1,"%s",VERSION);
			ocomm_printf(CH_DISP,3,1,"ϵͳ����RAM ��");
			__WATCH_DOG();	
		}
	}
}


