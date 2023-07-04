/*==================================================================================================================
 *【文件名称】  main.c
 *【功    能】  PWM信号生成
 ===================================================================================================================
 *【实验说明】
 *
 *【函数简介】  为简化及统一代码，已封装成２个对外函数，可完成各种基本操作
 **             TIM_PwmInit();              // 配置引脚、TIM时基、PWM周期
 *              TIM_SetCCR();               // 设置脉宽
 *
 *【更新记录】  2021-05-16  创建
 *              2022-04-21  修改各文件名称、函数名称
 *
 *【备注说明】  版权归魔女科技所有，请勿商用，谢谢！
 *              https://demoboard.taobao.com
===================================================================================================================*/
#include <stm32f10x.h>
#include "stm32f10x_conf.h"                  // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)  
#include "bsp_led.h"                         // LED指示灯
#include "bsp_key.h"                         // 按键
#include "bsp_usart.h"                       // USART1、2、3，UART4、5
#include "bsp_pwm.h"



static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖;
{
    ms = ms * 6500;
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约6500个空循环耗时1ms
}



int main(void)
{
    USART1_Init(115200);                                     // 初始化USART1作为调试信息输出

    Led_Init();                                              // LED 初始化
    LED_RED_ON ;
    LED_BLUE_ON ;

    // 测试1: PA0_TIM5_CH1(10K下拉, 并联100nF电容到GND)
    TIM_PwmInit(GPIOA, GPIO_Pin_0, TIM5, 1, 72, 1000, 0);    // 示例1， PA0引脚，TIM5_CH1, 1脉冲周期1us(即72000000/72), PWM信号周期: 1ms(即1us x 1000), 初始脉宽0ms(即占空比的时长）
    TIM_SetCCR(TIM5, 1, 500);                                // 改变PA0的脉宽输出：500us(因为上一步设置成了1脉冲为1us,所以500个脉冲信号为500us)

    // 测试2: PA1_TIM5_CH2(并联100nF电容到GND)
    TIM_PwmInit(GPIOA, GPIO_Pin_1, TIM5, 2, 72, 1000, 0);    // 示例2， PA1引脚，TIM5_CH2, 1脉冲周期1us(即72000000/72), PWM信号周期: 1ms(即1us x 1000), 初始脉宽0ms(即占空比的时长）
    TIM_SetCCR(TIM5, 2, 50);                                 // 改变PA1的脉宽输出：50us(因为上一步设置成了1脉冲为1us,所以50个脉冲信号为50us)

    // 测试2: PA10_TIM1_CH3(10K上拉)
    TIM_PwmInit(GPIOA, GPIO_Pin_10, TIM1, 3, 72, 20, 0);     // 示例3， PA10引脚，TIM1_CH3, 1脉冲周期1us(即72000000/72), PWM信号周期: 20us(即1us x 20), 初始脉宽0ms(即占空比的时长）
    TIM_SetCCR(TIM1, 3, 5);                                  // 改变PA10的脉宽输出：5us(因为上一步设置成了1脉冲为1us,所以5个脉冲信号为5us)


    while (1)                                                // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(500);                                       // 延时
        LED_RED_TOGGLE;                                      // 红色LED 每0.5秒闪灭一次，以监察系统正常工作
    }
}






