/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27
 **
 **【功能实现】  文本转换语音输出
 **
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5 + SYN6288
 **
 **【硬件重点】  接线，模块有五个引脚，可以只接三根线，即可实现文本转音：
 **              VCC --- 5V
 **              GND --- GND
 **              RXD --- PA2    说明：与所用串口配合，USART1-PA9, USART2-PA2, USART3-PB10, UART4-PC10, UART5-PC12, (本示例所用:USART2-PA2)
 **              TXD --- 空置
 **              BY  --- 空置   说明：模块忙状态引脚，测试时可空置，做真实项目时，建议用于配合检测状态。
 **
 **【移植说明】  1- 复制本工程bsp文件夹中的两个文件夹： USART、SYN6288，到目标工程文件夹中; (注意，本文件须配合bsp_usart.c文件使用)
 **              2- 在keil左侧工程管理器中，双击，把上述两者的c文件添加到工程;
 **              3- 点击魔术棒，在Options.../c.../Include...下添加头文件存放路径;
 **              4- 在代码中，#include "bsp_usart.h＂，和 #include "bsp_syn6288.h＂;
 **
 **【代码使用】  初 始 化：SYN6288_Init(USARTx);       // 使用不同的串口，就接不同的TX线
 **              输出语音：SYN6288_Say("你好吗?");     // 注意，传入参数可为格式化参数，如printf参数般使用
 **
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
#include "bsp_syn6288.h"          // SYN6288_文本转语音模块



float money = 18.5;                                   // 示例金额，仅用作测试



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

    SYN6288_Init(USART2);                             // 初始化; 须配合bsp_usart.c和h实现; 串口：USART1-PA9, USART2-PA2, USART3-PB10, UART4-PC10, UART5-PC12
    // SYN6288_Init("同学们，早上好");                // 示例1：直接输出语音
    // SYN6288_Say("[2]北京[3]东直门站[2]到了");      // 示例2：人工干预切分词组，合语间更自然, 分词参数：2、3
    // SYN6288_Say("[n0]12345,[n1]12345");            // 示例3：用不同的模块，输出数字语音，n0_自然拼读，n1_数字单独拼读
    SYN6288_Say("你的结算金额为: %3.1f元", money);    // 示例4：配合格式代输出带数值的语音
    delay_ms(2000);                                   // 重要： 语句间如果不插入足够的延时，新的语句，会打断正在播放的语句

    while (1)                                         // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(1500);                               // 间隔延时
        LED_RED_TOGGLE ;                              // 规律闪烁LED，方便观察系统是否正常运行
    }
}




