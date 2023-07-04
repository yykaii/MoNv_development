/*==================================================================================================================
 **【文件名称】  main.c
 **【功    能】  串口+中断收发
 ===================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27 + ESP8266
 **
 **【划 重 点】 1_引脚初始化时，注意引脚的工作模式
 **             2_USART1(PA9,PA10), USART2(PA2,PA3), USART3(PB10,PB11), UART4(PC10,PC11), UART5(PC12,PD2)
 **             3_注意：USART2所用的PA2、PA3接口，不支持5V电平信号;
 **
 **【操作说明】 1_USB线插到板上的CMSIS DAP接口，即可完成供电、烧录、串口通信的接线;
 **             2_打开任意一串口软件，本示例中使用的是火哥串口软件; 
 **             3_串口软件参数设置：115200-N-8-1, 选择相应的串口端口，并点击“打开串口”
 **
 **【文件移植】 1：复制bsp文件夹下的USART文件夹，粘贴到相应的工程文件夹里。USART文件夹内有两个文件：bsp_usart.c、bsp_usart.h；
 **             2：在keil左侧工程文件管理器Project中，双击某文件夹，以添加文件bsp_usart.c
 **             3: 在keil里添加路径：魔术棒->c/c++->include path...增加(刚才粘贴文件夹的位置)； 
 **
 **【代码使用】 1：在要使用USART功能的文件中，添加：#include “bsp_usart.h";
 **             2: 发送, 两个函数：USARTx_SendString(字符串地址) 和 USARTx_SendData(数据地址，字节长度);
 **             3: 接收，判断xUSART.USARTxReceivedNum这个变量是否大于0，即可知道是否收到新数据; 注意，处理完数据后，记得清0这个变量
 **         
 **【更    新】 
 **             2022-01-25  修改发送示例; 完善注释
 **             2021-11-30  修改文件名称
 **             2020-12-18  完善代码结构、完善注释格式
 **
 **【备注说明】 版权归魔女科技所有，请勿商用，谢谢！ 
 **             https://demoboard.taobao.com 
===================================================================================================================*/
#include <stm32f10x.h>            // 头文件引用(标准库); 内核、芯片外设....;(stm32f10x.conf.h, 对标准库头文件进行调用)     
#include "stm32f10x_conf.h"       // 头文件引用(标准库); 内核、芯片外设....;(stm32f10x.conf.h, 对标准库头文件进行调用) 
#include "bsp_led.h"              // LED指示灯
#include "bsp_key.h"              // 按键支持文件
#include "bsp_usart.h"            // 串口支持文件



static void delay_ms(uint32_t ms)                      // 定义一个ms延时函数，减少移植时对外部文件依赖;
{
    ms=ms*6500;                  
    for(uint32_t i=0; i<ms; i++);                      // 72MHz系统时钟下，大约6500个空循环耗时1ms
}



int main(void)                                         // 主函数, 整个工程的用户代码起始点
{   
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方
    USART1_Init(115200);                               // 串口1初始化; 用于与串口软件通信，方便代码调试; USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出
      
    Led_Init();                                        // LED 初始化
    LED_BLUE_ON ;                                      // 点亮蓝灯
    LED_RED_ON;                                        // 点亮红灯
        
    Key_Init();                                        // 按键初始化  
    delay_ms(200);        
    
    // 测试1：发送字符串       
    USART1_SendString("测试1：发送字符串，Test.\r\n"); 
    
    // 测试2：发送各类数值数据, 注意，串口软件可用16进制显示              
    uint8_t data[30];             // 将要发送的数据缓存
    char* strTemp="测试2：";      // 字符串
    data[0]= *strTemp++;          // 汉字的GBK编码，使用2个字节编码来表示一个汉字
    data[1]= *strTemp++;   
    data[2]= *strTemp++;   
    data[3]= *strTemp++; 
    data[4]= *strTemp++;   
    data[5]= *strTemp++;  
    data[6]= *strTemp++;         
    data[7]= 123;                 // 整型，注意：如果用ASCII方式显示到串口上，将为‘{’
    data[8]= '5';                 // 字符
    data[9]= '2';                 // 字符
    data[10]= '0';                // 字符
    data[11]=  5 ;                // 整型，注意：数据在ASCII编码非可显示的范围内，在串口软上，用ASCII方式将无法显示. 要用16进制显示才能看到数据
    data[12]=  2 ;                // 整型    
    data[13]=  0 ;                // 整型
    data[14]=0x05;                // 整型，注意：很多人没能理解16进制和十进制的区别，在数据传输上，是没有区别的，下方三个数值，等同于上方5,2,0三个数值
    data[15]=0x02;                // 整型
    data[16]=0x00;                // 整型
    USART1_SendData (data, 20);                       // USART1_SendData函数，可以很灵活地发送各种类型数据
    
    
    while(1)                                          // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    { 
        delay_ms(200);                                // 间隔延时
        LED_RED_TOGGLE;                               // 规律地闪烁红色LED，以方便观察系统是否正常运行
        
        // 测试3：接收从串口上位发送过来的数据，并再输出到串口上位机
        if(xUSART.USART1ReceivedNum )                                        // 判断是否接收到新数据：接收字节长度>0
        {
            printf("\r接收到%d个字节",  xUSART.USART1ReceivedNum);           // 准备输出, 格式   
            printf("\r收到的数据<<<：%s\r\n", xUSART.USART1ReceivedBuffer);  // 准备输出, 格式   
            xUSART.USART1ReceivedNum=0;                                      // 重要，处理完数据后，必须把接收字节长度清零
        }                   
    }          
}




