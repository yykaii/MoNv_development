#ifndef __OLED_H
#define __OLED_H
/*****************************************************************************
 ** OLED_0.96����ʾ�� (ģ��IIC��
 **
 ** ���Ž��ߣ� VCC --- 3.3V , ���鲻Ҫ��5.0V��
 **            GND --- GND
 **            SCL --- PB8   (���޸�)
 **            SDA --- PB9   (�������޸�)
 **
 **           OLED���Դ�, ��Ÿ�ʽ����.
 **           [0]0 1 2 3 ... 127
 **           [1]0 1 2 3 ... 127
 **           [2]0 1 2 3 ... 127
 **           [3]0 1 2 3 ... 127
 **           [4]0 1 2 3 ... 127
 **           [5]0 1 2 3 ... 127
 **           [6]0 1 2 3 ... 127
 **           [7]0 1 2 3 ... 127
 **
*****************************************************************************/
#include "stdlib.h"
#include <stm32f10x.h>
#include "stdio.h"



/*****************************************************************************
 ** ��ֲ
 ** ģ��IIc�� 2�����ţ� SCL, SDA
****************************************************************************/
#define OLED_SCL_GPIO   GPIOB
#define OLED_SCL_PIN    GPIO_Pin_8

#define OLED_SDA_GPIO   GPIOB
#define OLED_SDA_PIN    GPIO_Pin_9



/*****************************************************************************
 ** ���� ȫ�ֺ���
****************************************************************************/
void OLED_Init(void);

void OLED_Fill(void);
void OLED_On(void);
void OLED_Off(void);

void OLED_ShowString(uint8_t x, uint8_t y, char *p, uint8_t Char_Size);
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);

void OLED_GUI(void);


#endif


