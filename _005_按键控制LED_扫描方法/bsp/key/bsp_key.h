#ifndef _BSP_KEY_H
#define _BSP_KEY_H
#include <stm32f10x.h>  



/*****************************************************************************
 ** 移植配置区
****************************************************************************/
// KEY_1_WKUP, 闲时下拉，按下时被置高电平
#define KEY_1_GPIO                GPIOA
#define KEY_1_PIN                 GPIO_Pin_0 
// KEY_2, 闲时上拉，按下时被置低电平
#define KEY_2_GPIO                GPIOA
#define KEY_2_PIN                 GPIO_Pin_1 
// KEY_2, 闲时上拉，按下时被置低电平
#define KEY_3_GPIO                GPIOA
#define KEY_3_PIN                 GPIO_Pin_4
   


#define KEY_ON     1
#define KEY_OFF    0  

/*****************************************************************************
 ** 声明全局函数
****************************************************************************/
void    Key_Init(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin , uint8_t KeyDownVoltageStatue);

#endif

