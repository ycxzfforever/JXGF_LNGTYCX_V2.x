/*
 * key.h
 *
 * Created: 2012-8-6 16:57:42
 *  Author: luom
 */ 


#ifndef KEY_H_
#define KEY_H_


  #define KEY_0_MAP    0x00001000
  #define KEY_1_MAP    0x00000001
  #define KEY_2_MAP    0x00000002
  #define KEY_3_MAP    0x00000004
  #define KEY_4_MAP    0x00000010
  #define KEY_5_MAP    0x00000020
  #define KEY_6_MAP    0x00000040
  #define KEY_7_MAP    0x00010000
  #define KEY_8_MAP    0x00020000
  #define KEY_9_MAP    0x00040000
    
  #define KEY_QRY_MAP  0x00000008
  #define KEY_SET_MAP  0x00000080
  #define KEY_FIX_MAP  0x00080000 
  #define KEY_BACK_MAP 0x00004000   
  #define KEY_CLR_MAP  0x00008000   
  #define KEY_CNG_MAP  0x00000100 
  #define KEY_STOP_MAP 0x00000200   
  #define KEY_OK_MAP   0x00000400   
  #define KEY_RET_MAP  0x00000800     
  #define KEY_PT_MAP   0x00002000  
  
  void okey_portinit(void);
  //bool okey_wait_timeout(unsigned char nKey, unsigned int timeout_ms/*ms*/);
 
#endif /* KEY_H_ */