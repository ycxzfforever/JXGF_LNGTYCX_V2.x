/*
 * splc.c
 *
 * Created: 2012-8-10 11:13:40
 *  Author: luom
 */
#include "global_def.h"


prog_char SEGMAP_DISP[16]   = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0x86,0xFF,0xC6,0x00,0xBF};
/*************************************************************************
功能：控制SPLC的端口初始化函数
入口：
出口：
备注：
*************************************************************************/
void osplc_portinit(void)
{
    OSPLC_LCK_H();
    OSPLC_CLK_H();
    OSPLC_DATA_H();

    OSPLC_LCK_OUT();
    OSPLC_CLK_OUT();
    OSPLC_DATA_OUT();
}

/*************************************************************************
功能：控制SPLC的数据锁存函数
入口：
出口：
备注： 当将所有数据送入LED显示后，使用此函数进行显示数据的锁存
*************************************************************************/
void osplc_latch(void)
{
    OSPLC_LCK_H();
    __delay_cycles(100);//必须的延时，因为触发器输入输出延时受电源电压影响较大
    OSPLC_LCK_L();
    __delay_cycles(100);//必须的延时，因为触发器输入输出延时受电源电压影响较大
}

/*************************************************************************
功能：控制SPLC的一次时钟时能函数
入口：
出口：
备注： 注意本函数的硬件电路，控制的实际时序为从低电平拉高,但电路用了非门
*************************************************************************/
void osplc_enclk(void)
{
    OSPLC_CLK_H();
    __delay_cycles(100);//必须的延时，因为触发器输入输出延时受电源电压影响较大
    OSPLC_CLK_L();
    __delay_cycles(100);//必须的延时，因为触发器输入输出延时受电源电压影响较大
}

//////////////////////////////////////////////////////////////////中英新旧通用型
static unsigned char dot1,dot2,dot3;
void osplc_dot(unsigned char dcode)                     //写入小数点
{
    unsigned char i,temp;
    for(i=0; i<6; i++)
    {
        temp=(dcode<<i)&0x20;
        if(temp)
        {
            OSPLC_DATA_H();
        }
        else
        {
            OSPLC_DATA_L();
        }
        osplc_enclk();
    }
}

void osplc_disp_ex(unsigned char *pdisp,bool nPoint)                 //写入一帧数据
{

    unsigned char i, j, showdata;

    if(nPoint)     osplc_dot(0x2a);//小数点编码
    else           osplc_dot(0x00);//小数点全显
    for(i = 0; i < 22; i++) //22是一共22位数码管
    {
        showdata = pgm_read_byte(&SEGMAP_DISP[pdisp[i]]);
        for(j = 0; j < 7; j++) //7段
        {
            showdata = showdata << 1;//移位与
            if(showdata & 0x80) //80是全亮/灭 情况
            {
                OSPLC_DATA_H();//段亮灭
            }
            else
            {
                OSPLC_DATA_L();
            }
            osplc_enclk();//每一段使能一次时钟
        }
    }
    osplc_latch();//锁存
}


void osplc_show_ex(float Row1,float Row2,float Row3,bool bStop, bool bOverFlag)//刷新一次SPLC
{
    char buf[12] = {0};
    unsigned char dispbuf[22] = {0};
    unsigned char m = 0, n = 0, len = 0;
    OS_ENTER_CRITICAL();
    /*  if(bStop)
      {
        memset(dispbuf,14,sizeof(dispbuf));
        osplc_disp_ex(dispbuf,false);
        OS_EXIT_CRITICAL();
        return;
      }*/
    memset(dispbuf,12,sizeof(dispbuf));
    ///显示枪号
    dispbuf[5] = cur_sysparas.b_basic_p[BASIC_P_G_ID]/10;
    dispbuf[4] = cur_sysparas.b_basic_p[BASIC_P_G_ID]%10; //

    if(bOverFlag)
    {
        dispbuf[13]  = 10;
        dispbuf[21] =  13;
    }
    memset(buf,13,sizeof(buf));
    len = sprintf(buf,"%.2f",(double)Row1);
    len = (9>len) ? len : 9;
    m = len + 12;
    for(n=0; n<len; n++)
    {
        if(buf[n] == '.') continue;
        dispbuf[m] = buf[n]-0x30;
        m--;
    }
    if(bStop)
    {
        dispbuf[len +13] = 15;
    }
    memset(buf,0,sizeof(buf));
    len = sprintf(buf,"%.2f",(double)Row2);
    len = (9>len) ? len : 9 ;
    m = len + 4;
    for(n=0; n<len; n++)
    {
        if(buf[n] == '.') continue;
        dispbuf[m] = buf[n]-0x30;
        m--;
    }
    if(bStop)
    {
        dispbuf[len +5] = 15;
    }

    memset(buf,0,sizeof(buf));
    len = sprintf(buf,"%.2f",(double)Row3);
    len = (7>len) ? len:7;
    m = len - 2;
    for(n=0; n<len; n++)
    {
        if(buf[n] == '.') continue;
        dispbuf[m] = buf[n]-0x30;
        m--;
    }

    osplc_disp_ex(dispbuf,true);
    OS_EXIT_CRITICAL();
}
//pgm_read_byte( &_VCTRL_NUM_TYPE[n])


void osplc_disp(unsigned char *pdisp,bool nPoint)
{
    unsigned char len, m ,k ,n;
    unsigned char dt;
    //unsigned char SEGMAP[14] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0x86,0xFF,0xC6};
    OS_ENTER_CRITICAL();
    for(len=0; len<8; len++)
    {
        for(n=0; n<4; n++)
        {
            OSPLC_DATA_H();
            osplc_enclk();
        }
        for(m=0; m<2; m++)
        {
            k = 2*len+m;
            dt = pgm_read_byte(&SEGMAP_DISP[pdisp[k]]);//SEGMAP[pdisp[k]];
            //dt = SEGMAP[pdisp[k]];
            if(nPoint)
            {
                //if(k==2 || k== 8 || k == 14)
                if(k==dot1 || k== dot2 || k == dot3)
                {
                    dt &= 0x7F ;
                }
            }
            for(n=0; n<8; n++)
            {
                if(dt & 0x80)
                {
                    OSPLC_DATA_H();
                }
                else
                {
                    OSPLC_DATA_L();
                }
                osplc_enclk();
                dt = dt << 1 ;
            }
        }
    }
    osplc_latch();
    OS_EXIT_CRITICAL();
}

void osplc_show_null(void)
{
    unsigned char dispbuf[16] = {0};
    memset(dispbuf,12,sizeof(dispbuf));
    osplc_disp(dispbuf,false);
}

void osplc_show(float Row1,float Row2,float Row3,bool bStop, bool bOverFlag)
{
    char buf[12] = {0};             //存放当前行显示数据
    unsigned char dispbuf[16] = {0};//存放十六位显示数据
    unsigned char m = 0, n = 0, len = 0;
    if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_B)
    {
        osplc_show_ex(Row1, Row2 , Row3, bStop, bOverFlag);
        return;
    }
    if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_N)
    {
        osplc_show_new(Row1, Row2 , Row3);

        return;
    }
    /* if(bStop)
     {
       memset(dispbuf,10,sizeof(dispbuf));
       osplc_disp(dispbuf,false);
       return;
     }*/
    memset(dispbuf,12,sizeof(dispbuf));
    if(bOverFlag)
    {
        dispbuf[5]  = 13;
        dispbuf[11] = 10;
    }
//将要显示的数值送入dispbuf[16]
//////////////第一行
    memset(buf,0,sizeof(buf));
    len = sprintf(buf,"%.2f",(double)Row1);
    len = (7>len) ? len : 7;
    dot1=2;
    m = len-2;
    for(n=0; n<len; n++)
    {
        if(buf[n] == '.')
        {
            continue;
        }
        dispbuf[m] = buf[n]-0x30;
        m--;
    }
    if(bStop)
    {
        dispbuf[len-1] = 15;
    }
//////////////第二行
    memset(buf,0,sizeof(buf));
    if(Row2>=1000000) Row2=Row2/10;
    if(Row2>=100000) dot2=0;
    else if(Row2>=10000) dot2=7;
    else dot2=8;
    len = sprintf(buf,"%.2f",(double)Row2);
    len = (7>len) ? len : 7 ;
    //if(Row2>=100000)
    // len++;
    m = len + 4;
    for(n=0; n<len; n++)
    {
        if(buf[n] == '.')
        {
            continue;
        }
        dispbuf[m] = buf[n]-0x30;
        m--;
    }
    if(bStop)
    {
        dispbuf[len + 5] = 15;
    }
//////////////第三行
    memset(buf,0,sizeof(buf));
    if(Row3>=1000) dot3=0;
    else if(Row3>=100) dot3=13;
    else dot3=14;
    len = sprintf(buf,"%.2f",(double)Row3);
    len = (5>len) ? len:5;
    m = len + 10;
    for(n=0; n<len; n++)
    {
        if(buf[n] == '.')
        {
            continue;
        }
        dispbuf[m] = buf[n]-0x30;
        m--;
    }

    osplc_disp(dispbuf,true);
}
/******************************************************************************************
**
**  新版LED液晶屏驱动
**  2015年6月19日 09:22:16
**  LiS
**
********************************************************************************************/
static unsigned char disp_mem[35+5] = {0};

void odisp_mem_disp(void)                 //写入一帧数据
{

    unsigned char m, n, showdata;

    for(m = 0; m < 35+5; m++)
    {
        showdata = disp_mem[m];
        for(n = 0; n < 8; n++)
        {
            if(showdata & 0x80) //80是全灭情况
            {
                OSPLC_DATA_H();//段亮灭
            }
            else
            {
                OSPLC_DATA_L();
            }
            osplc_enclk();//每一段使能一次时钟
            showdata = showdata << 1;//移位与
        }
    }
    osplc_latch();//锁存
}
/*
#define ODISP_TITLE_LOOP_E1_ON()        disp_mem[22] &= 0xF7
#define ODISP_TITLE_LOOP_E2_ON()        disp_mem[22] &= 0xFB
#define ODISP_TITLE_LOOP_E3_ON()        disp_mem[22] &= 0xEF
#define ODISP_TITLE_LOOP_S11_ON()       disp_mem[17] &= 0xFD //S11

#define ODISP_TITLE_LOOP_E1_OFF()       disp_mem[22] |= (~0xF7)
#define ODISP_TITLE_LOOP_E2_OFF()       disp_mem[22] |= (~0xFB)
#define ODISP_TITLE_LOOP_E3_OFF()       disp_mem[22] |= (~0xEF)
#define ODISP_TITLE_LOOP_S11_OFF()      disp_mem[17] |= (~0xFD) //S11
*/
//气量
//#define ODISP_TITLE_NUM_ON()       disp_mem[22] &= 0xFE
//#define ODISP_TITLE_NUM_OFF()      disp_mem[22] |= 0x01
#define ODISP_TITLE_KG_ON()        disp_mem[29] &= 0xFB  //S4与S1共用
#define ODISP_TITLE_KG_OFF()       disp_mem[29] |= 0x04
//金额
//#define ODISP_TITLE_SUM_ON()       disp_mem[22] &= 0x7F
//#define ODISP_TITLE_SUM_OFF()      disp_mem[22] |= 0x80
#define ODISP_TITLE_SUMUNIT_ON()   disp_mem[29] &= 0xFD  //S5 (U2/Y2)与S2共用
#define ODISP_TITLE_SUMUNIT_OFF()  disp_mem[29] |= 0x02
//单价
//#define ODISP_TITLE_PRICE_ON()         disp_mem[21] &= 0xFE
//#define ODISP_TITLE_PRICE_OFF()        disp_mem[21] |= 0x01
#define ODISP_TITLE_PRICEUNIT_ON()     disp_mem[29] &= 0xFE //S (U2/Y1)与S18共用
#define ODISP_TITLE_PRICEUNIT_OFF()    disp_mem[29] |= 0x01
//进气KG/MIN
#define ODISP_TITLE_KGMIN_ON()      disp_mem[13] &= 0xFB
#define ODISP_TITLE_KGMIN_OFF()     disp_mem[13] |= 0x04 //SS(U5/Y11)
//报警
#define ODISP_TITLE_WARNING_ON()     disp_mem[21] &= 0xFD
#define ODISP_TITLE_WARNING_OFF()    disp_mem[21] |= 0x02
//枪号
#define ODISP_TITLE_GNUM_ON()       disp_mem[21] &= 0xFB
#define ODISP_TITLE_GNUM_OFF()      disp_mem[21] |= 0x04
//进气流量 S9
#define ODISP_TITLE_FLNUM_ON()      disp_mem[18] &= 0x7F
#define ODISP_TITLE_FLNUM_OFF()     disp_mem[18] |= 0x80
//回气流量 S10
#define ODISP_TITLE_STATE_ON()      disp_mem[17] &= 0xFE
#define ODISP_TITLE_STATE_OFF()     disp_mem[17] |= 0x01
//回气KG/MIN
#define ODISP_TITLE_STATEUPAY_ON() disp_mem[14] &= 0x7F //S12 (U5/Y8)
#define ODISP_TITLE_STATEUPAY_OFF() disp_mem[14] |= 0x80
//P11
//#define ODISP_TITLE_STATECASH_ON() disp_mem[13] &= 0xFD //P11 (U5/Y10)
//#define ODISP_TITLE_STATECASH_OFF() disp_mem[13] |= 0x02
//增益
#define ODISP_TITLE_STATEIDLE_ON() disp_mem[11] &= 0x7F //S14 (U5/Y10)
#define ODISP_TITLE_STATEIDLE_OFF() disp_mem[11] |= 0x80
///温度
//#define ODISP_TITLE_TEMP_N_ON()    disp_mem[22] &= 0xFD//S20 负号需要修改2015年8月5日 10:28:51
//#define ODISP_TITLE_TEMP_N_OFF()   disp_mem[22] |= (~0xFD)
#define ODISP_TITLE_TEMP_N_ON()    disp_mem[17] &= 0xFD//S20 
#define ODISP_TITLE_TEMP_N_OFF()   disp_mem[17] |= (~0xFD)

#define ODISP_TITLE_TEMP_ON()      disp_mem[10]  &= 0xFE //S16 (U5/Y10)
#define ODISP_TITLE_TEMP_OFF()     disp_mem[10] |= 0x01
#define ODISP_TITLE_TEMPUNIT_ON()  disp_mem[7]  &= 0xBF //S17 (U6/Y23)
#define ODISP_TITLE_TEMPUNIT_OFF() disp_mem[7] |= 0x40
//压力
#define ODISP_TITLE_PRESSURE_ON() disp_mem[7]  &= 0x7F //S18 (U6/Y24)与S8共用
#define ODISP_TITLE_PRESSURE_OFF() disp_mem[7] |= 0x80
//P10
//#define ODISP_TITLE_PRESSUREUNIT_ON() disp_mem[5]  &= 0x7F //P10 (U6/Y40)
//#define ODISP_TITLE_PRESSUREUNIT_OFF() disp_mem[5] |= 0x80
//LOGO
#define ODISP_TITLE_LOGO_ON() disp_mem[7]  &= 0xDF //S15 (U6/Y22)
#define ODISP_TITLE_LOGO_OFF() disp_mem[7] |= 0x20

/************************************************************************************************
*   增加回气流速位<4位>，增益位<3位>
*   2015年7月9日 15:06:44
*   Lis
*   编码说明:二维数组第一列:可以固定数码管在disp_mem数组中的位置
*           :二维数组第二列:初始化被选中的位，全部赋值为1
*           :二维数组后面列:0~9的编码，因为数码管的管脚可能在不同的IC但只可能是在最多两个IC上所以需要高地位共同控制，未用到的段写1
*           :赋值方式：Yx，最大的x先被赋为0~9编码的高位
***************************************************************************************************/

void odisp_drvgain_func(unsigned char id, unsigned char drvgainum)
{
    unsigned char h_B, l_B;
    unsigned int _disp_drvgain_arr[3][12] =  //增益
    {
        {0x1615,0xFA01,0x25FE,0xFDFE,0x15FF,0x55FE,0xCDFE,0x47FE,0x07FE,0xF5FE,0x05FE,0x45FE}, //NUM42
        {0x2316,0xF805,0x27FA,0xFFFA,0x17FB,0x57FA,0xCFFA,0x47FE,0x07FE,0xF7FA,0x07FA,0x47FA},  //NUM41
        {0x2423,0xF007,0x4FF8,0xFFF9,0x2FFA,0xAFF8,0x9FF9,0x8FFC,0x0FFC,0xEFF9,0x0FF8,0x8FF8},  //NUM40
    };
    h_B = _disp_drvgain_arr[id][0]/0x100;
    l_B = _disp_drvgain_arr[id][0]%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= _disp_drvgain_arr[id][1]%0x100;
        if(drvgainum <= 9)
            disp_mem[l_B] &= _disp_drvgain_arr[id][drvgainum+2]%0x100;
    }
    disp_mem[h_B] |= _disp_drvgain_arr[id][1]/0x100;
    if(drvgainum <= 9)
        disp_mem[h_B] &= _disp_drvgain_arr[id][drvgainum+2]/0x100;
}
bool odisp_drvgain(int nTnum)
{
    unsigned char num, n ,m, len = 0; //小数点  disp_mem[10] |= 0x80
    //unsigned int  nTnum;
    char buf[33];
    memset(buf,0,sizeof(buf));
    //odisp_drvgain_func(3,10);
    odisp_drvgain_func(2,10);
    odisp_drvgain_func(1,0);
    odisp_drvgain_func(0,10);
    ltoa(nTnum,buf,  10);
    len = strlen(buf);
    len = (4>len) ? len : 3;//len = (5>len) ? len : 4;
    m = len-1;
    for(n=0; n<len; n++)
    {
        num = buf[n]-0x30;
        odisp_drvgain_func(m,num);
        m--;
    }
    disp_mem[5] &= 0x7F;//P10
    return true;
}


void odisp_frate_back(unsigned char id, unsigned char ftnum)
{
    unsigned char h_B, l_B;
    unsigned int _disp_frate_bak[4][12] =  //回气流速
    {
        {0x2524, 0xE00F,0x9FF0,0xFFF3,0x5FF4,0x5FF1,0x3FF3,0x1FF9,0x1FF8,0xDFF3,0x1FF0,0x1FF1}, //NUM39
        {0x2625, 0xC01F,0x3FE1,0xFFE7,0xBFE8,0xBFE2,0x7FE6,0x3FF2,0x3FF0,0xBFE7,0x3FE0,0x3FE2}, //NUM38
        {0x2726, 0x803F,0x7FC2,0xFFCF,0x7FD1,0x7FC5,0xFFCC,0x7FE4,0x7FE0,0x7FCF,0x7FC0,0x7FC4}, //NUM37
        {0x27FF, 0x7FFF,0x84FF,0x9FFF,0xA2FF,0x8AFF,0x99FF,0xC8FF,0xC0FF,0x9EFF,0x80FF,0x88FF}, //NUM36
    };
    h_B = _disp_frate_bak[id][0]/0x100;
    l_B = _disp_frate_bak[id][0]%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= _disp_frate_bak[id][1]%0x100;
        if(ftnum <= 9)
            disp_mem[l_B] &= _disp_frate_bak[id][ftnum+2]%0x100;
    }
    disp_mem[h_B] |= _disp_frate_bak[id][1]/0x100;
    if(ftnum <= 9)
        disp_mem[h_B] &= _disp_frate_bak[id][ftnum+2]/0x100;
}
#if 0
odisp_frate_func();
odisp_flRate();
#endif

bool odisp_flRate_back(unsigned int nfrNum)
{
    unsigned char sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    //odisp_frate_back(4,10);
    //odisp_frate_back(3,10);
    if(nfrNum < 100)
    {
        odisp_frate_back(3,10);
        odisp_frate_back(2,0);
        odisp_frate_back(1,nfrNum/10);
        odisp_frate_back(0,nfrNum%10);
    }
    else
    {
        ltoa(nfrNum, buf,  10);
        len = strlen(buf);
        len = (5>len) ? len : 4;
        if(len <3) return false;
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            odisp_frate_back(m,sum);
            m--;
        }
    }
    disp_mem[13] &= 0xFD;//P11
    return true;
}


/********************************************************************************************************/
prog_uint16_t _disp_num_arr[8][12] =
{
    {0x2120, 0xF901,0x46FE,0xF6FF,0x27FE,0xA6FE,0x96FF,0x8EFE,0x0eFE,0xE6FF,0x06FE,0x86FE}, //气量NUM8
    {0x20FF, 0xFE00,0x21FF,0xF9FF,0x13FF,0x51FF,0xC9FF,0x45FF,0x05FF,0xF1FF,0x01FF,0x41FF}, //气量NUM7//{0x2000, 0xFE00,0x21FF,0xF9FF,0x13FF,0x51FF,0xC9FF,0x45FF,0x05FF,0xF1FF,0x01FF,0x41FF}, //气量NUM7
    {0x1F1E, 0x9E03,0x61FE,0xF9FF,0x73FC,0x71FD,0xE9FD,0x65FD,0x65FC,0xF1FF,0x61FC,0x61FD}, //气量NUM6
    {0x1F1E, 0x20FC,0xDF23,0xDFFB,0xFF13,0xDF53,0xDFCB,0xDF47,0xDF07,0xDFF3,0xDF03,0xDF43}, //气量NUM5
    {0x04FF, 0x7F00,0x90FF,0xFCFF,0x89FF,0xA8FF,0xE4FF,0xA2FF,0x82FF,0xF8FF,0x80FF,0xA0FF}, //气量NUM4//{0x0400, 0x7F00,0x90FF,0xFCFF,0x89FF,0xA8FF,0xE4FF,0xA2FF,0x82FF,0xF8FF,0x80FF,0xA0FF}, //气量NUM4
    {0x0403, 0x803F,0x7FC8,0x7FFB,0xFFC1,0x7FD1,0x7FF2,0x7FD4,0x7FC4,0x7FF9,0x7FC0,0x7FD0}, //气量NUM3
    {0x0302, 0xC01F,0x3FE4,0x3FFF,0x7FE2,0x3FEA,0x3FF9,0xBFE8,0xBFE0,0x3FFE,0x3FE0,0x3FE8}, //气量NUM2
    {0x0201, 0xE00F,0x1FF2,0x9FFF,0x3FF1,0x1FF5,0x9FFC,0x5FF4,0x5FF0,0x1FFF,0x1FF0,0x1FF4}, //气量NUM1
};
void odisp_num_func(unsigned char id, unsigned char num)
{
    unsigned char h_B, l_B;
    h_B = pgm_read_word(&_disp_num_arr[id][0])/0x100;
    l_B = pgm_read_word(&_disp_num_arr[id][0])%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= pgm_read_word(&_disp_num_arr[id][1])%0x100;
        if(num <= 9)
            disp_mem[l_B] &= pgm_read_word(&_disp_num_arr[id][num+2])%0x100;
    }
    disp_mem[h_B] |= pgm_read_word(&_disp_num_arr[id][1])/0x100;
    if(num <= 9)
        disp_mem[h_B] &= pgm_read_word(&_disp_num_arr[id][num+2])/0x100;
}

prog_uint16_t _disp_sum_arr[8][12] =
{
    {0x1B1A, 0xF007,0x0FF9,0xCFFF,0x9FF8,0x8FFA,0x4FFE,0x2FFA,0x2FF8,0x8FFF,0x0FF8,0x0FFA}, //NUM16
    {0x1A19, 0xF803,0x87FC,0xE7FF,0x0FFD,0x47FD,0x67FE,0x57FC,0x17FC,0xE7FD,0x07FC,0x47FC}, //NUM15
    {0x2219, 0x03F8,0xFC87,0xFFE7,0xFC4F,0xFD47,0xFF27,0xFD17,0xFC17,0xFFC7,0xFC07,0xFD07}, //NUM14
    {0x2221, 0xF806,0x87F9,0xE7FF,0x4FF9,0x47FD,0x27FF,0x17FD,0x17F9,0xC7FF,0x07F9,0x07FD}, //NUM13
    {0x0100, 0xF007,0x4FF8,0xEFFD,0x9FF8,0xAFF8,0x2FFD,0x2FFA,0x0FFA,0xEFFC,0x0FF8,0x2FF8}, //NUM12
    {0x1800, 0x03F8,0xFC87,0xFFE7,0xFC4F,0xFD47,0xFF27,0xFD17,0xFC17,0xFFC7,0xFC07,0xFD07}, //NUM11
    {0x1817, 0xFC04,0x43FB,0xF3FF,0x27FB,0xA3FB,0x93FF,0x8BFB,0x0BFB,0xE3FF,0x03FB,0x83FB}, //NUM10
    {0x17FF, 0xFBFF,0x24FF,0xFCFF,0x16FF,0x54FF,0xCCFF,0x45FF,0x05FF,0xF4FF,0x04FF,0x44FF}, //NUM9
};
void odisp_sum_func(unsigned char id, unsigned char sum)
{
    unsigned char h_B, l_B;
    
    h_B = pgm_read_word(&_disp_sum_arr[id][0])/0x100;
    l_B = pgm_read_word(&_disp_sum_arr[id][0])%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= pgm_read_word(&_disp_sum_arr[id][1])%0x100;
        if(sum <= 9)
            disp_mem[l_B] &= pgm_read_word(&_disp_sum_arr[id][sum+2])%0x100;
    }
    disp_mem[h_B] |= pgm_read_word(&_disp_sum_arr[id][1])/0x100;
    if(sum <= 9)
        disp_mem[h_B] &= pgm_read_word(&_disp_sum_arr[id][sum+2])/0x100;
}

prog_uint16_t _disp_price_arr[5][12] =
{
    {0x1D1C, 0xF806,0x87F9,0xE7FF,0x4FF9,0x47FD,0x27FF,0x17FD,0x17F9,0xC7FF,0x07F9,0x07FD}, //气量NUM21
    {0x1C1B, 0xF901,0x46FE,0xF6FF,0x27FE,0xA6FE,0x96FF,0x8EFE,0x0EFE,0xE6FF,0x06FE,0x86FE}, //气量NUM20
    {0x0B1B, 0x0F0E,0xF2F1,0xFFF9,0xF1F3,0xF5F1,0xFCF9,0xF4F5,0xF0F5,0xFFF1,0xF0F1,0xF4F1}, //气量NUM19
    {0x0CFF, 0xFEFF,0x09FF,0x3FFF,0x45FF,0x15FF,0x33FF,0x91FF,0x81FF,0x3DFF,0x01FF,0x11FF}, //气量NUM18
    {0x0D0C, 0xF901,0x16FE,0x7FFE,0x8EFE,0x2EFE,0x67FE,0x26FF,0x06FF,0x7EFE,0x06FE,0x26FE}, //气量NUM17
};
void odisp_price_func(unsigned char id, unsigned char pnum)
{
    unsigned char h_B, l_B;
    
    h_B = pgm_read_word(&_disp_price_arr[id][0])/0x100;
    l_B = pgm_read_word(&_disp_price_arr[id][0])%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= pgm_read_word(&_disp_price_arr[id][1])%0x100;
        if(pnum <= 9)
            disp_mem[l_B] &= pgm_read_word(&_disp_price_arr[id][pnum+2])%0x100;
    }
    disp_mem[h_B] |= pgm_read_word(&_disp_price_arr[id][1])/0x100;
    if(pnum <= 9)
        disp_mem[h_B] &= pgm_read_word(&_disp_price_arr[id][pnum+2])/0x100;
}

prog_uint16_t _disp_frate_arr[4][12] =
{
    {0x0EFF, 0x7FFF,0x84FF,0x9FFF,0xA2FF,0x8AFF,0x99FF,0xC8FF,0xC0FF,0x9EFF,0x80FF,0x88FF}, //气量NUM29
    {0x100F, 0x20FC,0xDF0B,0xFF3F,0xDF47,0xDF17,0xFF33,0xDF93,0xDF83,0xDF3F,0xDF03,0xDF13}, //气量NUM28
    {0x100F, 0x9E03,0x69FC,0x7FFE,0x65FD,0x75FC,0x73FE,0xF1FC,0xE1FC,0x7DFE,0x61FC,0x71FC}, //气量NUM27
    {0x1110, 0xFC01,0x13FE,0x7FFE,0x8BFE,0x2BFE,0x67FE,0x23FF,0x03FF,0x7BFE,0x03FE,0x23FE}, //气量NUM26
};

void odisp_frate_func(unsigned char id, unsigned char frnum)
{
    unsigned char h_B, l_B;
    
    h_B = pgm_read_word(&_disp_frate_arr[id][0])/0x100;
    l_B = pgm_read_word(&_disp_frate_arr[id][0])%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= pgm_read_word(&_disp_frate_arr[id][1])%0x100;
        if(frnum <= 9)
            disp_mem[l_B] &= pgm_read_word(&_disp_frate_arr[id][frnum+2])%0x100;
    }
    disp_mem[h_B] |= pgm_read_word(&_disp_frate_arr[id][1])/0x100;
    if(frnum <= 9)
        disp_mem[h_B] &= pgm_read_word(&_disp_frate_arr[id][frnum+2])/0x100;
}

bool odisp_num(unsigned long vnum)
{
    unsigned char num, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(vnum < 100)
    {
        odisp_num_func(7,10);
        odisp_num_func(6,10);
        odisp_num_func(5,10);
        odisp_num_func(4,10);
        odisp_num_func(3,10);
        odisp_num_func(2,0);
        odisp_num_func(1,vnum/10);
        odisp_num_func(0,vnum%10);
    }
    else
    {
        ltoa(vnum,buf,10);
        len = strlen(buf);
        len = (9>len) ? len : 8;
        if(len < 3) return false;
        m = len-1;
        for(n=0; n<len; n++)
        {
            num = buf[n]-0x30;
            odisp_num_func(m,num);
            m--;
        }
    }
    disp_mem[31] &= 0xFE;
    return true;
}

bool odisp_sum(unsigned long fvsum)
{
    unsigned char sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(fvsum < 100)
    {
        odisp_sum_func(7,10);
        odisp_sum_func(6,10);
        odisp_sum_func(5,10);
        odisp_sum_func(4,10);
        odisp_sum_func(3,10);
        odisp_sum_func(2,0);
        odisp_sum_func(1,fvsum/10);
        odisp_sum_func(0,fvsum%10);
    }
    else
    {
        ltoa(fvsum,buf,  10);
        len = strlen(buf);
        len = (9>len) ? len : 8;
        if(len <3) return false;
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            odisp_sum_func(m,sum);
            m--;
        }
    }
    disp_mem[25] &= 0xFB;
    return true;
}

bool odisp_price(unsigned long fvsum)
{
    unsigned char sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(fvsum < 100)
    {
        odisp_price_func(4,10);
        odisp_price_func(3,10);
        odisp_price_func(2,0);
        odisp_price_func(1,fvsum/10);
        odisp_price_func(0,fvsum%10);
    }
    else
    {
        ltoa(fvsum,buf,  10);
        len = strlen(buf);
        len = (6>len) ? len : 5;
        if(len <3) return false;
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            odisp_price_func(m,sum);
            m--;
        }
    }
    disp_mem[11] &= 0xEF;
    return true;
}

bool odisp_flRate(unsigned int nfrNum)
{
    unsigned char sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    //odisp_frate_func(4,10);
    //odisp_frate_func(3,10);
    if(nfrNum < 100)
    {
        //odisp_frate_func(2,0);
        odisp_frate_func(1,nfrNum/10);
        odisp_frate_func(0,nfrNum%10);
    }
    else
    {
        ltoa(nfrNum, buf,  10);
        len = strlen(buf);
        len = (5>len) ? len : 4;
        if(len <3) return false;
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            odisp_frate_func(m,sum);
            m--;
        }
    }
    disp_mem[16] &= 0xBF;
    return true;
}

void odisp_warnning(unsigned char wNO) //NUM22 NUM23
{
    unsigned int _disp_h_warr[10] =  {0x07FE, 0x9FFF, 0x57FC, 0x17FD, 0x8FFD, 0x27FD, 0x27FC, 0x97FF, 0x07FC, 0x07FD};
    unsigned int _disp_l_warr[10] =  {0x13FE, 0x7FFE, 0x8BFE, 0x2BFE, 0x67FE, 0x23FF, 0x03FF, 0x7BFE, 0x03FE, 0x23FE};
    if(wNO >99) return;
    if(wNO >= 10)
    {
        disp_mem[21] |= 0xF8;
        disp_mem[20] |= 0x03;
        disp_mem[21] &= _disp_h_warr[wNO/10]/0x100;
        disp_mem[20] &= _disp_h_warr[wNO/10]%0x100;
    }
    else
    {
        disp_mem[21] |= 0xF8;
        disp_mem[20] |= 0x03;
        disp_mem[21] &= _disp_h_warr[0]/0x100;
        disp_mem[20] &= _disp_h_warr[0]%0x100;
    }
    disp_mem[20] |= 0xFC;
    disp_mem[19] |= 0x01;
    disp_mem[20] &= _disp_l_warr[wNO%10]/0x100;
    disp_mem[19] &= _disp_l_warr[wNO%10]%0x100;
}

void odisp_gNumber(unsigned char gNO) //NUM24 NUM25
{
    unsigned char _disp_h_garr[10] =  {0x09, 0x3F, 0x45, 0x15, 0x33, 0x91, 0x81, 0x3D, 0x01, 0x11};
    unsigned char _disp_l_garr[10] =  {0x81, 0x9F, 0xA2, 0x8A, 0x9C, 0xC8, 0xC0, 0x9B, 0x80, 0x88};

    if(gNO >99) return;
    disp_mem[19] |= 0xFE; // disp_mem[19] &= _disp_h_garr[gNO%10];
    if(gNO >= 10)
    {
        disp_mem[19] &= _disp_h_garr[gNO/10];
    }
    else
    {
        disp_mem[19] &= _disp_h_garr[0];
    }
    disp_mem[18] |= 0x7F;
    disp_mem[18] &= _disp_l_garr[gNO%10];
}


prog_uint16_t _disp_temperature_arr[3][12] =
{
    // {0x0807, 0xC01F,0x3FE1,0xFFE7,0xBFE8,0xBFE2,0x7FE6,0x3FF2,0x3FF0,0xBFE7,0x3FE0,0x3FE2}, //NUM33
    {0x0908, 0x803F,0x7FC2,0xFFCF,0x7FD4,0x7FC5,0xFFC9,0x7FE1,0x7FE0,0x7FCF,0x7FC0,0x7FC1}, //NUM32
    {0x09FF,0x7FFF,0x84FF,0x9FFF,0xA2FF,0x8AFF,0x99FF,0xC8FF,0xC0FF,0x9EFF,0x80FF,0x88FF},  //31
    {0x0B0A,0x607C,0x9F87,0xFF9F,0xBFA3,0xBF8B,0xDF9B,0x9FCB,0x9FC3,0xBF9F,0x9F83,0x9F8B},  //NUM30
};
void odisp_temperature_func(unsigned char id, unsigned char tnum)
{
    unsigned char h_B, l_B;
    h_B = pgm_read_word(&_disp_temperature_arr[id][0])/0x100;
    l_B = pgm_read_word(&_disp_temperature_arr[id][0])%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] |= pgm_read_word(&_disp_temperature_arr[id][1])%0x100;
        if(tnum <= 9)disp_mem[l_B] &= pgm_read_word(&_disp_temperature_arr[id][tnum+2])%0x100;
    }
    disp_mem[h_B] |= pgm_read_word(&_disp_temperature_arr[id][1])/0x100;
    if(tnum <= 9)disp_mem[h_B] &= pgm_read_word(&_disp_temperature_arr[id][tnum+2])/0x100;
}
bool odisp_temperature_ex(int nTnum) //NUM30/1/2/3  温度
{
    unsigned char num, n ,m, len = 0; //小数点  disp_mem[10] |= 0x80
    char buf[33];
    memset(buf,0,sizeof(buf));
    odisp_temperature_func(3,10);
    odisp_temperature_func(2,10);
    odisp_temperature_func(1,0);
    odisp_temperature_func(0,0);
    ltoa(nTnum,buf,  10);
    len = strlen(buf);
    len = (4>len) ? len : 3;//len = (5>len) ? len : 4;
    m = len-1;
    for(n=0; n<len; n++)
    {
        num = buf[n]-0x30;
        odisp_temperature_func(m,num);
        m--;
    }
    return true;
}
bool odisp_temperature(float vfTemp)
{
    unsigned int npNum;
    ODISP_TITLE_TEMP_N_OFF();
    if(vfTemp<0) //显示负号
    {
        ODISP_TITLE_TEMP_N_ON();
    }
    npNum = (unsigned int)(fabs(vfTemp)*10);
    odisp_temperature_ex(npNum);
    disp_mem[10] |= 0x80;
    if(npNum<1000) //显示小数点
    {
        disp_mem[10] &= (~0x80);
    }
    return true;
}
void odisp_Pressure(unsigned int npNum) //NUM34/5  压力
{
    unsigned int vpNum, _disp_hh_warr[10] = {/*0x0807, 0xC01F*/0x3FE1,0xFFE7,0xBFE8,0xBFE2,0x7FE6,0x3FF2,0x3FF0,0xBFE7,0x3FE0,0x3FE2};
    unsigned int _disp_h_warr[10] = {0xE4FC,0xFFFC,0xE2FE,0xEAFC,0xF9FC,0xE8FD,0xE0FD,0xFEFC,0xE0FC,0xE8FC};
    unsigned int _disp_l_warr[10] = {0x9F87,0xFF9F,0xBFA3,0xBF8B,0xDF9B,0x9FCB,0x9FC3,0xBF9F,0x9F83,0x9F8B};
    if(npNum >999) return;
    vpNum = npNum/10;
    if(vpNum >= 10)
    {
        disp_mem[8] |= 0xC0;
        disp_mem[7] |= 0x1F;
        disp_mem[8] &= _disp_hh_warr[vpNum/10]/0x100;
        disp_mem[7] &= _disp_hh_warr[vpNum/10]%0x100;
    }
    else
    {
        disp_mem[8] |= 0xC0;
        disp_mem[7] |= 0x1F;
        //disp_mem[8] &= _disp_hh_warr[0]/0x100;
        //disp_mem[7] &= _disp_hh_warr[0]%0x100;
    }
    disp_mem[6] |= 0x1F; //0x0605,0x1F03,
    disp_mem[5] |= 0x03;
    disp_mem[6] &= _disp_h_warr[vpNum%10]/0x100;
    disp_mem[5] &= _disp_h_warr[vpNum%10]%0x100;

    disp_mem[6] |= 0x60;  //0x0605,0x607C,
    disp_mem[5] |= 0x7C;
    disp_mem[6] &= (_disp_l_warr[npNum%10]/0x100)&0x7F;
    disp_mem[5] &= _disp_l_warr[npNum%10]%0x100;
}

void odisp_title_on(void)
{

    ODISP_TITLE_KG_ON();
    ODISP_TITLE_SUMUNIT_ON();
    ODISP_TITLE_PRICEUNIT_ON();
    ODISP_TITLE_WARNING_ON();
    ODISP_TITLE_GNUM_ON();
    ODISP_TITLE_FLNUM_ON();
    ODISP_TITLE_STATE_ON();
    ODISP_TITLE_STATEUPAY_ON();
    ODISP_TITLE_STATEIDLE_ON();
    ODISP_TITLE_KGMIN_ON();
    ODISP_TITLE_TEMP_ON();
    ODISP_TITLE_TEMPUNIT_ON();
    ODISP_TITLE_PRESSURE_ON();
    ODISP_TITLE_LOGO_ON();


}
#if 0
void odisp_num_func_ex(unsigned char id, unsigned char num)
{
    unsigned char h_B, l_B;
    h_B = _disp_num_arr[id][0]/0x100;
    l_B = _disp_num_arr[id][0]%0x100;
    if(l_B != 0xFF)//if(l_B != 0)
    {
        disp_mem[l_B] &= ~(_disp_num_arr[id][1]%0x100);//disp_mem[l_B] |= _disp_num_arr[id][1]%0x100;
        //if( num <= 9 )disp_mem[l_B] &= _disp_num_arr[id][num+2]%0x100;
        if(num <= 9)disp_mem[l_B] |= ~(_disp_num_arr[id][num+2]%0x100);
    }
    disp_mem[h_B] &= ~(_disp_num_arr[id][1]/0x100);//disp_mem[h_B] |= _disp_num_arr[id][1]/0x100;
    if(num <= 9)disp_mem[h_B] |= ~(_disp_num_arr[id][num+2]/0x100);  //if( num <= 9 )disp_mem[h_B] &= _disp_num_arr[id][num+2]/0x100;
}
#endif
void odisp_mem_disp_init(void)
{
    unsigned long int temp1[10]= {00000000,11111111,22222222,33333333,44444444,55555555,66666666,77777777,88888888,99999999};
    unsigned int temp2[10]= {0000,1111,2222,3333,4444,5555,6666,7777,8888,9999};
    unsigned int temp3[10]= {000,111,222,333,444,555,666,777,888,999};
    unsigned char temp4[10]= {00,11,22,33,44,55,66,77,88,99};
    float temp5[10]= {00.0,11.1,22.2,33.3,44.4,55.5,66.6,77.7,88.8,99.9};
    unsigned char i,n=0;

    memset(disp_mem,0xFF,sizeof(disp_mem));
    odisp_title_on();
    for(i = 0; i < 10; i++)
    {
        odisp_price(temp1[i]);
        odisp_num(temp1[i]);
        odisp_sum(temp1[i]);
        odisp_flRate(temp2[i]);
        odisp_flRate_back(temp2[i]);
        odisp_drvgain(temp3[i]);
        odisp_Pressure(temp3[i]);
        odisp_temperature(-temp5[i]);
        odisp_warnning(temp4[i]);
        odisp_gNumber(temp4[i]);
        if(i==0)
            memset(disp_mem,0x0,sizeof(disp_mem));
        odisp_mem_disp();//写入数据该数据在disp_mem全局数组中
        ocomm_delay_ms(1000);
        if(i%2 == 0)
        {
            ocomm_printf(CH_DISP,4,1+n,">>");
            n++;
        }
    }
}
void odisp_mem_disp_init0(void)
{
    memset(disp_mem,0xFF,sizeof(disp_mem));
    odisp_title_on();
    odisp_price(0);
    odisp_num(0);
    odisp_sum(0);
    odisp_flRate(0);
    odisp_flRate_back(0);
    odisp_drvgain(0);
    odisp_Pressure(0);
    odisp_temperature(0.0);
    odisp_warnning(0);
    odisp_gNumber(0);
    odisp_mem_disp();//写入数据该数据在disp_mem全局数组中

}

void osplc_hardwarechk(void)
{
    unsigned char disp[22] = {0};
    unsigned char  m =0;
    unsigned char  n =0;
    osplc_portinit();

    if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_B)
    {
        for(m=0; m<=9; m++)
        {
            memset(&disp[0],m,22);
            osplc_disp_ex(disp,true);
            ocomm_delay_ms(500);
            if(m%2 == 0)
            {
                ocomm_printf(CH_DISP,4,1+n,">>");
                n++;
            }
        }
    }
    else if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_Y)
    {
        for(m=0; m<=9; m++)
        {
            memset(&disp[0],m,16);
            osplc_disp(disp,true);
            ocomm_delay_ms(500);
            if(m%2 == 0)
            {
                ocomm_printf(CH_DISP,4,1+n,">>");
                n++;
            }
        }
    }
    else //if(cur_sysparas.b_basic_p[BASIC_K_DISP_TYPE] == DISP_TYPE_N)
    {
        odisp_mem_disp_init();
    }
}
/*****************************************************************
**
**  row1 气量， row2 金额row3 单价，row5报警，4流量，
**  6温度，7压力，8枪号。
**
**  2015年6月23日 17:34:41
**  Lis
******************************************************************/
void osplc_show_new(float Row1,float Row2,float Row3)
{
    unsigned long  buf1, buf2,buf3;
    static unsigned char tmp=0;
    tmp++;
    tmp %= 100;
    buf1 = Row1*100;
    buf2 = Row2*100;
    buf3 = (Row3*10000)/100;
//    float vftemp=0.0,vfdgain=0.0;
//    vftemp  = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_TEMPERATURE);
//	vfdgain = omodbus_read_auto_float_value(_MODBUS_L_, AUTO_SCAN_LCNG_DRVGAIN);
    memset(disp_mem,0xFF,sizeof(disp_mem));
    odisp_title_on();//常亮
    odisp_price(buf3);//单价
    odisp_num(buf1);//气量
    odisp_sum(buf2);//金额
    if(true == b_Simulation)
        odisp_gNumber(tmp);//枪号
    else
        odisp_gNumber(cur_sysparas.b_basic_p[BASIC_P_G_ID]);//枪号
    odisp_flRate(cur_flowrate_l/6);//进气流速
    odisp_Pressure(cur_pressure_l*10);//压力
    odisp_temperature(vftemp);//温度
    if(true == b_Simulation)
        odisp_warnning(tmp);//停机异常报警
    else
        odisp_warnning(halt_err);//停机异常报警
    odisp_drvgain(vfdgain*10);//增益
    odisp_flRate_back(cur_flowrate_g*10/6);//回气流速 
    odisp_mem_disp();//写入数据该数据在disp_mem全局数组中

}

