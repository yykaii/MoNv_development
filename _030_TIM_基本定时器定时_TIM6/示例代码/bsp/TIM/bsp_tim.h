#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include <stm32f10x.h>
#include "stm32f10x_tim.h"



/*****************************************************************************
 ** ȫ�ֱ��� (��Ҫ�޸�)
****************************************************************************/
typedef struct 
{
    uint16_t   TIM6Counter;                     // �����жϵĴ����������Ϊ�Ѳ����˶��ٴ�����
    uint16_t   TIM7Counter;                     // �����жϵĴ���
    
}xTIM_TypeDef;

extern xTIM_TypeDef  xTIM;                      // ����Ϊȫ�ֱ���,�����¼��Ϣ��״̬
    


// ������ʱ�� TIM6
void TIM6_Init(uint16_t PSC, uint16_t ARR);     // ��ʼ��TIM6; PSC-ʱ�ӷ�Ƶֵ��ARR-�Զ���װ��ֵ(���ٸ���Ƶ������壬���һƵ������)
void TIM6_SetARR(uint16_t ARR);                 // ����TIM6���Զ���װ��ֵ����Ƶ������ֵ(���ٸ���Ƶ������壬���һ����)
void TIM6_Start(void);                          // ����TIM6, ʹ��TIM6_Init���������ò���
void TIM6_Stop(void);                           // ֹͣTIM6, ԭ���ò�����0;
// ������ʱ�� TIM7
void TIM7_Init(uint16_t PSC, uint16_t ARR);     // ��ʼ��TIM7; PSC-ʱ�ӷ�Ƶֵ��ARR-�Զ���װ��ֵ(���ٸ���Ƶ������壬���һƵ������)
void TIM7_SetARR(uint16_t ARR);                 // ����TIM7���Զ���װ��ֵ����Ƶ������ֵ(���ٸ���Ƶ������壬���һ����)
void TIM7_Start(void);                          // ����TIM7, ʹ��TIM6_Init���������ò���
void TIM7_Stop(void);                           // ֹͣTIM7, ԭ���ò�����0;


#endif



