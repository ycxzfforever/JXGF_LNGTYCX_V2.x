/*
 * store.c
 *
 * Created: 2012-8-14 11:37:52
 *  Author: luom
 */ 
#include "global_def.h"
//加气前枪累
unsigned long gas_total_bak = 0;
//加气前班累金额
unsigned long sum_total_bak = 0;
//加气后枪累
unsigned long cur_gas_total = 0;
//加气后班累金额
unsigned long cur_sum_total = 0;
//当前加气量
unsigned long cur_gas_num = 0;
//float cur_gas_num = 0.0;
//当前加气金额
unsigned long cur_gas_sum = 0;
//float cur_gas_sum = 0.0;
/////加气前回气气量
float cur_back_num_ex = 0.0;
float cur_back_sum_ex = 0.0;
///////加气中回气气量
float cur_back_num = 0.0;

//////中油节能///////////////////////////////////////////////
static bool omem_write_rec(void)
{
  unsigned long addr;
  unsigned long id = cur_sysinfo.g_uid-1;  //记录流水号从0开始的
  id = id%FlashRecMaxID; 
  //if( id==0 ) id = FlashRecMaxID;
  addr = FlashRecSADDR + id*FlashRecLen;  
  return oflash_wrbuf(addr,sizeof(struct def_LCNG_Record),(unsigned char*)&cur_rec.rec_id);
}

bool ostore_rec_write(void)
{
  //unsigned char n = 0;
  unsigned int  crc = 0;
  bool			ret = false;
  //unsigned char *buf = NULL;
  OS_DISABLE_KEYSCAN();	
  OS_ENTER_CRITICAL();
  //校验和 rec_id  (unsigned char*)&cur_rec.rec_id
  crc = ocomm_crc16_chksum((unsigned char*)&cur_rec.rec_id, sizeof(struct def_LCNG_Record)-2);
  cur_rec.rec_crc_hi  = crc/0x100;
  cur_rec.rec_crc_low = crc%0x100;
  ret = omem_write_rec();
  OS_ENABLE_KEYSCAN(); 
  OS_EXIT_CRITICAL();//sei();	
  return true;
}

bool ostore_rec_read(unsigned long id, unsigned char* rdbuf, unsigned char len)
{
#if 0
  unsigned long addr;
  OS_ENTER_CRITICAL();//cli();
  OS_DISABLE_KEYSCAN();
  if(rdbuf == NULL)
  {
	 //memset((unsigned char*)&cur_rec.rec_id, 0, sizeof(struct def_CNG_Record)); 
	 rdbuf = (unsigned char*)&cur_rec.rec_id;
	 len = sizeof(struct def_CNG_Record);
  }
  //else
  //{
   memset(rdbuf, 0, len);    
  //}
  id = id%FlashRecMaxID;  
  //if( id==0 ) id = FlashRecMaxID;
  addr = FlashRecSADDR + id*FlashRecLen;  
  //if(!oflash_readbuf(addr,sizeof(struct def_CNG_Record),(unsigned char*)&cur_rec.rec_id))
  if(!oflash_readbuf(addr,sizeof(struct def_CNG_Record),rdbuf))
  {
	  OS_EXIT_CRITICAL();//sei();	
	  OS_ENABLE_KEYSCAN();
	  return false;
  } 
  /*
  if(cur_rec.rec_id > FlashRecMaxID)
  {
	  if(!omem_read_rec_bak(id)){
		 OS_ENABLE_KEYSCAN();
		 OS_EXIT_CRITICAL();//sei();	 
		 return false;
	  }	  	 
  } 
  */ 
  OS_ENABLE_KEYSCAN();
  OS_EXIT_CRITICAL();//sei();	  
  return true;
 #endif
  unsigned long addr;
  bool          ret = true;
  _lcng_rec		rd_rec;
  OS_ENTER_CRITICAL();//cli();
  OS_DISABLE_KEYSCAN(); 
  ret = true; 
  memset((unsigned char*)&rd_rec.rec_id, 0, sizeof(struct def_LCNG_Record)); 
  id = id%FlashRecMaxID;  
  addr = FlashRecSADDR + id*FlashRecLen;  
  if(!oflash_readbuf(addr,sizeof(struct def_LCNG_Record),(unsigned char*)&rd_rec.rec_id))
  {
	  ret = false; 
  } 
/*  if(ret == true && rd_rec.rec_id > FlashRecMaxID)	
  {
	  ret = false;
  }*/
  if(ret == true)
  {
	  if(rdbuf == NULL) memcpy((unsigned char*)&cur_rec.rec_id, (unsigned char*)&rd_rec.rec_id,sizeof(struct def_LCNG_Record));
	  else              memcpy(rdbuf, (unsigned char*)&rd_rec.rec_id,len);
  }
  OS_ENABLE_KEYSCAN();
  OS_EXIT_CRITICAL();//sei();	  
  return ret;  
}


void  ostore_sysinfo_save(void) //float num, float sum)
{
	unsigned long tmp1 =0;
	long long tmp4 = 0;
	
	OS_ENTER_CRITICAL();//cli();	
	cur_sysparas.r_cur_g_id = cur_sysinfo.g_uid; 
   /************************班累**************************************************/  
   cur_sysinfo.shift_gascount  += 1;								//  //班累次数 
   
   tmp1 = cur_rec.rec_lcngnum + gas_total_bak;
   cur_gas_num = cur_rec.rec_lcngnum;
   cur_gas_total = tmp1;
   gas_total_bak = tmp1;
   ocomm_hex2bcdbuf(tmp1, (char *)cur_sysinfo.shift_gas_total,4);
   
   tmp1 = cur_rec.rec_lcngsum + sum_total_bak;
   cur_sum_total = tmp1;
   cur_gas_sum = cur_rec.rec_lcngsum;
   sum_total_bak = tmp1;
   ocomm_hex2bcdbuf(tmp1, (char *)cur_sysinfo.shift_m_amount,4);
  
   if(cur_sysparas.b_y_or_n_operate&BASIC_P_CARD_CHK) //用卡加气                                           //班累现金
   {
		//do nothing
   }
   else
   {	
		cur_rec.rec_lcngsum= cur_gas_sum;
		tmp1 = (unsigned long)ocomm_bcdbuf2hex((unsigned char *)&cur_sysinfo.shift_cash[0], 4);
		tmp1 = tmp1 + cur_gas_sum;
		ocomm_hex2bcdbuf(tmp1, (char *)&cur_sysinfo.shift_cash[0],4);
   }
   /************************枪总累**************************************************/  
   cur_sysinfo.gas_count += 1;     //      //总累次数
   //cur_rec.rec_lcngnum = (float)cur_gas_num/100.00;
   cur_rec.rec_lcngnum = cur_gas_num;
   //cur_sysinfo.g_sum     += cur_rec.rec_lcngnum ;//num;   //       //总枪累
   tmp4 = ocomm_bcdbuf2hex(cur_sysinfo.g_sum, 6);
   tmp4 = tmp4 + cur_gas_num;
   ocomm_hex2bcdbuf(tmp4,(char *)&cur_sysinfo.g_sum[0], 6);
  
   //cur_rec.rec_lcngnum = (float)cur_gas_num/100.00;
   cur_rec.rec_lcngnum = cur_gas_num;   //总累气量 
    tmp4 = ocomm_bcdbuf2hex(cur_sysinfo.gas_total, 6);
   tmp4 = tmp4 + cur_gas_num;
   ocomm_hex2bcdbuf(tmp4,(char *)&cur_sysinfo.gas_total[0], 6);
   
   //cur_rec.rec_lcngsum = (float)cur_gas_sum/100.00;
   cur_rec.rec_lcngsum = cur_gas_sum;
    tmp4 = ocomm_bcdbuf2hex(cur_sysinfo.m_amount, 6);  //总累金额
   tmp4 = tmp4 + cur_gas_sum;
   ocomm_hex2bcdbuf(tmp4,(char *)&cur_sysinfo.m_amount[0], 6);
   
   //cur_rec.rec_lcngnum = (long) (cur_gas_num + 0.5);
   //cur_rec.rec_lcngsum = (long) (cur_gas_sum + 0.5);/////结算量
   cur_rec.rec_lcngnum = cur_gas_num ;
   cur_rec.rec_lcngsum = cur_gas_sum ;/////结算量

   if(cur_rec.rec_lcngnum < 0)
   		cur_rec.rec_b_Print = 1;
   else
   		cur_rec.rec_b_Print = 0;

   cur_rec.rec_relcngnum = cur_back_num;////////加气过程中回气
   cur_rec.rec_back_num = cur_back_num_ex + cur_back_num;////回气量
   cur_rec.rec_alllcng_num = cur_rec.rec_lcngnum + cur_back_num_ex + cur_back_num;//////加液量=结算量+回气量
   
   ee24lc_write(EX_EEPROM_SYSINFO_ADDR, sizeof(struct def_SysInfo) ,(unsigned char*)&cur_sysinfo.g_uid);
   ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, r_cur_g_id), 4,(unsigned char*)&cur_sysparas.r_cur_g_id);

   OS_EXIT_CRITICAL();//sei();
}


bool ostore_rpt_read(unsigned long id, unsigned char* rdbuf)
{
  unsigned long addr;
  bool          ret = true;
  OS_ENTER_CRITICAL();//cli();
  OS_DISABLE_KEYSCAN(); 
  ret = true; 
  id = id%FLASH_RPT_MAX;  
  addr = FLASH_RPT_S_ADDR + id*FLASH_RPT_LEN;  
  if(!oflash_readbuf(addr,sizeof(struct def_LCNG_shift_rpt),rdbuf))
  {
	  ret = false; 
  } 
  OS_ENABLE_KEYSCAN();
  OS_EXIT_CRITICAL();//sei();	  
  return ret;  
}
bool ostore_shift_rpt_save(unsigned char newshiftid)
{
	_lcng_shiift_rpt       _shift_rpt;
	_lcng_shift_trans_Rpt  _shift_trans_rpt;
	unsigned long addr;
	unsigned long num;
	unsigned char n = 0;
	unsigned long id = cur_sysinfo.shift_uid,tmp = 0;							//班流水号从0开始的
    OS_ENTER_CRITICAL();
    OS_DISABLE_KEYSCAN(); 	
	_shift_rpt.shift_rpt_id				=  cur_sysinfo.shift_uid+1;		//班流水号
	_shift_rpt.shift_id					=  cur_sysinfo.shift_id;		//班号
	memset(_shift_rpt.shift_rpt_total_lcngcash,0,4);
	memcpy(_shift_rpt.shift_rpt_total_lcngcash,cur_sysinfo.shift_cash,4);//班现金
	
	_shift_rpt.shift_rec_num			=  cur_sysinfo.shift_gascount;	//班加气次数

	memset(_shift_rpt.shift_total_rec_lcngsum,0,4);
	memcpy(_shift_rpt.shift_total_rec_lcngsum, cur_sysinfo.shift_m_amount,4); //班累计金额
	
	memset(_shift_rpt.shift_rpt_s_lcng,0,4);
	memcpy(_shift_rpt.shift_rpt_s_lcng, cur_sysinfo.shift_s_gsum,4);//班开始枪累
	memset(_shift_rpt.shift_rpt_e_lcng,0,4);
	memcpy(_shift_rpt.shift_rpt_e_lcng, (unsigned char*)&cur_sysinfo.g_sum[2],4); //班结束枪累
	
	memset(_shift_rpt.shift_total_rpt_lcng,0,4);
	memcpy(_shift_rpt.shift_total_rpt_lcng,cur_sysinfo.shift_gas_total,4);//班加气量
	memset(_shift_rpt.shift_total_rec_lcngnum,0,4);
	memcpy(_shift_rpt.shift_total_rec_lcngnum,cur_sysinfo.shift_gas_total,4);//班加气量
	
	memcpy(&_shift_rpt.shift_rpt_stime[0],  &cur_sysinfo.shift_s_date[0], 3);  //班开始日期
	memcpy(&_shift_rpt.shift_rpt_stime[3],  &cur_sysinfo.shift_s_time[0], 3);  //班开始时间(转换)	 
	ds3231_read_date((unsigned char*)&_shift_rpt.shift_rpt_etime[0],true);
	ds3231_read_time((unsigned char*)&_shift_rpt.shift_rpt_etime[3],true);	
	id = cur_sysinfo.shift_uid;
	id = id%FLASH_RPT_MAX; 
	addr = FLASH_RPT_S_ADDR + id*FLASH_RPT_LEN;  
	oflash_wrbuf(addr,sizeof(struct def_LCNG_shift_rpt),(unsigned char*)&_shift_rpt.shift_rpt_id);

	memset(cur_sysinfo.shift_s_amount,0,4);//班开始金额
	memcpy(cur_sysinfo.shift_s_gsum,(unsigned char*)&cur_sysinfo.g_sum[2],4);//班开始枪累

	gas_total_bak = 0;
	memset(cur_sysinfo.shift_gas_total,0,4);
	
	memcpy(&cur_sysinfo.shift_s_date[0], &_shift_rpt.shift_rpt_etime[0], 3);
	memcpy(&cur_sysinfo.shift_s_time[0], &_shift_rpt.shift_rpt_etime[3], 3);
	cur_sysinfo.shift_uid += 1;				//班流水号
	cur_sysparas.r_cur_shift_id = cur_sysinfo.shift_uid; 
	cur_sysinfo.shift_id   = newshiftid;	//班号
	memset(cur_sysinfo.shift_cash,0,4);//班现金
	
	cur_sysinfo.shift_gascount = 0;         //班加气次数
	sum_total_bak = 0;
	memset(cur_sysinfo.shift_m_amount,0,4); //班累计金额 
	//存EEPROM
	ee24lc_write(EX_EEPROM_SYSINFO_ADDR ,sizeof(struct def_SysInfo) ,(unsigned char*)&cur_sysinfo.g_uid);
	ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, r_cur_shift_id), 4,(unsigned char*)&cur_sysparas.r_cur_shift_id);
	////转换
	ocomm_hex2bcdbuf(_shift_rpt.shift_rpt_id, (char*)&_shift_trans_rpt.UID[0],6 );					   //班流水号
	_shift_trans_rpt.UsrID = ocomm_hex2bcd(_shift_rpt.shift_id);									   //班号
	
	memset(_shift_trans_rpt.RptStartCNG,0,6);
	tmp = ocomm_bcdbuf2hex(_shift_rpt.shift_rpt_s_lcng, 4);
	ocomm_hex2bcdbuf(tmp, (char *)_shift_trans_rpt.RptStartCNG, 6); //开始枪累

	tmp = ocomm_bcdbuf2hex(_shift_rpt.shift_rpt_e_lcng,4);
	memset(_shift_trans_rpt.RptEndCNG,0,6);
	ocomm_hex2bcdbuf(tmp, (char *)_shift_trans_rpt.RptEndCNG, 6);  //结束枪累
	   
	memcpy(_shift_trans_rpt.RptTotalCNG,_shift_rpt.shift_total_rpt_lcng,4); //合计加气量（或卸气量）=班结束枪累-班开始枪累
	memcpy(_shift_trans_rpt.RptUseCNG,_shift_rpt.shift_total_rec_lcngnum,4);//合计加气量（按流水记录合计计算）
	 
	memcpy(_shift_trans_rpt.RptMoney,_shift_rpt.shift_total_rec_lcngsum,4);//合计金额
	memcpy(_shift_trans_rpt.RptCash,_shift_rpt.shift_rpt_total_lcngcash,4);//合计现金
	ocomm_hex2bcdbuf  (_shift_rpt.shift_rec_num, (char*)&_shift_trans_rpt.RptCNGNum[0],2);	//合计加气次数
	for(n=0; n<6; n++)
		_shift_trans_rpt.RptStartTime[n] = ocomm_hex2bcd(_shift_rpt.shift_rpt_stime[n]);
      for(n=0; n<6; n++)
		_shift_trans_rpt.RptEndTime[n] = ocomm_hex2bcd(_shift_rpt.shift_rpt_etime[n]);
    	_shift_trans_rpt.CalutUnit = ocomm_hex2bcd(0x01);
	_shift_trans_rpt.MechType = ocomm_hex2bcd(0x01);
	num = (unsigned long)cur_sysparas.b_basic_p[4];
	_shift_trans_rpt.StationNum = (unsigned char)(num/10*16+num%10);
     
	id = cur_sysinfo.shift_uid-1;
      addr = FLASH_RPT_TRANS_S_ADDR + id*FLASH_RPT_LEN;  
	oflash_wrbuf(addr,sizeof(struct def_LCNG_SHIFT_TRANS_Rpt),(unsigned char*)&_shift_trans_rpt.UID[0]);
	oprt_rpt(false, (unsigned char*)&_shift_rpt.shift_rpt_id);
	OS_EXIT_CRITICAL();//cli();
	OS_ENABLE_KEYSCAN(); 	
	return true;
}
bool ostore_losscard_chk( unsigned long lc_id)
{
	unsigned int  n, rdmax = 0;
	unsigned char    z_id = 0;
	unsigned long    tmprd = 0;
	unsigned long    addr = 0;
	bool			 ret = false;
    z_id = (lc_id/1000000)%10;  //站编号
	rdmax = oparas_get_cardlost_num(z_id);	
	if(rdmax >= FLASH_L_CARD_MAX || z_id > FLASH_L_CARD_MAX_Z) ret= true;
	ret = false;
	addr  = FLASH_L_CARD_SADDR;
	addr += FLASH_L_ZMAX_LEN*z_id;
	for(n=0; n<rdmax; n++)//while(rdmax--)
	{
			tmprd = 0;
			__WATCH_DOG();  //osys_opower_chk();
			addr += n*4;
			oflash_readbuf(addr,4,(unsigned char*)&tmprd);//oflash_readbuf(addr,4,&tmpul.dt_uc[0]);
			if(tmprd == lc_id){
				 ret = true;
				 break;
			}					 
	}	
	return ret;
}
bool ostore_losscard_save(unsigned long lc_id)
{
	//卡类型(1) + 发卡站编号(1) + 卡号(6)
	bool          ret = true;
	unsigned int  n, rdmax = 0;
	unsigned long z_num = 0;
	unsigned long addr  = FLASH_L_CARD_SADDR;
	//_union_ulong  tmpul;
	unsigned long   tmpul;
    OS_ENTER_CRITICAL();
    OS_DISABLE_KEYSCAN(); 	
	z_num = (lc_id/1000000)%10;  //站编号
	addr  = FLASH_L_CARD_SADDR;
	addr += FLASH_L_ZMAX_LEN*z_num;
	ret = true;
	rdmax = oparas_get_cardlost_num(z_num);
	if(rdmax >= FLASH_L_CARD_MAX || z_num > FLASH_L_CARD_MAX_Z) ret= false;
	if(true == ret)
	{   //判断是否已经挂失
		ret = false;
		n = 0;
		for(n=0; n<rdmax; n++)//while(rdmax--)
		{
			tmpul = 0;
			//sys_opower_chk();
			addr += n*4;
			oflash_readbuf(addr,4,(unsigned char*)&tmpul);//oflash_readbuf(addr,4,&tmpul.dt_uc[0]);
			if(tmpul == lc_id){
				 ret = true;
				 break;
			}					 
		}
	}
	if(false == ret)
	{
		addr += rdmax*4;
		oflash_wrbuf(addr,4,(unsigned char*)&lc_id);
		rdmax += 1;
		oparas_set_cardlost_num(z_num, rdmax);
		ret = true;
/*		SELECT_PRINT();
		oprt_printnormalcmd();
		ocomm_printf(CH_PRINT,0,0,"addr:%ld\r\n",addr);	
		ocomm_printf(CH_PRINT,0,0,"rdmax:%d\r\n",rdmax);	
		ocomm_printf(CH_PRINT,0,0,"z_num:%ld\r\n",z_num);	
		SELECT_CARD();
*/
	}
/*	SELECT_PRINT();
    oprt_printnormalcmd();
    ocomm_printf(CH_PRINT,0,0,"%d %d \r\n",z_num,rdmax);	
	ocomm_printf(CH_PRINT,0,0,"%ld\r\n",lc_id);	
    oprt_lfrow(6);    		
    SELECT_CARD(); */
 
	OS_EXIT_CRITICAL();//cli();
	OS_ENABLE_KEYSCAN(); 	
	return ret;
}

unsigned long ostore_losscard_read(unsigned char z_num, unsigned long lc_index)
{
	//卡类型(1) + 发卡站编号(1) + 卡号(6)
	unsigned int  rdmax = 0;
	unsigned long addr  = FLASH_L_CARD_SADDR;
	_union_ulong  tmpul;
    OS_ENTER_CRITICAL();
    OS_DISABLE_KEYSCAN(); 	
	addr += FLASH_L_ZMAX_LEN*z_num;
	tmpul.dt_ulong = 0;
	rdmax = oparas_get_cardlost_num(z_num);
	if(rdmax < FLASH_L_CARD_MAX && rdmax < lc_index && z_num<64) 
	{
		addr += lc_index*4;
		oflash_readbuf(addr,4,&tmpul.dt_uc[0]);
	}
	OS_EXIT_CRITICAL();
	OS_ENABLE_KEYSCAN(); 	
	return tmpul.dt_ulong;	
}
