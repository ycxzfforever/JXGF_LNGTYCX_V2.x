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
  UCSR0C = (1<<UCSZ01) |(1<<UCSZ00);//8λ����λ,����żУ�飬1λֹͣλ 
  UBRR = ((F_CPU>>4)/baudrate -1);//����UBRR
  UBRR0L = (unsigned char) (UBRR & 0xFF);//set baud rate lo
  UBRR0H = (unsigned char) ((UBRR >>8) &0xFF);//set baud rate hi
  UCSR0B = (1<<TXEN0) |(1<<RXEN0);//ʹ�ܽ��շ���
  //oprt_initprinter();
  //oprt_printnormalcmd();
  
  USART_SEL_S0OUT();
  USART_SEL_S1OUT();
  SELECT_CARD();    
}

void oprt_sendchar(unsigned char c)  
{
  while(!(UCSR0A &(1<<UDRE0))) ;//while(UCSR0A_UDRE0 == 0);//�ȴ����ͻ�����Ϊ��
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
     oprt_sendchar(0x0A);//����
     oprt_sendchar(0x0D);//�س���ȷ�ϴ�ӡ����(��֮ǰ��)����
  }
}

void oprt_printheadcmd(void)
{
  unsigned char buff[8] = {0x1c, 0x26, 0x1b, 0x56, 0x02, 0x1b, 0x63, 0x00};
  //0x1c 26:���뺺�ִ�ӡ��ʽ
  //0x1b 56 02:�����߶�������ӡ
  //0x1b 63 00:�ַ���������ķ����ӡ
  oprt_sendstr(buff,8);
}

void oprt_printnormalcmd(void)
{
  unsigned char cmd[14] = {0x1b, 0x40, 0x1c, 0x26, 0x1b, 0x31, 0x08, 
                           0x1b, 0x70, 0x02, 0x1b, 0x63, 0x00, 0x0D};
  //0x1c 26:���뺺�ִ�ӡ��ʽ
  //0x1b 56 02:�����߶�������ӡ
  //0x1b 63 00:�ַ���������ķ����ӡ
  oprt_sendstr(cmd,14);
}
////////////////////////////////////////////////////////////////////��ӡ��������
void oprt_total(void)
{
	unsigned  long tmp = 0;
	//memcpy(&_prt_shift_rpt.shift_rpt_id, buf, sizeof(_lcng_shiift_rpt));
	SELECT_PRINT();
        oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"      LCNG��������ƾ��\r\n");   
	oprt_printnormalcmd(); 
	//oprt_lfrow(2);
	ocomm_printf(CH_PRINT,0, 0, "ǹ��:%02d #\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
	tmp = ocomm_bcdbuf2hex(cur_sysinfo.g_sum, 6);
	ocomm_printf(CH_PRINT,0, 0, "ǹ��:%ld.%02d���� \r\n",tmp/100,tmp%100);
	tmp = ocomm_bcdbuf2hex(cur_sysinfo.m_amount, 6);
	ocomm_printf(CH_PRINT,0, 0, "���:%ld.%02dԪ \r\n",tmp/100,tmp%100);
	ocomm_printf(CH_PRINT,0, 0, "��������:%d \r\n",  cur_sysinfo.gas_count);
	tmp = ocomm_bcdbuf2hex(cur_sysinfo.gas_total, 6);
	ocomm_printf(CH_PRINT,0, 0, "����:%ld.%02d���� \r\n",tmp/100,tmp%100);
    ocomm_printf(CH_PRINT,0, 0, "ʱ��:20%02d-%02d-%02d %02d:%02d:%02d",cur_time[0],cur_time[1],cur_time[2],cur_time[3],cur_time[4],cur_time[5]);
	oprt_lfrow(6);    		
	SELECT_CARD(); 			
}
////////////////////////////////////////////////////////////////////��ӡ��ˮ��¼
void oprt_rpt(bool brd, unsigned char* buf)
{
       unsigned long tmp =0;
	_lcng_shiift_rpt       _prt_shift_rpt;
	memcpy(&_prt_shift_rpt.shift_rpt_id, buf, sizeof(_lcng_shiift_rpt));
	SELECT_PRINT();
        oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"      LCNG����ƾ��\r\n");   
	oprt_printnormalcmd(); 
	ocomm_printf(CH_PRINT,1,1,"����վ���:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_STATION_ID]);
	ocomm_printf(CH_PRINT,0,0,"����ˮ��:%ld \r\n",_prt_shift_rpt.shift_rpt_id);
	ocomm_printf(CH_PRINT,0,0,"���:%d \r\n",   _prt_shift_rpt.shift_id);
	
	ocomm_printf(CH_PRINT,0,0,"��������:%d\r\n",_prt_shift_rpt.shift_rec_num);
	
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_rpt_total_lcngcash, 4);
       ocomm_printf(CH_PRINT,0,0,"�����ֽ�:%ld.%02dԪ\r\n",tmp/100,tmp%100);
	 
	//ocomm_printf(CH_PRINT,0,0,"�����ܶ�:%.02fԪ\r\n",_prt_shift_rpt.shift_total_rec_lcngsum);
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_total_rec_lcngsum, 4);
	ocomm_printf(CH_PRINT,0,0,"�����ܶ�:%ld.%02dԪ\r\n",tmp/100,tmp%100);
	
	 tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_rpt_s_lcng, 4);
       ocomm_printf(CH_PRINT,0,0,"�࿪ʼǹ��:%ld.%02d����\r\n",tmp/100,tmp%100);
	   
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_rpt_e_lcng, 4);
	ocomm_printf(CH_PRINT,0,0,"�����ǹ��:%ld.%02d����\r\n",tmp/100,tmp%100);
	tmp = ocomm_bcdbuf2hex(_prt_shift_rpt.shift_total_rpt_lcng, 4);
       ocomm_printf(CH_PRINT,0,0,"���������:%ld.%02d����\r\n",tmp/100,tmp%100);
      
	ocomm_printf(CH_PRINT,0,0,"��ʼ:20%02d-%02d-%02d ",_prt_shift_rpt.shift_rpt_stime[0],_prt_shift_rpt.shift_rpt_stime[1],_prt_shift_rpt.shift_rpt_stime[2]);
	ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",_prt_shift_rpt.shift_rpt_stime[3],_prt_shift_rpt.shift_rpt_stime[4],_prt_shift_rpt.shift_rpt_stime[5]);

	ocomm_printf(CH_PRINT,0,0,"����:20%02d-%02d-%02d ",_prt_shift_rpt.shift_rpt_etime[0],_prt_shift_rpt.shift_rpt_etime[1],_prt_shift_rpt.shift_rpt_etime[2]);
	ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",_prt_shift_rpt.shift_rpt_etime[3],_prt_shift_rpt.shift_rpt_etime[4],_prt_shift_rpt.shift_rpt_etime[5]);
       
       oprt_lfrow(6);    		
       SELECT_CARD(); 			
}
////////////////////////////////////////////////////////////////////��ӡ��ˮ��¼
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
			ocomm_printf(CH_PRINT,0,0,"����ˮ���ݴ�\r\n");   
			SELECT_CARD();    	
			return;
		} 
	}

   ocomm_printf(CH_PRINT,0,0,"LNG����ƾ��\r\n");   
   oprt_lfrow(1);
   /*-------------- ��Ϣͷ -------------------*/      
   if(true == cur_rec.rec_b_CardOK) 
   {
		ocomm_printf(CH_PRINT,0,0,"���ۿ��쳣 \r\n");
        oprt_lfrow(1);
   }
 
   /*-------------- ��Ϣ�� -------------------*/////////////////////////////////
   oprt_printnormalcmd(); 
   ocomm_printf(CH_PRINT,0,0,"��Һ��:%ld.%02d����\r\n",cur_rec.rec_alllcng_num/100,cur_rec.rec_alllcng_num%100);
   if(cur_sysparas.b_y_or_n_operate & BASIC_P_BACK_GAS) 
   	{
   ocomm_printf(CH_PRINT,0,0,"������:%.02f����\r\n",(float)cur_rec.rec_back_num/100.00);
   ocomm_printf(CH_PRINT,0,0,"������:%.02f����\r\n",(float)cur_rec.rec_lcngnum/100.00);
   	}
   ocomm_printf(CH_PRINT,0,0,"����:%.02f Ԫ\r\n",   cur_rec.rec_lcngprice);

   tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_preM, 4);
   ocomm_printf(CH_PRINT,0,0,"����ǰ���:%ld.%02dԪ\r\n",tmp/100,tmp%100);

   //ocomm_printf(CH_PRINT,0,0,"���:%ld.%02d Ԫ\r\n",cur_rec.rec_lcngsum/100,cur_rec.rec_lcngsum%100);
   ocomm_printf(CH_PRINT,0,0,"���:%.02f Ԫ\r\n",(float)cur_rec.rec_lcngsum/100.00);
   
   //ocomm_printf(CH_PRINT,0,0,"���ƺ�:%04d \r\n",rec_CarNum);//(unsigned int)bcdbuf2hex(prt_CNGRec.RecCarNum, 2));
   ocomm_printf(CH_PRINT,0,0,"����:%08ld \r\n",  cur_rec.rec_cardnum);
   tmp = ocomm_bcdbuf2hex(cur_rec.rec_card_M, 4);
   ocomm_printf(CH_PRINT,0,0,"����:%ld.%02dԪ\r\n",tmp/100,tmp%100);
   
//   ocomm_printf(CH_PRINT,0,0,"���:%02d #\r\n",(unsigned char)bcdbuf2hex(&prt_CNGRec.UsrID,1));
//   OSysparas_CMDPara(PREAD, PSETKEYIN, id_sysp_0003 ,&rd);
ocomm_printf(CH_PRINT,1,1,"����վ���:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_STATION_ID]);
ocomm_printf(CH_PRINT,0,0,"ǹ��:%02d #\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
//   OSysparas_CMDPara(PREAD, PSETKEYIN, 1003 ,&rd);
//   ocomm_printf(CH_PRINT,0,0,"��˾����:%06d # \r\n", rd);
//   tmp = bcdbuf2hex(prt_CNGRec.UID,6);  
   ocomm_printf(CH_PRINT,0,0,"��ˮ��:%04ld\r\n",cur_rec.rec_id);
   ocomm_printf(CH_PRINT,0,0,"��ʼ: %u-%02d-%02d ",cur_rec.rec_stime[0], cur_rec.rec_stime[1], cur_rec.rec_stime[2]);
   ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",cur_rec.rec_stime[3], cur_rec.rec_stime[4], cur_rec.rec_stime[5]); 
   ocomm_printf(CH_PRINT,0,0,"����: %u-%02d-%02d ",cur_rec.rec_etime[0], cur_rec.rec_etime[1], cur_rec.rec_etime[2]);
   ocomm_printf(CH_PRINT,0,0,"%02d:%02d:%02d \r\n",cur_rec.rec_etime[3], cur_rec.rec_etime[4], cur_rec.rec_etime[5]);  
   oprt_lfrow(1);
   ocomm_printf(CH_PRINT,0,0, " ��ӭ����  ����ƽ��\r\n");

   ocomm_printf(CH_PRINT,0,0, "<�����Ʊ����ƾ��>\r\n");
   
   oprt_lfrow(6);    		
   SELECT_CARD();    						
}

void oprt_version_info(void)
{
   SELECT_PRINT();   
   oprt_printheadcmd();
   ocomm_printf(CH_PRINT,0,0,"�������汾��Ϣ\r\n");
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
   ocomm_printf(CH_PRINT,0,0,"������������Ϣ\r\n");
   oprt_printnormalcmd();   
   oprt_lfrow(2);
   ocomm_printf(CH_PRINT,1,1,"����վ���:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_STATION_ID]);
   ocomm_printf(CH_PRINT,1,1,"ǹ��:%d\r\n",(unsigned char)cur_sysparas.b_basic_p[BASIC_P_G_ID]);
  // ocomm_printf(CH_PRINT,2,1,"�ܶ�:%.04f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_DENSTY]);     
   //ocomm_printf(CH_PRINT,3,1,"ϵ��:%.04f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_CORRECT]); 
   ocomm_printf(CH_PRINT,2,1,"�������:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_ZERO]);
   ocomm_printf(CH_PRINT,3,1,"����ϵ��:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_CORRECT]); 
   ocomm_printf(CH_PRINT,4,1,"Һ�����:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_ZERO]);
   ocomm_printf(CH_PRINT,1,1,"Һ��ϵ��:%.02f\r\n",cur_sysparas.b_factor[BASIC_P_FACTOR_P_L_CORRECT]);  
   if(cur_sysparas.b_y_or_n_operate & BASIC_P_CARD_CHK) ocomm_printf(CH_PRINT,2,6,"�ÿ�����:��\r\n");
   else			ocomm_printf(CH_PRINT,2,6,"�ÿ�����:��\r\n");
   ocomm_printf(CH_PRINT,4,1,"��¼��:%ld\r\n",cur_sysparas.r_cur_g_id);
   ocomm_printf(CH_PRINT,1,1,"δ����ˮ��¼��:%ld\r\n",cur_sysparas.r_unupld_g_id);
   ocomm_printf(CH_PRINT,2,1,"����ˮ��:%ld\r\n",cur_sysparas.r_cur_shift_id);
   ocomm_printf(CH_PRINT,3,1,"δ������ˮ��:%ld\r\n",cur_sysparas.r_unupld_shift_id);   
   oprt_lfrow(6); 
   SELECT_CARD();    	
			
}

void oprt_ctrl_paras_info(void)
{
	SELECT_PRINT(); 
	oprt_printheadcmd();
	ocomm_printf(CH_PRINT,0,0,"������������Ϣ\r\n");
	oprt_printnormalcmd();   
	oprt_lfrow(2);	
	ocomm_printf(CH_PRINT,1,1,"Ԥ���ٽ��¶�:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[0]);
	ocomm_printf(CH_PRINT,2,1,"Ԥ���ٽ�����:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[1]);		
	ocomm_printf(CH_PRINT,3,1,"Ԥ�䳬ʱʱ��:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[2]);
	ocomm_printf(CH_PRINT,4,1,"ͣ����ʱ:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[3]);
	ocomm_printf(CH_PRINT,5,1,"ͣ������:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[4]);
	ocomm_printf(CH_PRINT,6,1,"ͣ����ʱʱ��:%.02f\r\n",(float)cur_sysparas.c_lcng_ctrl_paras[5]);
	oprt_lfrow(6); 
	SELECT_CARD();    	   	
}

void oprt_ctrl_iccard_test_info(unsigned char* wtbuf, unsigned char *rdbuf)
{
   SELECT_PRINT(); 
   oprt_printheadcmd();
   ocomm_printf(CH_PRINT,0,0,"     IC��������Ϣ\r\n");
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
		ocomm_printf(CH_PRINT,0,0,"     IC��������Ϣ\r\n");
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
   ocomm_printf(CH_PRINT,0,0,"        ��������\r\n");
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
	ocomm_printf(CH_PRINT,0,0,"        �ļ�����\r\n");
	oprt_printnormalcmd();
	oprt_lfrow(2);
	ocomm_printf(CH_PRINT,0,0, "����:%x %x %x %x %x %x %x %x \r\n",cpu_card_data.CpuCardNum[0],cpu_card_data.CpuCardNum[1],cpu_card_data.CpuCardNum[2],cpu_card_data.CpuCardNum[3],cpu_card_data.CpuCardNum[4],cpu_card_data.CpuCardNum[5],cpu_card_data.CpuCardNum[6],cpu_card_data.CpuCardNum[7]);
	ocomm_printf(CH_PRINT,0,0, "������%x \r\n",cpu_card_data.CpuCardType);
	//ocomm_printf(CH_PRINT,0,0, "��˾���%x %x %x %x\r\n",cpu_card_data.CpuCompanyNum[0],cpu_card_data.CpuCompanyNum[1],cpu_card_data.CpuStationNum[0],cpu_card_data.CpuStationNum[1]);
	//ocomm_printf(CH_PRINT,0,0, "����%x %x %x \r\n",cpu_card_data.CpuCardPassword[0],cpu_card_data.CpuCardPassword[1],cpu_card_data.CpuCardPassword[2]);
	//ocomm_printf(CH_PRINT,0,0, "��ƿ���� %x %x %x\r\n",cpu_card_data.CpuSteelDate[0],cpu_card_data.CpuSteelDate[1],cpu_card_data.CpuSteelDate[2]);
	ocomm_printf(CH_PRINT,0,0, "���������%x %x %x %x\r\n",cpu_card_data.CpuLastMon[0],cpu_card_data.CpuLastMon[1],cpu_card_data.CpuLastMon[2],cpu_card_data.CpuLastMon[3]);
	ocomm_printf(CH_PRINT,0,0, "����������%x %x %x %x \r\n",cpu_card_data.CpuLastVol[0],cpu_card_data.CpuLastVol[1],cpu_card_data.CpuLastVol[2],cpu_card_data.CpuLastVol[3]);
	ocomm_printf(CH_PRINT,0,0, "���ʹ��ʱ��%x %x %x %x %x %x\r\n",cpu_card_data.CpuLastTime[0],cpu_card_data.CpuLastTime[1],cpu_card_data.CpuLastTime[2],cpu_card_data.CpuLastTime[3],cpu_card_data.CpuLastTime[4],cpu_card_data.CpuLastTime[5]);
	//ocomm_printf(CH_PRINT,0,0, "������վ���%x %x  \r\n",cpu_card_data.CpuLastStation[0],cpu_card_data.CpuLastStation[1]);
	ocomm_printf(CH_PRINT,0,0, "ǹ��%x\r\n",cpu_card_data.CpuGunNum);
	ocomm_printf(CH_PRINT,0,0, "��ˮ��%x %x %x %x\r\n",cpu_card_data.CpuSerialNum[0],cpu_card_data.CpuSerialNum[1],cpu_card_data.CpuSerialNum[2],cpu_card_data.CpuSerialNum[3]);
	ocomm_printf(CH_PRINT,0,0, "��״̬%x\r\n",cpu_card_data.CpuValidity);
	ocomm_printf(CH_PRINT,0,0, "�����%x %x %x %x \r\n",cpu_card_data.CpuBalance[0],cpu_card_data.CpuBalance[1],cpu_card_data.CpuBalance[2],cpu_card_data.CpuBalance[3]);
	//ocomm_printf(CH_PRINT,0,0, "�ϴ����%x %x %x %x\r\n",cpu_card_data.CpuLastBalance[0],cpu_card_data.CpuLastBalance[1],cpu_card_data.CpuLastBalance[2],cpu_card_data.CpuLastBalance[3]);
	//ocomm_printf(CH_PRINT,0,0, "�ϴβ�����ʽ%x\r\n",cpu_card_data.CpuLastOper);
	//ocomm_printf(CH_PRINT,0,0, "�ϴβ���ʱ��%x %x %x %x %x %x\r\n",cpu_card_data.CpuLastOperTime[0],cpu_card_data.CpuLastOperTime[1],cpu_card_data.CpuLastOperTime[2],cpu_card_data.CpuLastOperTime[3],cpu_card_data.CpuLastOperTime[4],cpu_card_data.CpuLastOperTime[5]);
	ocomm_printf(CH_PRINT,0,0, "��������ʽ%x\r\n",cpu_card_data.CpuOper);
	//ocomm_printf(CH_PRINT,0,0, "����ֵ���%x %x %x %x\r\n",cpu_card_data.CpuRechargeMon[0],cpu_card_data.CpuRechargeMon[1],cpu_card_data.CpuRechargeMon[2],cpu_card_data.CpuRechargeMon[3]);
	oprt_lfrow(1);
   	ocomm_printf(CH_PRINT,0,0, " ��ӭ����  ����ƽ��\r\n");
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
