/*
 * print.c
 *
 * Created: 2012-8-16 14:36:00
 *  Author: luom
 */ 
#include "global_def.h"
void oprt_init(unsigned int baudrate)
{
  unsigned int UBRR;
  UCSR0B = 0x00;//disable while setting baud rate
  UCSR0A = 0x00;
  UCSR0C = (1<<UCSZ01) |(1<<UCSZ00);//8位数据位,无奇偶校验，1位停止位 
  UBRR = ((F_CPU>>4)/baudrate -1);//计算UBRR
  UBRR0L = (unsigned char) (UBRR & 0xFF);//set baud rate lo
  UBRR0H = (unsigned char) ((UBRR >>8) &0xFF);//set baud rate hi
  UCSR0B = (1<<TXEN0) |(1<<RXEN0);//使能接收发送
  //oprt_initprinter();
  //oprt_printnormalcmd();
  
  USART_SEL_S0OUT();
  USART_SEL_S1OUT();
  SELECT_CARD();    
}

void oprt_sendchar(unsigned char c)  
{
  while(!(UCSR0A &(1<<UDRE0))) ;//while(UCSR0A_UDRE0 == 0);//等待发送缓冲区为空
  UDR0 = c;
}

void oprt_sendstr(unsigned char *buf, unsigned char len) 
{
  unsigned char i;
  for (i =0; i <len; i++) oprt_sendchar(buf[i]);
}

void oprt_sendstrext(const char *buf) 
{
  unsigned char len, i;
  len = strlen(buf);
  for (i =0; i <len; i++) oprt_sendchar(buf[i]);
}

void oprt_initprinter(void)
{
  oprt_sendchar(0x1B);
  oprt_sendchar(0x40);
  oprt_sendchar(0x0D);
}

void oprt_lfrow(unsigned char rows)
{
  while(rows--)
  {
     oprt_sendchar(0x0A);//换行
     oprt_sendchar(0x0D);//回车，确认打印换行(及之前的)命令
  }
}

void oprt_printheadcmd(void)
{
  unsigned char buff[8] = {0x1c, 0x26, 0x1b, 0x56, 0x02, 0x1b, 0x63, 0x00};
  //0x1c 26:进入汉字打印方式
  //0x1b 56 02:正常高度两倍打印
  //0x1b 63 00:字符从右向左的反向打印
  oprt_sendstr(buff,8);
}

void oprt_printnormalcmd(void)
{
  unsigned char cmd[14] = {0x1b, 0x40, 0x1c, 0x26, 0x1b, 0x31, 0x08, 
                           0x1b, 0x70, 0x02, 0x1b, 0x63, 0x00, 0x0D};
  //0x1c 26:进入汉字打印方式
  //0x1b 56 02:正常高度两倍打印
  //0x1b 63 00:字符从右向左的反向打印
  oprt_sendstr(cmd,14);
}
////////////////////////////////////////////////////////////////////打印总累数据
void oprt_total(void)
{
	unsigned  long tmp = 0;
	//memcpy(&_prt_shift_rpt.shift_rpt_id, buf, sizeof(_lcng_shiift_rpt));
	SELECT_PRINT();
        oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"      LCNG总累数据凭条\r\n");   
	oprt_printnormalcmd(); 
	//oprt_lfrow(2);
	ocomm_printf(CH_PRINT,0, 0, "枪号:%02d #\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
	tmp = ocomm_bcdbuf2hex(cur_sysinfo.g_sum, 6);
	ocomm_printf(CH_PRINT,0, 0, "枪累:%ld.%02d公斤 \r\n",tmp/100,tmp%100);
	tmp = ocomm_bcdbuf2hex(cur_sysinfo.m_amount, 6);
	ocomm_printf(CH_PRINT,0, 0, "金额:%ld.%02d元 \r\n",tmp/100,tmp%100);
	ocomm_printf(CH_PRINT,0, 0, "加气次数:%d \r\n",  cur_sysinfo.gas_count);
	tmp = ocomm_bcdbuf2hex(cur_sysinfo.gas_total, 6);
	ocomm_printf(CH_PRINT,0, 0, "气量:%ld.%02d公斤 \r\n",tmp/100,tmp%100);
    ocomm_printf(CH_PRINT,0, 0, "时间:20%02d-%02d-%02d %02d:%02d:%02d",cur_time[0],cur_time[1],cur_time[2],cur_time[3],cur_time[4],cur_time[5]);
	oprt_lfrow(6);    		
	SELECT_CARD(); 			
}
////////////////////////////////////////////////////////////////////打印流水记录
void oprt_rpt(bool brd, unsigned char* buf)
{
       unsigned long tmp =0;
	_lcng_shiift_rpt       _prt_shift_rpt;
	memcpy(&_prt_shift_rpt.shift_rpt_id, buf, sizeof(_lcng_shiift_rpt));
	SELECT_PRINT();
        oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"      LCNG班累凭条\r\n");   
	oprt_printnormalcmd(); 
	ocomm_printf(CH_PRINT,1,1,"加气站编号:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_STATION_ID]);
	ocomm_printf(CH_PRINT,0,0,"班流水号:%ld \r\n",_prt_shift_rpt.shift_rpt_id);
	ocomm_printf(CH_PRINT,0,0,"班号:%d \r\n",   _prt_shift_rpt.shift_id);
	
	ocomm_printf(CH_PRINT,0,0,"加气次数:%d\r\n",_prt_shift_rpt.shift_rec_num);
	
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_rpt_total_lcngcash, 4);
       ocomm_printf(CH_PRINT,0,0,"本班现金:%ld.%02d元\r\n",tmp/100,tmp%100);
	 
	//ocomm_printf(CH_PRINT,0,0,"本班总额:%.02f元\r\n",_prt_shift_rpt.shift_total_rec_lcngsum);
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_total_rec_lcngsum, 4);
	ocomm_printf(CH_PRINT,0,0,"本班总额:%ld.%02d元\r\n",tmp/100,tmp%100);
	
	 tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_rpt_s_lcng, 4);
       ocomm_printf(CH_PRINT,0,0,"班开始枪累:%ld.%02d公斤\r\n",tmp/100,tmp%100);
	   
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_rpt_e_lcng, 4);
	ocomm_printf(CH_PRINT,0,0,"班结束枪累:%ld.%02d公斤\r\n",tmp/100,tmp%100);
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_total_rpt_lcng, 4);
       ocomm_printf(CH_PRINT,0,0,"本班加气量:%ld.%02d公斤\r\n",tmp/100,tmp%100);
      
	ocomm_printf(CH_PRINT,0,0,"开始:20%02d-%02d-%02d ",_prt_shift_rpt.shift_rpt_stime[0],_prt_shift_rpt.shift_rpt_stime[1],_prt_shift_rpt.shift_rpt_stime[2]);
	ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",_prt_shift_rpt.shift_rpt_stime[3],_prt_shift_rpt.shift_rpt_stime[4],_prt_shift_rpt.shift_rpt_stime[5]);

	ocomm_printf(CH_PRINT,0,0,"结束:20%02d-%02d-%02d ",_prt_shift_rpt.shift_rpt_etime[0],_prt_shift_rpt.shift_rpt_etime[1],_prt_shift_rpt.shift_rpt_etime[2]);
	ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",_prt_shift_rpt.shift_rpt_etime[3],_prt_shift_rpt.shift_rpt_etime[4],_prt_shift_rpt.shift_rpt_etime[5]);
       
       oprt_lfrow(6);    		
       SELECT_CARD(); 			
}
////////////////////////////////////////////////////////////////////打印流水记录
void oprt_rec(bool brd, unsigned long id)
{
   //if(!oprt_printready()) 	return ;
   unsigned long tmp = 0;
    SELECT_PRINT();
    oprt_printheadcmd();
	if(brd)
	{
		if(!ostore_rec_read(id,NULL,128))
		{
			ocomm_printf(CH_PRINT,0,0,"读流水数据错\r\n");   
			SELECT_CARD();    	
			return;
		} 
	}

   ocomm_printf(CH_PRINT,0,0,"LNG加气凭条\r\n");   
   oprt_lfrow(1);
   /*-------------- 消息头 -------------------*/      
   if(true == cur_rec.rec_b_CardOK) 
   {
		ocomm_printf(CH_PRINT,0,0,"卡扣款异常 \r\n");
        oprt_lfrow(1);
   }
 
   /*-------------- 消息体 -------------------*/////////////////////////////////
   oprt_printnormalcmd(); 
   ocomm_printf(CH_PRINT,0,0,"加液量:%ld.%02d公斤\r\n",cur_rec.rec_alllcng_num/100,cur_rec.rec_alllcng_num%100);
   if(cur_sysparas.b_y_or_n_operate & BASIC_P_BACK_GAS) 
   	{
   ocomm_printf(CH_PRINT,0,0,"回气量:%.02f公斤\r\n",(float)cur_rec.rec_back_num/100.00);
   ocomm_printf(CH_PRINT,0,0,"结算量:%.02f公斤\r\n",(float)cur_rec.rec_lcngnum/100.00);
   	}
   ocomm_printf(CH_PRINT,0,0,"单价:%.02f 元\r\n",   cur_rec.rec_lcngprice);

   tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
   ocomm_printf(CH_PRINT,0,0,"加气前余额:%ld.%02d元\r\n",tmp/100,tmp%100);

   //ocomm_printf(CH_PRINT,0,0,"金额:%ld.%02d 元\r\n",cur_rec.rec_lcngsum/100,cur_rec.rec_lcngsum%100);
   ocomm_printf(CH_PRINT,0,0,"金额:%.02f 元\r\n",(float)cur_rec.rec_lcngsum/100.00);
   
   //ocomm_printf(CH_PRINT,0,0,"车牌号:%04d \r\n",rec_CarNum);//(unsigned int)bcdbuf2hex(prt_CNGRec.RecCarNum, 2));
   ocomm_printf(CH_PRINT,0,0,"卡号:%08ld \r\n",  cur_rec.rec_cardnum);
   tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_M, 4);
   ocomm_printf(CH_PRINT,0,0,"卡余:%ld.%02d元\r\n",tmp/100,tmp%100);
   
//   ocomm_printf(CH_PRINT,0,0,"班号:%02d #\r\n",(unsigned char)bcdbuf2hex(&prt_CNGRec.UsrID,1));
//   OSysparas_CMDPara(PREAD, PSETKEYIN, id_sysp_0003 ,&rd);
ocomm_printf(CH_PRINT,1,1,"加气站编号:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_STATION_ID]);
ocomm_printf(CH_PRINT,0,0,"枪号:%02d #\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
//   OSysparas_CMDPara(PREAD, PSETKEYIN, 1003 ,&rd);
//   ocomm_printf(CH_PRINT,0,0,"公司代码:%06d # \r\n", rd);
//   tmp = bcdbuf2hex(prt_CNGRec.UID,6);  
   ocomm_printf(CH_PRINT,0,0,"流水号:%04ld\r\n",cur_rec.rec_id);
   ocomm_printf(CH_PRINT,0,0,"开始: %u-%02d-%02d ",cur_rec.rec_stime[0], cur_rec.rec_stime[1], cur_rec.rec_stime[2]);
   ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",cur_rec.rec_stime[3], cur_rec.rec_stime[4], cur_rec.rec_stime[5]); 
   ocomm_printf(CH_PRINT,0,0,"结束: %u-%02d-%02d ",cur_rec.rec_etime[0], cur_rec.rec_etime[1], cur_rec.rec_etime[2]);
   ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",cur_rec.rec_etime[3], cur_rec.rec_etime[4], cur_rec.rec_etime[5]);  
   oprt_lfrow(1);
   ocomm_printf(CH_PRINT,0,0, " 欢迎光临  出行平安\r\n");

   ocomm_printf(CH_PRINT,0,0, "<请妥善保存此凭条>\r\n");
   
   oprt_lfrow(6);    		
   SELECT_CARD();    						
}

void oprt_version_info(void)
{
   SELECT_PRINT();   
   oprt_printheadcmd();
   ocomm_printf(CH_PRINT,0,0,"加气机版本信息\r\n");
   oprt_printnormalcmd();
   oprt_lfrow(2);
   ocomm_printf(CH_PRINT,0,0,"%s\r\n",VERSION);   
   oprt_lfrow(6); 
   SELECT_CARD();    	
}

void oprt_basic_paras_info(void)
{
   SELECT_PRINT(); 
   oprt_printheadcmd();
   ocomm_printf(CH_PRINT,0,0,"加气机基本信息\r\n");
   oprt_printnormalcmd();   
   oprt_lfrow(2);
   ocomm_printf(CH_PRINT,1,1,"加气站编号:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_STATION_ID]);
   ocomm_printf(CH_PRINT,1,1,"枪号:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
  // ocomm_printf(CH_PRINT,2,1,"密度:%.04f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY]);     
   //ocomm_printf(CH_PRINT,3,1,"系数:%.04f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT]); 
   ocomm_printf(CH_PRINT,2,1,"气相零点:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO]);
   ocomm_printf(CH_PRINT,3,1,"气相系数:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT]); 
   ocomm_printf(CH_PRINT,4,1,"液相零点:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO]);
   ocomm_printf(CH_PRINT,1,1,"液相系数:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT]);  
   if(cur_sysparas.b_y_or_n_operate & BASIC_P_CARD_CHK) ocomm_printf(CH_PRINT,2,6,"用卡加气:√\r\n");
   else			ocomm_printf(CH_PRINT,2,6,"用卡加气:×\r\n");
   ocomm_printf(CH_PRINT,4,1,"记录号:%ld\r\n",cur_sysparas.r_cur_g_id);
   ocomm_printf(CH_PRINT,1,1,"未传流水记录号:%ld\r\n",cur_sysparas.r_unupld_g_id);
   ocomm_printf(CH_PRINT,2,1,"班流水号:%ld\r\n",cur_sysparas.r_cur_shift_id);
   ocomm_printf(CH_PRINT,3,1,"未传班流水号:%ld\r\n",cur_sysparas.r_unupld_shift_id);   
   oprt_lfrow(6); 
   SELECT_CARD();    	
			
}

void oprt_ctrl_paras_info(void)
{
	SELECT_PRINT(); 
	oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"加气机控制信息\r\n");
	oprt_printnormalcmd();   
	oprt_lfrow(2);	
	ocomm_printf(CH_PRINT,1,1,"预冷临界温度:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[0]);
	ocomm_printf(CH_PRINT,2,1,"预冷临界增益:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[1]);		
	ocomm_printf(CH_PRINT,3,1,"预冷超时时间:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[2]);
	ocomm_printf(CH_PRINT,4,1,"停机延时:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[3]);
	ocomm_printf(CH_PRINT,5,1,"停机流速:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[4]);
	ocomm_printf(CH_PRINT,6,1,"停机超时时间:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[5]);
	oprt_lfrow(6); 
	SELECT_CARD();    	   	
}

void oprt_ctrl_iccard_test_info(unsigned char* wtbuf, unsigned char *rdbuf)
{
   SELECT_PRINT(); 
   oprt_printheadcmd();
   ocomm_printf(CH_PRINT,0,0,"     IC卡测试信息\r\n");
   oprt_printnormalcmd();   
   oprt_lfrow(2);	
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n ",wtbuf[0],wtbuf[1],wtbuf[2],wtbuf[3]);
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n ",wtbuf[4],wtbuf[5],wtbuf[6],wtbuf[7]);
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n ",wtbuf[8],wtbuf[9],wtbuf[10],wtbuf[11]);
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n ",wtbuf[12],wtbuf[13],wtbuf[14],wtbuf[15]);
   oprt_lfrow(2);	
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n",rdbuf[0],rdbuf[1],rdbuf[2],rdbuf[3]);
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n",rdbuf[4],rdbuf[5],rdbuf[6],rdbuf[7]);
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n",rdbuf[8],rdbuf[9],rdbuf[10],rdbuf[11]);
   ocomm_printf(CH_PRINT,1,1,"%d %d %d %d\r\n",rdbuf[12],rdbuf[13],rdbuf[14],rdbuf[15]);
   oprt_lfrow(6); 
   SELECT_CARD();   
}
void oprt_ctrl_iccard_err_info(bool bwt)
{
		SELECT_PRINT(); 
		oprt_printheadcmd();
		ocomm_printf(CH_PRINT,0,0,"     IC卡测试信息\r\n");
		oprt_printnormalcmd();   
		oprt_lfrow(2);
		if(bwt == true) ocomm_printf(CH_PRINT,3,5,"wt:%d\r\n",icard_err_code_); 
		else			ocomm_printf(CH_PRINT,3,5,"rd:%d\r\n",icard_err_code_); 
		SELECT_CARD(); 	
};

#if ICCARD_M1_MINGTE
#else
//////////////////////20131120 addwen
void oprt_command_info(void)
{
   SELECT_PRINT();   
   oprt_printheadcmd();
   ocomm_printf(CH_PRINT,0,0,"        命令内容\r\n");
   oprt_printnormalcmd();
   oprt_lfrow(2);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[0],oicard_recvbuf[1],oicard_recvbuf[2],oicard_recvbuf[3],oicard_recvbuf[4],oicard_recvbuf[5]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[6],oicard_recvbuf[7],oicard_recvbuf[8],oicard_recvbuf[9],oicard_recvbuf[10],oicard_recvbuf[11]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[12],oicard_recvbuf[13],oicard_recvbuf[14],oicard_recvbuf[15],oicard_recvbuf[16],oicard_recvbuf[17]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[18],oicard_recvbuf[19],oicard_recvbuf[20],oicard_recvbuf[21],oicard_recvbuf[22],oicard_recvbuf[23]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[24],oicard_recvbuf[25],oicard_recvbuf[26],oicard_recvbuf[27],oicard_recvbuf[28],oicard_recvbuf[29]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[30],oicard_recvbuf[31],oicard_recvbuf[32],oicard_recvbuf[33],oicard_recvbuf[34],oicard_recvbuf[35]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[36],oicard_recvbuf[37],oicard_recvbuf[38],oicard_recvbuf[39],oicard_recvbuf[40],oicard_recvbuf[40]);
   ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[42],oicard_recvbuf[43],oicard_recvbuf[44],oicard_recvbuf[45],oicard_recvbuf[46],oicard_recvbuf[47]);
   //ocomm_printf(CH_PRINT,0,0, "%x %x %x %x %x %x  \r\n",oicard_recvbuf[48],oicard_recvbuf[49],oicard_recvbuf[50],oicard_recvbuf[51],oicard_recvbuf[52],oicard_recvbuf[53]);
   oprt_lfrow(2);
   SELECT_CARD();    	
}

void oprt_Cpufeil_info(void)
{
	SELECT_PRINT();   
	oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"        文件内容\r\n");
	oprt_printnormalcmd();
	oprt_lfrow(2);
	ocomm_printf(CH_PRINT,0,0, "卡号:%x %x %x %x %x %x %x %x \r\n",cpu_card_data.CpuCardNum[0],cpu_card_data.CpuCardNum[1],cpu_card_data.CpuCardNum[2],cpu_card_data.CpuCardNum[3],cpu_card_data.CpuCardNum[4],cpu_card_data.CpuCardNum[5],cpu_card_data.CpuCardNum[6],cpu_card_data.CpuCardNum[7]);
	ocomm_printf(CH_PRINT,0,0, "卡类型%x \r\n",cpu_card_data.CpuCardType);
	//ocomm_printf(CH_PRINT,0,0, "公司编号%x %x %x %x\r\n",cpu_card_data.CpuCompanyNum[0],cpu_card_data.CpuCompanyNum[1],cpu_card_data.CpuStationNum[0],cpu_card_data.CpuStationNum[1]);
	//ocomm_printf(CH_PRINT,0,0, "密码%x %x %x \r\n",cpu_card_data.CpuCardPassword[0],cpu_card_data.CpuCardPassword[1],cpu_card_data.CpuCardPassword[2]);
	//ocomm_printf(CH_PRINT,0,0, "钢瓶日期 %x %x %x\r\n",cpu_card_data.CpuSteelDate[0],cpu_card_data.CpuSteelDate[1],cpu_card_data.CpuSteelDate[2]);
	ocomm_printf(CH_PRINT,0,0, "最后加气金额%x %x %x %x\r\n",cpu_card_data.CpuLastMon[0],cpu_card_data.CpuLastMon[1],cpu_card_data.CpuLastMon[2],cpu_card_data.CpuLastMon[3]);
	ocomm_printf(CH_PRINT,0,0, "最后加气气量%x %x %x %x \r\n",cpu_card_data.CpuLastVol[0],cpu_card_data.CpuLastVol[1],cpu_card_data.CpuLastVol[2],cpu_card_data.CpuLastVol[3]);
	ocomm_printf(CH_PRINT,0,0, "最后使用时间%x %x %x %x %x %x\r\n",cpu_card_data.CpuLastTime[0],cpu_card_data.CpuLastTime[1],cpu_card_data.CpuLastTime[2],cpu_card_data.CpuLastTime[3],cpu_card_data.CpuLastTime[4],cpu_card_data.CpuLastTime[5]);
	//ocomm_printf(CH_PRINT,0,0, "最后加气站编号%x %x  \r\n",cpu_card_data.CpuLastStation[0],cpu_card_data.CpuLastStation[1]);
	ocomm_printf(CH_PRINT,0,0, "枪号%x\r\n",cpu_card_data.CpuGunNum);
	ocomm_printf(CH_PRINT,0,0, "流水号%x %x %x %x\r\n",cpu_card_data.CpuSerialNum[0],cpu_card_data.CpuSerialNum[1],cpu_card_data.CpuSerialNum[2],cpu_card_data.CpuSerialNum[3]);
	ocomm_printf(CH_PRINT,0,0, "卡状态%x\r\n",cpu_card_data.CpuValidity);
	ocomm_printf(CH_PRINT,0,0, "卡余额%x %x %x %x \r\n",cpu_card_data.CpuBalance[0],cpu_card_data.CpuBalance[1],cpu_card_data.CpuBalance[2],cpu_card_data.CpuBalance[3]);
	//ocomm_printf(CH_PRINT,0,0, "上次余额%x %x %x %x\r\n",cpu_card_data.CpuLastBalance[0],cpu_card_data.CpuLastBalance[1],cpu_card_data.CpuLastBalance[2],cpu_card_data.CpuLastBalance[3]);
	//ocomm_printf(CH_PRINT,0,0, "上次操作方式%x\r\n",cpu_card_data.CpuLastOper);
	//ocomm_printf(CH_PRINT,0,0, "上次操作时间%x %x %x %x %x %x\r\n",cpu_card_data.CpuLastOperTime[0],cpu_card_data.CpuLastOperTime[1],cpu_card_data.CpuLastOperTime[2],cpu_card_data.CpuLastOperTime[3],cpu_card_data.CpuLastOperTime[4],cpu_card_data.CpuLastOperTime[5]);
	ocomm_printf(CH_PRINT,0,0, "最后操作方式%x\r\n",cpu_card_data.CpuOper);
	//ocomm_printf(CH_PRINT,0,0, "最后充值金额%x %x %x %x\r\n",cpu_card_data.CpuRechargeMon[0],cpu_card_data.CpuRechargeMon[1],cpu_card_data.CpuRechargeMon[2],cpu_card_data.CpuRechargeMon[3]);
	oprt_lfrow(1);
   	ocomm_printf(CH_PRINT,0,0, " 欢迎光临  出行平安\r\n");
   	oprt_lfrow(6);  
	SELECT_CARD();    	
}
#endif

void oprt_debug_card_msg(char *fmt,...)
{
	OS_ENTER_CRITICAL();//cli();
	SELECT_PRINT();   
    va_list ap;
    char str[64];
    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
	oprt_sendstrext(str);
    va_end(ap);
    oprt_sendchar(0x0A);
    oprt_sendchar(0x0D);
	//ocomm_printf(CH_PRINT,0,0,fmt);
	SELECT_CARD();   
	OS_EXIT_CRITICAL();//sei();
}
