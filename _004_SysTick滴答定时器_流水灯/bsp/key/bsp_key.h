#ifndef _BSP_KEY_H
#define _BSP_KEY_H
#include <stm32f10x.h>  



/*****************************************************************************
 ** ��ֲ������
****************************************************************************/
// KEY_1_WKUP, ��ʱ����������ʱ���øߵ�ƽ
#define KEY_1_GPIO                GPIOA
#define KEY_1_PIN                 GPIO_Pin_0
#define KEY_1_TRIM                EXTI_TRIGGER_RISING    // EXTI_FTIR:1, �½��ش���;   EXTI_RTIR:2,�����ش���
#define KEY_1_IRQN                EXTI0_IRQn             // �ж��������
#define KEY_1_IRQHANDLER          EXTI0_IRQHandler       // �жϷ���������
#define KEY_1_EXTI_LINE           EXTI_Line0             // �ж��߱��
#define KEY_1_GPIOSOURCE          GPIO_PortSourceGPIOA   // �ж��߶˿�
#define KEY_1_PINSOURCE           GPIO_PinSource0    
// KEY_2, ��ʱ����������ʱ���õ͵�ƽ
#define KEY_2_GPIO                GPIOA
#define KEY_2_PIN                 GPIO_Pin_1
#define KEY_2_TRIM                EXTI_TRIGGER_FALLING   // EXTI_FTIR:1, �½��ش���;   EXTI_RTIR:2,�����ش���
#define KEY_2_IRQN                EXTI1_IRQn             // �ж��������
#define KEY_2_IRQHANDLER          EXTI1_IRQHandler       // �жϷ�����
#define KEY_2_GPIOSOURCE          GPIO_PortSourceGPIOA   // �ж��߶˿�
#define KEY_2_EXTI_LINE           EXTI_Line1             // �ж��߱��
#define KEY_2_PINSOURCE           GPIO_PinSource1   
// KEY_2, ��ʱ����������ʱ���õ͵�ƽ
#define KEY_3_GPIO                GPIOA
#define KEY_3_PIN                 GPIO_Pin_4
#define KEY_3_TRIM                EXTI_TRIGGER_FALLING   // EXTI_FTIR:1, �½��ش���;   EXTI_RTIR:2,�����ش���
#define KEY_3_IRQN                EXTI4_IRQn             // �ж��������
#define KEY_3_IRQHANDLER          EXTI4_IRQHandler       // �жϷ���������
#define KEY_3_GPIOSOURCE          GPIO_PortSourceGPIOA   // �ж��߶˿�
#define KEY_3_EXTI_LINE           EXTI_Line4             // �ж��߱��
#define KEY_3_PINSOURCE           GPIO_PinSource4    


#define KEY_ON     1
#define KEY_OFF    0  

/*****************************************************************************
 ** ����ȫ�ֺ���
****************************************************************************/
void    Key_Init(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin , uint8_t KeyDownVoltageStatue);

#endif

