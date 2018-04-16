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
  unsigned long  rec_id;             //UID加气记录枪流水号
  unsigned char  rec_shift_id;       //班号
  unsigned long  rec_cuid;           //UID班记录枪流水号
  unsigned char  rec_stime[6];       //加气开始时间
  unsigned char  rec_etime[6];       //加气结束时间
  unsigned long  rec_cardnum;        //IC卡号
  unsigned char  rec_card_type;      //类型

  unsigned char rec_card_preM[4];      //加气前余额
  //unsigned long         rec_lcngnum;        //本次加气量
  long         rec_lcngnum;        //本次加气量
  float          rec_lcngprice;	     //本次加气单价	
  //unsigned long		 rec_lcngsum;        //本次加金额  
  long		 rec_lcngsum;        //本次加金额  
  unsigned char  rec_card_M[4];         //本次加气后余额

  unsigned int   rec_card_accftimes; //累计充值次数
  unsigned char  rec_card_accfM[6];     //累计充值金额
  unsigned int   rec_card_connum;    //累计消费次数 *	
  unsigned char rec_card_accConlcng[6];//累计消费气量 *	
  unsigned char rec_card_accConM[6]; //累计消费金额 *
  unsigned char  rec_card_PreOverage[4];//期初余额 RecICPreOverage
  unsigned char  rec_card_EndOverage[4];//计算余额 RecICEndOverage  	
  unsigned int   rec_CarNum;         //车号后4位
  
  float          rec_s_p;            //加气前压力
  float          rec_e_p;            //加气后压力

  unsigned char  rec_s_acclcng[6];      //加气前枪累
  unsigned char  rec_e_acclcng[6];      //加气后枪累

  bool           rec_b_CardOK;       //是否灰卡了
  bool			 rec_b_Print;        //是否打印了 //是否上传了？
  bool           rec_lstCardType;    //bRecTransOK;//卡上次记录(改为加气充值)
  long           rec_card_num;
  unsigned char  rec_type;
  unsigned char  rec_crc_hi;
  unsigned char  rec_crc_low;
 //unsigned long rec_relcngnum;
 //unsigned long rec_relcngsum;
 long rec_relcngnum;
 long rec_relcngsum;
 long rec_back_num;////回气量
 unsigned long rec_alllcng_num;/////加液量
}_lcng_rec;//流水记录


typedef struct def_LCNG_shift_rpt
{
  unsigned long   shift_rpt_id;            //班流水号
  unsigned char  shift_id;                //班号
  unsigned char  shift_rpt_stime[6];      //起始时间
  unsigned char  shift_rpt_etime[6];      //结束时间
  unsigned char	shift_rpt_s_lcng[4];   //开始枪累
  unsigned char 	shift_rpt_e_lcng[4];  //结束枪累
  unsigned char	shift_total_rpt_lcng[4];  //合计加气量（或卸气量）=班结束枪累-班开始枪累
  unsigned char 	shift_total_rec_lcngnum[4]; //合计加气量（按流水记录合计计算）
  unsigned  char	 shift_total_rec_lcngsum[4]; //合计加气金额（按流水记录合计计算）
  unsigned  char	 shift_rpt_total_lcngcash[4];//合计实收现金额（现金卡＋充值）
  unsigned int	 shift_rec_num;           //合计加气次数（无小数）
  unsigned int	 shift_prt_crc;
}_lcng_shiift_rpt;//班累记录

ex _lcng_rec		cur_rec;

extern bool ostore_rec_write(void);
extern void ostore_sysinfo_save(void) ;//float num, float sum);
extern bool ostore_rec_read(unsigned long id, unsigned char* rdbuf, unsigned char len);
extern bool ostore_rpt_read(unsigned long id, unsigned char* rdbuf);
extern bool ostore_shift_rpt_save(unsigned char newshiftid);
extern bool ostore_losscard_save(unsigned long lc_id);
extern bool ostore_losscard_chk( unsigned long lc_id);
#endif /* STORE_H_ */
