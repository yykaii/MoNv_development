/**==================================================================================================================
 ** 【文件名称】  main.c
 **
 ** 【实现功能】  USB虚拟串口
 **==================================================================================================================
 **
 ** 【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 ** 【移植说明】  1- 复制工程文件夹下USB文件夹，到目标位置;
 **               2- 在目标工程中，引用如本示例: 用户配置CONFIG文件夹6个c文件，及无需修改的核心文件6个c文件;
 **               3- 在所需位置：#include "bsp_usb.h" ，即可调相关函数
 **
 **
 ** 【划 重 点】  1-WIN7 需要安装驱动，不然无法识别; WIN10 无需安装驱动;
 **               2-适合电路：PA12经1.5K电阻恒上拉到3.3V的电路; 不适合PA12上拉电阻经三级管控制的电路;
 **               3-F103系列芯片中，USB与CAN共用同一芯片缓存，不能同时使用。
 **               4-同上，检查引脚，是否有跳线帽切换：USB/CAN。
 **               5-重中之重：每帧数据，最大有效长度：64字节; 超过这个长度，需要另行编写代码实现分包处理;
 **               6-USB虚拟串口的枚举过程: 由端点0中断根据已设置好的描述符，自动完成
 **               7-USB虚拟串口的发送处理，由端点1中断处理;(只要把数据和长度，写入缓存区，然后使能端点1即可自动发送)
 **               8-USB虚拟串口的接收处理，由端点3中断处理;(发生中断时，读取相应缓存区，即可得到所接收的有效长度，和数据)
 **
 **
 ** 【代码说明】  1-重中之重：每帧数据，最大有效长度：64字节; 超过这个长度，需要另行编写代码实现分包处理;
 **
 **               2-初始化：  本函数，适合PA12经1.5K电阻恒上拉的电路;
 **                           ---USB_Config();                                // 调用后，即可完成所有配置工作
 **
 **               3-发送数据：通过下面两个函数，把数据存入发送缓存，端点1中断回调函数自行实现被主机读取数据;
 **                           ---USB_SendDatas(uint8_t *buf, uint16_t len);   // 发送指定长度的数据
 **                           ---USB_SendString(char *fmt, ...);              // 发送字符串; 使用方式如同printf
 **
 **               4-接收数据：数据的接收由端点3中断回调函数自动实现，我们只需处理接收到的数据即可;
 **                           ---数据长度 xUSB.ReceivedNum > 0                // xUSB.ReceivedNum，为最后一帧数据的负载长度，由 EP3_OUT_Callback() 刷新; 数据使用后，需手动清0
 **                           ---数据缓存 xUSB.ReceivedBuffer                 // xUSB.ReceivedBuffer[], 为最后一帧数据的负载缓存，由 EP3_OUT_Callback() 刷新; 注意，未做超64字节的分包处理
 **
 **
 ** 【更新记录】  2022-06-02  参考移植于正点原子示例
 **
 **
====================================================================================================================*/
#include <stm32f10x.h>            // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)     
#include "stm32f10x_conf.h"       // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用) 
#include "bsp_led.h"              // LED指示灯
#include "bsp_usart.h"            // USART1、2、3，UART4、5
#include "bsp_usb.h"              // USB配置文件



// ms延时函数，减少移植时对外部文件依赖；
static void delay_ms(uint32_t ms)
{
    ms = ms * 10240;
    for (uint32_t i = 0; i < ms; i++); // 72MHz系统时钟下，多少个空循环约耗时1ms
}



// 主函数
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);           // 使能AFIO辅助功能IO时钟
    AFIO->MAPR |= 0x2 << 24;                                       // 设置芯片调试方式(SWD): 000_全开, 010_只开SWD, 100:全关; 作用:关闭JTAG只保留SWD, 即释放PB3、PB4、PA15，只需PA13、PA14

    USART1_Init(115200);                                           // 串口初始化：USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出; 特别注意：调用printf()前，必须先初始化其关联的USARTx,不然程序会卡死

    Led_Init();                                                    // LED 初始化; 板载LED的重要作用，配合代码调试，能方便地定位错误
    LED_RED_OFF;                                                   // 红灯，这里用作程序运行指示，
    LED_BLUE_OFF;                                                  // 蓝灯，这里用作USB连接指示：亮-连接成功，灭-未连接

    USB_Config();                                                  // USB初始化; 本函数，适合PA12经1.5K电阻恒上拉到3.3V的电路，不适合PA12上拉电阻经三级管控制的电路;

    while (1)                                                      // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(200);                                             // 延时
        LED_RED_TOGGLE;                                            // 红色LED，间隔亮灭，以判断系统正常工作

        /****** 判断USB连接状态是否改变 ******/
        if (xUSB.status != bDeviceState)                           // USB连接状态发生了改变.
        {
            xUSB.status = bDeviceState;                            // 记录新的状态
            if (xUSB.status == CONFIGURED)
            {
                printf("USB虚拟串口：连接成功\r\n");               // 提示：成功
                LED_BLUE_ON ;                                      // 蓝色LED：亮
            }
            else
            {
                printf("USB虚拟串口：连接失败\r\n");               // 提示：失败
                LED_BLUE_OFF ;                                     // 蓝色LED：灭
            }
        }

#if 1
        /****** 在一个串口软件上发送、显示******/
        if (xUSB.ReceivedNum > 0)                                           // 当 USB虚拟串口 收到数据时
        {
            USB_SendString("USB收到: %dBytes\r\n", xUSB.ReceivedNum);       // 把收到的字节数量，发回与USB虚拟串口关联的串口软件
            USB_SendString("USB收到数据<<<:%s\r\r", xUSB.ReceivedBuffer);   // 把收到的内容，发回与USB虚拟串口关联的串口软件
            xUSB.ReceivedNum = 0;                                           // 清0数据长度，表示已处理完这一帧数据
        }
#else
        /****** USART1串口与USB虚拟串口间互相收发 ******/
        if (xUSART.USART1ReceivedNum > 0)                                                         // 当 USART1 收到数据时
        {
            USART1_SendString("USART1收到%d字节数据\r\n",  xUSART.USART1ReceivedNum);             // 把收到的字节数，发回与USART1关联的串口软件
            USART1_SendString("收到的数据<<<：%s\r", xUSART.USART1ReceivedBuffer);                // 把收到的内容，发回与USART1关联的串口软件
            USB_SendString("经USB虚拟串口发出的数据:%s\r", (char *)xUSART.USART1ReceivedBuffer);  // 把收到的内容，经USB虚拟串口，发送给与虚拟串口关联的串口软件
            xUSART.USART1ReceivedNum = 0;                                                         // 清0数据长度，表示已处理完这一帧数据
        }

        if (xUSB.ReceivedNum > 0)                                           // 当 USB虚拟串口 收到数据时
        {
            USB_SendString("USB收到: %dBytes\r\n", xUSB.ReceivedNum);       // 把收到的字节数量，发回与USB虚拟串口关联的串口软件
            USB_SendString("USB收到数据<<<:%s\r\r", xUSB.ReceivedBuffer);   // 把收到的内容，发回与USB虚拟串口关联的串口软件
            USART1_SendString((char *)xUSB.ReceivedBuffer);                 // 把收到的内容，经 USART1 串口，发送给与USART1关联的串口软件
            xUSB.ReceivedNum = 0;                                           // 清0数据长度，表示已处理完这一帧数据
        }
#endif
    }
}


