/*
 * lngctrl.c
 *
 * Created: 2012-8-9 16:45:28
 *  Author: luom
 */ 
#include "global_def.h"

extern void ocngctrl_vctrl_mvopen_pro(void);
extern void ocngctrl_pressure_chk_pro(void);

extern void ofixctrl_normal_lng_disp_pro(void);

void stop_precool_oper(void);

//void GetStartCondition(void);

static unsigned char cur_lngctrl_menu_id = 0;

float cur_init_lng_g_sum = 0.0;
extern unsigned char Precool_Flag;

void olngctrl_ctrl_start(void);


prog_char  _OLNGCTRL_PARAS_MENU[][16]={
  "Ԥ���ٽ��¶�",
  "Ԥ���ٽ�����",
  "Ԥ�䳬ʱʱ��",
  //"�ü�עƵ��(hz)", 
  "ͣ����ʱ",
  "ͣ������(kg/h)",
  "��Һ��ʱͣ��",
  "Сѭ������ѹ��",
  "Сѭ������ѹ��",
  "������ֹ����",
};

bool olngctrl_parasetvalue_chk(unsigned char id, float vfset)
{
	if(id == 0)
	{
		cur_sysparas.c_lcng_ctrl_paras[id] = -vfset;
		return true;
	}
	cur_sysparas.c_lcng_ctrl_paras[id] = vfset;
	return true;
}

void olngctrl_parasetok_event(void)
{
	float vftmp = 0;
	unsigned char dlycnt = 0;
	unsigned int  eepaddr = EEPROM_SYSPARAS_ADDR;
	if(strlen((char*)cur_input_cursor.str_input) > 0 )  //��������
	{
		vftmp = atof((char*)cur_input_cursor.str_input);
		if(olngctrl_parasetvalue_chk(cur_lngctrl_menu_id,vftmp))
		{
			eeprom_busy_wait();	
			eeprom_write_block((unsigned char*)&cur_sysparas.f_price[0] , (unsigned int*)eepaddr ,sizeof(struct def_SysParas));
			ee24lc_write(EX_EEPROM_SYSPARAS_ADDR, sizeof(struct def_SysParas) ,(unsigned char*)&cur_sysparas.f_price[0]);
			dlycnt = 10;
		}
	}		
	ocomm_message(4,1,"���óɹ�",dlycnt, NULL);
}


void olngctrl_parasetret_event(void)
{
	olngctrl_menuevent_pro(cur_lngctrl_menu_id);
}

Task_Event  _OLNGCTRL_PARASET_Pro[]  = 
{
  {KEY_OK_EV,			olngctrl_parasetok_event}, 	
  {KEY_RET_EV,			olngctrl_parasetret_event}, 		  
};

void olngctrl_parasset_pro(unsigned char id)
{
	float vftmp = 0;
	ocomm_input_clear();
	cur_input_cursor.b_psw = false;
	cur_input_cursor.b_pt  = true;
	cur_input_cursor.p_x   = 3;
	cur_input_cursor.p_y   = 1;
	cur_input_cursor.pos   = 0;
	cur_input_cursor.max   = 6;
	okernel_clreventpro();
    okernel_seteventpro(_OLNGCTRL_PARASET_Pro, sizeof(_OLNGCTRL_PARASET_Pro)/sizeof(_OLNGCTRL_PARASET_Pro[0]));
	okernel_seteventpro(_OCOMM_KEYINPUT_Pro, sizeof(_OCOMM_KEYINPUT_Pro)/sizeof(_OCOMM_KEYINPUT_Pro[0]));	
	olcm_clrscr();
	ocomm_printf(CH_DISP,2,1,"������:");
	vftmp = cur_sysparas.c_lcng_ctrl_paras[id];
	ocomm_printf(CH_DISP,1,1,"[%.02f]",vftmp);
}

void olngctrl_menu_ok_event(void)
{
	//unsigned char sw = odisp_menu_getid();
	cur_lngctrl_menu_id = odisp_menu_getid();
	switch(cur_lngctrl_menu_id)
	{
		case 0: //Ԥ���ٽ��¶�
		case 1: //Ԥ���ٽ�����
		case 2: //Ԥ�䳬ʱʱ��
		case 3: //ͣ����ʱ
		case 4: //ͣ������
		case 5://ͣ����ʱʱ��-----��Һ��ʱͣ��
		case 6://  Сѭ������ѹ��
		case 7://Сѭ������ѹ��
		case 8://Ԥ����������
			olngctrl_parasset_pro(cur_lngctrl_menu_id);
			break;
		default:
			okernel_putevent(KEY_RET_EV,0);	
			return;
	}	
}

void olngctrl_menuevent_pro(unsigned char id)
{
	unsigned char maxid = sizeof(_OLNGCTRL_PARAS_MENU)/sizeof(_OLNGCTRL_PARAS_MENU[0]);
	okernel_clreventpro();
	okernel_addeventpro(KEY_RET_EV, oset_paras);
	okernel_addeventpro(KEY_OK_EV, olngctrl_menu_ok_event);
	if(id>=maxid) okernel_putevent(KEY_RET_EV,0);
	else          odisp_menu_pro(_OLNGCTRL_PARAS_MENU, maxid ,id);//oset_price_pro();//odisp_menu_pro(_OLNGCTRL_PARAS_MENU, sizeof(_OLNGCTRL_PARAS_MENU)/sizeof(_OLNGCTRL_PARAS_MENU[0]),id);//oset_price_pro();
}

float  max_drv_gain = 0.0;
float  min_drv_gain = 0.0;
float Si_temp=0.0,Si_gain=0.0;

void olngctrl_precool_disp_pro(void)
{
   if( true == b_Simulation)	
   {
        Si_temp -= 0.11 ;
	 Si_gain += 0.11;
        ocomm_printf(CH_DISP,2,1,"�¶�:%.02f", Si_temp);
	 ocomm_printf(CH_DISP,3,1,"����:%.02f", Si_gain);

       if((Si_temp  > -1.00) || (Si_gain < 1.00))
       {
           otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
           return;
       }

       Si_temp = 0;
       Si_gain = 0;
	otimer_release(LCNGCTRL_DISP_EV);   //ģ��OK
	otimer_release(LCNG_COMM_EVENT4_EV); 
	bprecool_state = true;
    LNG_PUMP_IDLE();
	ocomm_message(4,1,"Ԥ��ɹ�!",10, NULL);	/**/
	//ocomm_message(4,1,"Ԥ��ɹ�!",5, olngctrl_ctrl_start);	/**/
	return;
   }	
   else
   {
    //      float vftemp  = 0.0;
    //     float vfdgain = 0.0; 
	//float min_drv_gain = 0;
	//float max_drv_gain = 0;
	unsigned int vstate = 0;
	
	if(!omodbus_read_auto_buffer(_MODBUS_L_,5))
	{
		if(++modbus_err_rd_cnt > 10)
		{
			otimer_release(LCNGCTRL_DISP_EV); 
			bprecool_state = false;
			ocomm_message(4,1,"������ͨѶ��!",20, NULL);
			return;
		}
	}
	
	vftemp  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TEMPERATURE);
	vfdgain = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_DRVGAIN);
    cur_flowrate_l  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_MASSFLOW);  			
    osplc_show(0,0,cur_price,0,false);//������ʾ 
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)
	{
		vstate  = omodbus_read_auto_uint_value(_MODBUS_L_, AUTO_SCAN_LCNG_STATE);	
		vfdgain *= 10;
		ocomm_printf(CH_DISP,4,1,"״̬:%d", vstate);
	}
	else
	{
		vstate = 0;
		if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)  vstate = 1;
	}

	ocomm_printf(CH_DISP,1,1,"Ԥ��");
    ocomm_printf(CH_DISP,2,1,"����:%.02f",cur_flowrate_l);
	ocomm_printf(CH_DISP,3,1,"�¶�:%.02f", vftemp);
	ocomm_printf(CH_DISP,4,1,"����:%.02f", vfdgain);
	
    if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)
    {
       /////2012-11-22
		if( vstate != 1 ||  vftemp > cur_sysparas.c_lcng_ctrl_paras[0] || vfdgain>5)
		{
			otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
			return;
		}
		
		
	      if(vfdgain > cur_sysparas.c_lcng_ctrl_paras[1])   // 4.0
	      { 
	               modbus_chk_cnt = 0;
	               max_drv_gain = vfdgain;   //(unsigned int)(fvrd*100);
			 otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	         return;
	     }
    }
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)
	{
	      if(vftemp > cur_sysparas.c_lcng_ctrl_paras[0])
	      {
			otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
			return;
	      }
	      
	      if(vfdgain > cur_sysparas.c_lcng_ctrl_paras[1])   // 4.0
	      { 
	               modbus_chk_cnt = 0;
	               max_drv_gain = vfdgain;   //(unsigned int)(fvrd*100);
			 otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	         return;
	      }
	}
	
     min_drv_gain = vfdgain;      //min = (unsigned int)(fvrd*100);
	//if( fabs(max_drv_gain - min_drv_gain) < 0.40)//40 )
	
    if(fabs(max_drv_gain - min_drv_gain) < 3.00) //40 )
    {
		//�����ȶ�֮������٣�    
		if(++modbus_chk_cnt > DRVGAIN_CHKLEN || vstate == 1)
		{
			otimer_release(LCNGCTRL_DISP_EV); 
			otimer_release(LCNG_COMM_EVENT4_EV); 
			otimer_release(LCNG_COMM_EVENT5_EV);  //2013-04-23
			LNG_PUMP_IDLE(); //LNG_PUMP_PRECOOL();//2013-04-28 
			//LNG_PUMP_PRECOOL();  //ycx20171106Ԥ���ֱ��ͣ��			                                
			bprecool_state = true;
			if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
        	{
        	    VCTRL_MID_CLOSE();  //˫���ط�2 (��)
        	}
            else
            {
                VCTRL_MID_OPEN();  //��������2 (��)
            }
            VCTRL_HIGH_CLOSE();//�ط�1 (��) 
			ocomm_message(4,1,"Ԥ��ɹ�!",10, NULL);
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
			return;
		}
		otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	}
	else
	{
		max_drv_gain = min_drv_gain;
		modbus_chk_cnt = 0;
		otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	}
   }

}
/*****************************
˵�����°�ܵ�Сѭ��Ԥ�����
******************************/
void olngctrl_Spre_cool(void)
{
	bprecool_state = false;
	modbus_chk_cnt = modbus_err_rd_cnt = 0;
	otimer_release(LCNGCTRL_DISP_EV); 
	okernel_addeventpro(LCNGCTRL_DISP_EV, olngctrl_precool_disp_pro);
  
	PUMPCTRL_HIGH_OUT();//�ܽ�1-��������������ܽſ��ƱõĿ���-���ʡ�
	PUMPCTRL_LOW_OUT(); //�ܽ�2-�������
	
    VCTRL_MID_OUT();    //Сѭ������-���������
	VCTRL_HIGH_OUT();   //��ѭ������-���������
    VCTRL_KEY_OUT();    //220v��Դ����-���������
   
	LNG_PUMP_PRECOOL(); //����-��һһ������ת��-��ʼԤ�䡣
    VCTRL_OPENKEY();    //��220v��Դ��

    VCTRL_MID_OPEN();   //����2 (��)-��Сѭ����
    VCTRL_HIGH_CLOSE(); //�ط�1 (��)	-�ش�ѭ����
    otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1); 	//200ms//otimer_reqire(LCNGCTRL_DISP_EV, 2, 1, 1); 	//200ms
    if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
		{
			//������A��1
			INTER_LINE_A_HIGH();
	  	}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
		{
			//������B��1
			INTER_LINE_B_HIGH();	
		}
	}
}
/*****************************
˵�����°�ܵ���ѭ��Ԥ�����
******************************/
void olngctrl_Bpre_cool(void)
{
	bprecool_state = false;
	modbus_chk_cnt = modbus_err_rd_cnt = 0;
	otimer_release(LCNGCTRL_DISP_EV); 
	okernel_addeventpro(LCNGCTRL_DISP_EV, olngctrl_precool_disp_pro);

	PUMPCTRL_HIGH_OUT();
	PUMPCTRL_LOW_OUT();
	
       VCTRL_MID_OUT();
	VCTRL_HIGH_OUT();
       VCTRL_KEY_OUT(); 
   
	LNG_PUMP_PRECOOL();
      VCTRL_OPENKEY();
    VCTRL_MID_CLOSE();//�ط�2 (��)//ycx20150602 �ͻ�Ҫ��Ԥ����ô�ѭ��
    VCTRL_HIGH_OPEN();//����1 (��)	
    otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1); 	//200ms//otimer_reqire(LCNGCTRL_DISP_EV, 2, 1, 1); 	//200ms
    if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
		{
			//������A��1
			INTER_LINE_A_HIGH();
	  	}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
		{
			//������B��1
			INTER_LINE_B_HIGH();	
		}
	}
}

unsigned int _lng_stop_cnt = 0;
void olngctrl_stop_condition(void)
{
	ocomm_printf(CH_UART,0,0,"%.02f  %.02f  %.02f %.02f %.02f\r\n",cur_totalize_l,cur_calFactor,cur_init_lcng_sum,cur_rec.rec_lcngnum,cur_rec.rec_lcngsum);       
	if(cur_flowrate_l < cur_sysparas.c_lcng_ctrl_paras[4]) //20130-4-08
	{
		if( ++_lng_stop_cnt >= 5)
		{
			_lng_stop_cnt = 0;
			otimer_release(LCNG_COMM_EVENT1_EV);
			okernel_putevent(KEY_STOP_EV,1);
			return;
		}
		otimer_reqire(LCNG_COMM_EVENT1_EV,2,0,1);
		return;
	}
	_lng_stop_cnt = 0;
	otimer_reqire(LCNG_COMM_EVENT1_EV,2,0,1);
}

void olngctrl_disp_pro_ex(void)
{
    //unsigned long lngnum = 0;
    long lngnum = 0;
    //unsigned long g_lngsum = 0,g_lngnum = 0;
    long g_lngsum = 0,g_lngnum = 0;
    unsigned long tmp_long = 0;
    //��ʼ����
   
    lngnum = (long)(cur_totalize_l*cur_calFactor*100 + 0.5);
    if(cur_init_lcng_sum<1)//�����λ���ϵ���һ�μ����쳣
    {
        cur_init_lcng_sum=cur_totalize_l*cur_calFactor+0.005;
    }
    if(lngnum <= (long)(cur_init_lcng_sum*100 + 0.5))
    {
        cur_init_lng_chk++;
        return;
    }

    lngnum = lngnum - (long)(cur_init_lcng_sum*100 + 0.5) - cur_back_num_ex;////��ǰ������=��ǰ����-��ʼ����-��������
    if(lngnum < cur_rec.rec_lcngnum)
    {
        cur_init_lng_chk++;
        return;
    }

    if(fabs(lngnum - cur_rec.rec_lcngnum) > cur_price*200)  //���������쳣
    {
        lngnum = cur_rec.rec_lcngnum;
        blcng_running = false;
        halt_err |= 1<<HALTERR_SYSTEM_DATA_ERR;
        okernel_putevent(KEY_STOP_EV,0);
        return;
    }

    cur_init_lng_chk = 0;
    cur_rec.rec_lcngnum = lngnum;
    cur_rec.rec_lcngsum = (long)(cur_rec.rec_lcngnum*cur_price+0.5);

    // #ifdef LNG_SUM_G
    if(cur_sysparas.b_y_or_n_operate & BASIC_P_BACK_GAS)
    {
        //ocomm_printf(CH_DISP,4,1,"Vg:%.02f Pg:%.02f",cur_flowrate_g, cur_pressure_g);
        //�����������
        g_lngnum = (long)(cur_totalize_g*100 + 0.5);
        //if(g_lngnum <= (long)(cur_init_lng_g_sum *100 + 0.5)) return;

        g_lngnum =g_lngnum -(long)(cur_init_lng_g_sum*100 + 0.5);
        //if(g_lngnum< cur_rec.rec_relcngnum)   return;

        if(fabs(g_lngnum - cur_rec.rec_relcngnum) > (long)(cur_price*200))
        {
            g_lngnum = cur_rec.rec_relcngnum;
            blcng_running = false;
            halt_err |= 1<<HALTERR_SYSTEM_DATA_ERR;
            okernel_putevent(KEY_STOP_EV,0);
            return;
        }
//        g_lngsum = (long)(g_lngnum*cur_price+0.5);
//        cur_rec.rec_relcngnum = g_lngnum;
//        cur_rec.rec_relcngsum = g_lngsum;

        cur_back_num = g_lngnum;////�������̻�����
        if(cur_back_num <= 0.2)
            cur_back_num = 0;

        cur_rec.rec_lcngnum = cur_rec.rec_lcngnum - cur_rec.rec_relcngnum;        
        //cur_rec.rec_lcngsum = cur_rec.rec_lcngsum - cur_rec.rec_relcngsum;
        cur_rec.rec_lcngsum = (long)(cur_rec.rec_lcngnum*cur_price+0.5);
    }
    //#endif
    //ocomm_printf(CH_DISP,2,1,"����:%.02f",(float)cur_rec.rec_lcngnum/100.00);//////and20141022
    //ocomm_printf(CH_DISP,3,1,"���:%.02f",(float)cur_rec.rec_lcngsum/100.00);
//    if(cur_flowrate_l < cur_flowrate_g)return;////�������ٴ󣬲�ˢ��
    if(cur_rec.rec_lcngnum < 0)
        osplc_show(fabs(cur_rec.rec_lcngnum)/100.00,fabs(cur_rec.rec_lcngsum)/100.00,cur_price,1,false);
    else    
        osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,false);

    opotocol_zyjn_lcngrunstate_pro();

    ocomm_printf(CH_DISP,2,1,"����:%.02f",cur_flowrate_l);
    ocomm_printf(CH_DISP,3,1,"�¶�:%.02f",vftemp);
    ocomm_printf(CH_DISP,4,1,"����:%.02f",vfdgain);    
    if(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK)  // �ÿ�����
    {
        tmp_long = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
#if ICCARD_M1_MINGTE
        if(cur_card_data.block2_buf[3]  == '3')//���˿�----���������
#else
        if(cpu_card_data.CpuCardType  == 0x03)//���˿�----���������
#endif
        {
            if((cur_rec.rec_lcngsum+ tmp_long) >= BASIC_ICCARD_MAX_MOMEY)
            {
                cur_lcng_opt_type = _CNG_FIX_M;
                cur_fix_money = BASIC_ICCARD_MAX_MOMEY/100 -(float)tmp_long/100;
                ofixctrl_fix_lng_stop_pro();
                return;
            }
        }
        else//�Ǽ��˿�-----����С���
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
    //��������
    if(_CNG_FIX_M == cur_lcng_opt_type)
    {
        if((cur_rec.rec_lcngsum + (unsigned long)(cur_price*10 + 0.05)) >= (unsigned long)(cur_fix_money*100))
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
            blcng_running = false;
            ofixctrl_fix_lng_stop_pro();
            return;
        }
    }
}

void olngctrl_vctrl_stop(void)
{
	unsigned char status_a_input = 0;
	unsigned char status_b_input = 0;
	unsigned char status_a = 0;
	unsigned char status_b = 0;
	blcng_running = false;
	otimer_release(LCNGCTRL_DISP_EV);	
	otimer_release_all();

	PUMPCTRL_HIGH_OUT();
	PUMPCTRL_LOW_OUT();

	VCTRL_MID_OUT();
	VCTRL_HIGH_OUT();
      
	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_ONE_MECH)
	{
		VCTRL_MID_OPEN();  //����2 (��)
		VCTRL_HIGH_CLOSE();//�ط�1 (��)	
		//LNG_PUMP_IDLE();
		LNG_PUMP_PRECOOL();  //05-01
	}
	else if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_INPUT)
		{
			//������A����״̬,������BΪ���
			status_a_input = PINL;
			status_a_input >>= 0;
			status_a = status_a_input&0x01;
			ocomm_printf(CH_DISP,1,1,"��12:%x",status_a);
			if((status_a_input&0x01) == 0)//�Է�����
			{
				VCTRL_MID_OPEN();  //����2 (��)
				VCTRL_HIGH_CLOSE();//�ط�1 (��)	
				//LNG_PUMP_IDLE();
				LNG_PUMP_PRECOOL();  //05-01
			}
			else if((status_a_input&0x01) == 1)//�Է���Һ
			{
				VCTRL_MID_CLOSE();
				VCTRL_HIGH_CLOSE();
				//LNG_PUMP_IDLE();
				LNG_PUMP_CTRL();
			}
			//������B	����
			status_b_input = 0; 
		}
 		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_INPUT)
 		{
			//������B����״̬
			status_b_input = PINL;
			status_b_input >>= 1;
			status_b = status_b_input&0x01;
			ocomm_printf(CH_DISP,1,1,"��22:%x",status_b);
			if((status_b_input&0x01) == 0)//�Է�����
			{
				VCTRL_MID_OPEN();  //����2 (��)
				VCTRL_HIGH_CLOSE();//�ط�1 (��)	
				LNG_PUMP_PRECOOL();  //05-01
			}
			else if((status_b_input&0x01) == 1)//�Է���Һ
			{
				VCTRL_MID_CLOSE();
				VCTRL_HIGH_CLOSE();
				//LNG_PUMP_IDLE();  
				LNG_PUMP_CTRL();
			}
			//������A����
			status_a_input = 0;	
		}
	}
	else
	{
		VCTRL_MID_OPEN();  //����2 (��)
		VCTRL_HIGH_CLOSE();//�ط�1 (��)	
		//LNG_PUMP_IDLE();
		LNG_PUMP_PRECOOL();  //05-01
	}
}

void olngctrl_stop_disp(void)
{
  unsigned int idle_dly = 18000;
  //unsigned char char_tmp = 0;
  unsigned long tmp = 0;
  if(halt_err != 0)
	  ocomm_printf(CH_DISP,1,1,"�쳣ͣ��:%d",halt_err);
  else
  	  ocomm_printf(CH_DISP,1,1,"����ͣ��:");  
  ds3231_read_date((unsigned char*)&cur_rec.rec_etime[0],true);
  ds3231_read_time((unsigned char*)&cur_rec.rec_etime[3],true); 
  
  if(!ostore_rec_write())    /////������
  {
	ocomm_printf(CH_DISP,1,4,"�洢�쳣");  
  }


  //cur_rec.rec_lcngnum = (long)(cur_gas_num + 0.5);
  //cur_rec.rec_lcngsum = (long)(cur_gas_sum + 0.5); 
  ocomm_printf(CH_DISP,2,1,"����:%.02f",(float)cur_rec.rec_lcngnum/100.00);
  ocomm_printf(CH_DISP,3,1,"���:%.02f",(float)cur_rec.rec_lcngsum/100.00);
  if(cur_rec.rec_lcngnum < 0)
	osplc_show(fabs(cur_rec.rec_lcngnum)/100.00,fabs(cur_rec.rec_lcngsum)/100.00,cur_price,1,true);
 else
	osplc_show((double)cur_rec.rec_lcngnum/100.00,(double)cur_rec.rec_lcngsum/100.00,cur_price,0,true);

  if(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK){
		 tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_M, 4);
		 ocomm_printf(CH_DISP,4,1,"���:%ld.%02d",tmp/100,tmp%100); 
  }  		 
  else{
		 ocomm_printf(CH_DISP,4,1,"    ��Һ����    ");
  }
  cur_lcng_opt_type = _CNG_NO_OPT;
  opotocol_zyjn_data_exchange(0, false,true,(bool)(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK),NULL);//opotocol_zyjn_data_exchange();
  
  if(cur_sysparas.b_y_or_n_operate&BASIC_P_AUTO_PRT)
  {
	 oprt_rec(false,0);
  }
  cur_back_num_ex = 0;
  okernel_clreventpro();
  okernel_addeventpro(KEY_LCNG_EV, olcngctrl_ctrl_start_pro);
  okernel_addeventpro(KEY_FIX_EV,  ocngctrl_ctrl_fix_start_pro);  
  okernel_addeventpro(KEY_OK_EV,   olcng_usr_prtrec);  
  okernel_addeventpro(LCNG_COMM_EVENT4_EV,  olcngctrl_ctrl_disp_shine_pro);
  okernel_addeventpro(KEY_RET_EV,  oui_main_ui); 
  
  okernel_addeventpro(LCNG_COMM_EVENT3_EV,Main_Release_pressure);
  otimer_reqire(LCNG_COMM_EVENT3_EV,4,0,1);
  if((unsigned int)cur_sysparas.c_lcng_ctrl_paras[5]<1)
  	olngctrl_vctrl_off();//��Һ��Ϻ�ֱ��ͣ�� ycx20161014
if(true == b_Simu_auto && true == b_Simulation)
	otimer_reqire(KEY_RET_EV, 20, 0, 1);	//5  = 2000ms
   
  b_lcng_disp_shine = true;
  idle_dly = (unsigned int)cur_sysparas.c_lcng_ctrl_paras[5];
  if(idle_dly>=1 && idle_dly<=72000) idle_dly = idle_dly*10;
  else                              idle_dly = 1800;
  otimer_reqire(LCNG_COMM_EVENT5_EV, idle_dly, 0, 1); 	//180000ms = 180 s	
}

#ifdef LNG_COEFFI_SET
	void olngctrl_start_init(void)
	{
	          VCTRL_MID_OUT();   
		VCTRL_MID_CLOSE();//�ط�2 (��)
	       _delay_ms(100);
		
		if(false == b_Simulation)
	       {
			if(!omodbus_read_auto_buffer(_MODBUS_L_,5))
			{
				ocomm_message(3,1,"�����¼���",20, oui_main_ui);
				//ocomm_message(3,1,"��Һ�������ƴ�",20, oui_main_ui);
				return;
			}
			cur_init_lcng_sum  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TOTALIZE);
			
	    // #ifdef LNG_SUM_G
		if(cur_sysparas.b_y_or_n_operate & BASIC_P_BACK_GAS ) 
		{
			if(!omodbus_read_auto_buffer(_MODBUS_G_,5))
			{
				ocomm_message(4,1,"�����������ƴ�",20, oui_main_ui);
				return;
			}
			cur_init_lng_g_sum  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_TOTALIZE);
		}
	   // #endif	
	       }
		else
		{
			cur_init_lcng_sum = 0;
			cur_init_lng_g_sum = 0;	
		}
		
		cur_init_lcng_sum = cur_init_lcng_sum*cur_calFactor;
		cur_init_lcng_sum = (float)((unsigned long)((cur_init_lcng_sum + 0.005)*100))/100; //(unsigned int)

	       PUMPCTRL_HIGH_OUT();
		PUMPCTRL_LOW_OUT();
		
		VCTRL_HIGH_OUT();
	       VCTRL_KEY_OUT();
		
		LNG_PUMP_CTRL();
		VCTRL_OPENKEY();
		VCTRL_MID_CLOSE();//�ط�2 (��)
		VCTRL_HIGH_OPEN();//����1 (��)
	 }


    void olngctrl_start_pro(void)
    {
    unsigned int idle_dly = 1800;
	head_cmd = NULL;
	tail_cmd = NULL;
	running_cmd = NULL;
	otimer_release(LCNG_COMM_EVENT5_EV);  //04-28
	olcm_clrscr();
	cur_calFactor =cur_sysparas.b_factor[0];
	if(true == b_Simulation)
	{
		cur_pressure_l = 0.6;
		cur_pressure_g = 0.6; 
	}

	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
		{
			//������A��1
			INTER_LINE_A_HIGH();
	  	}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
		{
			//������B��1
			INTER_LINE_B_HIGH();	
		}
	}
	 
	  okernel_clreventpro();

	  okernel_addeventpro(LCNG_COMM_EVENT1_EV, olngctrl_stop_condition);

	  okernel_addeventpro(KEY_STOP_EV,         olngctrl_stop_pro);
	  	 //zl add
	  okernel_addeventpro(LCNG_COMM_EVENT5_EV,olngctrl_vctrl_off);

	   ocomm_printf(CH_DISP,1,1,"״̬: ���ڼ�Һ");
	  blcng_running = true;
	  blcng_powersave = true;
	   opotocol_get_card_info();

	   ds3231_read_date((unsigned char*)&cur_rec.rec_stime[0],true);
	   ds3231_read_time((unsigned char*)&cur_rec.rec_stime[3],true);

	   _lng_stop_cnt = 0;
	   Precool_Flag = 3;
	idle_dly = (unsigned int)cur_sysparas.c_lcng_ctrl_paras[3];
	if(idle_dly>=1 && idle_dly<=72000) idle_dly = idle_dly*10;
	else                              idle_dly = 1800;
	otimer_reqire(LCNG_COMM_EVENT1_EV, idle_dly, 0, 1); 	//180000ms = 180 s	
	//otimer_reqire(LCNG_COMM_EVENT1_EV,(unsigned int)cur_sysparas.c_lcng_ctrl_paras[3],0,1);
	}

#else
	void olngctrl_start_pro(void)
	{
		unsigned int idle_dly = 1800;
		head_cmd = NULL;
		tail_cmd = NULL;
		running_cmd = NULL;
		unsigned long tmp_l = 0;
		otimer_release(LCNG_COMM_EVENT5_EV);  //04-28
		olcm_clrscr();
		
		//if(cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY] < 0.0 || cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT] < 0.0 || cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT] > 1.2)
		//{
		 // ocomm_message(4,1,"�ܶ����ô���",20, oui_main_ui);
		 // return;	
		//}
		//cur_calFactor = cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT]/cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY];	
		//cur_calFactor = 1;//cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT]; //LNG������Ϊ��λ	
              cur_calFactor =cur_sysparas.b_factor[0];
              
		VCTRL_MID_OUT();   
		VCTRL_MID_CLOSE();//�ط�2 (��)
	       
		if(false == b_Simulation)
	       {
			if(!omodbus_read_auto_buffer(_MODBUS_L_,5))
			{
				ocomm_message(3,1,"�����¼���",20, oui_main_ui);
				//ocomm_message(3,1,"��Һ�������ƴ�",20, oui_main_ui);
				       if(cur_sysparas.b_y_or_n_operate & BASIC_P_CARD_CHK ) 
					{
						oapp_iccard_back_write_flag();
					} 
				return;
			}
			cur_init_lcng_sum  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TOTALIZE);
			
	     //#ifdef LNG_SUM_G
		if(cur_sysparas.b_y_or_n_operate & BASIC_P_BACK_GAS ) 
		{
			if(!omodbus_read_auto_buffer(_MODBUS_G_,5))
			{
				if(cur_sysparas.b_y_or_n_operate & BASIC_P_CARD_CHK ) 
					ocomm_message(4,1,"�����������ƴ�",2, oapp_iccard_back_write_flag);
				else
					ocomm_message(3,1,"�����������ƴ�",2, oui_main_ui);
				return;
			}
			cur_init_lng_g_sum  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_TOTALIZE);
		}
		  //#endif	
	       }
		else
		{
		         cur_init_lcng_sum = 0;
		    //#ifdef LNG_SUM_G		 
			  cur_init_lng_g_sum = 0;	
		   // #endif	
		}
		
		cur_init_lcng_sum = cur_init_lcng_sum*cur_calFactor;
		cur_init_lcng_sum = (float)((unsigned long)((cur_init_lcng_sum + 0.005)*100))/100; //(unsigned int)
		
	      if(true == b_Simulation)
	     {
	        	cur_pressure_l = 0.6;
			cur_pressure_g = 0.6; 
	     }
		  
	     if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	     {
	     	if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
	    	{
				//������A��1
				INTER_LINE_A_HIGH();
		 	}
			else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
			{
				//������B��1
				INTER_LINE_B_HIGH();
			}
	     }
	     
	     okernel_clreventpro();

	     okernel_addeventpro(LCNG_COMM_EVENT1_EV, olngctrl_stop_condition);
	    
	     okernel_addeventpro(KEY_STOP_EV,         olngctrl_stop_pro);
		
	    //zl add
	     okernel_addeventpro(LCNG_COMM_EVENT5_EV,olngctrl_vctrl_off);
	    	
	    // ocomm_printf(CH_DISP,1,1,"״̬: ���ѹ��"); 
		//okernel_addeventpro(LCNG_COMM_EVENT2_EV,GetStartCondition);
	    //_lng_stop_cnt = 0;
		//VCTRL_MID_CLOSE();   
	    //otimer_reqire(LCNG_COMM_EVENT2_EV, 1, 0, 1);
		 
		ocomm_printf(CH_DISP,1,1,"״̬: ���ڼ�Һ");
		blcng_running = true;
		blcng_powersave = true;
		opotocol_get_card_info();
		//cur_lcng_r_rec.brunCNG = LCNGSTATE_HANSHAKING;

		ds3231_read_date((unsigned char*)&cur_rec.rec_stime[0],true);
		ds3231_read_time((unsigned char*)&cur_rec.rec_stime[3],true);//������ʼʱ��
		
		_lng_stop_cnt = 0;
	      Precool_Flag = 3;
	    
	       PUMPCTRL_HIGH_OUT();
		PUMPCTRL_LOW_OUT();
		
		VCTRL_HIGH_OUT();
	       VCTRL_KEY_OUT();
		
		LNG_PUMP_CTRL();
		VCTRL_OPENKEY();
		VCTRL_MID_CLOSE();//�ط�2 (��)
		VCTRL_HIGH_OPEN();//����1 (��)
		idle_dly = (unsigned int)cur_sysparas.c_lcng_ctrl_paras[3];
		if(idle_dly>=1 && idle_dly<=72000) idle_dly = idle_dly*10;
		else                              idle_dly = 1800;
		otimer_reqire(LCNG_COMM_EVENT1_EV, idle_dly, 0, 1); 	//180000ms = 180 s	
		//otimer_reqire(LCNG_COMM_EVENT1_EV,(unsigned int)cur_sysparas.c_lcng_ctrl_paras[3],0,1);
	}	
#endif




//����ǰԤ����ʾ
void start_precool_disp(void)
{
         //zl update
   if( true == b_Simulation)	
   {
        Si_temp -= 0.11 ;
	 Si_gain += 0.11;
        ocomm_printf(CH_DISP,2,1,"�¶�:%.02f", Si_temp);
	 ocomm_printf(CH_DISP,3,1,"����:%.02f", Si_gain);

       if((Si_temp  > -1.00) || (Si_gain < 1.00))
       {
           otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
           return;
       }

       Si_temp = 0;
       Si_gain = 0;
	otimer_release(LCNGCTRL_DISP_EV);   //ģ��OK
	otimer_release(LCNG_COMM_EVENT4_EV); 
	bprecool_state = true;
	ocomm_message(4,1,"Ԥ��ɹ�!",5, olngctrl_ctrl_start);	/**/
	return;
   }	
   else
   {
       //float vftemp  = 0.0;
       //float vfdgain = 0.0; 
	unsigned int vstate = 0;
	
	if(!omodbus_read_auto_buffer(_MODBUS_L_,5))
	{
		if(++modbus_err_rd_cnt > 10)
		{
			otimer_release(LCNGCTRL_DISP_EV); 
			bprecool_state = false;
			ocomm_message(4,1,"������ͨѶ��!",20, NULL);
			return;
		}
	}
	
	vftemp  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TEMPERATURE);
	vfdgain = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_DRVGAIN);
    cur_flowrate_l  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_MASSFLOW);  			
	osplc_show(0,0,cur_price,0,false);//������ʾ
	
	if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)
	{
		vstate  = omodbus_read_auto_uint_value(_MODBUS_L_, AUTO_SCAN_LCNG_STATE);	
		vfdgain *= 10;
		ocomm_printf(CH_DISP,4,1,"״̬:%d", vstate);
	}
	else
	{
		vstate = 0;
		if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)  vstate = 1;
	}

	ocomm_printf(CH_DISP,2,1,"�¶�:%.02f", vftemp);
	ocomm_printf(CH_DISP,3,1,"����:%.02f", vfdgain);
	
    if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_RM)
    {
       /////2012-11-22
		if( vstate != 1 ||  vftemp > cur_sysparas.c_lcng_ctrl_paras[0] || vfdgain>5)
		{
			otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
			return;
		}
		
	      if(vfdgain > cur_sysparas.c_lcng_ctrl_paras[1])   // 4.0
	      { 
	               modbus_chk_cnt = 0;
	               max_drv_gain = vfdgain;   //(unsigned int)(fvrd*100);
			 otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	               return;
	     }

    }
	else if(cur_sysparas.b_basic_p[BASIC_P_MODBUS_TYPE] == MODBUS_TYPE_EH)
	{
	      if(vftemp > cur_sysparas.c_lcng_ctrl_paras[0])
	      {
			otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
			return;
	      }
	      
	      if(vfdgain > cur_sysparas.c_lcng_ctrl_paras[1])   // 4.0
	      { 
	               modbus_chk_cnt = 0;
	               max_drv_gain = vfdgain;   //(unsigned int)(fvrd*100);
			 otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	         return;
	      }
	}
	
     min_drv_gain = vfdgain;      //min = (unsigned int)(fvrd*100);
	//if( fabs(max_drv_gain - min_drv_gain) < 0.40)//40 )
    if(fabs(max_drv_gain - min_drv_gain) < 3.00)//40 )
    {
		//�����ȶ�֮������٣�    
		if(++modbus_chk_cnt > DRVGAIN_CHKLEN || vstate == 1)
		{
			otimer_release(LCNGCTRL_DISP_EV); 
                     bprecool_state = true;
			
		       olngctrl_ctrl_start();
		}
		otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	}
	else
	{
		max_drv_gain = min_drv_gain;
		modbus_chk_cnt = 0;
		otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1);
	}
   }
}


//����ǰԤ�����
void start_precool(void)
{
	bprecool_state = false;
	modbus_chk_cnt = modbus_err_rd_cnt = 0;
	otimer_release(LCNGCTRL_DISP_EV); 
	okernel_addeventpro(LCNGCTRL_DISP_EV, start_precool_disp);

	PUMPCTRL_HIGH_OUT();
	PUMPCTRL_LOW_OUT();

	VCTRL_MID_OUT();
	VCTRL_HIGH_OUT();
	VCTRL_KEY_OUT();  

	LNG_PUMP_PRECOOL();
	VCTRL_OPENKEY();

	VCTRL_MID_OPEN();//����2 (��)
	VCTRL_HIGH_CLOSE();//�ط�1 (��)
	otimer_reqire(LCNGCTRL_DISP_EV, 2, 0, 1); 	//200ms//otimer_reqire(LCNGCTRL_DISP_EV, 2, 1, 1); 	//200ms           
	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH)
	{
		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_OUTPUT)
		{
			//������A��1
			INTER_LINE_A_HIGH();
		}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_OUTPUT)
		{
			//������B��1
			INTER_LINE_B_HIGH();	
		}
	}
}

//����ǰԤ��
void ctrl_start_precool(void)
{
    olcm_clrscr();	
    Precool_Flag = 1;
    ocomm_printf(CH_DISP,1,1,"����Ԥ��...");
    okernel_clreventpro();
    okernel_addeventpro(LCNG_MAINUI_EV, oui_main_ui);
    okernel_addeventpro(KEY_STOP_EV,stop_precool_oper);
    start_precool();
}

void olngctrl_back_disp_pro_ex(void)//////��ʵ������ʾ
{
	long g_lngnum = 0;
	ocomm_printf(CH_DISP,4,1,"Vg:%.02f Pg:%.02f",cur_flowrate_g, cur_pressure_g);
	g_lngnum =  (long)(cur_totalize_g*100+0.5);
	if(g_lngnum <= (long)(cur_init_lng_g_sum*100+0.5))return;
	g_lngnum =g_lngnum - (long)(cur_init_lng_g_sum*100+0.5);
	//cur_rec.rec_relcngnum = g_lngnum;
	//g_lngnum = cur_rec.rec_relcngnum;
	cur_back_num_ex = g_lngnum;
	ocomm_printf(CH_DISP,3,1,"����:%.02f",(cur_back_num_ex/100.00));
	cur_back_sum_ex = cur_back_num_ex*cur_price + 0.005;
	osplc_show(cur_back_num_ex/100.00,cur_back_sum_ex/100.00,cur_price,1,false);	
}

unsigned char back_stop_cnt = 0;////����ֹͣ����
void olngctrl_back_disp_pro(void)
{
	if(!omodbus_read_auto_buffer(_MODBUS_G_,5))
	{
		if(nmodbus_Errcnt++ > 10) 
		{
			halt_err |= 1<<HALTERR_MODBUS_G_AUTO_ERR;
			otimer_release(LCNG_COMM_EVENT1_EV);
			okernel_putevent(KEY_STOP_EV,0); 
		}
		return;
	}
	cur_totalize_g  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_TOTALIZE); 
	nmodbus_Errcnt = 0;
	olngctrl_back_disp_pro_ex();
	if(back_stop_cnt++ > 4)
	{
		otimer_release(LCNG_COMM_EVENT1_EV);
		okernel_putevent(KEY_STOP_EV,0); 
		return;
	}
	otimer_reqire(LCNG_COMM_EVENT1_EV, 2, 0, 1);
}

void olngctrl_stop_back_disp(void)
{
	olcm_clrscr();	
	ocomm_printf(CH_DISP,2,1,"��������:%.02f",cur_back_num_ex/100.00);
	ocomm_printf(CH_DISP,1,1,"    ��������");
  	okernel_addeventpro(KEY_LCNG_EV, olngctrl_ctrl_start_ex);
	okernel_addeventpro(KEY_CLR_EV, olngctrl_ctrl_back_ex);
	okernel_addeventpro(KEY_RET_EV, olngctrl_ctrl_prepare);
}

void olngctrl_stop_back_pro(void)
{
	back_stop_cnt  = 0;
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, olngctrl_back_disp_pro);		
	okernel_addeventpro(KEY_STOP_EV,         olngctrl_stop_back_disp);	
	otimer_reqire(LCNG_COMM_EVENT1_EV, 2, 0, 1);  
}

void ocngctrl_simu_back_disp_pro_ex(void)////////ģ�������ʾ
{
	long g_lngnum = 0;
	cur_totalize_g += 0.01;
	cur_pressure_g += 0.01; 
	cur_flowrate_g += 0.01;
	ocomm_printf(CH_DISP,4,1,"Vg:%.02f Pg:%.02f",cur_flowrate_g, cur_pressure_g);
	g_lngnum =  (long)(cur_totalize_g*100+0.5);
	if(g_lngnum <= (long)(cur_init_lng_g_sum*100+0.5))return;
	g_lngnum =g_lngnum - (long)(cur_init_lng_g_sum*100+0.5);
	//cur_rec.rec_relcngnum = g_lngnum;
	//g_lngnum = cur_rec.rec_relcngnum;

	cur_price = cur_sysparas.f_price[0]; 
	cur_back_num_ex = g_lngnum;
	g_lngnum = cur_back_num_ex;
	cur_back_sum_ex = cur_back_num_ex*cur_price + 0.005;
	ocomm_printf(CH_DISP,3,1,"����:%.02f",cur_back_num_ex/100.00);
	osplc_show(cur_back_num_ex/100.00,cur_back_sum_ex/100.00,cur_price,1,false);		
}

void olcngctrl_back_ctrl(void)
{
	if(true == b_Simulation)
	{
		ocngctrl_simu_back_disp_pro_ex();
		return;
	}
	if(!omodbus_read_auto_buffer(_MODBUS_G_,5))
	{
		if(nmodbus_Errcnt++ > 10) 
		{
			blcng_running = false;
			halt_err |= 1<<HALTERR_MODBUS_G_AUTO_ERR;
			okernel_putevent(KEY_STOP_EV,0); 
		}
		return;
	}
	nmodbus_Errcnt = 0;
	cur_flowrate_g  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_MASSFLOW);  			
	cur_totalize_g  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_TOTALIZE); 
	ovctrl_getpressure(_MODBUS_G_, &cur_pressure_g);		
	olngctrl_back_disp_pro_ex(); ////������ʾ	
}

void olngctrl_stop_back_condition(void)
{
	olcngctrl_back_ctrl();
	if(cur_flowrate_g < cur_sysparas.c_lcng_ctrl_paras[8])
	{
		if( ++_lng_stop_cnt >= 5)
		{
			_lng_stop_cnt = 0;
			otimer_release(LCNG_COMM_EVENT1_EV);
			okernel_putevent(KEY_STOP_EV,1);
			return;
		}
		otimer_reqire(LCNG_COMM_EVENT1_EV,2,0,1);
		return;
	}
	_lng_stop_cnt = 0;
	otimer_reqire(LCNG_COMM_EVENT1_EV,2,0,1);
}

void olngctrl_back_start(void)/////��ʼ����
{
	olcm_clrscr();
	if(false == b_Simulation)
	{
		if(!omodbus_read_auto_buffer(_MODBUS_G_,5))
		{
			ocomm_message(3,1,"�����������ƴ�",2, oui_main_ui);
			return;
		}
		cur_init_lng_g_sum  = omodbus_read_auto_float_value(_MODBUS_G_, AUTO_SCAN_LCNG_TOTALIZE);
	 }
	else
	{
		cur_init_lng_g_sum = 0;	
	}
	okernel_clreventpro();
	okernel_addeventpro(LCNG_COMM_EVENT1_EV, olngctrl_stop_back_condition);//ֹͣ��������
	okernel_addeventpro(KEY_STOP_EV,         olngctrl_stop_back_pro);//ֹͣ����
	ocomm_printf(CH_DISP,1,1,"״̬: ���ڻ���");
	otimer_reqire(LCNG_COMM_EVENT1_EV,5,0,1);
}	

void olngctrl_ctrl_prepare(void)//////��Һǰ׼��
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,1,"��Һǰ���Ȼ���");
	ocomm_printf(CH_DISP,2,1,"��ȷ����  ��Һ");
	ocomm_printf(CH_DISP,3,1,"�������  ����");
	ocomm_printf(CH_DISP,4,1,"�����ء�  ȡ��");
	okernel_clreventpro();
	//okernel_addeventpro(KEY_OK_EV, ctrl_start_precool);
	okernel_addeventpro(KEY_OK_EV, olngctrl_ctrl_start);
	okernel_addeventpro(KEY_CLR_EV, olngctrl_back_start);
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);
}

void olngctrl_ctrl_back_ex(void)////��ʼ����
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,3,"��ȷ��");
	ocomm_printf(CH_DISP,2,1,"����ǹ���������");
	ocomm_printf(CH_DISP,3,1,"��ȷ�ϡ�  ����");
	ocomm_printf(CH_DISP,4,1,"�����ء�  ȡ��");
	okernel_clreventpro();
	okernel_addeventpro(KEY_OK_EV, olngctrl_back_start);
	okernel_addeventpro(KEY_RET_EV, olngctrl_ctrl_prepare);
}	

void olngctrl_ctrl_start_ex(void)////��ʼ��Һ
{
	olcm_clrscr();
	ocomm_printf(CH_DISP,1,3,"��ȷ��");
	ocomm_printf(CH_DISP,2,1,"��Һǹ���복��ƿ");
    ocomm_printf(CH_DISP,3,1,"��ȷ�ϡ�  ��Һ");
	ocomm_printf(CH_DISP,4,1,"�����ء�  ȡ��");
	okernel_clreventpro();
	if(true == b_Simu_auto && true == b_Simulation)
	{
		ctrl_start_precool();
	}
	//okernel_addeventpro(KEY_OK_EV, olngctrl_ctrl_start);
	okernel_addeventpro(KEY_OK_EV, ctrl_start_precool);
	//okernel_addeventpro(KEY_RET_EV, olngctrl_ctrl_prepare);
	okernel_addeventpro(KEY_RET_EV, oui_main_ui);
}

void olngctrl_ctrl_start(void)
{
	olcm_clrscr();
	halt_err = 0;
	if( false == b_Simulation)
	{
		if( bprecool_state == false) //δԤ��
		{
			ocomm_printf(CH_DISP,1,1,"ϵͳ����");
			ocomm_message(2,1,"  δԤ��",20, oui_main_ui);
			return;		
		}
		if(sys_err != 0 || cur_lcng_opt_type == _CNG_NO_OPT)
		{
			ocomm_printf(CH_DISP,2,1,"�������:%d",sys_err);
			ocomm_message(1,1,"  ϵͳ����",20, oui_main_ui);
			return;
		}
	}

      if(true == b_Simulation)
      {
             if( bprecool_state == false) //δԤ��
		{
			ocomm_printf(CH_DISP,1,1,"ϵͳ����");
			ocomm_message(2,1,"  δԤ��",20, oui_main_ui);
			return;
		}
		cur_totalize_l = 0;	 
	       cur_totalize_g = 0;
      }

	if(!osys_rec_continuity_chk())
	{
		ocomm_message(1,1,"��ˮ�Ų�����",50, oui_main_ui);
		return;
	}
      
	memset((unsigned char*)&cur_rec.rec_id, 0, sizeof(struct def_LCNG_Record));
	//memset((unsigned char*)&cur_lcng_t_rec.UID[0], 0, sizeof(struct def_LCNG_TRANS_REC));  //2013-01-04 update
	okernel_clreventpro();
	okernel_addeventpro(LCNGCTRL_START_EV, olngctrl_start_pro);
	
	cur_price = cur_sysparas.f_price[0]; 
	cur_rec.rec_lcngprice = cur_price;
	if(cur_lcng_opt_type == _CNG_FIX_N) cur_fix_money = cur_fix_num*cur_price; 
	
	//ocomm_float2bcdbuf(cur_price, (char*)&cur_lcng_t_rec.RecPrice[0],2 );	//2012-01-04 update
	ocomm_float2bcdbuf(cur_price, (char*)&cur_lcng_r_rec.brunPrice[0],2);

       if(cur_sysparas.b_y_or_n_operate & BASIC_P_CARD_CHK ) 
       {
		oapp_iccard_ready_execute();//oapp_iccard_ready_pro(); //oapp_card_chk_pro();  //�ÿ�
       } 
	else
	{
	      #ifdef LNG_COEFFI_SET
		  unsigned int tmp = (unsigned int)(cur_sysparas.b_factor[1]);
		  if(tmp != 0)
		  {
		         ocomm_printf(CH_DISP,1,1,"��Һ�ȴ���...");
		         olngctrl_start_init();
			  otimer_reqire(LCNGCTRL_START_EV,tmp,0,1); 
	         }
		  else
		  {
		          olngctrl_start_init(); 
		          okernel_putevent(LCNGCTRL_START_EV,0);
		  }
	      #else
		  okernel_putevent(LCNGCTRL_START_EV,0);
	      #endif	  
	}
}


/*
@��ü�������
@GetStartCondition
*/
/*
void GetStartCondition(void)
{
       float tmp_l= 0.0,tmp_g=0.0;
	if(true == b_Simulation)
	{
		tmp_l = cur_pressure_l;
		tmp_g = cur_pressure_g;
	}
	else
	{
		ovctrl_getpressure(_MODBUS_L_, &cur_pressure_l);
	       ovctrl_getpressure(_MODBUS_G_, &cur_pressure_g);
		tmp_l = cur_pressure_l;
		tmp_g = cur_pressure_g;
	}
	
       if(true == b_Simulation)
       {
           cur_pressure_g --;
       }
	   
	 if((tmp_l - tmp_g) < 0.3)
	 {  
	       _lng_stop_cnt ++;
	         
		if(_lng_stop_cnt > 100)
		{
		       ocomm_message(2,1,"��Һʧ��",20, oui_main_ui);
			VCTRL_MID_OPEN();
		}
		else
		{
	            ocomm_printf(CH_DISP,4,1,"Pl:%0.2f Pg:%0.2f",tmp_l,tmp_g);
	            otimer_reqire(LCNG_COMM_EVENT2_EV,2,0,1);
		}
	}
	else
	{
	       ocomm_printf(CH_DISP,1,1,"״̬: ���ڼ�Һ");
		blcng_running = true;
		blcng_powersave = true;
	       opotocol_get_card_info();
		//cur_lcng_r_rec.brunCNG = LCNGSTATE_HANSHAKING;
		ds3231_read_date((unsigned char*)&cur_rec.rec_stime[0],true);
	       ds3231_read_time((unsigned char*)&cur_rec.rec_stime[3],true);
		_lng_stop_cnt = 0;

		LNG_PUMP_CTRL();
	       VCTRL_OPENKEY();
	       VCTRL_MID_CLOSE();//�ط�2 (��)
	       VCTRL_HIGH_OPEN();//����1 (��)
		otimer_reqire(LCNG_COMM_EVENT1_EV,30,0,1); 
	}
}
*/
/*
@���жѹ������
@release_pressure
@
*/
float test_press = 2.0;
void release_pressure(void)
{
	float tmp = 0.0;
	unsigned char status_a_input = 0;
	unsigned char status_b_input = 0;
	ovctrl_getpressure(_MODBUS_L_, &cur_pressure_l);
	tmp = cur_pressure_l;

	VCTRL_KEY_OUT();  //08-01
	VCTRL_OPENKEY();
	VCTRL_MID_OUT();
 
	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_ONE_MECH)
	{
		//VCTRL_MID_OPEN();
		//otimer_reqire(LCNG_COMM_EVENT3_EV, 50, 0, 1);
		if((Precool_Flag == 1)||(Precool_Flag == 3))
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 30, 0, 1);
			return;
		}
		if(tmp > cur_sysparas.c_lcng_ctrl_paras[6])
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else if(tmp<cur_sysparas.c_lcng_ctrl_paras[7])
		{
			VCTRL_MID_CLOSE();//�ط�2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
	}	 
	else if((cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH))
    	{
    		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_INPUT)
		{
			//������A����״̬PB0
			//INTER_LINE_A_IN(); 
			status_a_input = PINL;
			status_a_input >>= 0;
			//status_a = status_a_input&0x01;
			//ocomm_printf(CH_DISP,1,1,"��13:%x",status_a);
			if((status_a_input&0x01) == 0)//�Է�����
			{
				VCTRL_MID_OPEN();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}
			if((status_a_input&0x01) == 1)//�Է���Һ
			{
				VCTRL_MID_CLOSE();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}	
		}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_INPUT)
		{
			//������B����״̬PB1
			//INTER_LINE_B_IN();
			status_b_input = PINL;
			status_b_input >>= 1;
			//status_b = status_b_input&0x01;
			//ocomm_printf(CH_DISP,1,1,"��23:%x",status_b);
			if((status_b_input&0x01) == 0)//�Է����Ӵ���
			{
				VCTRL_MID_OPEN();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}
			else if((status_b_input&0x01) == 1)//�Է����Ӽ�Һ
			{
				VCTRL_MID_CLOSE();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}
		}	
	}	
	else
	{
		//VCTRL_MID_OPEN();
		//otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		if((Precool_Flag == 1)||(Precool_Flag == 3))
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 30, 0, 1);
			return;
		}
		if(tmp > cur_sysparas.c_lcng_ctrl_paras[6])
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else if(tmp<cur_sysparas.c_lcng_ctrl_paras[7])
		{
			VCTRL_MID_CLOSE();//�ط�2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
	}

	  if(true == b_Simulation)
	  {  
	          VCTRL_MID_OPEN();//����2 (��)
	         test_press -= 0.01;
		  ocomm_printf(CH_DISP,3,1,"����жѹ:%.02f",test_press);

		  if(test_press < 0.8)
		  {
		         VCTRL_MID_CLOSE();//�ط�2 (��)        
		          olcm_clrscr();
	                 //ocomm_message(2,2,"  жѹ�ɹ�",20, oui_main_ui);
	                 ocomm_printf(CH_DISP,1,1,"жѹ�ɹ�");
			   //otimer_release(LCNG_COMM_EVENT4_EV);
			   //okernel_addeventpro(LCNG_MAINUI_EV, oui_main_ui);
			   okernel_putevent(LCNG_MAINUI_EV, 1);
			   test_press=2.00;
			   return;
		  }
		  otimer_reqire(LCNG_COMM_EVENT4_EV, 3, 0, 1);  	 
	  }
}

void Main_Release_pressure(void)
{
	unsigned char status_a_input = 0;
	unsigned char status_b_input = 0;
	unsigned char status_a = 0;
	unsigned char status_b = 0;
	float tmp = 0.0;
	ovctrl_getpressure(_MODBUS_L_, &cur_pressure_l);
	tmp = cur_pressure_l;
	VCTRL_KEY_OUT();  //08-01
	VCTRL_OPENKEY();
	VCTRL_MID_OUT();
	if(cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_ONE_MECH)
	{
		VCTRL_MID_OPEN();
		otimer_reqire(LCNG_COMM_EVENT3_EV, 50, 0, 1);
		/*if((Precool_Flag == 1)||(Precool_Flag == 3))
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 30, 0, 1);
			return;
		}
		if(tmp > cur_sysparas.c_lcng_ctrl_paras[6])
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else if(tmp<cur_sysparas.c_lcng_ctrl_paras[7])
		{
			VCTRL_MID_CLOSE();//�ط�2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);*/
	}	 
	else if((cur_sysparas.b_basic_p[BASIC_PUMP_TO_MECH] == ONE_PUMP_TWO_MECH))
    	{
    		if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_A] == PUMP_INTERLINE_INPUT)
		{
			//������A����״̬PB0
		    //INTER_LINE_A_IN(); 
		    status_a_input = PINL;
		    status_a_input >>= 0;
			status_a = status_a_input&0x01;
			ocomm_printf(CH_DISP,1,1,"��13:%x",status_a);
			if((status_a_input&0x01) == 0)//�Է�����
			{
				VCTRL_MID_OPEN();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}
	        if((status_a_input&0x01) == 1)//�Է���Һ
		    {
		        VCTRL_MID_CLOSE();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		    }	
		}
		else if(cur_sysparas.b_basic_p[BASIC_PUMP_INTERLINE_B] == PUMP_INTERLINE_INPUT)
		{
			//������B����״̬PB1
			//INTER_LINE_B_IN();
			status_b_input = PINL;
			status_b_input >>= 1;
			status_b = status_b_input&0x01;
			ocomm_printf(CH_DISP,1,1,"��23:%x",status_b);
			if((status_b_input&0x01) == 0)//�Է����Ӵ���
			{
				VCTRL_MID_OPEN();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}
			else if((status_b_input&0x01) == 1)//�Է����Ӽ�Һ
			{
				VCTRL_MID_CLOSE();
				otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
			}
		}	
	}	
	else
	{
		VCTRL_MID_OPEN();
		otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		/*if((Precool_Flag == 1)||(Precool_Flag == 3))
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 30, 0, 1);
			return;
		}
		if(tmp > cur_sysparas.c_lcng_ctrl_paras[6])
		{
			VCTRL_MID_OPEN();//����2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else if(tmp<cur_sysparas.c_lcng_ctrl_paras[7])
		{
			VCTRL_MID_CLOSE();//�ط�2 (��)
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);
		}
		else
			otimer_reqire(LCNG_COMM_EVENT3_EV, 4, 0, 1);*/
	}

	/*if((Precool_Flag == 1)||(Precool_Flag == 3))
	{
		otimer_reqire(LCNG_COMM_EVENT3_EV, 30, 0, 1);
		return;
	}*/
}
