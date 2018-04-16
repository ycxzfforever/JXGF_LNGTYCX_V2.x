/*
 * protocol.c
 *
 * Created: 2012-8-14 15:09:07
 *  Author: luom
 */ 
#include "global_def.h"
//中油节能

volatile unsigned char			Rs485Buf[128];
prog_char CRC8_TAB[]=	{//多项式X^8 + X^5 + X^4 + 1
			0x00,0x5e,0xbc,0xe2,0x61,0x3f,0xdd,0x83,//0~7
			0xc2,0x9c,0x7e,0x20,0xa3,0xfd,0x1f,0x41,//8~15
			0x9d,0xc3,0x21,0x7f,0xfc,0xa2,0x40,0x1e,//16_23
			0x5f,0x01,0xe3,0xbd,0x3e,0x60,0x82,0xdc,//24~31
			0x23,0x7d,0x9f,0xc1,0x42,0x1c,0xfe,0xa0,//32~39
			0xe1,0xbf,0x5d,0x03,0x80,0xde,0x3c,0x62,//40~47
			0xbe,0xe0,0x02,0x5c,0xdf,0x81,0x63,0x3d,//48~55
			0x7c,0x22,0xc0,0x9e,0x1d,0x43,0xa1,0xff,//56~63
			0x46,0x18,0xfa,0xa4,0x27,0x79,0x9b,0xc5,//64~71
			0x84,0xda,0x38,0x66,0xe5,0xbb,0x59,0x07,//72~79
			0xdb,0x85,0x67,0x39,0xba,0xe4,0x06,0x58,//80~87
			0x19,0x47,0xa5,0xfb,0x78,0x26,0xc4,0x9a,//88~95
			0x65,0x3b,0xd9,0x87,0x04,0x5a,0xb8,0xe6,//96~103
			0xa7,0xf9,0x1b,0x45,0xc6,0x98,0x7a,0x24,//104~111
			0xf8,0xa6,0x44,0x1a,0x99,0xc7,0x25,0x7b,//112~119
			0x3a,0x64,0x86,0xd8,0x5b,0x05,0xe7,0xb9,//120~127
			0x8c,0xd2,0x30,0x6e,0xed,0xb3,0x51,0x0f,//128~135
			0x4e,0x10,0xf2,0xac,0x2f,0x71,0x93,0xcd,//136~143
			0x11,0x4f,0xad,0xf3,0x70,0x2e,0xcc,0x92,//144~151
			0xd3,0x8d,0x6f,0x31,0xb2,0xec,0x0e,0x50,//152~159
			0xaf,0xf1,0x13,0x4d,0xce,0x90,0x72,0x2c,//160~167
			0x6d,0x33,0xd1,0x8f,0x0c,0x52,0xb0,0xee,//168~175
			0x32,0x6c,0x8e,0xd0,0x53,0x0d,0xef,0xb1,//176~183
			0xf0,0xae,0x4c,0x12,0x91,0xcf,0x2d,0x73,//184~191
			0xca,0x94,0x76,0x28,0xab,0xf5,0x17,0x49,//192~199
			0x08,0x56,0xb4,0xea,0x69,0x37,0xd5,0x8b,//200~207
			0x57,0x09,0xeb,0xb5,0x36,0x68,0x8a,0xd4,//208~215
			0x95,0xcb,0x29,0x77,0xf4,0xaa,0x48,0x16,//216~223
			0xe9,0xb7,0x55,0x0b,0x88,0xd6,0x34,0x6a,//224~231
			0x2b,0x75,0x97,0xc9,0x4a,0x14,0xf6,0xa8,//232~239
			0x74,0x2a,0xc8,0x96,0x15,0x4b,0xa9,0xf7,//240~247
			0xb6,0xe8,0x0a,0x54,0xd7,0x89,0x6b,0x35,//248~255
}; 
 
unsigned char int_crc8(unsigned char *p,unsigned int m)/////////////////////CRC8
{
  unsigned int  i;
  unsigned char CRC8=0;
  for(i=0;i<m;i++)
  	CRC8 = pgm_read_byte(&CRC8_TAB[p[i]^CRC8]);//CRC8 = CRC8_TAB[p[i]^CRC8];
  return(CRC8);
}

void opotocol_zyjn_setheader(unsigned char cmdID)//////////////////////////////////帧头
{
  //memset((unsigned char*)&Rs485RecvBuf,0,sizeof(Rs485RecvBuf));
  Rs485Buf[0] = 0xBB;       //起始符 AAH
  Rs485Buf[1] = 0xEE;       //起始符 FFH
  Rs485Buf[2] = 0x00;       //目标设备 （00H PC机）（15H 加气机）（20H 通信器）
  Rs485Buf[3] = cmdID;      //命令 01H~90H
  Rs485Buf[4] = cur_sysparas.b_basic_p[BASIC_P_G_ID];     //抢号 0~32
}

void opotocol_zyjn_settail_send(unsigned char len)//////////////////////////////////帧尾
{
	Rs485Buf[5] = len;	
	Rs485Buf[len+6] = int_crc8((unsigned char*)&Rs485Buf[2],len+4);;//int_crc8(0;//crc;
	Rs485Buf[len+7] = RS485TAIL_H;
	Rs485Buf[len+8] = RS485TAIL_L;
	ouart2_sendstr_ex(&Rs485Buf[0],len+9);
}

static unsigned char    cur_upld_type = NOUPLD;
void opotocol_zyjn_responseok(void)
{
	unsigned char recv_cmd;	
	unsigned char resp_id;	
	unsigned long recv_id;
	unsigned int eepaddr = EEPROM_SYSPARAS_ADDR;
	recv_cmd = pctrans_recvbuf[6];
	recv_id = ocomm_bcdbuf2hex((unsigned char *)&pctrans_recvbuf[9],6);
	resp_id = CMD_RECVERR;
	switch(recv_cmd)
	{
		case CMD_UPLDREC://流水记录
			if(cur_upld_type == UPLDREC)
			{
				if(cur_sysparas.r_unupld_g_id >= cur_sysparas.r_cur_g_id || recv_id != cur_sysparas.r_unupld_g_id+1) return;//break;
				cur_sysparas.r_unupld_g_id += 1;
				//存EEPROM
				ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, r_unupld_g_id), 4,(unsigned char*)&cur_sysparas.r_unupld_g_id);
				eepaddr = EEPROM_SYSPARAS_ADDR;
				eepaddr  += offsetof(Sysparas, r_unupld_g_id);
				eeprom_busy_wait();	
				eeprom_write_dword((uint32_t*)eepaddr , cur_sysparas.r_unupld_g_id);
				ee24lc_write(eepaddr ,4, (unsigned char*)&cur_sysparas.r_unupld_g_id); //////外部EEPROM
				resp_id = 0x69;
			}
			break;
		//case UPLDRPT:
		  case CMD_UPLDRPT://班累记录
			if(cur_upld_type == UPLDRPT)
			{
				if(cur_sysparas.r_unupld_shift_id >= cur_sysparas.r_cur_shift_id || recv_id != cur_sysparas.r_unupld_shift_id+1) return;//break;
				cur_sysparas.r_unupld_shift_id += 1;
				//存EEPROM
				ee24lc_write(EX_EEPROM_SYSPARAS_ADDR + offsetof(Sysparas, r_unupld_shift_id), 4,(unsigned char*)&cur_sysparas.r_unupld_shift_id);
				eepaddr   = EEPROM_SYSPARAS_ADDR;
				eepaddr  += offsetof(Sysparas, r_unupld_shift_id);
				eeprom_busy_wait();	
				eeprom_write_dword((uint32_t*)eepaddr , cur_sysparas.r_unupld_shift_id);
				ee24lc_write(eepaddr ,4, (unsigned char*)&cur_sysparas.r_unupld_shift_id); //////外部EEPROM
				resp_id = 0x69;
			}
			break;
		default:
			break;
	}
	cur_upld_type = NOUPLD;
	opotocol_zyjn_setheader(resp_id);
	opotocol_zyjn_settail_send(0);
}	

/*
@发送加气流水记录
@opotocol_zyjn_upload_rec
*/
void opotocol_zyjn_upload_rec(void)
{
	//if(ostore_rec_read(cur_sysinfo.unupld_rec_id,(unsigned char*)&rd_rec.rec_id,sizeof(struct def_CNG_Record)))
	if(opotocol_zyjn_recdata_read(cur_sysparas.r_unupld_g_id,(unsigned char*)&Rs485Buf[6])) //cur_sysinfo.unupld_rec_id
	{
		cur_upld_type = UPLDREC;
		opotocol_zyjn_setheader(0x11);
		opotocol_zyjn_settail_send(sizeof(struct def_LCNG_TRANS_REC));
		return;
	}
}


/*
@没有记录发送
@opotocol_zyjn_send_idle
*/
void opotocol_zyjn_send_idle(void)
{
       cur_upld_type = NOUPLD;
       //opotocol_zyjn_setheader(0x51);
	  //memset((unsigned char*)&Rs485RecvBuf,0,sizeof(Rs485RecvBuf));

	 ///////帧头
	Rs485Buf[0] = 0xAA;       //起始符 AAH
	Rs485Buf[1] = 0xFF;       //起始符 FFH
	Rs485Buf[2] = 0x00;       //目标设备 （00H PC机）（15H 加气机）（20H 通信器）
	Rs485Buf[3] = 0X51;      //命令 01H~90H
	Rs485Buf[4] = cur_sysparas.b_basic_p[BASIC_P_G_ID];     //抢号 0~32
       	opotocol_zyjn_settail_send(sizeof(struct def_LCNG_SHIFT_TRANS_Rpt));
}


/*
@发送班统计记录
@opotocol_zyjn_upload_shiftrpt
*/
void opotocol_zyjn_upload_shiftrpt(void)
{
	if(opotocol_zyjn_shiftrptdata_read(cur_sysparas.r_unupld_shift_id ,(unsigned char*)&Rs485Buf[6])) //cur_sysinfo.unupld_rec_id
	{
		cur_upld_type = UPLDRPT;
		opotocol_zyjn_setheader(0x12);
		opotocol_zyjn_settail_send(sizeof(struct def_LCNG_SHIFT_TRANS_Rpt));
		return;
	}	
}

void opotocol_zyjn_cmd_poll(void)
{  
    if(cur_sysparas.b_y_or_n_operate&BASIC_P_RPT_FIRST)
	{
		if(cur_sysparas.r_unupld_shift_id < cur_sysparas.r_cur_shift_id)
		{
			opotocol_zyjn_upload_shiftrpt();
			return;
		}
	}

	if(cur_sysparas.r_unupld_g_id < cur_sysparas.r_cur_g_id)
	{
		
		opotocol_zyjn_upload_rec();
		return;
	}
	else if(cur_sysparas.r_unupld_shift_id < cur_sysparas.r_cur_shift_id)
	{

		opotocol_zyjn_upload_shiftrpt();
		return;
	}
	else
	{
		opotocol_zyjn_send_idle();
		return;
	}
}

/*
@发送工作状态命令
@opotocol_zyjn_cmd_lcngrunstate
*/
void opotocol_zyjn_cmd_lcngrunstate(void)
{
	  unsigned char len = sizeof(struct def_CNG_RunSRec);
      opotocol_zyjn_setheader(CMD_CNGIDLE);//0x19
      memcpy((unsigned char*)&Rs485Buf[6],&cur_lcng_r_rec.runRFCardNum[0], len);//消息体
	  opotocol_zyjn_settail_send(len);
}
/*
@发送工作状态
@opotocol_zyjn_cmd_lcngrunstate
*/
void opotocol_zyjn_lcngrunstate_pro(void)
{
  //  ocomm_float2bcdbuf(cur_rec.rec_lcngnum, (char*)&cur_lcng_r_rec.brunCNGNum[0],4 );   //运行时加气气量
    //ocomm_float2bcdbuf(cur_rec.rec_lcngsum, (char*)&cur_lcng_r_rec.brunCNGCash[0],4 ); //运行时加气金额 
    //zl 2013-12-16
     ocomm_hex2bcdbuf(cur_rec.rec_lcngnum, (char*)&cur_lcng_r_rec.brunCNGNum[0],4 );   //运行时加气气量
    ocomm_hex2bcdbuf(cur_rec.rec_lcngsum, (char*)&cur_lcng_r_rec.brunCNGCash[0],4 ); //运行时加气金额

	cur_lcng_r_rec.CalutUnit = ocomm_hex2bcd(0x01);
	cur_lcng_r_rec.MechType = ocomm_hex2bcd(0x01);	
	cur_lcng_r_rec.StationNum = cur_sysparas.b_basic_p[4];
	if(LCNGSTATE_RUNNING == cur_lcng_r_rec.brunCNG)
	{
		opotocol_zyjn_cmd_lcngrunstate();
	}
}

/*
@挂失卡信息发送
@opotocol_loss_card
*/
void opotocol_loss_card(void)
{
	unsigned long cid = 0;
	cid = ocomm_bcdbuf2hex((unsigned char *)&pctrans_recvbuf[11],4);
	if(ostore_losscard_save(cid))
	{
		opotocol_zyjn_setheader(COMM_LOSS_CARD);
		Rs485Buf[6] = 1; //挂失
		memcpy((unsigned char*)&Rs485Buf[7],(unsigned char*)&pctrans_recvbuf[11],4);//memcpy(&Rs485Buf[7],&pctrans_recvbuf[9],4);
	    opotocol_zyjn_settail_send(5);
	}
	
}

unsigned int bcd2dec(unsigned char  h)
{
  return h/0x10*10+h%0x10;
}

void opotocol_ex_shift(void)
{
	unsigned char newshiftid = pctrans_recvbuf[6];
	unsigned char buf[6];
	/*buf[0] = pctrans_recvbuf[12];
	buf[1] = pctrans_recvbuf[13];
	buf[2] = pctrans_recvbuf[14];
	buf[3] = pctrans_recvbuf[18];
	buf[4] = pctrans_recvbuf[19];
	buf[5] = pctrans_recvbuf[20];
	buf[0] = hexTolong((unsigned char*)&pctrans_recvbuf[12],1);
	buf[1] = hexTolong((unsigned char*)&pctrans_recvbuf[13],1);
	buf[2] = hexTolong((unsigned char*)&pctrans_recvbuf[14],1);
	buf[3] = hexTolong((unsigned char*)&pctrans_recvbuf[18],1);
	buf[4] = hexTolong((unsigned char*)&pctrans_recvbuf[19],1);
	buf[5] = hexTolong((unsigned char*)&pctrans_recvbuf[20],1);*/

	buf[0] = (unsigned char)bcd2dec(pctrans_recvbuf[12]); 
	buf[1] = (unsigned char)bcd2dec(pctrans_recvbuf[13]); 
	buf[2] = (unsigned char)bcd2dec(pctrans_recvbuf[14]); 
	buf[3] = (unsigned char)bcd2dec(pctrans_recvbuf[18]); 
	buf[4] = (unsigned char)bcd2dec(pctrans_recvbuf[19]); 
	buf[5] = (unsigned char)bcd2dec(pctrans_recvbuf[20]); 
	
	
	//oiic_write_bytes(TWI_SLA_DS_RT,0x00, 3, buf[0]);
	//oiic_write_bytes(TWI_SLA_DS_RT,0x04, 3, buf[3]);
	
	ds3231_set_date((unsigned char*)&buf[0]);
	ds3231_set_time((unsigned char*)&buf[3]);
	
	//newshiftid = (newshiftid/0x10)*10+newshiftid%0x10;
	if(newshiftid>=10 || newshiftid<1 )              return;
	if(cur_sysinfo.shift_id != newshiftid)
	{
		//新的班号
		//cur_sysinfo.shift_id = newshiftid;
		ostore_shift_rpt_save(newshiftid);
		///上传班累
		opotocol_zyjn_upload_shiftrpt();
		ocomm_message(4,1,"换班成功",2,oui_main_ui);
	}
}

void opotocol_rd_uid_rec(void)
{
	unsigned long recid = 0;
	if(CMD_UPLDREC == pctrans_recvbuf[6])  //流水号
	{
		recid = ocomm_bcdbuf2hex((unsigned char *)&pctrans_recvbuf[9],6);
		if(cur_sysparas.r_cur_g_id < recid) return;
		opotocol_zyjn_data_exchange(recid,true,false,true,(unsigned char*)&Rs485Buf[6]);
		opotocol_zyjn_setheader(0x11);
		opotocol_zyjn_settail_send(sizeof(struct def_LCNG_TRANS_REC));
		return;
	}
	else if(CMD_UPLDRPT == pctrans_recvbuf[6])
	{
	        recid = ocomm_bcdbuf2hex((unsigned char *)&pctrans_recvbuf[9],6);
		 if(opotocol_zyjn_shiftrptdata_read((recid-1),(unsigned char*)&Rs485Buf[6])) //cur_sysinfo.unupld_rec_id
		{
			opotocol_zyjn_setheader(0x12);
			opotocol_zyjn_settail_send(sizeof(struct def_LCNG_SHIFT_TRANS_Rpt));
			return;
		}
	}
}

void ouart2_sendchar(unsigned char c);
void ouart2_sendstr_ex(volatile unsigned char *buf, unsigned char Len);

void opotocol_zyjn_analysis(void)
{
      // otimer_t0_off();
     //  RS485_CLR_RXIRPT();
       //ocomm_printf(CH_DISP,4,1,"命令:%x",pctrans_recvbuf[3]);
	memset((unsigned char*)&Rs485Buf,0,sizeof(Rs485Buf));
	if(true == blcng_running && COMM_POLL == pctrans_recvbuf[3])
	{
		 if(pctrans_recvbuf[6] == 0x01)
		 {
			 cur_lcng_r_rec.brunCNG = LCNGSTATE_RUNNING;
		 } 
		 else
		 {
			 cur_lcng_r_rec.brunCNG = LCNGSTATE_HANSHAKING;
		 }
		 opotocol_zyjn_cmd_lcngrunstate();
		 pctrans_recvlen = 0;
	}		
	else 
	{
		//memset((unsigned char*)&Rs485Buf,0,sizeof(Rs485Buf))
		
		switch(pctrans_recvbuf[3])
		{
			case COMM_POLL:
				opotocol_zyjn_cmd_poll();
				pctrans_recvlen = 0;
				break;
			case COMM_RESPONSEOK:
				opotocol_zyjn_responseok();
				pctrans_recvlen = 0;
				break;
			case COMM_LOSS_CARD: 
				opotocol_loss_card();
				pctrans_recvlen = 0;
				break;	
			case COMM_EX_SHIFI://换班
				
				opotocol_ex_shift();
				pctrans_recvlen = 0;
				break;	
			case COMM_RD_UID_REC:
				opotocol_rd_uid_rec();
				pctrans_recvlen = 0;
				break;	
			default:
				pctrans_recvlen = 0;
				break;	
		}
	}	
	pctrans_recvlen = 0;
	//pctrans_flag = 0;
	//memset((unsigned char *)pctrans_recvbuf,0,64);
	if(cur_lcng_r_rec.brunCNG == LCNGSTATE_RUNNING) return; 
       RS485_SET_RXIRPT();
     //otimer_t0_on();
}

void opotocol_zyjn_runstate_end(void)
{
	unsigned char n = 0;
	cur_lcng_r_rec.brunCNG = LCNGSTATE_RUN_STOP;
	for(n=0; n<10; n++) 
	{
		opotocol_zyjn_cmd_lcngrunstate();
	}
	RS485_SET_RXIRPT();	
}

void opotocol_zyjn_data_exchange(unsigned long recid,bool brdrec, bool bsave, bool bcard, unsigned char *rdbuf)
{
    _lcng_trans_rec  cur_lcng_t_rec;
    unsigned char    n = 0;
    unsigned long    addr = 0,id=0;
    if(true == brdrec)
    {
        
		ostore_rec_read(recid-1,NULL,128);
		
    }
    else
    {
        recid = cur_rec.rec_id;
    }
    if(cur_rec.rec_id == recid)
    {
        memset((unsigned char*)&cur_lcng_t_rec.UID[0], 0, sizeof(struct def_LCNG_TRANS_REC));
        //zl 2013-12-11
        cur_lcng_t_rec.UsrID = cur_sysinfo.shift_id;
        memset((char*)&cur_lcng_t_rec.CUID,0,6);
        ocomm_hex2bcdbuf(cur_sysinfo.shift_uid+1, (char*)&cur_lcng_t_rec.CUID,6);
        //
        ocomm_hex2bcdbuf(cur_rec.rec_id , (char*)&cur_lcng_t_rec.UID[0],6);
        ocomm_float2bcdbuf(cur_rec.rec_lcngprice, (char*)&cur_lcng_t_rec.RecPrice[0],2);    //单价

        ocomm_hex2bcdbuf(cur_rec.rec_lcngnum, (char*)&cur_lcng_t_rec.RecCNGNum[0],4);
        ocomm_hex2bcdbuf(cur_rec.rec_lcngsum, (char*)&cur_lcng_t_rec.RecCNGMoney[0],4);  //本次加气金额
        ocomm_hex2bcdbuf(cur_rec.rec_lcngnum, (char*)&cur_lcng_r_rec.brunCNGNum[0],4);    //运行时加气气量
        ocomm_hex2bcdbuf(cur_rec.rec_lcngsum, (char*)&cur_lcng_r_rec.brunCNGCash[0],4);  //运行时加气金额
        //ocomm_float2bcdbuf(cur_rec.rec_lcngnum, (char*)&cur_lcng_t_rec.RecCNGNum[0],4 );   //本次加气气量
        //ocomm_float2bcdbuf(cur_rec.rec_lcngsum, (char*)&cur_lcng_t_rec.RecCNGMoney[0],4 ); //本次加气金额
        //ocomm_float2bcdbuf(cur_rec.rec_lcngnum, (char*)&cur_lcng_r_rec.brunCNGNum[0],4 );   //运行时加气气量
        //ocomm_float2bcdbuf(cur_rec.rec_lcngsum, (char*)&cur_lcng_r_rec.brunCNGCash[0],4 ); //运行时加气金额
        memcpy((unsigned char*)&cur_lcng_t_rec.RecStartAccCNG[0],cur_rec.rec_s_acclcng,6);
        memcpy((unsigned char*)&cur_lcng_t_rec.RecEndAccCNG[0],cur_rec.rec_e_acclcng,6);
        cur_lcng_t_rec.bRecPrint = cur_rec.rec_b_Print;
        if(true == bcard)
        {
            ocomm_hex2bcdbuf(cur_rec.rec_cardnum,(char*)&cur_lcng_t_rec.RecICNum[0],4);               //IC卡卡号
            //ocomm_float2bcdbuf(cur_rec.rec_card_preM,(char*)&cur_lcng_t_rec.RecICPreMoney[0],4);      //加气前余额
            memcpy((unsigned char*)&cur_lcng_t_rec.RecICPreMoney[0],cur_rec.rec_card_preM,4);

			cur_lcng_t_rec.RecICType = ocomm_hex2bcd((unsigned char)cur_rec.rec_card_type);            //卡类型

			//ocomm_float2bcdbuf(cur_rec.rec_card_M,(char*)&cur_lcng_t_rec.RecICCurMoney[0],4);			//本次加气后余额
			memcpy((unsigned char*)&cur_lcng_t_rec.RecICCurMoney[0],cur_rec.rec_card_M,4);

			ocomm_hex2bcdbuf(cur_rec.rec_card_accftimes,(char*)&cur_lcng_t_rec.RecICAccFTimes[1],2);	//累计充值次数
			cur_lcng_t_rec.RecICAccFTimes[0] = 0;
			//ocomm_float2bcdbuf(cur_rec.rec_card_accfM,(char*)&cur_lcng_t_rec.RecICAccFMoney[0],6);		//累计充值金额
			memcpy((unsigned char*)&cur_lcng_t_rec.RecICAccFMoney[0],cur_rec.rec_card_accfM,6);

			ocomm_hex2bcdbuf(cur_rec.rec_card_connum,(char*)&cur_lcng_t_rec.RecICAccConNum[1],2);		//累计消费次数
			cur_lcng_t_rec.RecICAccConNum[0] = 0;
			//ocomm_float2bcdbuf(cur_rec.rec_card_accConlcng,(char*)&cur_lcng_t_rec.RecICAccConCNG[0],6);//累计消费气量
			memcpy((unsigned char*)&cur_lcng_t_rec.RecICAccConCNG[0],cur_rec.rec_card_accConlcng,6);

			//ocomm_float2bcdbuf(cur_rec.rec_card_accConM,(char*)&cur_lcng_t_rec.RecICAccConMoney[0],6);
			memcpy((unsigned char*)&cur_lcng_t_rec.RecICAccConMoney[0],cur_rec.rec_card_accConM,6);
			//ocomm_float2bcdbuf(cur_rec.rec_card_PreOverage,(char*)&cur_lcng_t_rec.RecICPreOverage[0],4);
			memcpy((unsigned char*)&cur_lcng_t_rec.RecICPreOverage[0],cur_rec.rec_card_PreOverage,4);

			//ocomm_float2bcdbuf(cur_rec.rec_card_EndOverage,(char*)&cur_lcng_t_rec.RecICEndOverage[0],4);
			memcpy((unsigned char*)&cur_lcng_t_rec.RecICEndOverage[0],cur_rec.rec_card_EndOverage,4);

			ocomm_hex2bcdbuf(cur_rec.rec_CarNum,(char*)&cur_lcng_t_rec.RecCarNum[0],2);	
			ocomm_float2bcdbuf(cur_rec.rec_s_p,(char*)&cur_lcng_t_rec.RecPrePressure[0],2);
			ocomm_float2bcdbuf(cur_rec.rec_e_p,(char*)&cur_lcng_t_rec.RecEndPressure[0],2);

			//ocomm_float2bcdbuf(cur_rec.rec_s_acclcng,(char*)&cur_lcng_t_rec.RecStartAccCNG[0],6);
			//ocomm_float2bcdbuf(cur_rec.rec_e_acclcng,(char*)&cur_lcng_t_rec.RecEndAccCNG[0],6);

			cur_lcng_t_rec.lstCardType = 1;// rec_lstCardType = 1;   //上次为：加气
			cur_lcng_t_rec.bRecCardOK = cur_rec.rec_b_CardOK;//0; 
		}


		//zl add 2013-8-21
		cur_lcng_t_rec.CalutUnit = ocomm_hex2bcd(0x01);
		cur_lcng_t_rec.MechType = ocomm_hex2bcd(0x01);
		cur_lcng_t_rec.StationNum = cur_sysparas.b_basic_p[4];
		
		for(n=0; n<6; n++)
			cur_lcng_t_rec.RecStartTime[n] = ocomm_hex2bcd(cur_rec.rec_stime[n]);
		for(n=0; n<6; n++)
			cur_lcng_t_rec.RecEndTime[n] = ocomm_hex2bcd(cur_rec.rec_etime[n]);	
		if(true == bsave)
		{
			id = (cur_rec.rec_id-1)%FlashRecMaxID;
            addr = FlashTRANSRecSADDR + id*FlashRecLen;
			oflash_wrbuf(addr,sizeof(struct def_LCNG_TRANS_REC),(unsigned char*)&cur_lcng_t_rec.UID[0]); 
			opotocol_zyjn_runstate_end();		
		}		
	}	
	if( true == brdrec && false == bsave && rdbuf !=NULL)
	{
		memcpy(rdbuf,(unsigned char*)&cur_lcng_t_rec.UID[0],sizeof(struct def_LCNG_TRANS_REC));
	}		
}

bool opotocol_zyjn_shiftrptdata_read(unsigned long id, unsigned char *rdbuf)
{
  unsigned long addr;
  bool          ret = true;
  ret = true; 
  //id = id%FLASH_RPT_MAX;  
  addr = FLASH_RPT_TRANS_S_ADDR + id*FLASH_RPT_LEN;  
  if(!oflash_readbuf(addr,sizeof(struct def_LCNG_SHIFT_TRANS_Rpt),rdbuf))
  {
	  ret = false; 
  } 
  return ret;  	
}

bool opotocol_zyjn_recdata_read(unsigned long id, unsigned char *rdbuf)
{
  unsigned long addr;
  bool          ret = true;
  ret = true; 
  id = id%FlashRecMaxID;  
  addr = FlashTRANSRecSADDR + id*FlashRecLen;  
  if(!oflash_readbuf(addr,sizeof(struct def_LCNG_TRANS_REC),rdbuf))
  {
	  ret = false; 
  } 
  return ret;  	
}
//加气前信息
void opotocol_get_card_info(void)
{
	ocomm_hex2bcdbuf(cur_rec.rec_cardnum,(char*)&cur_lcng_r_rec.runRFCardNum[0],4);
	memcpy((unsigned char*)&cur_lcng_r_rec.runRFCardPreM[0],cur_rec.rec_card_preM,4);
	cur_lcng_r_rec.runRFCardType = ocomm_hex2bcd((unsigned char)cur_rec.rec_card_type);
}
