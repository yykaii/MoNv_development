/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  OLED-0.96寸显示屏
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27 + 0.96寸LOED
 **
 **【实验操作】  1-接线
 **                VCC --- 3.3V; 注意, 切勿接5V, 可能产生莫名错误
 **                GND --- GND
 **                SCL --- PB8
 **                SDA --- PB9
 **              2-烧录代码到开发板
 **              3-打开串口上位机即可观察数据输出，及显示屏输出
 **
 **【划 重 点】  1-本代码为软件模拟IIC
 **
 **
 **【更新记录】
 **
 **【备注说明】  版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>            // 头文件引用(标准库); 内核、芯片外设....;(stm32f10x.conf.h, 对标准库头文件进行调用)     
#include "stm32f10x_conf.h"       // 头文件引用(标准库); 内核、芯片外设....;(stm32f10x.conf.h, 对标准库头文件进行调用) 
#include "system_f103.h"
#include "stdio.h"                // C标准库文件，用于调用printf, sprintf等常用函数
#include "bsp_led.h"              // LED指示灯
#include "bsp_key.h"              // 按键
#include "bsp_usart.h"            // USART1、2、3，UART4、5
#include "bsp_oled.h"



static char strTemp[30];  



extern unsigned char BMP1[];

static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖; 本函数仅作粗略延时使用，而并非精准延时;
{
    ms = ms * 11993;                                  // 注意：此参考值运行条件：打勾 Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约多少个空循环耗时1ms
}}



// 主函数
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方
    System_SwdMode();                                 // 设置芯片调试方式(SWD); 关闭JTAG只保留SWD; 目的:释放PB3、PB4、PA15，只需PA13、PA14

    USART1_Init(115200);                              // 串口1初始化; 用于与串口软件通信，方便代码调试; USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出

    Led_Init();                                       // LED 初始化
    LED_BLUE_ON ;                                     // 点亮蓝灯
    LED_RED_ON;                                       // 点亮红灯

    OLED_Init();
    OLED_Fill();
    OLED_GUI();
    

    while (1)                                         // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(500);                                // 间隔延时
        LED_RED_TOGGLE ;                              // 每隔一段时间，就闪烁一次LED，以方便观察代码是否正常运行中

        OLED_DrawBMP(0, 0, 128, 8, BMP1);

    }

}




