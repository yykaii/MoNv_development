/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  RS485间通信
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27 + RS485(自动换向)
 **
 **【实验操作】  接线:
 **                1_两个或多个RS485端口间，A接A, B接B;
 **                2_接地问题：不用接地;
 **                3_终端电阻：短距离间，不用终端电阻，两三百米以上的才接终端电阻;
 **
 **              代码操作：
 **                1_移植USART文件夹中的bsp_usart.h和bsp_usart.c两文件，到目标工程中; 
 **                2_RS485的TXD和RXD，反接至开发板USARTx上的RX，TX; 代码中所用的是UART4,即PC10,PC11，可自行修改;
 **                3_电脑端打开串口软件，并打开其与板子相关的串口;
 **
 **               测试现象：
 **                1：通过串口软件，可显示收发的数据信息;
 **
 **【划 重 点】  1_RS485间接线：A接A,B接B, 无需反接，无接共地;
 **              2_RS485与USART间接线：485芯片的OUT引脚接USART的RX引脚， 同理，IN引脚接USART的TX引脚;
 **              2_本代码适用自动换向硬件;
 **
 **【更新记录】  2022-04-23  创建
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
#include "bsp_RS485.h"




static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖;
{
    ms = ms * 6500;
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约6500个空循环耗时1ms
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

    RS485_Init(UART4, 115200);                        // 初始化;    参数：串口端口，波特率     注意：波特率默认为115200


    while (1)                                                                           // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(200);                                                                  // 间隔延时

        // 监察：电脑串口软件所发过来的数据，并判断处理。（展示RS485的发送方式)
        if (xUSART.USART1ReceivedNum)
        {
            RS485_SendString((char *)xUSART.USART1ReceivedBuffer);                      // 把上位机发过来的数据，发送到RS485
            printf("\r\n发送数据>>>：%s", (char *)xUSART.USART1ReceivedBuffer);         // 把所发送的数据，输出到串口上位机，方便观察
            xUSART.USART1ReceivedNum = 0;                                               // 清空串口1的接收标志
        }

        // 处理RS485所发过来的数据，并判断处理。（展示RS485的接收方式)
        if (RS485_CheckReceivedNum())                                                   // 检查RS485所用的串口是否收到数据
        {
            printf("\r\n收到字节数 : %dBytes", xRS485.ReceivedNum);                     // 把接收收到的数据，输出到串口上位机，方便观察
            printf("\r\n接收数据<<<：%s", xRS485.ReceivedBuffer);                       // 把接收收到的数据，输出到串口上位机，方便观察
            // 判断数据以执行动作
            if (strstr((char *)xRS485.ReceivedBuffer, "LED_RED_ON"))     LED_RED_ON;    // 判断APP发过来的数据包，是否包含字符串：LED_RED_ON
            if (strstr((char *)xRS485.ReceivedBuffer, "LED_RED_OFF"))    LED_RED_OFF;   // 判断APP发过来的数据包，是否包含字符串：LED_RED_OFF
            if (strstr((char *)xRS485.ReceivedBuffer, "LED_BLUE_ON"))    LED_BLUE_ON;   // 判断APP发过来的数据包，是否包含字符串：LED_BLUE_ON
            if (strstr((char *)xRS485.ReceivedBuffer, "LED_BLUE_OFF"))   LED_BLUE_OFF;  // 判断APP发过来的数据包，是否包含字符串：LED_BLUE_OFF
            RS485_CleanReceivedFlag();                                                  // 重要：处理完数据后，置零RS485的接收缓存，方便下一帧数据的接收;
        }
    }
}




