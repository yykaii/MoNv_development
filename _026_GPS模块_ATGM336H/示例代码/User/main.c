/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  GPS模块—ATGM336H数据获取
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27 + ATGM336H
 **
 **【实验操作】  1-接线
 **                VCC --- 5V
 **                GND --- GND
 **                TX  --- PB11
 **                RX  --- PB10
 **              2-烧录代码到开发板
 **              3-打开串口上位机，即可观察数据输出 
 **
 **【划 重 点】  1-模块无需任何设置，上电就自动接收数据，且自动输出数据到串口(波特率：9600); 编写串口接收代码即可;
 **              2-输出的信息为：NMEA-0183格式，需自行设计代码解读其中信息，如UTC时间，卫星数量，经纬度等;
 **              3-搜不到卫星?？ 加天线！加天线！加天线！
 **              4-还是搜不到卫星?? 放室外！放室外！放室外！室内是几乎搜不到信号的，不要怀疑模块质量问题。
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
#include "bsp_led.h"              // LED指示灯
#include "bsp_key.h"              // 按键
#include "bsp_usart.h"            // USART1、2、3，UART4、5



static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖; 本函数仅作粗略延时使用，而并非精准延时;
{
    ms = ms * 10260;                                  // 注意：此参考值运行条件：打勾 Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约多少个空循环耗时1ms
}



// 主函数
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方
    System_SwdMode();                                 // 设置芯片调试方式(SWD); 关闭JTAG只保留SWD; 目的:释放PB3、PB4、PA15，只需PA13、PA14

    USART1_Init(115200);                              // 串口1初始化; 用于与串口软件通信，方便代码调试; USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出

    Led_Init();                                       // LED 初始化
    LED_BLUE_ON ;                                     // 点亮蓝灯
    LED_RED_ON;                                       // 点亮红灯

    Key_Init();                                       // 按键 初始化

    USART3_Init(9600);                                // GPS模块_ATGM336H

    while (1)                                                                     // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(500);                                                            // 间隔延时

        // 监察：电脑串口软件所发过来的数据，并判断处理
        if (xUSART.USART1ReceivedNum)
        {
            printf("\r\n发送数据>>>：%s", (char *)xUSART.USART1ReceivedBuffer);   // 把所发送的数据，输出到串口上位机，方便观察
            xUSART.USART1ReceivedNum = 0;                                         // 清空串口1的接收标志
        }

        // 输出UART所发过来的数据
        if (xUSART.USART3ReceivedNum > 0)                                         // 检查所用的串口是否收到数据
        {
            printf("\r\n收到字节数 : %dBytes", xUSART.USART3ReceivedNum);         // 把接收收到的数据，输出到串口上位机，方便观察
            printf("\r\n接收数据<<<：\r%s", xUSART.USART3ReceivedBuffer);         // 把接收收到的数据，输出到串口上位机，方便观察

            xUSART.USART3ReceivedNum = 0;                                         // 处理完数据后，置零接收字节数量，方便下一帧数据的接收;
        }
    }
}




