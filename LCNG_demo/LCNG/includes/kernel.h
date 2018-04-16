/*
 * kernel.h
 *
 * Created: 2012-8-6 13:39:49
 *  Author: luom
 */ 


#ifndef KERNEL_H_
#define KERNEL_H_


	enum  EVENT_SYS_ID{
		SYS_NOEVENT = 0,
		KEY_0_EV,  
		KEY_1_EV,
		KEY_2_EV,
		KEY_3_EV,
		KEY_4_EV,
		KEY_5_EV,
		KEY_6_EV,
		KEY_7_EV,
		KEY_8_EV,
		KEY_9_EV,
		KEY_LCNG_EV,
		KEY_STOP_EV,
		KEY_PT_EV,
		KEY_OK_EV,
		KEY_BACK_EV,
		KEY_RET_EV,
		KEY_CLR_EV,
		KEY_FIX_EV,
		KEY_SET_EV,
		KEY_QRY_EV,

		KEY_END,
	//key input
		KEY_INPUT_EV,
		KEY_INPUT_RET_EV,
	//ui
		LCNG_UIFIXM_EV,
		LCNG_UIFIXC_EV,
		LCNG_MAINUI_EV,	
		LCNG_UITICK_EV,
		LCNG_UITIME_EV,
	//set
		LCNG_PARAS_SET_EV,	
		LCNG_FAC_SET,	
	//lcng ctrl
	    LCNGCTRL_START_EV,
		LCNGCTRL_DISP_EV,
		LCNGCTRL_FIXMCHK_EV,	
	//communication event
		LCNGCTRL_COMM_READY_EV,	
		LCNGCTRL_COMM_TIMEOUT_EV,
	//comm event
		LCNG_COMM_EVENT1_EV,
		LCNG_COMM_EVENT2_EV,
		LCNG_COMM_EVENT3_EV,
		LCNG_COMM_EVENT4_EV,
		LCNG_COMM_EVENT5_EV,
		LCNG_COMM_EVENT6_EV,			
		MAX_EV_ID
	};        
	
	#define KEY_UP_EV         KEY_6_EV
	#define KEY_DOWN_EV       KEY_9_EV
	#define KEY_PRECOOL_EV    KEY_BACK_EV
	/*
	typedef struct UI_CurSorDef{
		unsigned char x;
		unsigned char y;
		unsigned char eable;
		bool ctrl;
	}CurSor;
    */


	typedef struct UI_CurSorDef{
		//unsigned char *str_title;
		unsigned char *str_input;
		//unsigned char t_x;
		//unsigned char t_y;
		unsigned char p_x;
		unsigned char p_y;
		unsigned char pos;
		unsigned char max;
		bool          b_psw;
		bool          b_pt;
		bool		  ctrl;
	}CurSor;

    ex struct UI_CurSorDef  cur_input_cursor;

  
   //ex volatile unsigned char   Os_Enter_Sum;
   ex volatile bool			   bkeyscan;
   ex volatile bool            bkeydown;
   //ex volatile bool          btick;
  
   ex volatile unsigned int    tick_10ms;
   ex volatile unsigned int    tick_100ms;
   ex volatile unsigned int    tick_sec;   
   
   #define OS_ENTER_CRITICAL()     //cli(); //  {cli(); Os_Enter_Sum++;}
   #define OS_EXIT_CRITICAL()      //sei(); //  {if(--Os_Enter_Sum==0) sei();}
  
  
   //#define OS_DISABLE_KEYSCAN()     {otimer_t0_off();otimer_t1_off();otimer_t2_off();} //{otimer_t0_off();otimer_t1_off();otimer_t2_off();RS485_CLR_RXIRPT();}//{otimer_t0_off();RS485_CLR_RXIRPT();}
//	#define OS_DISABLE_KEYSCAN()     {otimer_t0_off();otimer_t1_off();otimer_t2_off();}//RS485_CLR_RXIRPT();} //{otimer_t0_off();otimer_t1_off();otimer_t2_off();RS485_CLR_RXIRPT();}//{otimer_t0_off();RS485_CLR_RXIRPT();}
   #define OS_DISABLE_KEYSCAN()     {otimer_t0_off();}
   //#define OS_ENABLE_KEYSCAN()      {otimer_t0_on();} // {otimer_t0_on(); RS485_SET_RXIRPT();}
    #define OS_ENABLE_KEYSCAN()      {otimer_t0_on();}// RS485_SET_RXIRPT();} // {otimer_t0_on(); RS485_SET_RXIRPT();}	   
  
   #define OK              1
   #define ERROR           0
  
   #define EMPTY           0
   #define NORMAL          1
   #define FULL           -1
  
  #if 1
	typedef struct user_command {
		const char *name;
		void (*cmdfunc)(int argc, const char **);
		struct user_command *next_cmd;
	} user_command_t;
 #endif 
   typedef void  (*pFunc)(void);  
    
   typedef struct Task_Event_Struct  
   {
     unsigned char    EventID ;
     pFunc            Func;//void             (*Func)(void); 
   }Task_Event; 
  
   typedef struct Sys_Queue_Struct 
   {
    unsigned int   length           ;
    unsigned int   write_pointer    ;
    unsigned int   read_pointer     ;
    signed char    status           ;
    unsigned char  queue[32]        ;
   }Sys_Queue;

   //
   void odisp_menu_pro(prog_char ui_menu[][16], unsigned char maxid, unsigned char curid);
   void odisp_menu_pro_ex(prog_char ui_menu[][16], unsigned char maxid, unsigned char curid, unsigned char showid);
   unsigned char odisp_menu_getid(void);
   
   unsigned char otimer_reqire (unsigned char  EventID, unsigned int  Interval, unsigned char  Mode, unsigned char  Prior);
   unsigned char otimer_release (unsigned char  EventID);
   unsigned char otimer_release_all (void);
   //void			 otimer_t1_tickclr(void);
   //unsigned long otimer_t1_tickget(void);
   
   void			 osys_portinit(void);
   void			 osys_init(void);
   void			 osys_powerOn();
   void			 okernel_addeventpro(unsigned char id, void (*cmd_func)(void));
   void			 okernel_seteventpro(Task_Event * EventTable, unsigned char  Lenth);
   void			 okernel_modifyeventpro(Task_Event *EventTable, unsigned char  id, void (*cmd_func)(void)) ;
   //void			 okernel_modifyeventpro(Task_Event *EventTable, unsigned char  id, void (*cmd_func)(void));
   void			 okernal_init(void);
   void			 okernel_clreventpro(void);
   pFunc		 okernel_geteventpro(unsigned char  EventID);
   unsigned char okernel_geteventid(void);
   unsigned char okernel_putevent(unsigned char EventID, unsigned char  Privior);
   unsigned char okernel_getkey(void);
  
   void otimer_t0_init(void);
   void otimer_t1_init(void);
   void otimer_t2_init(void);
   void otimer_t0_on(void);
   void otimer_t0_off(void);
   void otimer_t1_on(void);
   void otimer_t1_off(void);
   void otimer_t2_on(void);
   void otimer_t2_off(void);
   /*
   ex user_command_t *head_cmd;
   ex user_command_t *tail_cmd;
   ex user_command_t *running_cmd;
   */
   struct user_command *head_cmd;
   struct user_command *tail_cmd;
   struct user_command *running_cmd;
   
   extern Task_Event  _OCOMM_KEYINPUT_Pro[12];

#endif /* KERNEL_H_ */