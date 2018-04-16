/*
 * flash.h
 *
 * Created: 2012-8-10 9:46:08
 *  Author: luom
 */ 


#ifndef FLASH_H_
#define FLASH_H_


#define FLASHCHIPADDR                     0x8000

#define OFlash_Enable()                 
#define OFlash_Disable()   

#define INTEL_MANUFACT 		          	  0x89
#define INTEL_ID_28F128J3A 		  		  0x18
  
#define FLASH_BUSY                        false
#define FLASH_READY                       true
  
#define CMD_RDID                          0x90
  
#define ERR_PROTECT                       0x01
#define ERR_BUSY                          0x02
  
#define OFLASH_DLY_CYCLES                 20
  
#define OFLASH_BLOCKS                     128

#define IODATA *(volatile unsigned char*)

typedef union 
{ 
     unsigned int TVar; 
     struct { 
                unsigned char TVar_Bit0:1, 
                              TVar_Bit1:1, 
                              TVar_Bit2:1, 
                              TVar_Bit3:1, 
                              TVar_Bit4:1, 
                              TVar_Bit5:1, 
                              TVar_Bit6:1, 
                              TVar_Bit7:1, 
                              
                              TVar_Bit8:1, 
                              TVar_Bit9:1, 
                              TVar_Bit10:1, 
                              TVar_Bit11:1, 
                              TVar_Bit12:1, 
                              TVar_Bit13:1, 
                              TVar_Bit14:1, 
                              TVar_Bit15:1;                               
     }; 
}var_Bitctrl;

void oflash_init(void);
bool oflash_checkid(void);	
bool oflash_eraseall(unsigned char blocks);	
bool oflash_erase_blocks(unsigned char blocks);
bool oflash_wrbuf(unsigned long wtAddr, unsigned long Cnt, unsigned char *buf);
bool oflash_readbuf(unsigned long rdAddr, unsigned long Cnt, unsigned char *buf);
#endif /* FLASH_H_ */