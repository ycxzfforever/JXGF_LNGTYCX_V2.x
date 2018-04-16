/*
 * lcngctrl.h
 *
 * Created: 2012-8-9 17:12:58
 *  Author: luom
 */ 


#ifndef LCNGCTRL_H_
#define LCNGCTRL_H_

  #define  _PRESSURE_G_SW_	_MODBUS_G_   //����
  #define  _PRESSURE_L_SW_	_MODBUS_L_   //Һ��
	
  #define VCTRL_HIGH_OUT()        SET_PORT_BIT(DDRH,PH6)////DDRH_Bit6   = 1
  #define VCTRL_HIGH_OPEN()       CLR_PORT_BIT(PORTH,PH6)//PORTH_Bit6  = 0
  #define VCTRL_HIGH_CLOSE()      SET_PORT_BIT(PORTH,PH6)//PORTH_Bit6  = 1
  
  #define VCTRL_MID_OUT()         SET_PORT_BIT(DDRH,PH5)//DDRH_Bit5   = 1
  #define VCTRL_MID_OPEN()        CLR_PORT_BIT(PORTH,PH5)//PORTH_Bit5  = 0
  #define VCTRL_MID_CLOSE()       SET_PORT_BIT(PORTH,PH5)//PORTH_Bit5  = 1
  
  #define VCTRL_LOW_OUT()         SET_PORT_BIT(DDRH,PH4)//DDRH_Bit4   = 1
  #define VCTRL_LOW_OPEN()        CLR_PORT_BIT(PORTH,PH4)//PORTH_Bit4  = 0
  #define VCTRL_LOW_CLOSE()       SET_PORT_BIT(PORTH,PH4)//PORTH_Bit4  = 1
  
  #define VCTRL_KEY_OUT()         SET_PORT_BIT(DDRH,PH3)//DDRH_Bit3   = 1 
  #define VCTRL_OPENKEY()         CLR_PORT_BIT(PORTH,PH3)//PORTH_Bit3  = 0
  #define VCTRL_CLOSEKEY()	      SET_PORT_BIT(PORTH,PH3)//PORTH_Bit3  = 1


  #define VCTRL_ADC_IN()		  CLR_PORT_BIT(DDRK,PK1)//DDRK_Bit1  = 0
  #define VCTRL_ADC_H()			  SET_PORT_BIT(PORTK,PK1)//PORTK_Bit1 = 1
  #define VCTRL_ADC_L()			  CLR_PORT_BIT(PORTK,PK1)//PORTK_Bit1 = 0
 
 
  #define LCNG_L_ADC_IN()		  CLR_PORT_BIT(DDRK,PK0)//DDRK_Bit1  = 0
  #define LCNG_L_ADC_H()		  SET_PORT_BIT(PORTK,PK0)//PORTK_Bit1 = 1
  #define LCNG_L_ADC_L()		  CLR_PORT_BIT(PORTK,PK0)//PORTK_Bit1 = 0
 


  #define PUMPCTRL_HIGH_OUT()        SET_PORT_BIT(DDRK,PK2)
  #define PUMPCTRL_HIGH_OPEN()       CLR_PORT_BIT(PORTK,PK2)
  #define PUMPCTRL_HIGH_CLOSE()      SET_PORT_BIT(PORTK,PK2)

  #define PUMPCTRL_LOW_OUT()        SET_PORT_BIT(DDRK,PK3)
  #define PUMPCTRL_LOW_OPEN()       CLR_PORT_BIT(PORTK,PK3)
  #define PUMPCTRL_LOW_CLOSE()      SET_PORT_BIT(PORTK,PK3)


  #define MAX_MODBUS_ERRCNT			5
  #define MAX_SAFE_STOP_CNT			2   //300 ms
  #define FULL_STOP_CNT				2   //300 ms
  
  #define  LNG_PUMP_STOP()				ovctrl_pump_switch(0)///ͣ��
  #define  LNG_PUMP_PRECOOL()			ovctrl_pump_switch(1)///Ԥ��
  #define  LNG_PUMP_IDLE()				ovctrl_pump_switch(2)///����
  #define  LNG_PUMP_CTRL()				ovctrl_pump_switch(3)///��Һ


 /* //������A����
  #define INTER_LINE_A_OUT()        SET_PORT_BIT(DDRK,PK6)////DDRB_Bit0   = 1
  #define INTER_LINE_A_HIGH()      SET_PORT_BIT(PORTK,PK6)//PORTB_Bit0  = 1
  #define INTER_LINE_A_LOW()       CLR_PORT_BIT(PORTK,PK6)//PORTB_Bit0  = 0
  
  #define  INTER_LINE_A_IN()         CLR_PORT_BIT(DDRK,PK6)//DDRB_Bit0   = 0
 
  //������B����
  #define INTER_LINE_B_OUT()        SET_PORT_BIT(DDRK,PK7)////DDRB_Bit1   = 1
  #define INTER_LINE_B_HIGH()      SET_PORT_BIT(PORTK,PK7)//PORTB_Bit1  = 1
  #define INTER_LINE_B_LOW()       CLR_PORT_BIT(PORTK,PK7)//PORTB_Bit1  = 0
  
  #define  INTER_LINE_B_IN()         CLR_PORT_BIT(DDRK,PK7)//DDRB_Bit1   = 0 */
    //������A����
  #define INTER_LINE_A_OUT()        SET_PORT_BIT(DDRL,PL0)////DDRB_Bit0   = 1
  #define INTER_LINE_A_HIGH()      SET_PORT_BIT(PORTL,PL0)//PORTB_Bit0  = 1
  #define INTER_LINE_A_LOW()       CLR_PORT_BIT(PORTL,PL0)//PORTB_Bit0  = 0
  
  #define  INTER_LINE_A_IN()         CLR_PORT_BIT(DDRL,PL0)//DDRB_Bit0   = 0
 
  //������B����
  #define INTER_LINE_B_OUT()        SET_PORT_BIT(DDRL,PL1)////DDRB_Bit1   = 1
  #define INTER_LINE_B_HIGH()      SET_PORT_BIT(PORTL,PL1)//PORTB_Bit1  = 1
  #define INTER_LINE_B_LOW()       CLR_PORT_BIT(PORTL,PL1)//PORTB_Bit1  = 0
  
  #define  INTER_LINE_B_IN()         CLR_PORT_BIT(DDRL,PL1)//DDRB_Bit1   = 0 

  enum _enumCNGType
  {
	  _CNG_NO_OPT,
	  _CNG_NORMAL,
	  _CNG_FIX_M,
	  _CNG_FIX_N,
  };
  
  enum _enumLCNGSTOP
  {
	 _STOP_30MPA = 0,
	 _STOP_25MPA,
	 _STOP_MODBUS_ERR,
	 _STOP_ICCARD_OFF,
	 _STOP_NORMAL, 
  };

  ex  volatile bool             b_Simulation;       //ģ�����
  ex  volatile bool             b_Simu_auto;        //ģ���Զ�����
  ex  volatile bool				bprecool_state;
  ex  volatile bool				b_lcng_disp_shine;
  ex  unsigned char             nSimu_auto_step;
  ex  unsigned char     nErrPress_cnt; 
  ex  unsigned char     nmodbus_Errcnt;  
  ex  unsigned char		cur_stoptype;		//��ǰͣ������
  ex  unsigned char		cur_lcng_opt_type;	//��ǰ��������(���桢����������)
  ex  float				cur_vr_arr[16];	    //����
  ex  unsigned char     cur_vr_arr_len;	

  ex unsigned int cur_init_lng_chk ;

  ex  float             cur_init_lcng_sum ;   //��ǰ��ʼ����
 //////////////////////////////////// 
  ex  float             cur_flowrate_g;        //��ǰ��������
  ex  float             cur_totalize_g;        //��ǰ��������
  ex  float             cur_flowrate_l;        //��ǰҺ������
  ex  float             cur_totalize_l;        //��ǰҺ������
  ex  float             cur_pressure_g;        //��ǰ����ѹ��
  ex  float             cur_pressure_l;
  ex  float             cur_fix_money;
  ex  float				cur_calFactor;         //��ǰϵ���ܶ�
  ex  unsigned int      cur_fix_num;

  ex  float             cur_price;
 // ex  unsigned char     cur_vctrl_step;		//��ǰ��ط�
  ex  unsigned long     cur_dlytick;		//��ǰ�ȴ�ʱ��
  //ex  unsigned long		cur_endRate;		//��ǰ��ֹ����
  ex  float				cur_endRate;		//��ǰ��ֹ����
 /////////////////////////   
  void			ovctrl_init(void);
  void			ovctrl_stop_all(void);
  void			ovctrl_pump_switch(unsigned char sw);
  unsigned long 	ovctrl_gettick(void);
  void			ovctrl_clrtick(unsigned long tick);
  unsigned int  		ovctrl_get_pressure_adc(unsigned char sw);
  bool		    	ovctrl_getpressure(unsigned char sw, float *vp);
  //void			olngctrl_precool_timeout_pro(void);
  //void			olngctrl_pre_cool(void); //Ԥ��
  void olngctrl_Bpre_cool(void);//bigԤ��
  void olngctrl_Spre_cool(void);//smart Ԥ��
  void          		olngctrl_ctrl_start(void);   
  void          		olngctrl_ctrl_start_ex(void);   
  void			olngctrl_disp_pro_ex(void);

void 				olngctrl_ctrl_prepare(void);//////��Һǰ׼��
void 				olngctrl_ctrl_back_ex(void);//////��ʼ����
  
  void			olngctrl_stop_pro(void);
  void			ocngctrl_ctrl_fix_start_pro(void);
  void			ocngctrl_simu_disp_pro_ex(void);  
  void			ovctrl_simu_pressure(void);
  void			ovctrl_simu_flowrate(void);
  void			olcng_usr_prtrec(void);
  void			olcngctrl_ctrl_start_pro(void);
  
  void			olngctrl_ctrl_disp_pro(float vftotal);
  void			ofixctrl_fix_lng_stop_pro(void);
  void			olngctrl_vctrl_stop(void);
  void          		olngctrl_start_pro(void);
  void          		olngctrl_stop_disp(void); 
  void          		olcngctrl_ctrl_disp_shine_pro(void);
  void			olngctrl_vctrl_off(void);

void release_pressure(void);

void Main_Release_pressure(void);

#ifdef LNG_COEFFI_SET
  void olngctrl_start_init(void);
#endif  

#endif
