#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include <stm32f10x.h>
#include "stm32f10x_tim.h"



/*****************************************************************************
 ** ȫ�ֱ��� (��Ҫ�޸�)
****************************************************************************/
typedef struct 
{
    uint16_t   TIM6_CNT;                         // �жϴ�������
    uint16_t   TIM7_CNT; 
    
}xTIM_TypeDef;

extern xTIM_TypeDef  xTIM;                      // ����Ϊȫ�ֱ���,�����¼��Ϣ��״̬
    



void TIM6_Init(uint16_t PSC, uint16_t ARR, FunctionalState NewState);


#endif