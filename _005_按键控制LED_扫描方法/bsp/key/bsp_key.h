#ifndef _BSP_KEY_H
#define _BSP_KEY_H
#include <stm32f10x.h>  



/*****************************************************************************
 ** ��ֲ������
****************************************************************************/
// KEY_1_WKUP, ��ʱ����������ʱ���øߵ�ƽ
#define KEY_1_GPIO                GPIOA
#define KEY_1_PIN                 GPIO_Pin_0 
// KEY_2, ��ʱ����������ʱ���õ͵�ƽ
#define KEY_2_GPIO                GPIOA
#define KEY_2_PIN                 GPIO_Pin_1 
// KEY_2, ��ʱ����������ʱ���õ͵�ƽ
#define KEY_3_GPIO                GPIOA
#define KEY_3_PIN                 GPIO_Pin_4
   


#define KEY_ON     1
#define KEY_OFF    0  

/*****************************************************************************
 ** ����ȫ�ֺ���
****************************************************************************/
void    Key_Init(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin , uint8_t KeyDownVoltageStatue);

#endif

