/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  用库函数的方式点亮LED_引脚电平控制
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27
 **
 **【实验操作】  USB线，插到开发板CMSIS-DAP接口上;
 **              因使用的是板载的LED，因而无需其它接线及器材;
 **              点击软件左上角的编译，编译无错后，点击LOAD，即可烧录至开发板;
 **
 **【划 重 点】  红色LED，已连接芯片的PC5引脚，至低电平可通路;
 **              蓝色LED，已连接芯片的PB2引脚，至低电平可通路;
 **              标准库，即库函数，必须包括在工程内，才能使用; 左侧工程文件管理器中FWLB文件夹，存放的就是标准库的支持文件;
 **              要使用库函数，必须在代码文件中引用相关的头文件，具体操作：在文件中引用stm32f10x_conf.h文件，并在其中取消相关文件的注释符号;
 **
 **【更新记录】  2022-01-25  完善代码结构、注释
 **              2021-12-03  创建
 **
 **【备注说明】  版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>            // 头文件引用(标准库); 内核、芯片外设....;(stm32f10x.conf.h, 对标准库头文件进行调用)     
#include "stm32f10x_conf.h"       // 头文件引用(标准库); 内核、芯片外设....;(stm32f10x.conf.h, 对标准库头文件进行调用) 




int main(void)                                              // 主函数, 整个工程的用户代码起始点
{
    GPIO_InitTypeDef GPIO_InitStructure;                    // 定义GPIO配置的结构体

    // 配置红色LED, PC5, 置低电平通路
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // 使能GPIOC时钟
   
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;             // 选择要控制的GPIO引脚
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 设置引脚模式为通用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚速率为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);                  // 调用库函数，初始化PC5

    // 配置蓝色LED, PB2, 置低电平通路
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // 使能GPIOB时钟
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;             // 选择要控制的GPIO引脚
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 设置引脚模式为通用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚速率为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  // 调用库函数，初始化PB2

    GPIO_ResetBits(GPIOC, GPIO_Pin_5);                      // 点亮红色LED：PC5置低电平
    GPIO_SetBits(GPIOB, GPIO_Pin_2);                        // 熄灭蓝色LED：PB2置高电平


    while (1)                                               // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
    }
}




