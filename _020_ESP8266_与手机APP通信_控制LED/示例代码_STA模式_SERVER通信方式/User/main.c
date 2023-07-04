/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  ESP8266之与手机APP通信
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27 + ESP8266
 **
 **【实验操作】  手机APP端(安卓手机):
 **                1_手机连接WiFi;
 **                2_下载APP："TCP连接";
 **                3_打开APP，并记录界面顶部的:IP地址、端口号
 **              代码操作：
 **                1_修改main文件函数中的WiFi名称、密码，和APP中显示的IP地址、端口号，编译修改至无错误，烧录！
 **                2_ESP8266的TXD和RXD，反接至开发板USARTx上的RX，TX; 代码中所用的是USART3,即PB10,PB11，可自行修改;
 **                3_电脑端打开串口软件，并打开其与板子相关的串口;
 **                4_开发板按复位键，即可自动与APP尝试连接;
 **               测试现象：
 **                1：串口软件中，显示连接状态信息，并实时显示各种收发数据;
 **                2：代码已开启透明传输，串口软件可与APP实现聊天式数据传输;
 **                3：可于APP中设定约定的字符串，当开发板收到这些数据，即可判断操作, 如LED开关，电机控制等
 **
 **【划 重 点】  1_ESP8266连线：VCC接3.3V, IO0、EN、RST这3引脚接3.3V，IO15、GND这2引脚接GND, TXD、RXD这2引脚接开发板的USART;
 **              2_ESP8266信号: 因模块使用的是板载天线，信号比较弱，不能离路由器过远，适合8米内操作;
 **
 **【更新记录】  2021-12-26  完善代码结构、注释
 **              2021-12-03  创建
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
#include "bsp_ESP8266.h"




static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖; 本函数仅作粗略延时使用，而并非精准延时;
{
    ms = ms * 11993;                                  // 注意：此参考值运行条件：打勾 Options/ c++ / One ELF Section per Function
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

    delay_ms(2000);
    ESP8266_Init(UART4, 115200);                      // 初始化;    参数：串口端口，波特率     注意：波特率默认为115200
    ESP8266_JoinAP("CMCC-zml", "55025865502586");     // 加入WiFi;  参数: SSID, Password;      注意：ESP8266、手机所加入的Wifi是同一个AP(路由)
    ESP8266_TCPConnect("192.168.1.2", 1234);          // 建立连接;  参数: 目标IP地址，端口号;  注意：这个IP和端口号，在目标APP“TCP连接”界面的正上方有显示
    ESP8266_EnablePassThrough();                      // 开始透明传输

    while (1)                                         // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(200);                                // 间隔延时

        // 监察：电脑串口软件所发过来的数据，并判断处理
        if (xUSART.USART1ReceivedNum)
        {
            ESP8266_SendString((char *)xUSART.USART1ReceivedBuffer);                   // 把上位机发过来的数据，发送到ESP8266
            printf("\r\n发送数据>>>：%s", (char *)xUSART.USART1ReceivedBuffer);        // 把所发送的数据，输出到串口上位机，方便观察
            xUSART.USART1ReceivedNum = 0;                                              // 清空串口1的接收标志
        }

        // 监察：ESP8266所发过来的数据，并判断处理
        if (ESP8266_CheckReceivedNum())                                                                  // 检查ESP8266所用的串口是否收到数据
        {
            printf("\r\nESP8266收到 %d 字节数据<<<：%s", xESP8266.ReceivedNum, xESP8266.ReceivedBuffer); // 把接收收到的数据，输出到串口上位机，方便观察
            // 判断数据以执行动作
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_RED_ON"))   LED_RED_ON;                     // 判断APP发过来的数据包，是否包含字符串：LED_RED_ON
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_RED_OFF"))   LED_RED_OFF;                   // 判断APP发过来的数据包，是否包含字符串：LED_RED_OFF
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_BLUE_ON"))   LED_BLUE_ON;                   // 判断APP发过来的数据包，是否包含字符串：LED_BLUE_ON
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_BLUE_OFF"))   LED_BLUE_OFF;                 // 判断APP发过来的数据包，是否包含字符串：LED_BLUE_OFF
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_ALL_ON"))
            {
                LED_RED_ON;     // 判断APP发过来的数据包，是否包含字符串：LED_ALL_ON
                LED_BLUE_ON;
            }
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_ALL_OFF"))
            {
                LED_RED_OFF;    // 判断APP发过来的数据包，是否包含字符串：LED_ALL_OFF
                LED_BLUE_OFF;
            }

            if (strstr((char *)xESP8266.ReceivedBuffer, "CLOSED"))         printf("ESP8266与目标的通信连接，已断开！\r\n"); // 当ESP8266发现连接已断开时，会反馈这个数据帧
            ESP8266_CleanReceivedFlag();              // 处理完后，置零ESP8266的接收缓存，方便下一帧数据的接收;
        }
    }
}




