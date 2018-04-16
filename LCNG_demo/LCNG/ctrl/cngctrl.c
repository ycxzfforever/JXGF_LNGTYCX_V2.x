/*
 * cngctrl.c
 *
 * Created: 2012-8-9 16:45:12
 *  Author: luom
 */ 
#include "global_def.h"
extern float cur_init_lng_g_sum;

void ocngctrl_simu_disp_pro_ex(void)
{
	//unsigned long lngnum = 0,lngsum = 0;
	long lngnum = 0,lngsum = 0;
	//unsigned long g_lngsum = 0,g_lngnum = 0;
	long g_lngsum = 0,g_lngnum = 0;
	unsigned long tmp_long = 0;
		
 		cur_totalize_l += 0.02;
		cur_flowrate_l += 0.5;
		cur_pressure_l += 0.01;
        if(cur_pressure_l>100)
            cur_pressure_l=0;
        cur_flowrate_g +=0.05;
        vftemp -= 1;
        if(vftemp<-1000)
            vftemp=0;
        vfdgain +=0.1;
        if(vfdgain>100)
            vfdgain=0;
		ocomm_printf(CH_DISP,4,1,"Vl:%.02f Pl:%.02f",cur_flowrate_l, cur_pressure_l);
		lngnum = (long)(cur_totalize_l*cur_calFactor*100+0.5);
		if(lngnum <= (long)(cur_init_lcng_sum*100+0.5) ) 
		{
			cur_init_lng_chk++;
			return;//当前液相总量 < 液相初始值
		}

		lngnum = lngnum - (long)(cur_init_lcng_sum*100+0.5) - cur_back_num_ex;////当前加气量=当前总量-初始气量-回气气量
		cur_rec.rec_lcngnum = lngnum;
		if(lngnum < cur_rec.rec_lcngnum) 
		{
			cur_init_lng_chk++;
			return;
		}
		
		if( fabs(lngnum-cur_rec.rec_lcngnum) > cur_price*200 ) //气量数据异常
		{
			lngnum = cur_rec.rec_lcngnum;
			blcng_running = false;
			halt_err |= 1<<HALTERR_SYSTEM_DATA_ERR;
			okernel_putevent(KEY_STOP_EV,0); 
			return;
		}
		
		cur_init_lng_chk = 0;
		cur_rec.rec_lcngnum = lngnum;	
		lngsum = (long)(cur_rec.rec_lcngnum*cur_price+0.5); 
	  	cur_rec.rec_lcngsum = lngsum;

		lngnum = cur_rec.rec_lcngnum;
		lngsum = cur_rec.rec_lcngsum;

          //#ifdef LNG_SUM_G  
		if(cur_sysparas.b_y_or_n_operate & BASIC_P_BACK_GAS ) 
		{
			cur_totalize_g += 0.01;
			cur_pressure_g += 0.01; 
			cur_flowrate_g += 0.01;

			ocomm_printf(CH_DISP,3,1,"Vg:%.02f Pg:%.02f",cur_flowrate_g, cur_pressure_g);
			//气相回气质量
			g_lngnum =  (long)(cur_totalize_g*100+0.5);
			//if(g_lngnum <= (long)(cur_init_lng_g_sum*100+0.5))return;
			g_lngnum =g_lngnum -(long)(cur_init_lng_g_sum*100+0.5);
			//if(g_lngnum< cur_rec.rec_relcngnum)return;
			
			if(fabs(g_lngnum - cur_rec.rec_relcngnum) > cur_price*200)
			{
				g_lngnum = cur_rec.rec_relcngnum; 
				blcng_running = false;
				halt_err |= 1<<HALTERR_SYSTEM_DATA_ERR;
				okernel_putevent(KEY_STOP_EV,0); 
				return;
			}
			cur_rec.rec_relcngnum = g_lngnum;	
			g_lngsum = (unsigned long)(cur_rec.rec_relcngnum*cur_price+0.5); 
			cur_rec.rec_relcngsum = g_lngsum;

			g_lngnum = cur_rec.rec_relcngnum;
			g_lngsum = cur_rec.rec_relcngsum;

			cur_back_num = g_lngnum;////加气过程回气量

			lngnum = lngnum - g_lngnum;
			lngsum = lngsum - g_lngsum;

			cur_rec.rec_lcngnum = cur_rec.rec_lcngnum - cur_rec.rec_relcngnum;
			cur_rec.rec_lcngsum = cur_rec.rec_lcngsum - cur_rec.rec_relcngsum;
			if(cur_flowrate_l < cur_flowrate_g)return;
		}
        // #endif 
		//cur_gas_num = (float)cur_rec.rec_lcngnum;
		//cur_gas_sum = (float)cur_rec.rec_lcngsum;
        //ocomm_printf(CH_DISP,2,1,"气量:%.02f",(float)cur_rec.rec_lcngnum/100.00);//////end20141022
		//ocomm_printf(CH_DISP,3,1,"金额:%.02f",(float)cur_rec.rec_lcngsum/100.00);
		if(cur_rec.rec_lcngnum < 0)
			osplc_show(fabs(cur_rec.rec_lcngnum)/100.00,fabs(cur_rec.rec_lcngsum)/100.00,cur_price,1,false);
		else
			osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,false);
		
		opotocol_zyjn_lcngrunstate_pro(); 

	    if(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK )
		{
			 tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
		#if ICCARD_M1_MINGTE
			 if(cur_card_data.block2_buf[3]  == '3')//记账卡----卡最大消费
		#else
			 if(cpu_card_data.CpuCardType  == 0x03)//记账卡----卡最大消费
		#endif
			 {
			 if((cur_rec.rec_lcngsum + tmp_long) >= BASIC_ICCARD_MAX_MOMEY)
				{
					cur_lcng_opt_type = _CNG_FIX_M;
					cur_fix_money = BASIC_ICCARD_MAX_MOMEY/100 -(float)tmp_long/100;
					ofixctrl_fix_lng_stop_pro();
					return;
				}
			 }
			 else//非记账卡-----卡最小余额
			 {
			 	if((cur_rec.rec_lcngsum + (cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY]+0.1)*100)>=tmp_long)
			 	{
					cur_lcng_opt_type = _CNG_FIX_M;
					cur_fix_money = (float)tmp_long/100 - cur_sysparas.b_basic_p[BASIC_ICCARD_MIN_MOMEY];
					ofixctrl_fix_lng_stop_pro();
					return;
			 	}
			 }
		}
		//定量加气
		if(_CNG_FIX_M == cur_lcng_opt_type)
		{
			if((cur_rec.rec_lcngsum + 10*cur_price) >= (unsigned long)(cur_fix_money*100))	
			{
				blcng_running = false;
				ofixctrl_fix_lng_stop_pro();
				return;
			}
		}
		if(_CNG_FIX_N == cur_lcng_opt_type)
		{
			if((cur_rec.rec_lcngnum + 10) >= (long)(cur_fix_num*100))	
			{
				cur_fix_money = cur_fix_num*cur_price;
				blcng_running = false;			
				ofixctrl_fix_lng_stop_pro();
				return;
			}
		}			
}
void olcngctrl_ctrl_disp_shine_pro(void)
{
	if(b_lcng_disp_shine == false)
	{
		b_lcng_disp_shine = true;
		osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,true);
	}
	else
	{
		b_lcng_disp_shine = false;
		osplc_show_null();
	}
	otimer_reqire(LCNG_COMM_EVENT4_EV, 5, 0, 1); 
}

void olcngctrl_ctrl_start_pro(void)
{
	otimer_release(LCNG_COMM_EVENT4_EV);
	otimer_release(LCNG_COMM_EVENT5_EV);  //2013-04-23
	osplc_show(0,0,cur_price,0,false);
	if(false == b_Simu_auto)
	{
		cur_fix_money = 999999.99;
		cur_fix_num   = 0;
		cur_lcng_opt_type = _CNG_NORMAL;		
	}
	else
	{
		cur_fix_money = 56.88;
		cur_fix_num   = 0;
		cur_lcng_opt_type = _CNG_FIX_M;		
		if(true == b_Simu_auto)  b_Simulation = true;
	}

	if(true == b_Simulation)
	{
	      cur_totalize_l = 0;	 
	      cur_totalize_g = 0;
	}
	olngctrl_ctrl_start_ex();/////开始加液
	//olngctrl_ctrl_prepare();//准备加液
}

void ocngctrl_ctrl_fix_money_chk(void)
{
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //数据输入
	{
		cur_fix_money = atof((char*)cur_input_cursor.str_input);
		if(cur_fix_money < 2.0)
		{
			ocomm_message(4,1,"定量金额太小",10, ocngctrl_ctrl_fix_start_pro);
			return;
		}
	}	
	else
	{
		ocomm_message(4,1,"输入错误",10, ocngctrl_ctrl_fix_start_pro);
		return;
	}
	cur_fix_num   = 0;
	cur_lcng_opt_type = _CNG_FIX_M;
	olngctrl_ctrl_start_ex();
}

void ocngctrl_ctrl_fix_money_pro(void)
{
	ocomm_input_clear();
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV,   ocngctrl_ctrl_fix_money_chk);  
	okernel_addeventpro(KEY_RET_EV,  oui_main_ui);

	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,2,"定金额加气"); 
	ocomm_printf(CH_DISP,2,1,"请输入金额:");
}

void ocngctrl_ctrl_fix_num_chk(void)
{
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //数据输入
	{
		cur_fix_num = atoi((char*)cur_input_cursor.str_input);
		if(cur_fix_num <= 1)
		{
			ocomm_message(1,1,"定量气量太小",10, ocngctrl_ctrl_fix_start_pro);
			return;
		}		
	}	
	else
	{
		ocomm_message(4,1,"输入错误",10, ocngctrl_ctrl_fix_start_pro);
		return;
	} 
	cur_fix_money = 0;
	cur_lcng_opt_type = _CNG_FIX_N;
	olngctrl_ctrl_start_ex();
}

void ocngctrl_ctrl_fix_num_pro(void)
{
	ocomm_input_clear();
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV,   ocngctrl_ctrl_fix_num_chk);  
	okernel_addeventpro(KEY_RET_EV,  oui_main_ui);	
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,2,"定气量加气"); 
	ocomm_printf(CH_DISP,2,1,"请输入气量:"); 	
}

void ocngctrl_ctrl_fix_start_pro(void)
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,2,"定量加气"); 
	ocomm_printf(CH_DISP,2,1,"1.定金额"); 
	ocomm_printf(CH_DISP,3,1,"2.定气量"); 
	otimer_release(LCNG_COMM_EVENT4_EV);
	otimer_release(LCNG_COMM_EVENT5_EV);  //2013-04-23
	if(false == b_lcng_disp_shine) 
		osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,true);
	okernel_clreventpro();
	okernel_addeventpro(KEY_1_EV,    ocngctrl_ctrl_fix_money_pro);  
	okernel_addeventpro(KEY_2_EV,    ocngctrl_ctrl_fix_num_pro);  
	okernel_addeventpro(KEY_RET_EV,  oui_main_ui);	
	ocomm_input_clear(); //2013-01-09	
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = true;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 5;
}

