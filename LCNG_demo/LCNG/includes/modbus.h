/*
 * modbus.h
 *
 * Created: 2012-8-13 15:11:20
 *  Author: luom
 */ 


#ifndef MODBUS_H_
#define MODBUS_H_

   enum _MODBUS_SW_DEF
  {
	_MODBUS_G_ = 1, //∆¯œ‡
	_MODBUS_L_,     //“∫œ‡
  };

  #define  REG_TEMP_EH				2016
  #define  REG_TEMP_RM				250		


  #define  MODBUS_NONE				0
  #define  MODBUS_G_RDREG			1
  #define  MODBUS_G_WTREG			2
  #define  MODBUS_L_RDREG			3
  #define  MODBUS_L_WTREG			4
  
  #define  MODBUS_OK				1
  #define  MODBUS_ERR_TIMEOUT		2
  #define  MODBUS_ERR_CRC			3
  #define  MODBUS_ERR				4

  #define MODBUS_G_CLR_RXIRPT()   CLR_PORT_BIT(UCSR3B, RXCIE3)  
  #define MODBUS_G_SET_RXIRPT()   SET_PORT_BIT(UCSR3B, RXCIE3)

  #define MODBUS_L_CLR_RXIRPT()   CLR_PORT_BIT(UCSR1B, RXCIE1)  
  #define MODBUS_L_SET_RXIRPT()   SET_PORT_BIT(UCSR1B, RXCIE1)


  #define MODBUS_TIMEOUT_MAX		100  //ms
  #define DRVGAIN_CHKLEN			50
  
  
  #define MODBUS_MAX_RESPONSE_LEN			32
  
  #define AUTO_SCAN_LCNG_MASSFLOW		0
  #define AUTO_SCAN_LCNG_TOTALIZE		1
  #define AUTO_SCAN_LCNG_TEMPERATURE	2
  #define AUTO_SCAN_LCNG_DENSTY			3
  #define AUTO_SCAN_LCNG_STATE			4
  #define AUTO_SCAN_LCNG_FLOWCUT		5
  
  #define AUTO_SCAN_LCNG_DRVGAIN		AUTO_SCAN_LCNG_DENSTY
  
  typedef union def_unfloat_
  {  
		float          dt_float;
		unsigned char  dt_uc[4];
  }_union_float;

  ex  volatile unsigned char _modbus_g_buf[MODBUS_MAX_RESPONSE_LEN];//
  ex  volatile unsigned char _modbus_response_g_len;  
  ex  volatile unsigned char _modbus_l_buf[MODBUS_MAX_RESPONSE_LEN];
  ex  volatile unsigned char _modbus_response_l_len;
  ex  unsigned long			 modbus_err_rd_cnt;  
  ex  unsigned long          modbus_chk_cnt ;
  void omodbus_init(unsigned int baudrate);
  bool omobus_init_reg(void);
 bool omobus_init_chk(unsigned char ntype, unsigned char mtype);
  bool omodbus_clrtotalreg(unsigned char sw);
  bool omodbus_read_totalreg(unsigned char sw, float *vfrd);
  bool omodbus_read_flowrate(unsigned char sw, float* vfrd);
  bool omodbus_read_drvgain(unsigned char sw, float* vfrd);
  //bool omodbus_read_l_temp(float* vfrd);
  bool omodbus_read_temp(unsigned char sw, float* vfrd);
  bool omodbus_read_state(unsigned char sw, unsigned int* vrd);
  //unsigned char omodbus_write_reg_vavle(unsigned char sw ,unsigned int reg_id, unsigned int v_value);
  unsigned char omodbus_write_reg_vavle(unsigned char sw ,unsigned int reg_id, unsigned char regcoil ,unsigned int v_value);
  bool omodbus_read_float(unsigned char sw, unsigned int reg, float* vfrd);
  bool omodbus_write_float(unsigned char sw, unsigned int reg, float vfrd);
  unsigned char omodbus_read_reg(unsigned char sw ,unsigned int reg_id, unsigned char regnum );
  bool omodbus_read_int(unsigned char sw, unsigned int reg, unsigned int* vrd);
  bool omodbus_read_auto_buffer(unsigned char sw, unsigned char regnum);
  bool omodbus_write_multi_reg(unsigned char sw, unsigned int regaddr, unsigned int reg);
  float omodbus_read_auto_float_value(unsigned char sw,unsigned char id);
  unsigned int omodbus_read_auto_uint_value(unsigned char sw,unsigned char id);

  void omodbus_l_test(void);
  void omodbus_g_test(void);
#endif /* MODBUS_H_ */