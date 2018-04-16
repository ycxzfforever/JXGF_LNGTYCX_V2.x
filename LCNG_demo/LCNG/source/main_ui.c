/*
 * main_ui.c
 *
 * Created: 2012-8-6 16:41:39
 *  Author: luom
 */ 
#include "global_def.h"
unsigned char Precool_Flag = 0;
float cumulative_num = 0;////�ۼ�����
float cumulative_sum = 0;////�ۼƽ��
unsigned long cumulative_chk = 0;////�ۼƴ��� 
unsigned long cumulative_upid = 0;////�ۼ���ˮ��
unsigned long cumulative_downid = 0;////�ۼ���ˮ��

static unsigned char ui_sec_cnt;
extern void oui_main_ui(void);
void oui_idle_tick(void)
{
	//if(!bidle) return;
	if(++ui_sec_cnt >= 50)  //10
	{
		ds3231_read_date((unsigned char*)&cur_time[0],true);
		ds3231_read_time((unsigned char*)&cur_time[3],true);
		ui_sec_cnt = 0;
	}	
	if(bui_tick)
	{  
		bui_tick = false;
		ui_tick_cnt++;
		if(ui_tick_cnt < 4)
		{
			if(false == bprecool_state) ocomm_printf(CH_DISP,1,5,"  δԤ��");
		}			
		else
		{
			if(ui_tick_cnt < 7) ocomm_printf(CH_DISP,1,5,"ǹ��:%02d", cur_sysparas.b_basic_p[BASIC_P_G_ID]);	
			else				ui_tick_cnt = 0;
		}
		ocomm_printf(CH_DISP,4,1,"20%02d-%02d-%02d %02d:%02d",cur_time[0],cur_time[1],cur_time[2],cur_time[3],cur_time[4]); 
	}
      else
      {
		ocomm_printf(CH_DISP,4,1,"20%02d-%02d-%02d %02d %02d",cur_time[0],cur_time[1],cur_time[2],cur_time[3],cur_time[4]); 
		bui_tick = true;
      }
      otimer_reqire(LCNG_UITICK_EV, 5, 0, 1);
}
#if 0  //2013-03-01
void oui_idle_time(void)
{
    ds3231_read_date((unsigned char*)&cur_time[0],true);
    ds3231_read_time((unsigned char*)&cur_time[3],true);
	otimer_reqire(LCNG_UITIME_EV, 10, 0, 1);	
}
#endif

void oui_precool_ret_pro(void)
{
	unsigned int idle_dly = 1800;
	otimer_release(LCNGCTRL_DISP_EV); 
	okernel_addeventpro(LCNG_COMM_EVENT5_EV,olngctrl_vctrl_off);
    if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
	    VCTRL_MID_CLOSE();  //˫���ط�2 (��)
	}
    else
    {
        VCTRL_MID_OPEN();  //��������2 (��)
    }
	VCTRL_HIGH_CLOSE();//�ط�1 (��)
	///////04-28 
	//LNG_PUMP_IDLE();  //05-01
	if(false == bprecool_state) {
		otimer_reqire(LCNG_COMM_EVENT5_EV, 3000, 0, 1);	//300s
	}	
	else{
			idle_dly = (unsigned int)cur_sysparas.c_lcng_ctrl_paras[2];
			if(idle_dly>=1 && idle_dly<=72000) idle_dly = idle_dly*10;
			else                             idle_dly = 1800;
			otimer_reqire(LCNG_COMM_EVENT5_EV, idle_dly, 0, 1); 	//180000ms = 180 s
	}
	///////04-28 end
	okernel_putevent(LCNG_MAINUI_EV,0);
}


static unsigned long cur_ui_dispid = 0;

void oui_recdisp_up_pro(void)
{
    cur_ui_dispid += 1;
	if( cur_ui_dispid >= cur_sysinfo.g_uid)
	{
	  cur_ui_dispid = cur_sysinfo.g_uid-1;	
	  return;
	}	
    oqry_disprec(cur_ui_dispid);
	if(cur_sysinfo.g_uid < 2) return;
	olcm_narrowdown(4,8);
	if( cur_ui_dispid < cur_sysinfo.g_uid-1 ) olcm_narrowup(1,8); 	
}

void oui_recdisp_down_pro(void)
{
    if( cur_ui_dispid == 0) return;
    if( cur_ui_dispid > 0) cur_ui_dispid -= 1;
	oqry_disprec(cur_ui_dispid);		
	if(cur_sysinfo.g_uid < 2) return;
	olcm_narrowup(1,8);
	if( cur_ui_dispid > 0) olcm_narrowdown(4,8); 
	
}
//oprt_rec
void oui_recdisp_print_pro(void)
{
	oprt_rec(true, cur_ui_dispid);
}

void oui_cumulative_up_pro(void)
{
	if(!ostore_rec_read(cur_ui_dispid,NULL,128))
	{
		//ocomm_printf(CH_PRINT,0,0,"����ˮ���ݴ�\r\n");     	
		return;
	} 
	if(cumulative_upid == cur_ui_dispid) return;
		
	cumulative_num = cumulative_num+(float)cur_rec.rec_lcngnum/100.00;
	cumulative_sum = cumulative_sum+(float)cur_rec.rec_lcngsum/100.00;
	cumulative_chk ++;
	ocomm_printf(CH_DISP,1,1,"�ۼƼ�¼��:%ld",cumulative_chk);
    ocomm_printf(CH_DISP,2,1,"����:%.02f",cur_rec.rec_lcngprice);
	ocomm_printf(CH_DISP,3,1,"����:%.02f", cumulative_num);
	ocomm_printf(CH_DISP,4,1,"���:%.02f", cumulative_sum);	
	cumulative_upid = cur_ui_dispid;

	if(cumulative_num < 0)
		osplc_show(fabs(cumulative_num),fabs(cumulative_sum),cur_rec.rec_lcngprice,1,true);
	else
		osplc_show(cumulative_num,cumulative_sum,cur_rec.rec_lcngprice,0,true);
}

void oui_cumulative_down_pro(void)
{
	if(!ostore_rec_read(cur_ui_dispid,NULL,128))
	{
		//ocomm_printf(CH_PRINT,0,0,"����ˮ���ݴ�\r\n");   
		return;
	} 

	if(cumulative_chk == 1)return;
	if(cumulative_downid == cur_ui_dispid) return;
	
	cumulative_num = cumulative_num-(float)cur_rec.rec_lcngnum/100.00;
	cumulative_sum = cumulative_sum-(float)cur_rec.rec_lcngsum/100.00;
	cumulative_chk --;
	ocomm_printf(CH_DISP,1,1,"�ۼƼ�¼��:%ld",cumulative_chk);
    ocomm_printf(CH_DISP,2,1,"����:%.02f",cur_rec.rec_lcngprice);
	ocomm_printf(CH_DISP,3,1,"����:%.02f", cumulative_num);
	ocomm_printf(CH_DISP,4,1,"���:%.02f", cumulative_sum);	
	cumulative_downid = cur_ui_dispid;
	
	if(cumulative_num < 0)
		osplc_show(fabs(cumulative_num),fabs(cumulative_sum),cur_rec.rec_lcngprice,1,true);
	else
		osplc_show(cumulative_num,cumulative_sum,cur_rec.rec_lcngprice,0,true);
}


void oui_oqry_rec_info_No(void)
{
#if 0
  olcm_clrscr();
  ocomm_printf(CH_DISP,1, 1, "��ˮ��¼��ѯ");
  ocomm_printf(CH_DISP,2, 1, "��ˮ��:%ld",cur_sysinfo.g_uid); //cur_sysparas.r_cur_g_id /* ��ǰǹ��ˮ��*/ == cur_sysinfo.g_uid
  ocomm_printf(CH_DISP,3, 1, "r_cur_g_id:%ld",cur_sysparas.r_cur_g_id); //cur_sysparas.r_cur_g_id /* ��ǰǹ��ˮ��*/ == cur_sysinfo.g_uid
  ocomm_printf(CH_DISP,4, 1, "g_id:%ld", cur_sysinfo.unupld_rec_id);//g_uid); 
  okernel_clreventpro();
  okernel_addeventpro(KEY_RET_EV, oui_main_ui);	
#endif
	unsigned long ultmp = 0;
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //��������	
		ultmp = atol((char*)cur_input_cursor.str_input); //��������
	else
		ultmp = cur_sysinfo.g_uid;
	cumulative_num = 0;
	cumulative_sum = 0;
	cumulative_chk = 0;
	cumulative_upid = 0;
	cumulative_downid = 0;
	olcm_clrscr();	  
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oui_qry_pro);
    okernel_addeventpro(KEY_OK_EV, oui_recdisp_print_pro);
    okernel_addeventpro(KEY_UP_EV, oui_recdisp_up_pro);
    okernel_addeventpro(KEY_DOWN_EV, oui_recdisp_down_pro);
	okernel_addeventpro(KEY_1_EV, oui_cumulative_up_pro);////�ۼ����ݲ���----��
	okernel_addeventpro(KEY_4_EV, oui_cumulative_down_pro);////�ۼ����ݲ���----��

	if(cur_sysinfo.g_uid == 0)
	{
		ocomm_message(4,1,"����ˮ��¼",20, NULL);
	    return;
	}	

	if(ultmp > cur_sysinfo.g_uid)
	{
		ocomm_message(4,1,"��ˮ�ų���",20, NULL);
	    return;
	}
	//cur_ui_dispid = cur_sysinfo.g_uid-1;
	cur_ui_dispid = ultmp - 1;
	oqry_disprec(cur_ui_dispid);	
}


void oui_oqry_rec_info(void)
{
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = false;
	cur_input_cursor.p_x   = 2;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;

	olcm_clrscr();	  
	okernel_clreventpro();
    okernel_addeventpro(KEY_RET_EV, oui_qry_pro);
    okernel_addeventpro(KEY_OK_EV, oui_oqry_rec_info_No);
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"������������:"); 
	if(cur_sysinfo.g_uid == 0)
	{
		ocomm_message(4,1,"����ˮ��¼",20, NULL);
	    return;
	}		
}


void stop_precool_close_ctrl(void)
{
        VCTRL_MID_CLOSE();//�ط�2���У�
	 VCTRL_HIGH_CLOSE();//�ط�1 (��)

	/*//����жѹ����
	 otimer_release(LCNG_COMM_EVENT4_EV);
	 okernel_addeventpro(LCNG_COMM_EVENT4_EV, release_pressure);
	 otimer_reqire(LCNG_COMM_EVENT4_EV,20, 0, 1);//��ʱ2��ִ��*/
}

void stop_precool_oper(void)
{
     LNG_PUMP_IDLE();
	 otimer_release(LCNGCTRL_DISP_EV);
	 otimer_release(LCNG_COMM_EVENT1_EV);
	 //oui_precool_ret_pro();
        okernel_addeventpro(LCNG_COMM_EVENT1_EV,stop_precool_close_ctrl);
     
	 otimer_reqire(LCNG_COMM_EVENT1_EV,3000, 0, 1);
	 
	 okernel_putevent(LCNG_MAINUI_EV,0);
	 if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
		{
			//����A������
			INTER_LINE_A_LOW();
		}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
		{
			//����B������
			INTER_LINE_B_LOW();
		}
	}
}


void oui_Sprecool_sure(void)//Сѭ��Ԥ��
{
     olcm_clrscr();		
     Precool_Flag = 1;
     ocomm_printf(CH_DISP,1,1,"����Ԥ��...");  
     okernel_clreventpro();
     okernel_addeventpro(KEY_RET_EV, oui_precool_ret_pro);
     okernel_addeventpro(LCNG_MAINUI_EV, oui_main_ui);
     okernel_addeventpro(KEY_STOP_EV,stop_precool_oper);
     olngctrl_Spre_cool();
}
void oui_Bprecool_sure(void)//��ѭ��Ԥ��
{
     olcm_clrscr();		
     Precool_Flag = 1;
     ocomm_printf(CH_DISP,1,1,"����Ԥ��...");  
     okernel_clreventpro();
     okernel_addeventpro(KEY_RET_EV, oui_precool_ret_pro);
     okernel_addeventpro(LCNG_MAINUI_EV, oui_main_ui);
     okernel_addeventpro(KEY_STOP_EV,stop_precool_oper);
     olngctrl_Bpre_cool();	
}
void oui_disp_back(void) //Ԥ��
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"��ѭ��Ԥ����ȷ��");
	ocomm_printf(CH_DISP,2,1,"��Һǹ���ڲ�ǹ��");
	ocomm_printf(CH_DISP,3,1,"�� 1  Сѭ��Ԥ��");
	ocomm_printf(CH_DISP,4,1,"�� 2  ��ѭ��Ԥ��");
	okernel_clreventpro();
	okernel_addeventpro(KEY_1_EV, oui_Sprecool_sure);//��1��ʼСѭ��Ԥ��
	okernel_addeventpro(KEY_2_EV, oui_Bprecool_sure);//��2��ʼ��ѭ��Ԥ��
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);//�������ء��ص������� 
}


void oui_qry_pro(void)
{
	oqry_main_menu_pro(0);
}

void oui_main_ui(void)
{
 unsigned char tmp_char = 0;
 unsigned long tmp_long = 0;
  head_cmd = NULL;
  tail_cmd = NULL;
  running_cmd = NULL;	
  otimer_release_all();

  otimer_release(LCNG_UITIME_EV);
  otimer_release(LCNG_UITICK_EV);
  otimer_release(LCNGCTRL_DISP_EV); //otimer_release(LCNGCTRL_DISP_EV); 
  otimer_release(LCNG_COMM_EVENT3_EV);
  
  if(false == b_lcng_disp_shine) osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,true);
  olcm_clrscr();
  ocomm_printf(CH_DISP,1,1,"���:%02d ǹ��:%02d", cur_sysinfo.shift_id, cur_sysparas.b_basic_p[BASIC_P_G_ID]);
  
  tmp_long = sum_total_bak/100;
  tmp_char = sum_total_bak%100;
  ocomm_printf(CH_DISP,2,1,"���:%ld.%02d",tmp_long,tmp_char);
  
  tmp_long = gas_total_bak/100;
  tmp_char = gas_total_bak%100;
  ocomm_printf(CH_DISP,3,1,"����:%ld.%02d",tmp_long,tmp_char);
  bui_tick = true;
  
  //��ϵͳʱ��
  ds3231_read_date((unsigned char*)&cur_time[0],true);
  ds3231_read_time((unsigned char*)&cur_time[3],true);	  
  
  ocomm_printf(CH_DISP, 4, 1, "20%02d-%02d-%02d %02d:%02d",cur_time[0],cur_time[1],cur_time[2],cur_time[3],cur_time[4]);  
  okernel_clreventpro();
  
  okernel_addeventpro(LCNG_MAINUI_EV, oui_main_ui);
  okernel_addeventpro(KEY_SET_EV, oset_main_pro);//����
  okernel_addeventpro(KEY_QRY_EV, oui_qry_pro);//��ѯ
  okernel_addeventpro(LCNG_UITICK_EV, oui_idle_tick);
  //okernel_addeventpro(LCNG_UITIME_EV, oui_idle_time);
  okernel_addeventpro(KEY_LCNG_EV, olcngctrl_ctrl_start_pro);//����
  okernel_addeventpro(KEY_FIX_EV, ocngctrl_ctrl_fix_start_pro);//��������
  okernel_addeventpro(KEY_BACK_EV, oui_disp_back); //Ԥ��

  
 #if ICCARD_M1_MINGTE
 #else
   okernel_addeventpro(KEY_PT_EV,PsamCardSelect_Oper); //PSAM��λ
 #endif
   //okernel_addeventpro(KEY_PT_EV,ICCARD_CPU_Oper); //PSAM��λ
  
  ui_sec_cnt = 0;
  otimer_reqire(LCNG_UITICK_EV, 5, 0, 1);	//5  = 500ms  

  //zl add 2013-9-17
  okernel_addeventpro(LCNG_COMM_EVENT3_EV,Main_Release_pressure);
  otimer_reqire(LCNG_COMM_EVENT3_EV,4,0,1);
  
  //otimer_reqire(LCNG_UITIME_EV, 10, 0, 1);  //10 = 1000ms
  if(true == b_Simu_auto && true == b_Simulation)
  {
	  otimer_release(LCNG_UITICK_EV);
	  otimer_reqire(KEY_LCNG_EV, 20, 0, 1);	//5  = 2000ms
  }
  else
  {
	  otimer_release(KEY_LCNG_EV);
  }
}

