/*
 * splc.h
 *
 * Created: 2012-8-10 11:15:57
 *  Author: luom
 */ 


#ifndef SPLC_H_
#define SPLC_H_

#define  OSPLC_LCK_H()    SET_PORT_BIT(PORTL,PL2)//{ PORTL_Bit2 = 1; }
#define  OSPLC_LCK_L()    CLR_PORT_BIT(PORTL,PL2)//{ PORTL_Bit2 = 0; }
#define  OSPLC_LCK_OUT()  SET_PORT_BIT(DDRL,DDL2)//{ DDRL_Bit2  = 1; }
#define  OSPLC_LCK_IN()   CLR_PORT_BIT(DDRL,DDL2)//{ DDRL_Bit2  = 0; }

#define  OSPLC_CLK_H()    SET_PORT_BIT(PORTL,PL3)//{ PORTL_Bit3 = 1; }
#define  OSPLC_CLK_L()    CLR_PORT_BIT(PORTL,PL3)//{ PORTL_Bit3 = 0; }
#define  OSPLC_CLK_OUT()  SET_PORT_BIT(DDRL,DDL3)//{ DDRL_Bit3  = 1; }
#define  OSPLC_CLK_IN()   CLR_PORT_BIT(DDRL,DDL3)//{ DDRL_Bit3  = 0; }

#define  OSPLC_DATA_H()   SET_PORT_BIT(PORTL,PL4)//{ PORTL_Bit4 = 1; }
#define  OSPLC_DATA_L()   CLR_PORT_BIT(PORTL,PL4)//{ PORTL_Bit4 = 0; }
#define  OSPLC_DATA_OUT() SET_PORT_BIT(DDRL,DDL4)//{ DDRL_Bit4  = 1; }
#define  OSPLC_DATA_IN()  CLR_PORT_BIT(DDRL,DDL4)//{ DDRL_Bit4  = 0; }

void osplc_hardwarechk(void);
void osplc_show_null(void);
void osplc_show(float Row1,float Row2,float Row3,bool bStop, bool bOverFlag);
void osplc_show_ex(float Row1,float Row2,float Row3,bool bStop, bool bOverFlag);
void osplc_disp_ex(unsigned char *pdisp,bool nPoint);
void osplc_disp_ex_my(unsigned char *pdisp,bool nPoint);
void osplc_disp(unsigned char *pdisp,bool nPoint);


void odisp_mem_disp_init0(void);
void osplc_show_new(float Row1,float Row2,float Row3);

#endif /* SPLC_H_ */