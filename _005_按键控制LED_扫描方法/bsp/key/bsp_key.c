#include "bsp_key.h"






void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                    // 作用：配置引脚工作模式 
    
    // 使能GPIO时针, 为减少调试过程忘了使能时钟而出错，把相关GPIO端口时钟，都使能了;
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF ; 
   
    // KEY_1
    GPIO_InitStructure.GPIO_Pin  = KEY_1_PIN;               // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;          // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;      // 输出电平反转速度;在输入状态时无效，但GPIO_Init函数需要用到;
    GPIO_Init(KEY_1_GPIO, &GPIO_InitStructure);             // 初始化，调用引脚工作模式配置函数
    
    // KEY_2
    GPIO_InitStructure.GPIO_Pin  = KEY_2_PIN;               // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;          // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;      // 输出电平反转速度;在输入状态时无效，但GPIO_Init函数需要用到;
    GPIO_Init(KEY_2_GPIO, &GPIO_InitStructure);             // 初始化，调用引脚工作模式配置函数

    // KEY_3
    GPIO_InitStructure.GPIO_Pin  = KEY_3_PIN;               // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;          // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;      // 输出电平反转速度;在输入状态时无效，但GPIO_Init函数需要用到;
    GPIO_Init(KEY_3_GPIO, &GPIO_InitStructure);             // 初始化，调用引脚工作模式配置函数
}



/******************************************************************************
 * 函  数： Key_Scan
 * 功  能： 扫描按键状态
 * 参  数： GPIOx：     GPIO端口 
 *          PINx :      Pin引脚编号
 *          downStatus: 按下时的电平
 *
 * 返回值： KEY_ON : 按下
 *          KEY_OFF：无动作
 *     
 * 说  明： 1: 本函数检测方式为“while不断扫描”，发现按键按下且等待按键松开，为一次有效按下动作;   
 *          2：本方法建议只用示例使用，实际项目中，建议使用中断方法检测;
 ******************************************************************************/ 
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t PINx , uint8_t downStatus)
{	
    if(GPIO_ReadInputDataBit(GPIOx, PINx)==downStatus)
    {   
        while(GPIO_ReadInputDataBit(GPIOx, PINx)==downStatus);
        return KEY_ON;
    }		
    return KEY_OFF;	     
}




