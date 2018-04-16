/*
 * print.h
 *
 * Created: 2012-8-16 14:36:48
 *  Author: luom
 */ 


#ifndef PRINT_H_
#define PRINT_H_

void oprt_printheadcmd(void);
void oprt_printnormalcmd(void);
void oprt_lfrow(unsigned char rows);
void oprt_sendstrext(const char *buf) ;
void oprt_init(unsigned int baudrate);
//void oprt_rec(unsigned long id);
void oprt_rec(bool brd, unsigned long id);
void oprt_rpt(bool brd, unsigned char* buf);
void oprt_total(void);
void oprt_version_info(void);
void oprt_basic_paras_info(void);
void oprt_ctrl_paras_info(void);
void oprt_sysinfo(void);
void oprt_debug_card_msg(char *fmt,...);
void oprt_ctrl_iccard_test_info(unsigned char* wtbuf, unsigned char *rdbuf);
void oprt_ctrl_iccard_err_info(bool bwt);

void oprt_command_info(void);
void oprt_Cpufeil_info(void);
#endif /* PRINT_H_ */