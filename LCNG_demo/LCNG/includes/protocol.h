/*
 * protocol.h
 *
 * Created: 2012-8-14 16:17:51
 *  Author: luom
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define LCNGSTATE_RUNNING		2
#define LCNGSTATE_HANSHAKING	1
#define LCNGSTATE_RUN_STOP		0

#define RS485TAIL_H   	        0xDD//终止符
#define RS485TAIL_L   	        0xEE//终止符

#define PROCOTOL_RS485_EVENT_TIME		10
//////中油节能协议需求//////////////////
/////////page 0~3:存放挂失卡  每个站32K空间*64个站
#define  FLASH_L_CARD_SADDR		0x00000     
#define  FLASH_L_ZMAX_LEN		0x1000
#define  FLASH_L_CARD_LEN		4
#define  FLASH_L_CARD_MAX		4000  //4096
#define  FLASH_L_CARD_MAX_Z		31
 
/////////page 4~6:存放班累
#define  FLASH_RPT_S_ADDR		0x80000     //128*1024*4  //pages: 4\5\6
#define  FLASH_RPT_LEN			64
#define  FLASH_RPT_MAX			6000        //pages * 2048
/////////page 7~9:存放班累传输数据
#define  FLASH_RPT_TRANS_S_ADDR	0xE0000		//128*1024*7  //pages: 7\8\9


/////////Page 10~49:存放流水索引   0x140000~0x640000 流水存储第二区
/////////存放流水记录   每条记录按128字节存放，每一页存1024条
#define  FlashRecSADDR		0x140000 //128*1024*10   //  //#define  FlashRecEADDR	0x640000 //128*1024*50-1
#define  FlashRecLen		128
#define  FlashRecMaxID		40000    //51200
/////////Page 50~99:存放传输数据   0x640000~0xB40000 流水存储第一区
#define  FlashTRANSRecSADDR	0x640000   //128*1024*50  //#define  FlashRecEADDR	0x640000 //128*1024*50-1
#define  FlashTRANSRecLen	128
#define  FlashTRANSRecMaxID	40000   //51200

//实时周期巡检
#define COMM_POLL			0x51  
//发送挂失卡信息
#define COMM_LOSS_CARD		0x54  
 //发送换班信息
#define COMM_EX_SHIFI		0x55 
//读取指定UID流水记录
#define COMM_RD_UID_REC		0x67  //读取指定UID流水记录
//应答
#define COMM_RESPONSEOK		0x69  

#define PROTOCOL_REC_LEN	101


#define CMD_UPLDREC			0x11//流水记录
#define CMD_UPLDRPT			0x12//班累记录
#define CMD_CNGIDLE			0x19//工作状态  
#define CMD_RECVERR			0x50
/* 发送加气流水记录命令11H(在待机阶段、加气结束阶段)
1	6(BCD)	UID加气记录枪流水号
2	1(BCD)	班号（1,2,3,4）
3	6(BCD)	UID班记录枪流水号
4	6(BCD)	加气开始时间
5	6(BCD)	加气结束时间
6	4(BCD)	IC卡卡号
7	1(BCD)	IC卡类型
8	4(BCD)	加气前余额=IC卡上加气前余额
9	4(BCD)	本次加气气量
10	2(BCD)	本次加气单价
11	4(BCD)	本次加气金额
12	4(BCD)	加气后余额＝加气前余额＋本次加气金额
13	3(BCD)	累计充值次数=IC卡上上次累计充值次数
14	6(BCD)	累计充值金额=IC卡上上次累计充值金额
15	3(BCD)	累计消费次数=IC卡上累计消费次数＋1
16	6(BCD)	累计消费气量=IC卡上累计消费气量＋本次加气气量
17	6(BCD)	累计消费金额=IC卡上累计消费金额＋本次加气金额
18	4(BCD)	期初余额＝IC卡上期初余额
19	4(BCD)	计算余额=初期余额+充值总累-消费金额总累
20	2(BCD)	车号后三位
21	2(BCD)	加气前压力
22	2(BCD)	加气后压力
23	6(BCD)	加气前枪累(最大9999999999.99)
24	6(BCD)	加气后枪累(最大9999999999.99)
25	1(BCD)	是否灰卡了（0否/1是）
26	1(BCD)	是否打印了（0否/1是）
27	1(BCD)	是否上传了（0否/1是）
合计	101	
*/

enum {
  NOUPLD = 0, 
  UPLDREC, 
  UPLDRPT, 
  UPLDWARN
};


void opotocol_get_card_info(void);
void opotocol_zyjn_analysis(void);
//void opotocol_zyjn_data_exchange(void);
void opotocol_zyjn_data_exchange(unsigned long recid,bool brdrec, bool bsave, bool bcard, unsigned char *rdbuf);
bool opotocol_zyjn_recdata_read(unsigned long id, unsigned char *rdbuf);
bool opotocol_zyjn_shiftrptdata_read(unsigned long id, unsigned char *rdbuf);
void opotocol_zyjn_upload_shiftrpt(void);
void opotocol_zyjn_lcngrunstate_pro(void);

/*
@发送工作状态命令
@opotocol_zyjn_cmd_lcngrunstate
*/
void opotocol_zyjn_cmd_lcngrunstate(void);
void opotocol_zyjn_cmd_poll(void);
void opotocol_zyjn_responseok(void);

void opotocol_loss_card(void);
void opotocol_ex_shift(void);
void opotocol_rd_uid_rec(void);


#endif /* PROTOCOL_H_ */