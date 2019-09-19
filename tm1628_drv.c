/////////////////////////////////////////////////////////////////////////////////////////
//
//                              JUNESONS       2015.09.11
//
//  TM1628 LED  Ó²¼þÇý¶¯
//
/////////////////////////////////////////////////////////////////////////////////////////
#define __TM1628_DRV__

#include "includes.h"
#include "config.h"
#include "common.h"

#if (DISP_DRIVER == DISP_DRV_TM1628)
#include "tm1628_drv.h"


//#include "includes.h"
//#include "sys_detect.h"
//#include "config.h"

#define LCD_DISP_ON  0x88|0x02 //0x04 // (0xC4|0x08)
#define LCD_DISP_ON_BRL  0x88|0x05//0x88|0x07  //bright light
#define LCD_DISP_ON_DML  0x88|0x00 //dim light

#define LCD_CLK BIT(10)
#define LCD_STB  BIT(9)
#define LCD_DATA BIT(11)

#define LCD_CLK_HIGH()   PORTA_OUT |= LCD_CLK;
#define LCD_CLK_LOW()  	PORTA_OUT &= ~LCD_CLK;

#define LCD_STB_HIGH() PORTA_OUT |= LCD_STB;
#define LCD_STB_LOW()  PORTA_OUT &= ~LCD_STB;

#define LCD_DATA_HIGH()  PORTA_OUT |= LCD_DATA;
#define LCD_DATA_LOW() 	PORTA_OUT &= ~LCD_DATA;

#define SET_CLK_OUT()  	PORTA_DIR &= ~LCD_CLK;
#define SET_DATA_OUT() 	PORTA_DIR &= ~LCD_DATA;
#define SET_STB_OUT()    PORTA_DIR &= ~LCD_STB;

#define SET_DATA_PULLUP()  {PORTA_PU &= ~LCD_DATA;PORTA_PD &= ~LCD_DATA;}
#define SET_STB_PULLUP()  {PORTA_PU &= ~LCD_CLK;PORTA_PD &= ~LCD_CLK;}
#define SET_CLK_PULLUP()  {PORTA_PU &= ~LCD_STB;PORTA_PD &= ~LCD_STB;}

#define SET_DATA_IN()	PORTA_DIR |= LCD_DATA
#define IS_LCD_DATA_HIGH()	(PORTA_IN & LCD_DATA)


#define LCD_CLK_DELAY 0
u8 fgIsLED0;
u8 cKey[5];
u8 DispBuff[20];
//BYTE acDisp_ram_buf[14];

void led_drvier_init(void)
{
	SET_CLK_PULLUP();
	SET_STB_PULLUP();
	SET_DATA_PULLUP();

	SET_CLK_OUT();
	SET_DATA_OUT();

	SET_STB_OUT();
	LCD_STB_HIGH();

	LCD_CLK_HIGH();
	LCD_DATA_HIGH();
}



/*
Func: led_drive_wirte_8bit
Desc:
*/
void led_drive_wirte_8bit(u8 cData)
{
	u8 cTmp;
	SET_STB_OUT();
	for(cTmp = 8; cTmp > 0; cTmp--)
	{
		LCD_CLK_LOW();
		if((cData&0x01) == 1)
		{
		LCD_DATA_HIGH();
		}
		else
		{
		LCD_DATA_LOW() ;
		}
//		WaitUs(0);
		LCD_CLK_HIGH();
		cData >>=1;

//		WaitUs(0);
	}
}
/*
u8 lcd_clk_test(void)
{
	SET_DATA_PULLUP();
	SET_DATA_IN();
	if(IS_LCD_DATA_HIGH())
	{
		SET_CLK_PULLUP();
		SET_CLK_OUT();
		LCD_CLK_LOW();
		LCD_CLK_HIGH();
	}
}
*/

/*
Func: led_drive_wirte_8bit
Desc:
*/
u8 lcd_drive_read_8bit(void)
{
	u8 cTmp;
	u8 cData;


	cData = 0;
	SET_STB_OUT();
	SET_DATA_IN();
//	SetGpioRegBit(GPIO_A_PU, 0xc0);
//	ClrGpioRegBit(GPIO_A_PD, 0xc0);
	for(cTmp = 8; cTmp > 0; cTmp--)
	{
		cData >>=1;
//		SET_DATA_IN();
//		SET_DATA_PULLUP();
		LCD_CLK_LOW();
		delay(0);
		LCD_CLK_HIGH();
		if(IS_LCD_DATA_HIGH())
			{
				cData |= 0x80;
			}
	}
	SET_DATA_OUT();
//	ClrGpioRegBit(GPIO_A_PU, 0x40);
//	ClrGpioRegBit(GPIO_A_PD, 0x40);
	return cData;
}


/*
Func: led_write_byte
Desc:
*/
void led_write_byte(u8 cData)
{
	SET_STB_OUT();
	LCD_STB_LOW();
	delay(LCD_CLK_DELAY);
	led_drive_wirte_8bit(cData);
	delay(LCD_CLK_DELAY);
	LCD_STB_HIGH();
}



/*
Func: led_set
Desc:
*/
void tm1628_reset(void)
{
	led_drvier_init();
//	led_write_byte(LCD_DISP_SET);
	led_write_byte(LCD_GRID);
	led_write_byte(LCD_DISP_ON_BRL/*LCD_DISP_ON*/);
}





/*
Func: led_clear
Desc:
*/
void led_clear(void)
{
	u8 cTmp;
//	u8 i = 2;
	SET_STB_OUT();

	/*DISP PARA*/
	LCD_STB_LOW();
	led_drive_wirte_8bit(LCD_DISP_SET);
	LCD_STB_HIGH();

	delay(LCD_CLK_DELAY);

	/*SET START ADDR*/
	LCD_STB_LOW();
	led_drive_wirte_8bit(LCD_ADDR|0);

	delay(LCD_CLK_DELAY);
	for(cTmp=0;cTmp<10;cTmp++)
	{
		led_drive_wirte_8bit(0x00);
		delay(LCD_CLK_DELAY);
	}
	for(cTmp=10;cTmp<14;cTmp++)
	{
		led_drive_wirte_8bit(0xff);
		delay(LCD_CLK_DELAY);
	}
	LCD_STB_HIGH();

}



/*
Func: led_standby
Desc:
*/
//public
void tm1628_standby(void)
{
	puts("tm1628_standby");
	led_write_byte(LCD_DISP_OFF/*LCD_DISP_ON*/);
//	led_write_byte(LCD_DISP_STANDBY);
}


//public
void led_off(void)
{
	led_write_byte(LCD_DISP_OFF);
}

/*
Func: tm1628_write_data
Desc:
*/
void tm1628_write_data(u8* sStr, u8 cAddr, u8 cNum)
{
	u8 cTmp;
	u8* pData;
	u8 i = 2;
	SET_STB_OUT();
		i--;
		fgIsLED0 = ((i == 0) ? TRUE : FALSE);

		pData = sStr;//(sStr + i);
		tm1628_drv_printf("tm1628_write_data\n");

		/*DISP MODE*/
		LCD_STB_LOW();
		delay(LCD_CLK_DELAY);
		led_drive_wirte_8bit(LCD_GRID);
		delay(LCD_CLK_DELAY);
		LCD_STB_HIGH();
		delay(LCD_CLK_DELAY);

		/*DISP PARA*/
		LCD_STB_LOW();
		delay(LCD_CLK_DELAY);
		led_drive_wirte_8bit(LCD_DISP_SET);
		delay(LCD_CLK_DELAY);
		LCD_STB_HIGH();
		delay(LCD_CLK_DELAY);

		/*SET START ADDR*/
		LCD_STB_LOW();
		delay(LCD_CLK_DELAY);

		led_drive_wirte_8bit(LCD_ADDR|cAddr);

		delay(LCD_CLK_DELAY);
		for(cTmp=0;cTmp<cNum;cTmp++)
		{
			led_drive_wirte_8bit(*(pData));
//			led_drive_wirte_8bit(0xFF);
//			led_drive_wirte_8bit(0x07);
	//		led_drive_wirte_8bit(cLedTest);
			pData++;
			delay(LCD_CLK_DELAY);
		}

		LCD_STB_HIGH();
}


//
//
//
void led_test(u8 cAddr, u8 cData)
{
	SET_STB_OUT();
	LCD_STB_LOW();
	delay(LCD_CLK_DELAY);
	led_drive_wirte_8bit(LCD_DISP_SET|0x04);
	delay(LCD_CLK_DELAY);
	LCD_STB_HIGH();
	delay(LCD_CLK_DELAY);

	LCD_STB_LOW();
	delay(LCD_CLK_DELAY);
	led_drive_wirte_8bit(LCD_ADDR|cAddr);

	led_drive_wirte_8bit(cData); //data sent

	delay(LCD_CLK_DELAY);
	LCD_STB_HIGH();
	delay(LCD_CLK_DELAY);
}



#if 0
/*
Func: lcd_get_key
Desc:
*/
u8 lcd_get_key(void)
{
	u8 cCnt;
	cKey[0]	= 0;
	SET_STB_OUT();
	LCD_STB_LOW();
	delay(LCD_CLK_DELAY);
	led_drive_wirte_8bit(LCD_KEY_GET);

	LCD_DATA_LOW();
	for(cCnt =0; cCnt<5;cCnt++)
	{
		delay(5);
		delay(LCD_CLK_DELAY);
		cKey[cCnt] = lcd_drive_read_8bit();
		if(cKey[cCnt]!=0)
		{
			//DBG(("acKey[%bu]: %bu\n", cCnt,cKey[cCnt]));
			break;
		}

	}

	delay(LCD_CLK_DELAY);
	LCD_STB_HIGH();

	if(cKey[0] != 0)
	{

	}
	return -1;//cKey[0];
}
#endif




/*++++++++++++++++++++++++++++++++++++++++++++++
Func: TM1628_get_key
Return:
++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
u8 tm1628_get_key(void)
{
//	BYTE acKey[5];
	u8 cVal = 0;

	u8 cCnt;
//	cKey[0]	= 0;
	SET_STB_OUT();
	LCD_STB_LOW();
	delay(LCD_CLK_DELAY);
	led_drive_wirte_8bit(LCD_KEY_GET);

	for(cCnt =0; cCnt<5;cCnt++)
	{
		LCD_DATA_LOW();
		delay(5);
		delay(LCD_CLK_DELAY);
		cVal = lcd_drive_read_8bit();
//		DB(("acKey[%bu]: %bu\n", cCnt,acKey[cCnt]));

		if(cVal)
			{
//			DB(("acKey==%bu\n", cVal));
			break;
			}
	}
	delay(LCD_CLK_DELAY);
	LCD_STB_HIGH();

	if(cVal != 0)
	{
		//(("1111 %bu\n", cVal));
		return TRUE;
	}
	else
	{
		return FALSE;
	}
//	return cVal;
}
#endif


void Lcd_Test(void)
{
	//DB(("LCD_TESTING______\n"));
	led_drvier_init();
	SET_STB_OUT();
	tm1628_reset();
	led_clear();
	tm1628_write_data(DispBuff,0x00, 16);
//	WaitMs(2000);
//	led_off();
	delay_2ms(5000);
	//DB(("LCD_TESTINGEND\n"));

}


void DispBuffClear(void)
{
	memset(DispBuff,0x00,19);
}

void DispDataSend(void)
{
	// acDisp_ram_buf_bak
//			memset(DispBuff,0xff,14);


//	if(BuffDataCompare())
	{
//	 	tm1628_reset();
//	   	tm1628_write_data(DispBuff,0x00, 16);
//		tm1628_reset();
	}
//	else if((CurrEventId == SDC) ||(CurrEventId == USB))
	{
//	 	tm1628_reset();
//	   	tm1628_write_data(panel.uDispBuffer,0x00, 14);
	}
}



//
//tm1628_get_key
//get five bytes data
u8 tm1628_get_key(void)
{
	u8 cCnt;
	u8 cKeyVal;
	
	SET_STB_OUT();
	LCD_STB_LOW();
	delay(LCD_CLK_DELAY);
	led_drive_wirte_8bit(LCD_KEY_GET);

	LCD_DATA_LOW();
	for(cCnt =0; cCnt<5;cCnt++)
	{
		delay(5);
		delay(LCD_CLK_DELAY);
		cKeyVal = lcd_drive_read_8bit();
		if(cKeyVal!=0)
		{
			//DBG(("acKey[%bu]: %bu\n", cCnt,cKey[cCnt]));
			break;
		}

	}

	delay(LCD_CLK_DELAY);
	LCD_STB_HIGH();

	if(cKeyVal!= 0)
	{
		return cKeyVal;
	}
	return -1;
}

#endif
/***********************************HT1628.c end**********************************/


