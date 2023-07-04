/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  DHT11-温湿度获取
 **==================================================================================================================
 **【适用平台】 STM32F103 + KEIL5.27 + 标准库v3.5 + DHT11
 **
 **【实验操作】  1-模块接线，VCC  接 3.3V 或 5V
 **                          DATA 接 PC3
 **                          GND  接 GND
 **              2-烧录代码至开发板，并重新上电或复位，使新程序运行；
 **              3-打开电脑串口上位机，115200-N-8-1, 即可观察到采集的数据；
 **
 **
 **【划 重 点】  1-电压范围：3.3V~5.0V，工作电流：0.2~1mA; 待机电流：150uA
 **              2-上电稳定：不同店家的DHT11, 上电后进入稳定状态需时不同，约1s~6s; 当发现刚开始采集数据时是错误的，而后面的数据正确，可增加第一次采集数据前的数据延时;
 **              3-采样周期，不同店家的DHT11，采样间隔要求不同：约0.5~2秒; 当移植后发现采样出错，可尝试增加采样的间隔时间;
 **              4-DHT11时序要求较高，us级别，当使用delay粗略延时，同一份代码同一个模块，在不同keil里跑也可能发生错误(代码优化设置对while延时的影响)。建议使用TIM的精准延时;
 **              5-注意判断使用的DHT11器件，是单独的DHT11元件，还是完整的DHT11模块
 **                单独的DHT11元件：蓝色塑料主体、三个引脚，不带上拉电阻；适合低成本工程方案使用，建议测试时引脚使用推挽输出模式；
 **                完整的DTH11模块：蓝色塑料主体、三个引脚、PCB底板、上拉电阻、电源指示LED，适合方案搭建；
 **              6-精度误差：DHT11分旧版和新版，10元内的绝大部分是旧版，新版约12元左右；
 **                          旧版数据稳定，但只有整数部分，新版有小数部分，但数据跳动稍大；
 **                          温度0-50°C; 精度±2°C; 小数部份无效
 **                          湿度20-80%; 精度±5%; 小数部分无效
 **
 **
 **【文件移植】  步骤1-复制文件：可复制bsp_DHT11.c和bsp_DHT11h两个文件，或复制DHT11文件夹，保存到所需工程目录文件夹下；
 **              步骤2-添加文件：在keil工各程左侧文件管理器中，双击某文件夹，以添加bsp_DHT11.c文件;
 **              步骤3-添加路径：点击魔术棒工具按钮，在“c/c++"选项页中，点击”Include Path"后面的按键，以添加文件存放所在路径(是文件夹，不是文件);
 **　　　　　    步骤4-添加引脚：在所需DHT11功能的代码文件头部，添加：#include "bsp_DHT11.h";
 **
 **
 **【函数使用】  函数2-DHT11_GetData(GPIOx, GPIO_Pin_x);           // 获取数据, 获取的数据存放于结构体xDHT11中，具体使用方法，可参考示例代码
 **
 **
 **【备注说明】  代码版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>            // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)     
#include "stm32f10x_conf.h"       // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用) 
#include "bsp_led.h"              // LED指示灯
#include "bsp_usart.h"            // USART1、2、3，UART4、5
#include "bsp_DHT11.h"            // DHT11



float temperature = 0.0;          // 用于存储温度值; DHT11的精度，小数部分无效
float humidity = 0.0;             // 用于存储湿度值; DHT11的精度，小数部分无效



static void delay_ms(uint32_t ms)                              // 定义一个ms延时函数，减少移植时对外部文件依赖; 本函数仅作粗略延时使用，而并非精准延时;
{
    ms = ms * 10250;                                           // 注意：此参考值运行条件：打勾 Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);                         // 72MHz系统时钟下，大约多少个空循环耗时1ms
}



// 主函数
int main(void)
{
    USART1_Init(115200);                                        // 串口初始化：USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出

    Led_Init();                                                 // LED 初始化
    LED_RED_ON;                                                 // 点亮红灯

        
    while (1)                                                   // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(1000);                                         // 延时1000ms，注意：DHT11的采样间隔，一般要1s左右或以上，不同厂家要求不同
        LED_RED_TOGGLE;                                         // 红色LED，间隔亮灭，以判断系统正常工作
            
        printf("DHT11  ");
            
        temperature = DHT11_GetTemperature(GPIOC, GPIO_Pin_3);  // 获取DHT11温度值, 注意，DHT11的精度，小数部分无效
        printf("温度:%4.1f℃   ", temperature);                  // 把数据输出到电脑串口软件，方便观察
            
        humidity =  DHT11_GetHumidity(GPIOC, GPIO_Pin_3);       // 获取DHT11湿度值, 注意，DHT11的精度，小数部分无效
        printf("湿度:%4.1fRH%%\r\n", humidity);                 // 把数据输出到电脑串口软件，方便观察
    }
}




