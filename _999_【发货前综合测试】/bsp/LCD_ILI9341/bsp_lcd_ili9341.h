#ifndef __BSP__LCD_ILI93XX_H
#define __BSP__LCD_ILI93XX_H
/**==================================================================================================================
 **���ļ����ơ�  bsp_lcd_2.8_ili93xx.h
 **�����ܲ��ԡ�  ��ʾ��
 **==================================================================================================================
 **������ƽ̨��  STM32F103 + KEIL5.27 + 2.8����ʾ��_ILI9341
 **
 **��ʵ�������
 **
 **
 **���ļ���ֲ��
 **
 **�����¼�¼��  2022-05-22  �޸��������ô��룬ʹ�ñ�׼��
 **              2020-12-12  �����޸���Ӣ����ʾ����
 **              2020-09     ����
 **
 **����ע˵����  �����Ȩ��ħŮ�Ƽ����У��������ã�лл��
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>
#include <stdio.h>
#include "bsp_w25qxx.h"
#include "bsp_usart.h"


/*****************************************************************************
 ** ��ֲ����
****************************************************************************/
// BL_����  
#define    LCD_BL_GPIO     GPIOA              
#define    LCD_BL_PIN      GPIO_Pin_15
// BL_����  
#define    LCD_RD_GPIO     GPIOC              
#define    LCD_RD_PIN      GPIO_Pin_6
// BL_����  
#define    LCD_WE_GPIO     GPIOC              
#define    LCD_WE_PIN      GPIO_Pin_7
// BL_����  
#define    LCD_RS_GPIO     GPIOC             
#define    LCD_RS_PIN      GPIO_Pin_8
// BL_����  
#define    LCD_CS_GPIO     GPIOC              
#define    LCD_CS_PIN      GPIO_Pin_9

// ��Ļ����
#define    LCD_WIDTH       240             // ��Ļ������أ�ע�⣺0~239
#define    LCD_HIGH        320             // ��Ļ�߶����أ�ע�⣺0~319
#define    LCD_DIR         0               // ������ʾ����0-��������3-��������5-������, 6-������
//  ���� ��ֲ�޸� ************************************************************



typedef struct          // �ṹ�壺���ڱ�����Ϣ��״̬
{
    u8  FlagInit;       // ��ʼ����ɱ�־
    uint16_t width;     // LCD ���
    uint16_t height;    // LCD �߶�
    u8  dir;            // ���������������ƣ�0��������1������
    uint16_t id;
} xLCD_TypeDef;
extern xLCD_TypeDef xLCD;



/******************************* ���� ILI934 ��ʾ��������ɫ ********************************/
#define      WHITE               0xFFFF       // ��ɫ
#define      BLACK               0x0000       // ��ɫ 
#define      GREY                0xF7DE       // ��ɫ 
#define      RED                 0xF800       // �� 
#define      MAGENTA             0xF81F       // ���ɫ 
#define      GRED                0xFFE0       // ���ɫ
#define      GREEN               0x07E0       // �� 
#define      CYAN                0x7FFF       // ��ɫ 
#define      YELLOW              0xFFE0       // ��ɫ 
#define      LIGHTGREEN          0X841F       // ǳ��ɫ 
#define      BLUE                0x001F       // �� 
#define      GBLUE               0x07FF       // ǳ�� 1
#define      LIGHTBLUE           0X7D7C       // ǳ�� 2
#define      BLUE2               0x051F       // ǳ�� 3
#define      GRAYBLUE            0X5458       // ���� 
#define      DARKBLUE            0X01CF       // ����

#define      LGRAY               0XC618       // ǳ��ɫ,���屳��ɫ
#define      LGRAYBLUE           0XA651       // ǳ����ɫ(�м����ɫ)
#define      LBBLUE              0X2B12       // ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)



/*****************************************************************************
 ** ����ȫ�ֺ���

****************************************************************************/
void LCD_Init(void);                                                                             // ��ʼ��
void LCD_DisplayOn(void);                                                                        // ����ʾ
void LCD_DisplayOff(void);                                                                       // ����ʾ
void LCD_DisplayDir(u8 dir);                                                                     // ��ʾ����
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);                                      // ����
void LCD_Circle(uint16_t x0, uint16_t y0, u8 r, uint16_t color);                                 // ��Բ
void LCD_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);               // ����
void LCD_Cross(uint16_t x, uint16_t y, uint16_t len, uint32_t fColor);                           // ��ʮ����
void LCD_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);          // ������
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);               // ��䵥ɫ
void LCD_String(uint16_t x, uint16_t y, char *pFont, u8 size, uint32_t fColor, uint32_t bColor); // ��ʾ��Ӣ���ַ���
void LCD_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const u8 *image) ;       // ��ʾͼ��
void LCD_GUI(void);                                                                              // ���Ƽ򵥽���

#endif

