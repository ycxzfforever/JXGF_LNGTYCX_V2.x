/*
 * set.h
 *
 * Created: 2012-8-9 15:43:50
 *  Author: luom
 */ 


#ifndef SET_H_
#define SET_H_

	enum{
		PSW_NORMAL = 1,
		PSW_SUPER,
		PSW_HI_SUPER,
	};
//zl 2013-10-28
#define ONE_PUMP_ONE_MECH       1
#define ONE_PUMP_TWO_MECH      2
#define PUMP_INTERLINE_INPUT   3
#define PUMP_INTERLINE_OUTPUT   4
//zl 
void oset_paras(void);	
void oset_ctrl_pro(void);;
////ÉèÖÃ²Ù×÷
void ooperate_event_pro(unsigned char id);
////
void oset_price_pro(void);
////
void olngctrl_menuevent_pro(unsigned char id);
////
void omodule_event_pro(unsigned char id);
#endif /* SET_H_ */