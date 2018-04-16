/*
 * store.h
 *
 * Created: 2012-8-15 17:25:28
 *  Author: luom
 */ 


#ifndef STORE_H_
#define STORE_H_

typedef struct def_LCNG_Record
{
  unsigned long  rec_id;             //UID������¼ǹ��ˮ��
  unsigned char  rec_shift_id;       //���
  unsigned long  rec_cuid;           //UID���¼ǹ��ˮ��
  unsigned char  rec_stime[6];       //������ʼʱ��
  unsigned char  rec_etime[6];       //��������ʱ��
  unsigned long  rec_cardnum;        //IC����
  unsigned char  rec_card_type;      //����

  unsigned char rec_card_preM[4];      //����ǰ���
  //unsigned long         rec_lcngnum;        //���μ�����
  long         rec_lcngnum;        //���μ�����
  float          rec_lcngprice;	     //���μ�������	
  //unsigned long		 rec_lcngsum;        //���μӽ��  
  long		 rec_lcngsum;        //���μӽ��  
  unsigned char  rec_card_M[4];         //���μ��������

  unsigned int   rec_card_accftimes; //�ۼƳ�ֵ����
  unsigned char  rec_card_accfM[6];     //�ۼƳ�ֵ���
  unsigned int   rec_card_connum;    //�ۼ����Ѵ��� *	
  unsigned char rec_card_accConlcng[6];//�ۼ��������� *	
  unsigned char rec_card_accConM[6]; //�ۼ����ѽ�� *
  unsigned char  rec_card_PreOverage[4];//�ڳ���� RecICPreOverage
  unsigned char  rec_card_EndOverage[4];//������� RecICEndOverage  	
  unsigned int   rec_CarNum;         //���ź�4λ
  
  float          rec_s_p;            //����ǰѹ��
  float          rec_e_p;            //������ѹ��

  unsigned char  rec_s_acclcng[6];      //����ǰǹ��
  unsigned char  rec_e_acclcng[6];      //������ǹ��

  bool           rec_b_CardOK;       //�Ƿ�ҿ���
  bool			 rec_b_Print;        //�Ƿ��ӡ�� //�Ƿ��ϴ��ˣ�
  bool           rec_lstCardType;    //bRecTransOK;//���ϴμ�¼(��Ϊ������ֵ)
  long           rec_card_num;
  unsigned char  rec_type;
  unsigned char  rec_crc_hi;
  unsigned char  rec_crc_low;
 //unsigned long rec_relcngnum;
 //unsigned long rec_relcngsum;
 long rec_relcngnum;
 long rec_relcngsum;
 long rec_back_num;////������
 unsigned long rec_alllcng_num;/////��Һ��
}_lcng_rec;//��ˮ��¼


typedef struct def_LCNG_shift_rpt
{
  unsigned long   shift_rpt_id;            //����ˮ��
  unsigned char  shift_id;                //���
  unsigned char  shift_rpt_stime[6];      //��ʼʱ��
  unsigned char  shift_rpt_etime[6];      //����ʱ��
  unsigned char	shift_rpt_s_lcng[4];   //��ʼǹ��
  unsigned char 	shift_rpt_e_lcng[4];  //����ǹ��
  unsigned char	shift_total_rpt_lcng[4];  //�ϼƼ���������ж������=�����ǹ��-�࿪ʼǹ��
  unsigned char 	shift_total_rec_lcngnum[4]; //�ϼƼ�����������ˮ��¼�ϼƼ��㣩
  unsigned  char	 shift_total_rec_lcngsum[4]; //�ϼƼ���������ˮ��¼�ϼƼ��㣩
  unsigned  char	 shift_rpt_total_lcngcash[4];//�ϼ�ʵ���ֽ��ֽ𿨣���ֵ��
  unsigned int	 shift_rec_num;           //�ϼƼ�����������С����
  unsigned int	 shift_prt_crc;
}_lcng_shiift_rpt;//���ۼ�¼

ex _lcng_rec		cur_rec;

extern bool ostore_rec_write(void);
extern void ostore_sysinfo_save(void) ;//float num, float sum);
extern bool ostore_rec_read(unsigned long id, unsigned char* rdbuf, unsigned char len);
extern bool ostore_rpt_read(unsigned long id, unsigned char* rdbuf);
extern bool ostore_shift_rpt_save(unsigned char newshiftid);
extern bool ostore_losscard_save(unsigned long lc_id);
extern bool ostore_losscard_chk( unsigned long lc_id);
#endif /* STORE_H_ */
