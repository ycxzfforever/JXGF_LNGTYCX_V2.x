/*
 * global_def.h
 *
 * Created: 2012-8-6 13:38:16
 *  Author: luom
 */ 


#ifndef GLOBAL_DEF_H_
#define GLOBAL_DEF_H_


	#ifdef _JKJN_APP_
		#define ex 
	#else
		#define ex extern
	#endif
	#define   ADMIN_PSW		"760103"//管理密码
	#define   ADMIN_PSW_EX  "100086" //外部管理密码
	#define  MENU_PRO_EX
	//#define LNG_SUM_G
	#define  LNG_COEFFI_SET
	//11为西华M1卡，01为铭特M1卡，00为铭特CPU卡
	#define OLD_ICCARD_MODULE	0// 1=西华，0=铭特
	#define  ICCARD_M1_MINGTE  	1// 1=M1卡，0=CPU卡
	#define LNG_LIANDONG		1// 1=联动A线，0=联动B线

	#if ICCARD_M1_MINGTE
	#define   VERSION		"JQLTYCX-20171225"//基本版
	#else
	#define   VERSION		"JQL08VC-20140919"//CPU卡
	#endif

	#include "stdio.h"
	#include "stdlib.h"
	#include "string.h"
	#include "stdbool.h"
	#include "math.h" 
	#include "stddef.h"

	#include "avr/io.h"
	#include "avr/pgmspace.h"
	
	#include "avr/interrupt.h"
	#include "avr/eeprom.h"

	#include <util/delay.h>
	#include <util/delay_basic.h>
	#include <compat/twi.h>
	#include <avr/wdt.h>
	#include "kernel.h"
	#include "uart.h"
	#include "lcd.h"
	#include "iic.h"
	#include "key.h"
	#include "flash.h"
	#include "splc.h"
	
	#include "ui.h"
	#include "set.h"
	#include "sysparas.h"
	#include "lcngctrl.h"
	#include "modbus.h"	
	#include "card.h"	
	#include "print.h"	
	#include "qry.h"	
	#include "factory.h"	
	#include "protocol.h"	
	#include "store.h"	
	#include "app.h"	


	#define SET_PORT_BIT(ADDRESS,BIT) 	((ADDRESS) |= (1<<(BIT)))  ;loop_until_bit_is_set(ADDRESS, BIT)
    #define CLR_PORT_BIT(ADDRESS,BIT)  	((ADDRESS) &= ~(1<<(BIT))) ;loop_until_bit_is_clear(ADDRESS, BIT)
    #define TEST_PORT_BIT(ADDRESS,BIT)  ((ADDRESS) & (1<<(BIT)))  
	
	#define __delay_cycles(x)		_delay_loop_2(x)
	
	
	#define __WATCH_DOG()		   wdt_reset()


	#define CompareFloatA(a,b)	((fabs(a-b)<0.01) ? true : false)


	
	enum{
		CH_DISP = 1,
		CH_UART,
		CH_PRINT,
	};


	enum{
		PARAS_CHK_EEPROM_ERR = 1,
		PARAS_CHK_REC_ERR,
	};

	enum{//系统异常原因
		SYSERR_FLASH_CHK = 1,///2
		SYSERR_REC_NUM_NOMATCH,///4
		SYSERR_MODBUS_ERR,///8
		SYSERR_MODBUS_G_AUTO_ERR,///16
		SYSERR_MODBUS_L_AUTO_ERR,//32
		SYSERR_TIME,  //系统时间错误64
		SYSERR_EEPROM,///128
	};

	enum{//停机异常原因
		HALTERR_SYSTEM_DATA_ERR,///2系统数据异常
		HALTERR_MODBUS_G_DATA_ERR,///4气相流量计数据异常
		HALTERR_MODBUS_L_DATA_ERR,//8液相流量计数据异常
		HALTERR_MODBUS_G_AUTO_ERR,///16气相流量计通讯异常
		HALTERR_MODBUS_L_AUTO_ERR,//32液相流量计通讯异常
	};

	ex  volatile bool              blcng_running;      //正在加气
	ex  volatile bool              blcng_powersave;      //正在加气
	ex  volatile bool			   bui_tick;
//	ex  volatile bool              bsyslock;
	ex  volatile bool              bmodbus_chk_ok;
	ex  volatile unsigned long     vTicks;
	ex  volatile unsigned char     super_psw_type;
	ex  unsigned char			   ui_tick_cnt;
	//ex  unsigned char			   sys_err;
	ex  unsigned int			   sys_err;//系统异常
	ex  unsigned int			   halt_err;//停机异常
	ex  unsigned char			   cur_time[6];
    ex  char					   psw[7]; // = {0};	
    ex  float vftemp;
    ex  float vfdgain;

	//zl add 2013-10-15
	ex unsigned long gas_total_bak;
	ex unsigned long sum_total_bak;
	ex unsigned long cur_gas_total;
	ex unsigned long cur_sum_total;
	ex unsigned long cur_gas_num;
	ex unsigned long cur_gas_sum;
	//ex float cur_gas_num;
	//ex float cur_gas_sum;
	ex float cur_back_num_ex;/////加气前回气气量
	ex float cur_back_sum_ex;/////加气前回气金额
	ex float cur_back_num;////////加气过程中回气气量
    //
	
	//ex  unsigned char              paras_chk_type;	
	//common
	void ocomm_cmd_add(user_command_t *cmd);
	void ocomm_exec_string(char *buf);
	void ocomm_printf(unsigned char ch, unsigned char posx, unsigned posy, char *fmt,...);
	bool ocomm_checkdate(unsigned char * str);
	bool ocomm_checktime(unsigned char * str);
	unsigned char ocomm_asc2bcd(unsigned char* asc);
	void ocomm_bcd2asc(unsigned char* dest,unsigned char bcd);
	bool ocomm_cmp2float(double vcmp1, double vcmp2);//unsigned long ocomm_float2ulong(double vfdt);
	unsigned long ocomm_hex2bcd(unsigned long sdt);
	unsigned long ocomm_bcd2hex(unsigned long sdt);
	long  long    ocomm_bcdbuf2hex(unsigned char* buf,unsigned char len);
	void ocomm_hex2bcdbuf(unsigned long sdt, char* buf, unsigned char len);
	void ocomm_float2bcdbuf(double fdt, char* buf, unsigned char len);
	void ocomm_float2asciibuf(double fdt, char* buf, unsigned char len);
	bool LongTochar(unsigned long sdt,unsigned char *buffer,unsigned char len);
	unsigned long charTolong(unsigned char *array,unsigned char len);
	
    //void ocomm_delay_ms(double ms);
	void ocomm_delay_ms(unsigned int ms);
	unsigned int ocomm_crc16_chksum(unsigned char* buf, unsigned char len);
	void ocomm_input_clear(void);
	void ocomm_otip_ctrlend_msg(unsigned char xpos, unsigned char ypos, unsigned char id);
	void ocomm_message(unsigned char x, unsigned char y, char* str, int dlyms, void (*cmd_func)(void));
	void osys_opower_chk(void);
	bool osys_rec_continuity_chk(void);

	void ocomm_disp_str(unsigned char posx, unsigned posy, unsigned long dispvalue);

	void ocomm_hex2asciibuf(unsigned long fdt,char *buf,unsigned char len);

#endif /* GLOBAL_DEF_H_ */
