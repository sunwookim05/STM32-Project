#include "lcd_user.h"
#include "oledfont.h"  	


//I2C_HandleTypeDef *hi2c;
extern I2C_HandleTypeDef hi2c1;

//
//**************************************************************************************************
//  Send comman to LCD display
//**************************************************************************************************
//
void IIC_Writecmd(uint8_t com)
{
	HAL_I2C_Mem_Write(&hi2c1,0x78,0x00,1,&com,1,100);
}

void IIC_Writedata(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1,0x78,0x40,1,&data,1,100);
}

void OLED_Clear(void)  
{  
	uint8_t i,n;
	for(i=0;i<8;i++)  
	{
		IIC_Writecmd(0xb0+i);
		IIC_Writecmd(0x00);
		IIC_Writecmd(0x10);
		for(n=0;n<128;n++)IIC_Writedata(0); 
	}
	OLED_Set_Pos(0,0);
}

void OC(void)
{
	uint8_t i,n;
	for(i=1;i<8;i++)
	{
		IIC_Writecmd(0xb0+i);
		IIC_Writecmd(0x00);
		IIC_Writecmd(0x10);
		for(n=0;n<128;n++)IIC_Writedata(0);
	}
	OLED_Set_Pos(0,0);
}


//
//**************************************************************************************************
//	LCD display initialization
//**************************************************************************************************
//
void lcd_init(I2C_HandleTypeDef *h)
{
	IIC_Writecmd(0xAE);//--turn off oled panel
	IIC_Writecmd(0x00);//---set low column address
	IIC_Writecmd(0x10);//---set high column address
	IIC_Writecmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	IIC_Writecmd(0x81);//--set contrast control register
	IIC_Writecmd(0xCF); // Set SEG Output Current Brightness
	IIC_Writecmd(0xA1);//--Set SEG/Column Mapping
	IIC_Writecmd(0xC8);//Set COM/Row Scan Direction
	IIC_Writecmd(0xA6);//--set normal display
	IIC_Writecmd(0xA8);//--set multiplex ratio(1 to 64)
	IIC_Writecmd(0x3f);//--1/64 duty
	IIC_Writecmd(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	IIC_Writecmd(0x00);//-not offset
	IIC_Writecmd(0xd5);//--set display clock divide ratio/oscillator frequency
	IIC_Writecmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	IIC_Writecmd(0xD9);//--set pre-charge period
	IIC_Writecmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	IIC_Writecmd(0xDA);//--set com pins hardware configuration
	IIC_Writecmd(0x12);
	IIC_Writecmd(0xDB);//--set vcomh
	IIC_Writecmd(0x40);//Set VCOM Deselect Level
	IIC_Writecmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	IIC_Writecmd(0x02);//
	IIC_Writecmd(0x8D);//--set Charge Pump enable/disable
	IIC_Writecmd(0x14);//--set(0x10) disable
	IIC_Writecmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
	IIC_Writecmd(0xA6);// Disable Inverse Display On (0xa6/a7) 
	IIC_Writecmd(0xAF);//--turn on oled panel
	
	IIC_Writecmd(0xAF); /*display ON*/ 
	OLED_Clear();
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	IIC_Writecmd(0xb0+y);
	IIC_Writecmd(((x&0xf0)>>4)|0x10);
	IIC_Writecmd((x&0x0f)|0x01); 
}

void set_poin(uint8_t x,uint8_t y,uint8_t state,uint8_t *p)
{
	OLED_Set_Pos(x,y/8);
	if(state)
		p[x+((y/8)*128)] |= 0x01<<(y%8);
	else
		if(p[x+((y/8)*128)] & 0x01<<(y%8))
			p[x+((y/8)*128)] ^= 0x01<<(y%8);
	IIC_Writedata(p[x+((y/8)*128)]);
}

void OLED_ShowChar(uint32_t x,uint32_t y,char chr,uint8_t mode)
{      	
	unsigned char c=chr-' ',i;
	if(x>Max_Column-1){x=0;y=y+2;}	
	OLED_Set_Pos(x,y);
	for(i=0;i<6;i++)
	IIC_Writedata((mode)?F6x8[c][i]:~F6x8[c][i]);
}

void OLED_ShowBigChar(uint32_t x,uint32_t y,char chr,uint8_t mode)
{
	unsigned char c=chr-' ',i;
	if(x>Max_Column-1){x=0;y=y+2;}
	OLED_Set_Pos(x,y);
	for(i=0;i<8;i++)
	IIC_Writedata((mode)?F8X16[c][i]:~F8X16[c][i]);
	OLED_Set_Pos(x,y+1);
	for(i=8;i<16;i++)
	IIC_Writedata((mode)?F8X16[c][i]:~F8X16[c][i]);
}

void OLED_ShowString(uint32_t x,uint32_t y,char *chr,uint8_t mode)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		if(chr[j]=='\n') 
		{
			x=0;
			y++;
		}
		else if(chr[j]=='\f') OLED_Clear();
		else
		{
			OLED_ShowChar(x,y,chr[j],mode);
			x+=6;
			if(x>120)
			{
				x=0;
				y++;
			}
		}
		j++;
	}
}

void OLED_ShowBigString(uint32_t x,uint32_t y,char *chr,uint8_t mode)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{
		if(chr[j]=='\n')
		{
			x=0;
			y+=2;
		}
		else if(chr[j]=='\f') OLED_Clear();
		else
		{
			OLED_ShowBigChar(x,y,chr[j],mode);
			x+=8;
			if(x>160)
			{
				x=0;
				y+=2;
			}
		}
		j++;
	}
}

void OLED_ShowStringXY(uint32_t x,uint32_t y,char *chr,uint8_t mode)
{
   OLED_ShowString(6*x,y,chr,mode);
}

void OLED_ShowBigStringXY(uint32_t x,uint32_t y,char *chr,uint8_t mode)
{
   OLED_ShowBigString(8*x,y,chr,mode);
}

//
//**************************************************************************************************
//	Set cursor to position: ROW, COL
//**************************************************************************************************
//
//void lcd_goto_rc(uint8_t row, uint8_t col)
//{
//    if (row == 0)
//    {
//        lcd_write_cmd(0x80 + col);
//    }
//    else
//    {
//        lcd_write_cmd(0xC0 + col);
//    }
//}


//
//**************************************************************************************************
//	Send one character to display
//**************************************************************************************************
//
//void lcd_putchar(char ch)
//{
//    uint8_t buff[2] = {0xC0, 0x00};
//    
//    buff[1] = ch;
//    HAL_I2C_Master_Transmit(hi2c, LCD_WR_ADDRESS, buff, sizeof(buff), 0xFFFF);
//    
//}


//
//**************************************************************************************************
//	Send string to to display
//**************************************************************************************************
//void lcd_putstr(const char *str)
//{
//    while (*str != 0)
//    {
//        lcd_putchar(*str++);
//    }
//}


