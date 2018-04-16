/*
 * lcd.h
 *
 * Created: 2012-8-6 16:12:51
 *  Author: luom
 */ 


#ifndef LCD_H_
#define LCD_H_


	#define  LCMCMD			true
	#define  LCMDATA		false

	#define FYD_CS_PORT    PORTH
	#define FYD_CS_DDR	   DDRH
	#define FYD_CS_PIN	   PINH
	#define FYD_CS_BIT	   PH2

	#define FYD_SID_PORT   PORTE
	#define FYD_SID_DDR	   DDRE
	#define FYD_SID_PIN	   PINE
	#define FYD_SID_BIT	   PE6

	#define FYD_CLK_PORT   PORTE
	#define FYD_CLK_DDR	   DDRE
	#define FYD_CLK_PIN	   PINE
	#define FYD_CLK_BIT	   PE5


	#define OLcd_CS_H()    SET_PORT_BIT(FYD_CS_PORT,FYD_CS_BIT)//FYD_CS_PORT |= (1<<FYD_CS_BIT);loop_until_bit_is_set(FYD_CS_PORT, FYD_CS_BIT)
	#define OLcd_CS_L()    CLR_PORT_BIT(FYD_CS_PORT,FYD_CS_BIT)//FYD_CS_PORT &=~(1<<FYD_CS_BIT);loop_until_bit_is_clear(FYD_CS_PORT, FYD_CS_BIT)
	#define OLcd_CS_IN()   CLR_PORT_BIT(FYD_CS_DDR,FYD_CS_BIT)//FYD_CS_DDR  &=~(1<<FYD_CS_BIT);loop_until_bit_is_clear(FYD_CS_DDR, FYD_CS_BIT)
	#define OLcd_CS_OUT()  SET_PORT_BIT(FYD_CS_DDR,FYD_CS_BIT)//FYD_CS_DDR  |= (1<<FYD_CS_BIT);loop_until_bit_is_set(FYD_CS_DDR, FYD_CS_BIT)
  
	#define OLcd_SID_H()   SET_PORT_BIT(FYD_SID_PORT,FYD_SID_BIT)//FYD_SID_PORT |= (1<<FYD_SID_BIT);loop_until_bit_is_set(FYD_SID_PORT, FYD_SID_BIT)	
	#define OLcd_SID_L()   CLR_PORT_BIT(FYD_SID_PORT,FYD_SID_BIT)//FYD_SID_PORT &=~(1<<FYD_SID_BIT);loop_until_bit_is_clear(FYD_SID_PORT, FYD_SID_BIT)
	#define OLcd_SID_IN()  CLR_PORT_BIT(FYD_SID_DDR,FYD_SID_BIT)//FYD_SID_DDR  &=~(1<<FYD_SID_BIT);loop_until_bit_is_clear(FYD_SID_DDR, FYD_SID_BIT)
	#define OLcd_SID_OUT() SET_PORT_BIT(FYD_SID_DDR,FYD_SID_BIT)//FYD_SID_DDR  |= (1<<FYD_SID_BIT);loop_until_bit_is_set(FYD_SID_DDR, FYD_SID_BIT)
  
	#define OLcd_CLK_H()   SET_PORT_BIT(FYD_CLK_PORT,FYD_CLK_BIT)//FYD_CLK_PORT |= (1<<FYD_CLK_BIT);loop_until_bit_is_set(FYD_CLK_PORT, FYD_CLK_BIT)
	#define OLcd_CLK_L()   CLR_PORT_BIT(FYD_CLK_PORT,FYD_CLK_BIT)//FYD_CLK_PORT &=~(1<<FYD_CLK_BIT);loop_until_bit_is_clear(FYD_CLK_PORT, FYD_CLK_BIT)
	#define OLcd_CLK_IN()  CLR_PORT_BIT(FYD_CLK_DDR,FYD_CLK_BIT) //FYD_CLK_DDR  &=~(1<<FYD_CLK_BIT);loop_until_bit_is_clear(FYD_CLK_DDR, FYD_CLK_BIT)
	#define OLcd_CLK_OUT() SET_PORT_BIT(FYD_CLK_DDR,FYD_CLK_BIT) //FYD_CLK_DDR  |= (1<<FYD_CLK_BIT);loop_until_bit_is_set(FYD_CLK_DDR, FYD_CLK_BIT)

    void olcm_dispprogconst(unsigned char row, unsigned char col, prog_char* str, bool bClrLeft);
	void olcm_wrtstr(unsigned char row,unsigned char col,char *str, bool bClrLeft);
    void olcm_init(void);
	void olcm_clrscr(void);
	void olcm_narrowup(unsigned char px, unsigned char py);
	void olcm_narrowdown(unsigned char px, unsigned char py);
#endif /* LCD_H_ */