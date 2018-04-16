/*
 * qry.c
 *
 * Created: 2012-8-14 9:53:20
 *  Author: luom
 */ 
#include "global_def.h"
static unsigned char cur_qry_disp_index = 0;
bool oqry_disprec(unsigned long id)
{
	if(!ostore_rec_read(id,NULL,128))
	{
		ocomm_printf(CH_DISP,1,1,"读错误");
		return false;
	} 
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"流水号:%ld",cur_rec.rec_id);
       ocomm_printf(CH_DISP,2,1,"单价:%.02f",cur_rec.rec_lcngprice);
	//if(bup) olcm_narrowup(1,8);
	ocomm_printf(CH_DISP,3,1,"气量:%.02f", (float)cur_rec.rec_lcngnum/100.00);
	ocomm_printf(CH_DISP,4,1,"金额:%.02f", (float)cur_rec.rec_lcngsum/100.00);	
	//ocomm_printf(CH_DISP,4,1,"%ld",cur_rec.rec_id);
	//olcm_dispprintf(4,1,"%02d-%02d-%02d %02d:%02d",(unsigned char)bcd2hex(rd_CNGRec.rec_stime[0]),(unsigned char)bcd2hex(rd_CNGRec.rec_stime[1]),
	//			(unsigned char)bcd2hex(rd_CNGRec.rec_stime[2]),(unsigned char)bcd2hex(rd_CNGRec.rec_stime[3]),(unsigned char)bcd2hex(rd_CNGRec.rec_stime[4]));
    //olcm_clrscr();
	//ocomm_printf(CH_DISP,4,1,"id = %ld %ld",id,cur_sysinfo.g_uid);
	return true;
}


prog_char   _UI_QRY_MENU_[][16]={
  "总累数据查询",  
  "班累记录查询",  
  "流水记录查询",  
  "IC卡信息查询",
  //"异常信息查询",
  //"电磁阀门次数",
  "系统信息查询",
};
/*
Task_Event  _OQRY_TOTAL_INFO_Pro_[]  = 
{
  {KEY_RET_EV,			oui_main_ui},
//  {KEY_OK_EV,			oprt_total_info},	   
};
*/

void oqry_total_info(void)
{
  unsigned  long tmp = 0;
  unsigned  long tmp_long = 0;
  unsigned char tmp_char = 0;
  olcm_clrscr();
  tmp = ocomm_bcdbuf2hex(cur_sysinfo.g_sum, 6);
  //ocomm_printf(CH_DISP,1, 1, "枪累:%.02f",   (double)tmp/100.00);
  tmp_long = tmp/100;
  tmp_char = tmp%100;
  ocomm_printf(CH_DISP,1, 1, "枪累:%ld.%02d",tmp_long,tmp_char);
  //ocomm_disp_str(1,4,tmp);
  
  tmp = ocomm_bcdbuf2hex(cur_sysinfo.m_amount, 6);
  //ocomm_printf(CH_DISP,2, 1, "金额:%.02f",   (double)tmp/100.00); 
  tmp_long = tmp/100;
  tmp_char = tmp%100;
  ocomm_printf(CH_DISP,2, 1, "金额:%ld.%02d",tmp_long,tmp_char);
  //ocomm_disp_str(2,4,tmp);
  
  ocomm_printf(CH_DISP,3, 1, "加气次数:%d",  cur_sysinfo.gas_count);
  
  tmp = ocomm_bcdbuf2hex(cur_sysinfo.gas_total, 6);
  //ocomm_printf(CH_DISP,4, 1, "气量:%.02f",   (double)tmp/100.00); 
  tmp_long = tmp/100;
  tmp_char = tmp%100;
  ocomm_printf(CH_DISP,4, 1, "气量:%ld.%02d",tmp_long,tmp_char);
  //ocomm_disp_str(4,4,tmp);
  
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oui_qry_pro);
  okernel_addeventpro(KEY_OK_EV, oprt_total);
  //okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
  //otimer_reqire(LCNG_COMM_EVENT4_EV,30, 0, 1);//延时1秒执行 
}

void oqry_rptdisp_print_pro(void)
{
	_lcng_shiift_rpt       prt_shift_rpt;	
	if(ostore_rpt_read(cur_qry_disp_index,(unsigned char*)&prt_shift_rpt.shift_rpt_id))
	{
		oprt_rpt(false, (unsigned char*)&prt_shift_rpt.shift_rpt_id);	
	}
}
void oqry_disprpt(void)
{
  unsigned long tmp = 0;
  _lcng_shiift_rpt       rd_shift_rpt;	
  olcm_clrscr();
  if(ostore_rpt_read(cur_qry_disp_index,(unsigned char*)&rd_shift_rpt.shift_rpt_id))
  {
	ocomm_printf(CH_DISP,1, 1, "班号:%02d %ld",rd_shift_rpt.shift_id,rd_shift_rpt.shift_rpt_id);
	tmp = ocomm_bcdbuf2hex(rd_shift_rpt.shift_total_rpt_lcng, 4);
	//ocomm_printf(CH_DISP,2, 1, "气量:%.02f" , (double)tmp/100.00);
	ocomm_printf(CH_DISP,2, 1, "气量:%ld.%d" , tmp/100,tmp%100);
	tmp = ocomm_bcdbuf2hex(rd_shift_rpt.shift_rpt_total_lcngcash, 4);
	//ocomm_printf(CH_DISP,3, 1, "现金:%.02f"   ,(double)tmp/100.00);
	ocomm_printf(CH_DISP,3, 1, "现金:%ld.%d"   ,tmp/100,tmp%100);
	ocomm_printf(CH_DISP,4, 1, "%02d-%02d-%02d %02d:%02d",rd_shift_rpt.shift_rpt_etime[0],rd_shift_rpt.shift_rpt_etime[1],rd_shift_rpt.shift_rpt_etime[2],rd_shift_rpt.shift_rpt_etime[3],rd_shift_rpt.shift_rpt_etime[4]);  
  }
}
void oqry_rptdisp_up_pro(void)
{
    cur_qry_disp_index += 1;
	if( cur_qry_disp_index >= cur_sysinfo.shift_uid)
	{
	  cur_qry_disp_index = cur_sysinfo.shift_uid-1;	
	  return;
	}	
    oqry_disprpt();
	if(cur_sysinfo.shift_uid < 2) return;
	olcm_narrowdown(4,8);
	if( cur_qry_disp_index < cur_sysinfo.shift_uid-1 ) olcm_narrowup(1,8); 	
}

void oqry_rptdisp_down_pro(void)
{
    if( cur_qry_disp_index == 0) return;
    if( cur_qry_disp_index > 0) cur_qry_disp_index -= 1;
	oqry_disprpt();		
	if(cur_sysinfo.shift_uid < 2) return;
	olcm_narrowup(1,8);
	if( cur_qry_disp_index > 0) olcm_narrowdown(4,8); 
	
}
void oqry_shift_info(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1, 1, "班累查询");
  okernel_clreventpro();
  
  okernel_addeventpro(KEY_RET_EV, oui_qry_pro);	
  okernel_addeventpro(KEY_UP_EV,   oqry_rptdisp_up_pro);
  okernel_addeventpro(KEY_DOWN_EV, oqry_rptdisp_down_pro);
  if(cur_sysinfo.shift_uid == 0)
  {
		ocomm_message(4,1,"无班累记录",20, NULL);
	    return;
  }
  okernel_addeventpro(KEY_OK_EV,   oqry_rptdisp_print_pro);		
  cur_qry_disp_index = cur_sysinfo.shift_uid-1;
  oqry_disprpt();  
  //okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
  //otimer_reqire(LCNG_COMM_EVENT4_EV,30, 0, 1);//延时1秒执行 
}

/*
Task_Event  _OQRY_BASIC_VERSION_INFO_Pro_[]   = 
{
  {KEY_RET_EV,			oqry_system_info},
  {KEY_OK_EV,			oprt_version_info},	  
};
*/

void oqry_sys_version_info(void)
{
    okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oqry_system_info);	
	okernel_addeventpro(KEY_OK_EV, oprt_version_info);	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"V24 主板  加气机");
	//ocomm_printf(CH_DISP,2,1,"E+H");
	ocomm_printf(CH_DISP,3,1,"%s",VERSION);
	//oprt_sysinfo();
	//okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
   	//otimer_reqire(LCNG_COMM_EVENT4_EV,30, 0, 1);//延时1秒执行 
}


void oqry_sys_basic_info_disp(void)
{
	olcm_clrscr();
	if(cur_qry_disp_index == 0)
	{
		ocomm_printf(CH_DISP,1,1,"枪号:%d",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
		//ocomm_printf(CH_DISP,2,1,"密度:%.04f",cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY]);     
		//ocomm_printf(CH_DISP,3,1,"系数:%d.%04d",cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT]); 
		ocomm_printf(CH_DISP,2,1,"气相零点:%.02f",cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO]);
		ocomm_printf(CH_DISP,3,1,"气相系数:%.02f",cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT]); 
		ocomm_printf(CH_DISP,4,1,"液相零点:%.02f",cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO]);
		cur_qry_disp_index = 1;
		return;
	}
	if(cur_qry_disp_index == 1)
	{	
		ocomm_printf(CH_DISP,1,1,"液相系数:%.02f",cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT]); 
		ocomm_printf(CH_DISP,2,1,"用卡加气:"); 
  		 if(cur_sysparas.b_y_or_n_operate & BASIC_P_CARD_CHK ) 
   			ocomm_printf(CH_DISP,2,6,"√");
	   	else
			ocomm_printf(CH_DISP,2,6,"×");
	   ocomm_printf(CH_DISP,3,1,"记录号:%ld",cur_sysparas.r_cur_g_id);
		cur_qry_disp_index = 2;
		return;
	}
	if(cur_qry_disp_index == 2)
	{
		//ocomm_printf(CH_DISP,1,1,"记录号:%ld",cur_sysparas.r_cur_g_id);
		ocomm_printf(CH_DISP,1,1,"已传流水:%ld",cur_sysparas.r_unupld_g_id);
		ocomm_printf(CH_DISP,2,1,"班流水号:%ld",cur_sysparas.r_cur_shift_id);
		ocomm_printf(CH_DISP,3,1,"已传班流水:%ld",cur_sysparas.r_unupld_shift_id);
	    cur_qry_disp_index = 0;
	    return;
	}
}

void oqry_sys_basic_info(void)
{
	cur_qry_disp_index = 0;
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV,  oqry_system_info);	
	okernel_addeventpro(KEY_OK_EV,   oprt_basic_paras_info);	
	okernel_addeventpro(KEY_DOWN_EV, oqry_sys_basic_info_disp);	
	okernel_putevent(KEY_DOWN_EV,0);	
	//okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
	//otimer_reqire(LCNG_COMM_EVENT4_EV,30, 0, 1);//延时1秒执行 
}

void oqry_sys_paras_info_disp(void)
{
	olcm_clrscr();
	if(cur_qry_disp_index == 0)
	{
            ocomm_printf(CH_DISP,1,1,"预冷临界温度:%.02f", cur_sysparas.c_lcng_ctrl_paras[0]);
            ocomm_printf(CH_DISP,2,1,"预冷临界增益:%.02f",cur_sysparas.c_lcng_ctrl_paras[1]);
            ocomm_printf(CH_DISP,3,1,"预冷超时时间:%.02f",cur_sysparas.c_lcng_ctrl_paras[2]); 
            ocomm_printf(CH_DISP,4,1,"停机延时:%.02f",cur_sysparas.c_lcng_ctrl_paras[3]);
		cur_qry_disp_index = 1;
		return;
	}		
	if(cur_qry_disp_index == 1)
	{	
		ocomm_printf(CH_DISP,1,1,"停机流速:%.02f",cur_sysparas.c_lcng_ctrl_paras[4]);
		ocomm_printf(CH_DISP,2,1,"停机超时时间:%.02f", cur_sysparas.c_lcng_ctrl_paras[5]);
		//ocomm_printf(CH_DISP,2,1,"低压阀切换:%d", cur_sysparas.c_dly[1]);
		//ocomm_printf(CH_DISP,3,1,"开中关低延时:%d",cur_sysparas.c_dly[2]); 
		//ocomm_printf(CH_DISP,4,1,"中压切换延时%d", cur_sysparas.c_dly[3]);		
		cur_qry_disp_index = 0;
		return;
	}
  //{15/*2005*/,17,19, 22, 29, 32, 35, 40, 42}; 
  //memcpy(&cur_sysparas.c_p_stop_arr[0] , p_stop_arr, sizeof(p_stop_arr));
	/*
	if(cur_qry_disp_index == 2)
	{
	    ocomm_printf(CH_DISP,1,1,"开高关中延时%d",cur_sysparas.c_dly[4]);
	    ocomm_printf(CH_DISP,2,1,"高压关断延时%d",cur_sysparas.c_dly[5]);
		ocomm_printf(CH_DISP,3,1,"20~20.5M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[0])/10));		
		ocomm_printf(CH_DISP,4,1,"20.5~21M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[1])/10));		
		cur_qry_disp_index = 3;
		return;
	}	
	if(cur_qry_disp_index == 3)
	{
		ocomm_printf(CH_DISP,1,1,"21~21.5M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[2])/10));		
		ocomm_printf(CH_DISP,2,1,"21.5~22M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[3])/10));		
		ocomm_printf(CH_DISP,3,1,"22~22.5M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[4])/10));		
		ocomm_printf(CH_DISP,4,1,"22.5~23M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[5])/10));		
		cur_qry_disp_index = 4;
		return;
	}		
	if(cur_qry_disp_index == 4)
	{
		ocomm_printf(CH_DISP,1,1,"23~23.5M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[6])/10));		
		ocomm_printf(CH_DISP,2,1,"23.5~24M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[7])/10));		
		ocomm_printf(CH_DISP,3,1,"24~24.5M:%.02f",(float)(((float)cur_sysparas.c_p_stop_arr[8])/10));		
		cur_qry_disp_index = 0;
		return;
	}	
	*/
}

void oqry_sys_paras_info(void)
{
	cur_qry_disp_index = 0;
    okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV,  oqry_system_info);	
	okernel_addeventpro(KEY_OK_EV,   oprt_ctrl_paras_info);	
	okernel_addeventpro(KEY_DOWN_EV, oqry_sys_paras_info_disp);		
    okernel_putevent(KEY_DOWN_EV,0);	
}

void oqry_system_info(void)
{
  olcm_clrscr();
  ocomm_printf(CH_DISP,1, 2, "系统信息查询");
  ocomm_printf(CH_DISP,2, 1, "1.版本信息查询");
  ocomm_printf(CH_DISP,3, 1, "2.基本参数查询");
  ocomm_printf(CH_DISP,4, 1, "3.控制参数查询");  
  okernel_clreventpro();
  //okernel_addeventpro(KEY_RET_EV,  oui_main_ui);	
  okernel_addeventpro(KEY_RET_EV,  oui_qry_pro);
  okernel_addeventpro(KEY_1_EV,   oqry_sys_version_info);	
  okernel_addeventpro(KEY_2_EV, oqry_sys_basic_info);	  
  okernel_addeventpro(KEY_3_EV, oqry_sys_paras_info);
  
  //okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
  //otimer_reqire(LCNG_COMM_EVENT4_EV,30, 0, 1);//延时1秒执行 
}

extern void oui_oqry_rec_info(void);
void oqry_mainmenu_ok_pro(void)
{
	unsigned char sw = odisp_menu_getid();
	switch(sw)
	{
		case 0: //总累数据查询
			oqry_total_info();
			return;
		case 1: //班累记录查询
			oqry_shift_info();
			return;
		case 2: //流水记录查询
			oui_oqry_rec_info();
			return;
		case 3: //IC卡信息查询
		       oapp_iccard_check_imfor();
			return;
		//case 4: //异常信息查询
		//	break;
		//case 5:	//电磁阀门次数
		//	break;
		case 4:	//系统信息查询
			oqry_system_info();
			return;
		default:
			break;
	}
	okernel_putevent(KEY_RET_EV,0);	
}

void oqry_main_menu_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_UI_QRY_MENU_)/sizeof(_UI_QRY_MENU_[0]);
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV,  oui_main_ui);	
    okernel_addeventpro(KEY_OK_EV,   oqry_mainmenu_ok_pro);	
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_UI_QRY_MENU_, maxid ,id);
}