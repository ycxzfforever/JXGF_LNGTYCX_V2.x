/*
 * vctrl.c
 *
 * Created: 2012-8-9 16:49:25
 *  Author: luom
 */ 
#include "global_def.h"

void ovctrl_init(void)
{
  VCTRL_ADC_IN();
  VCTRL_ADC_L(); 

  //LCNG_L_ADC_IN();
  //LCNG_L_ADC_L(); 
  
  //AD��ʼ��
  ADCSRA = 0x00;    	//��ֹʹ��
  ADMUX = (1<<REFS0); 	//ͨ��ѡ��--00000 - ADC0,��������
  //���òο���ѹ 01 --AVCC
  //ת������Ҷ��� 0
  ADCSRB = (1<<MUX5);
  ADMUX |= (1<<MUX0);

  ACSR  = (1<<ACD);     //����ģ��Ƚ���
  ADCSRA =(1<<ADEN) |(1<<ADPS2) |(1<<ADPS0);	
  //���ų�ʼ��
  VCTRL_KEY_OUT();
  VCTRL_LOW_OUT();
  VCTRL_MID_OUT();
  VCTRL_HIGH_OUT();
  VCTRL_CLOSEKEY();//�ܷ���,�°汾���޴˷���
  VCTRL_HIGH_CLOSE();
  VCTRL_MID_CLOSE();
  VCTRL_LOW_CLOSE();
}


void ovctrl_pump_switch(unsigned char sw)
{
	switch(sw)
	{
		case 0: 
			PUMPCTRL_HIGH_CLOSE();
			PUMPCTRL_LOW_CLOSE();	
			break;
		case 1:
			PUMPCTRL_HIGH_OPEN();
			PUMPCTRL_LOW_CLOSE();	
			break;
		case 2:
			PUMPCTRL_LOW_OPEN();	
			PUMPCTRL_HIGH_CLOSE();			
			break;
		case 3:
			PUMPCTRL_LOW_OPEN();	
			PUMPCTRL_HIGH_OPEN();			
			break;
		default:
			break;	
	}
}

static unsigned int ovctrl_getcuradc(void)//��ȡת�����
{
  unsigned int rd1, rd2;
  ADCSRA |= (1<<ADSC) ;	   //����ת��
  while(ADCSRA &(1<<ADSC));//�ȴ�ת������
  rd1 = ADCL;
  rd2 = ADCH<<8;
  return rd1+rd2;
}
void ovctrl_channel_pressure_g(void)
{
	ADMUX |= (1<<MUX0);   //����
}
void ovctrl_channel_pressure_l(void)
{
	ADMUX &= ~(1<<MUX0);
}

unsigned int ovctrl_get_pressure_adc(unsigned char sw)//16�ν��ƽ��ֵ
{
	  unsigned char		   i = 0;
	  unsigned int         cal = 0;
	  if     (_PRESSURE_G_SW_ == sw)   ovctrl_channel_pressure_g();
	  else if(_PRESSURE_L_SW_ == sw)   ovctrl_channel_pressure_l();
	  else return 0;
	  ADCSRA |= (1<<ADEN) ;	
	  _delay_loop_2(1843);//ocomm_delay_ms(1); 
	  ovctrl_getcuradc();
	  for(i=0; i<16; i++)
	  {
		cal += ovctrl_getcuradc();
	  }  
	  ADCSRA &= ~(1<<ADEN) ;	
	  return cal/16;
}

bool ovctrl_getpressure(unsigned char sw, float *vp)
{
	unsigned int curadv = 0;
	float pzero = cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO];
	float pratio = cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT];
	curadv = ovctrl_get_pressure_adc(sw);
	if(curadv<pzero) 
	{
		 *vp = 0;
	     return false;
	}			 
	*vp = ((float)(curadv-pzero))/pratio; 
	return true;
}



static  unsigned char     nmaxSafeStop_cnt;       //��߰�ȫ����ͣ������ 
static  unsigned char     nfixFullStop_cnt;       //��߰�ȫ����ͣ������ 
bool ocngctrl_maxsafe_stop_chk(void)
{
	if(cur_flowrate_g > cur_sysparas.c_rate[2])
	{
		if(nmaxSafeStop_cnt++ > MAX_SAFE_STOP_CNT) 
		{
			blcng_running = false;
			okernel_putevent(KEY_STOP_EV,0);
			return true;
		}
		
	}
	nmaxSafeStop_cnt = 0;
	return false;
}

bool ocngctrl_fixfull_stop_chk(void)
{
	unsigned char n = 0;
	if(cur_pressure_g < 20 ) return false;
	n = (unsigned char)((cur_pressure_g-20)/0.5);
	if((unsigned int)(cur_flowrate_g*10) < cur_sysparas.c_p_stop_arr[n]) //����ͣ�� //�ܿ���ֵ
	{
		if(++nfixFullStop_cnt > FULL_STOP_CNT)
		{
			blcng_running = false;
			okernel_putevent(KEY_STOP_EV,0);
			return true;
		}
	}
	nfixFullStop_cnt = 0;
	return false;	
}

