#ifndef __BSP__LCD_ILI93XX_H
#define __BSP__LCD_ILI93XX_H    
/***********************************************************************************************************************************
 *���ļ����ơ�  lcd_ili9341.h
 *���Ա����ӡ�  https://demoboard.taobao.com
 ********************************************************************************************************************************
 *��ʵ��ƽ̨��  ħŮ������_STM32F103RC + KEIL5.27 + 2.8����ʾ��_ILI9341
 *
 *����ֲ˵����  1��������ʹ����F103RC�ϣ�ʹ��IOģ��FSMCͨ�ţ�ע�����ŵ��޸�
 *              2�����ֵ���ʾ��ʹ�ÿ������ϵ��ⲿFLASH���ֿ�
 *     
 *�����¼�¼��
 *              2020-09-xx  ����
 *              2020-12-12  �����޸���Ӣ����ʾ���� 
 *              2020-12-xx  �´����ƣ��������������ֲ�޸�

    
************************************************************************************************************************************/
#include <stm32f10x.h>  
#include <stdio.h>  
#include "system_f103.h"
  


////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 
#define    LCD_LED         PAout(15)                   // LCD����           PC10 


#define    LCD_CS_HIGH     GPIOC->BSRR=0x200   // Ƭѡ�˿�          PC9   1<<9
#define    LCD_RS_HIGH     GPIOC->BSRR=0x100   // ����/����         PC8   1<<8 
#define    LCD_WR_HIGH     GPIOC->BSRR=0x80    // д����            PC7   1<<7
#define    LCD_RD_HIGH     GPIOC->BSRR=0x40    // ������            PC6   1<<6
                                    
#define    LCD_CS_LOW      GPIOC->BRR=0x200    // Ƭѡ�˿�          PC9   1<<9 
#define    LCD_RS_LOW      GPIOC->BRR=0x100    // ����/����         PC8   1<<8  
#define    LCD_WR_LOW      GPIOC->BRR=0x80     // д����            PC7   1<<7
#define    LCD_RD_LOW      GPIOC->BRR=0x40     // ������            PC6   1<<6

// ��Ļ����
#define    LCD_WIDTH       240                 // ��Ļ������أ�ע�⣺0~239
#define    LCD_HIGH        320                 // ��Ļ�߶����أ�ע�⣺0~319
#define    LCD_DIR         0                   // ������ʾ����0-��������3-��������5-������, 6-������
//  ���� ��ֲ�޸� ************************************************************



typedef struct          // �ṹ�壺���ڱ�����Ϣ��״̬
{     
    u8  FlagInit;       // ��ʼ����ɱ�־
    u16 width;          // LCD ���
    u16 height;         // LCD �߶�
    u8  dir;            // ���������������ƣ�0��������1������
    u16 id;
}_LCD;
extern _LCD xLCD;



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
 ** ħŮ������     ����QȺ�ļ���262901124
****************************************************************************/
void LCD_Init (void);                                                        // ��ʼ��
void LCD_DisplayOn(void);                                                    // ����ʾ
void LCD_DisplayOff(void);                                                   // ����ʾ
void LCD_DisplayDir(u8 dir);                                                 // ��ʾ����
void LCD_DrawPoint(u16 x, u16 y, u16 color);                                 // ����
void LCD_Circle(u16 x0,u16 y0,u8 r, u16 color);                              // ��Բ
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);                    // ����
void LCD_Cross( uint16_t x, uint16_t y, uint16_t len, uint32_t fColor);      // ��ʮ����
void LCD_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);               // ������
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);                        // ��䵥ɫ
void LCD_String(u16 x, u16 y, char* pFont, u8 size, u32 fColor, u32 bColor); // ��ʾ��Ӣ���ַ���
void LCD_Image(u16 x, u16 y, u16 width, u16 height, const u8 *image) ;       // ��ʾͼ��
void LCD_GUI(void);                                                          // ���Ƽ򵥽���
         
#endif

