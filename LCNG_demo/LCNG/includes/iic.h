/*
 * iic.h
 *
 * Created: 2012-8-6 16:33:02
 *  Author: luom
 */ 


#ifndef IIC_H_
#define IIC_H_


#define SLA_W_24CXXX	0xA0
#define SLA_R_24CXXX	0xA1

#define EEPROM_WP_OUT()  SET_PORT_BIT(DDRL,PL5)  //DDRL_Bit5  = 1
#define EEPROM_WP_L()    CLR_PORT_BIT(PORTL,PL5) //PORTL_Bit5 = 0//写保护置低
#define EEPROM_WP_H()    SET_PORT_BIT(PORTL,PL5) //PORTL_Bit5 = 1//写保护置高

#define TWI_SLA_DS_RT   0xD0
#define TWI_SLA_DS_SL   0xA2
#define TWI_SLA_DS_MB   0xA0


#define TW_START				0x08
#define TW_REP_START			0x10
#define TW_MT_SLA_ACK			0x18
#define TW_MT_SLA_NACK			0x20
#define TW_MT_DATA_ACK			0x28
#define TW_MR_SLA_ACK			0x40
#define TW_MR_DATA_ACK			0x50
#define TW_MR_DATA_NACK			0x58
#define TW_MT_DATA_NACK			0x30
#define TW_MT_ARB_LOST			0x38
#define TW_MR_ARB_LOST			0x38
#define TW_MR_SLA_NACK			0x48
#define TW_READ						1
#define TW_WRITE					0 
//#define TWI_SLA_DS        0xD0 //20110812
#define MAX_ITER				20
//#define TW_STATUS				(TWSR & 0xF8)
#define TWI_ACT_START()         TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
#define TWI_ACT_STOP()          TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)
#define TWI_ACT_WAIT()          while ((TWCR & (1<<TWINT)) == 0)


extern void oiic_init(void);
extern bool ds3231_read_date(unsigned char *buf, bool bhex);
extern bool ds3231_read_time(unsigned char *buf, bool bhex);
extern bool ds3231_set_time(unsigned char *buf);
extern bool ds3231_set_date(unsigned char *buf);
extern int  ee24cxxx_read_bytes(unsigned char Dvaddr,unsigned int addr, int len, unsigned char* buf);
extern int  ee24cxxx_write_bytes(unsigned char Dvaddr,unsigned int addr, int len, unsigned char* buf);
extern bool ee24lc_read(unsigned int addr, int len, unsigned char *buf);
extern bool ee24lc_write(unsigned int addr, int len, unsigned char *buf);
extern bool ds3231_hardware_chk(void);

#endif /* IIC_H_ */