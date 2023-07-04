#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include <stm32f10x.h>
#include "stm32f10x_tim.h"



/*****************************************************************************
 ** 全局变量 (无要修改)
****************************************************************************/
typedef struct 
{
    uint16_t   TIM6_CNT;                         // 中断次数计数
    uint16_t   TIM7_CNT; 
    
}xTIM_TypeDef;

extern xTIM_TypeDef  xTIM;                      // 声明为全局变量,方便记录信息、状态
    



void TIM6_Init(uint16_t PSC, uint16_t ARR, FunctionalState NewState);


#endif