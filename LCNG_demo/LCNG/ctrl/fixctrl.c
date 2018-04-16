/*
 * fixctrl.c
 *
 * Created: 2012-8-31 13:34:59
 *  Author: luom
 */ 
#include "global_def.h"
//extern void xieya(void);
extern unsigned char Precool_Flag;

void ofixctrl_powerdown_save(void)
{
	long tmp = 0;
	unsigned char buf[6];
	memcpy(cur_rec.rec_s_acclcng,cur_sysinfo.g_sum,6);//加气前枪累
	tmp = ocomm_bcdbuf2hex(cur_rec.rec_s_acclcng, 6)+cur_rec.rec_lcngnum;
	memset(cur_rec.rec_e_acclcng,0,6);
	ocomm_hex2bcdbuf(tmp,(char *)cur_rec.rec_e_acclcng, 6);//加气后枪累
	
	////先保证流水记录和基础信息存下来先
	cur_sysinfo.g_uid += 1;	  
	cur_rec.rec_id = cur_sysinfo.g_uid;
    cur_rec.rec_type=0xFF;//掉电流水
	ostore_sysinfo_save();

	if(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK)
	{
	#if ICCARD_M1_MINGTE
		//累计消费气量
		tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_accConlcng, 6);
		tmp = tmp + cur_rec.rec_lcngnum;
		ocomm_hex2bcdbuf(tmp,(char *)cur_rec.rec_card_accConlcng, 6);

		//累计消费金额					
		tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_accConM, 6);
		tmp = tmp + cur_rec.rec_lcngsum;
		ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_accConM, 6);
		
		if(cur_card_data.block2_buf[3]  == '3')//记账卡
		{
			//加气后余额
			tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp = tmp + cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_M, 4);

			//计算余额							
			tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_EndOverage, 4);
			tmp = tmp + cur_rec.rec_lcngsum; 
			ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_EndOverage, 4);   
		}
		else//非记账卡
		{
			//加气后余额
			tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp = tmp - cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_M, 4);

			//计算余额							
			tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_EndOverage, 4);
			tmp = tmp - cur_rec.rec_lcngsum; 
			ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_EndOverage, 4);   
		}

		cur_rec.rec_card_connum     +=1; 		//累计加气次数
	#else
		if(cpu_card_data.CpuCardType  == 0x03)//记账卡
		{
			//加气后余额
			tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp = tmp + cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_M, 4);
		}
		else//非记账卡
		{
			//加气后余额
			tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp = tmp - cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp, (char *)cur_rec.rec_card_M, 4);
		}
		
		//累计消费气量
		memset(cur_rec.rec_card_accConlcng,0,6);
		//累计消费金额
		memset(cur_rec.rec_card_accConM,0,6);
		//计算余额	
		memcpy(cur_rec.rec_card_EndOverage,cur_rec.rec_card_M, 4);
		//累计加气次数
		cur_rec.rec_card_connum  = 0; 
		
	#endif
		//oapp_iccard_update_execute();	//写卡 ycx
		memset(buf,'\0',6);
		memcpy((unsigned char *)buf,(unsigned char*)&cur_card_data.block13_buf[6],4);
		cur_rec.rec_CarNum = atoi((char *)buf);
	}			
	//ostore_rec_write();
}

void ofixctrl_stop_pro(void)  
{
	//float tmp = 0.0;
	//unsigned long tmp = 0;
	//unsigned long tmp_l = 0;
	long tmp = 0;
	long tmp_l = 0;
	blcng_powersave = false;
	
	ocomm_printf(CH_DISP,3,1,"存储数据......");
	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
		{
			//互锁A线清零
			INTER_LINE_A_LOW();
		}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
		{
			//互锁B线清零
			INTER_LINE_B_LOW();
		}
	}

	memcpy(cur_rec.rec_s_acclcng,cur_sysinfo.g_sum,6);//加气前枪累
	tmp = ocomm_bcdbuf2hex(cur_rec.rec_s_acclcng, 6)+cur_rec.rec_lcngnum;
	memset(cur_rec.rec_e_acclcng,0,6);
	ocomm_hex2bcdbuf(tmp,(char *)cur_rec.rec_e_acclcng, 6);//加气后枪累

	////先保证流水记录和基础信息存下来先
	cur_sysinfo.g_uid += 1;
	cur_rec.rec_id = cur_sysinfo.g_uid;
	cur_rec.rec_shift_id = cur_sysinfo.shift_id;
	cur_rec.rec_cuid = cur_sysinfo.shift_uid+1;
	
	ostore_sysinfo_save();//cngnum, cngsum);
	////更新卡
	if(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK)
	{
		ocomm_printf(CH_DISP,4,1,"更新卡数据...");
	#if ICCARD_M1_MINGTE
		//累计消费气量
		tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_accConlcng, 6);					
		tmp_l = tmp_l + cur_rec.rec_lcngnum;  
		ocomm_hex2bcdbuf(tmp_l,(char *)cur_rec.rec_card_accConlcng, 6);

		//累计消费金额						
		tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_accConM, 6);
		tmp_l = tmp_l + cur_rec.rec_lcngsum;
		ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_accConM, 6);
		
		if(cur_card_data.block2_buf[3]  == '3')//记账卡
		{
			//加气后余额
			tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp_l = tmp_l + cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_M, 4);

			//计算余额							
			tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_EndOverage, 4);
			tmp_l = tmp_l + cur_rec.rec_lcngsum; 
			ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_EndOverage, 4);   
		}
		else//非记账卡
		{
			//加气后余额
			tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp_l = tmp_l - cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_M, 4);

			//计算余额							
			tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_EndOverage, 4);
			tmp_l = tmp_l - cur_rec.rec_lcngsum; 
			ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_EndOverage, 4);   
		}

		//累计次数
		cur_rec.rec_card_connum   +=1; 
	#else
		if(cpu_card_data.CpuCardType  == 0x03)//记账卡
		{
			//加气后余额
			tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp_l = tmp_l + cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_M, 4);
		}
		else//非记账卡
		{
			//加气后余额
			tmp_l = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
			tmp_l = tmp_l - cur_rec.rec_lcngsum;
			memset(cur_rec.rec_card_M,0,4);
			ocomm_hex2bcdbuf(tmp_l, (char *)cur_rec.rec_card_M, 4);
		}
		
		//累计消费气量
		memset(cur_rec.rec_card_accConlcng,0,6);
		//累计消费金额
		memset(cur_rec.rec_card_accConM,0,6);
		//计算余额	
		memcpy(cur_rec.rec_card_EndOverage,cur_rec.rec_card_M, 4);
		//累计加气次数
		cur_rec.rec_card_connum  = 0; 
		
	#endif
		oapp_iccard_update_execute();	//写卡
		return;
	}
	osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,true);
	olngctrl_stop_disp();
}
void ofixctrl_disp_pro(void)
{
  long vfix_m = 0;
  if( cur_lcng_opt_type == _CNG_FIX_N )
  {
	  vfix_m = (long)(cur_fix_num*cur_price*100+0.5);
	  if(cur_rec.rec_lcngsum  >= vfix_m)
	  {
		cur_rec.rec_lcngsum = vfix_m;
		cur_rec.rec_lcngnum = cur_fix_num*100; 
		otimer_release(LCNG_COMM_EVENT1_EV);
		okernel_putevent(KEY_STOP_EV,0);			  
		return;		  
	  }		  
  }
  if( cur_lcng_opt_type == _CNG_FIX_M )
  {
	  if((cur_rec.rec_lcngsum +1)>= (long)(cur_fix_money*100))	
	  {
		  cur_rec.rec_lcngsum = (long)(cur_fix_money*100);
		  cur_rec.rec_lcngnum = (long)(cur_rec.rec_lcngsum/cur_price+0.5);
		  otimer_release(LCNG_COMM_EVENT1_EV);
		  okernel_putevent(KEY_STOP_EV,0);
		  return;
	  }
  }
  
  cur_rec.rec_lcngsum += 1;
  cur_rec.rec_lcngnum= (long)(cur_rec.rec_lcngsum/cur_price +0.5);
  
  osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,true);	
  otimer_reqire(LCNG_COMM_EVENT1_EV, 1, 0, 1);
}

/////////////////////////////////////LNG STOP//////////////////////////////////
static unsigned char cng_stop_cnt = 0;
void ofixctrl_fix_lng_stop_pro(void)
{
  olngctrl_vctrl_stop();
  otimer_release(LCNG_COMM_EVENT1_EV);
  okernel_clreventpro();
  okernel_addeventpro(LCNG_COMM_EVENT1_EV, ofixctrl_disp_pro);		
  okernel_addeventpro(KEY_STOP_EV,         ofixctrl_stop_pro);  
    
  if(cur_rec.rec_lcngsum>= (long)(cur_fix_money*100+0.5))	
  {
	cur_rec.rec_lcngsum = (long)(cur_fix_money*100+0.5);
	cur_rec.rec_lcngnum = (long)(cur_fix_money/cur_price*100+0.5);
	okernel_putevent(KEY_STOP_EV,0);
  }
  else
  {
	  //cur_rec.rec_lcngsum += 0.01;
	  cur_rec.rec_lcngsum += 1;
	  otimer_reqire(LCNG_COMM_EVENT1_EV, 1, 0, 1); 	//500ms	
  }
}

void ofixctrl_normal_lng_disp_pro(void)
{
	if(!omodbus_read_auto_buffer(_MODBUS_L_,5))
	{
		if(nmodbus_Errcnt++ > 10) 
		{
			halt_err |= 1<<HALTERR_MODBUS_L_AUTO_ERR;
			otimer_release(LCNG_COMM_EVENT1_EV);
			okernel_putevent(KEY_STOP_EV,0); 
		}
		return;
	}
    //ycxtest 加液机停止加液后，加液机还在计量
	//cur_totalize_l  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TOTALIZE); 
    nmodbus_Errcnt = 0;
	olngctrl_disp_pro_ex();
	if(cur_init_lng_chk > 100)
	{
		otimer_release(LCNG_COMM_EVENT1_EV);
		okernel_putevent(KEY_STOP_EV,0); 
		return;
	}

	if(cng_stop_cnt++ > 4)
	{
		otimer_release(LCNG_COMM_EVENT1_EV);
		okernel_putevent(KEY_STOP_EV,0); 
		return;
	}
	otimer_reqire(LCNG_COMM_EVENT1_EV, 2, 0, 1);
}

void olngctrl_stop_pro(void)
{
	cng_stop_cnt  = 0;
	olngctrl_vctrl_stop();
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, ofixctrl_normal_lng_disp_pro);		
	okernel_addeventpro(KEY_STOP_EV,         ofixctrl_stop_pro);	
	otimer_reqire(LCNG_COMM_EVENT1_EV, 2, 0, 1);  
}

void olngctrl_vctrl_off(void) //2013-04-26
{
	bprecool_state = false;
	PUMPCTRL_HIGH_OUT();
	PUMPCTRL_LOW_OUT();

	VCTRL_MID_OUT();
	VCTRL_HIGH_OUT();

	//VCTRL_MID_OPEN();//关阀2（中）
	VCTRL_HIGH_CLOSE();//关阀1 (高)		

	LNG_PUMP_IDLE();	 //05-01
	Precool_Flag =2;
/*	//增加卸压过程
	otimer_release(LCNG_COMM_EVENT4_EV);
	okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
	otimer_reqire(LCNG_COMM_EVENT4_EV,20, 0, 1);//延时1秒执行 */
}
