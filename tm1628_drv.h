/////////////////////////////////////////////////////////////////////////////////////////
//
//                              JUNESONS       2015.09.11
//
//  TM1628 LED  硬件驱动
//
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TM1628_drv_H_
#define _TM1628_drv_H_

#if (DISP_DRIVER == DISP_DRV_TM1628)

//#define TM1628_DRV_DBG
#ifdef TM1628_DRV_DBG
    #define tm1628_drv_puts           puts
    #define tm1628_drv_printf         printf
    #define tm1628_drv_buf            put_buf
#else
    #define tm1628_drv_puts(...)
    #define tm1628_drv_printf(...)
    #define tm1628_drv_buf(...)
#endif/*TM1628_DRV_DBG*/


#ifdef	__TM1628_DRV__
#define	tm1628_ext
#else
#define	tm1628_ext	extern
#endif

/////////////////////////////////////////////////////////////////////////////
//                           COMMAND   LIST
/////////////////////////////////////////////////////////////////////////////
#define LCD_GRID 0x03  //0x00  /*4 grid*/
#define LCD_DISP_SET  0x40
#define LCD_KEY_GET 0x42
#define LCD_ADDR 0xC0 //0x80
#define LCD_DISP_OFF 0x80 // 0xC4
#define LCD_DISP_STANDBY (0x88|0x00)


tm1628_ext void tm1628_reset(void);
tm1628_ext void tm1628_write_data(u8* sStr, u8 cAddr, u8 cNum);
tm1628_ext u8 tm1628_get_key(void);
tm1628_ext void tm1628_standby(void);


////////////////////////////////////////////////////////////////////////////
// 驱动接入口
////////////////////////////////////////////////////////////////////////////
#define SEG_PANEL_DATA_SIZE 14
#define SEG_PANEL_DATA_UPDATE(Str,Addr,Num) {tm1628_reset();\
												tm1628_write_data(Str,Addr,Num);}
#define SEG_PANEL_STANDBY()	tm1628_standby()
#endif
#endif/*_TM1628_drv_H_*/


