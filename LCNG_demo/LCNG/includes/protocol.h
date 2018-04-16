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

#define RS485TAIL_H   	        0xDD//��ֹ��
#define RS485TAIL_L   	        0xEE//��ֹ��

#define PROCOTOL_RS485_EVENT_TIME		10
//////���ͽ���Э������//////////////////
/////////page 0~3:��Ź�ʧ��  ÿ��վ32K�ռ�*64��վ
#define  FLASH_L_CARD_SADDR		0x00000     
#define  FLASH_L_ZMAX_LEN		0x1000
#define  FLASH_L_CARD_LEN		4
#define  FLASH_L_CARD_MAX		4000  //4096
#define  FLASH_L_CARD_MAX_Z		31
 
/////////page 4~6:��Ű���
#define  FLASH_RPT_S_ADDR		0x80000     //128*1024*4  //pages: 4\5\6
#define  FLASH_RPT_LEN			64
#define  FLASH_RPT_MAX			6000        //pages * 2048
/////////page 7~9:��Ű��۴�������
#define  FLASH_RPT_TRANS_S_ADDR	0xE0000		//128*1024*7  //pages: 7\8\9


/////////Page 10~49:�����ˮ����   0x140000~0x640000 ��ˮ�洢�ڶ���
/////////�����ˮ��¼   ÿ����¼��128�ֽڴ�ţ�ÿһҳ��1024��
#define  FlashRecSADDR		0x140000 //128*1024*10   //  //#define  FlashRecEADDR	0x640000 //128*1024*50-1
#define  FlashRecLen		128
#define  FlashRecMaxID		40000    //51200
/////////Page 50~99:��Ŵ�������   0x640000~0xB40000 ��ˮ�洢��һ��
#define  FlashTRANSRecSADDR	0x640000   //128*1024*50  //#define  FlashRecEADDR	0x640000 //128*1024*50-1
#define  FlashTRANSRecLen	128
#define  FlashTRANSRecMaxID	40000   //51200

//ʵʱ����Ѳ��
#define COMM_POLL			0x51  
//���͹�ʧ����Ϣ
#define COMM_LOSS_CARD		0x54  
 //���ͻ�����Ϣ
#define COMM_EX_SHIFI		0x55 
//��ȡָ��UID��ˮ��¼
#define COMM_RD_UID_REC		0x67  //��ȡָ��UID��ˮ��¼
//Ӧ��
#define COMM_RESPONSEOK		0x69  

#define PROTOCOL_REC_LEN	101


#define CMD_UPLDREC			0x11//��ˮ��¼
#define CMD_UPLDRPT			0x12//���ۼ�¼
#define CMD_CNGIDLE			0x19//����״̬  
#define CMD_RECVERR			0x50
/* ���ͼ�����ˮ��¼����11H(�ڴ����׶Ρ����������׶�)
1	6(BCD)	UID������¼ǹ��ˮ��
2	1(BCD)	��ţ�1,2,3,4��
3	6(BCD)	UID���¼ǹ��ˮ��
4	6(BCD)	������ʼʱ��
5	6(BCD)	��������ʱ��
6	4(BCD)	IC������
7	1(BCD)	IC������
8	4(BCD)	����ǰ���=IC���ϼ���ǰ���
9	4(BCD)	���μ�������
10	2(BCD)	���μ�������
11	4(BCD)	���μ������
12	4(BCD)	������������ǰ�����μ������
13	3(BCD)	�ۼƳ�ֵ����=IC�����ϴ��ۼƳ�ֵ����
14	6(BCD)	�ۼƳ�ֵ���=IC�����ϴ��ۼƳ�ֵ���
15	3(BCD)	�ۼ����Ѵ���=IC�����ۼ����Ѵ�����1
16	6(BCD)	�ۼ���������=IC�����ۼ��������������μ�������
17	6(BCD)	�ۼ����ѽ��=IC�����ۼ����ѽ����μ������
18	4(BCD)	�ڳ���IC�����ڳ����
19	4(BCD)	�������=�������+��ֵ����-���ѽ������
20	2(BCD)	���ź���λ
21	2(BCD)	����ǰѹ��
22	2(BCD)	������ѹ��
23	6(BCD)	����ǰǹ��(���9999999999.99)
24	6(BCD)	������ǹ��(���9999999999.99)
25	1(BCD)	�Ƿ�ҿ��ˣ�0��/1�ǣ�
26	1(BCD)	�Ƿ��ӡ�ˣ�0��/1�ǣ�
27	1(BCD)	�Ƿ��ϴ��ˣ�0��/1�ǣ�
�ϼ�	101	
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
@���͹���״̬����
@opotocol_zyjn_cmd_lcngrunstate
*/
void opotocol_zyjn_cmd_lcngrunstate(void);
void opotocol_zyjn_cmd_poll(void);
void opotocol_zyjn_responseok(void);

void opotocol_loss_card(void);
void opotocol_ex_shift(void);
void opotocol_rd_uid_rec(void);


#endif /* PROTOCOL_H_ */