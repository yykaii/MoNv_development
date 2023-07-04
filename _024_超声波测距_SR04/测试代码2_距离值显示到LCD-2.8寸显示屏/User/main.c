/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  DHT11-温湿度获取
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27 + DHT11
 **
 **【实验操作】  1-模块接线，VCC  接 3.3V
 **                          DATA 接 PC3
 **                          GND  接 GND
 **              2-烧录代码至开发板，并重新上电或复位，使新程序运行；
 **              3-打开电脑串口上位机，115200-N-8-1, 即可观察到采集的数据；
 **              4-注意：DHT11的上电稳定时间约:6秒，数据更新时长约: 2秒;
 **
 **
 **【文件移植】  步骤1-复制文件：可复制bsp_DHT11.c和bsp_DHT11h两个文件，或复制DHT11文件夹，保存到所需工程目录文件夹下；
 **              步骤2-添加文件：在keil工各程左侧文件管理器中，双击某文件夹，以添加bsp_DHT11.c文件;
 **              步骤3-添加路径：点击魔术棒工具按钮，在“c/c++"选项页中，点击”Include Path"后面的按键，以添加文件存放所在路径(是文件夹，不是文件);
 **　　　　　    步骤4-添加引脚：在所需DHT11功能的代码文件头部，添加：#include "bsp_DHT11.h";
 **
 **
 **【函数使用】  获取温度值：DHT11_GetTemperature(GPIOC, GPIO_Pin_3); // 获取DHT11温度值, 注意，DHT11的精度，小数部分无效
 **              获取温度值：DHT11_GetHumidity(GPIOC, GPIO_Pin_3);    // 获取DHT11湿度值, 注意，DHT11的精度，小数部分无效
 **
 **
 **【备注说明】  代码版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>            // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)     
#include "stm32f10x_conf.h"       // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用) 
#include "bsp_led.h"              // LED指示灯
#include "bsp_usart.h"            // USART1、2、3，UART4、5
#include "bsp_lcd_ili9341.h"      // 2.8寸显示屏  
#include "bsp_w25Qxx.h"           // 外部FLASH，用于中文支持
#include "bsp_DHT11.h"            // DHT11



float temperature = 0.0;          // 用于存储温度值; DHT11的精度，小数部分无效
float humidity = 0.0;             // 用于存储湿度值; DHT11的精度，小数部分无效
char   strTem[100];               // 用于临时存储字符串



// ms延时函数，减少移植时对外部文件依赖；
static void delay_ms(uintt32_t ms)
{
    ms = ms * 6500;
    for (uintt32_t i = 0; i < ms; i++); // 72MHz系统时钟下，多少个空循环约耗时1ms
}



// 主函数
int main(void)
{
    USART1_Init(115200);                                        // 串口初始化：USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出

    Led_Init();                                                 // LED 初始化
    LED_RED_ON;                                                 // 点亮红灯
        
    W25qx_Init();                                               // 初始化外部Flash, 已烧录有中文字库
    
    LCD_Init();                                                 // 初始化LCD-2.8寸屏-驱动芯片ILI9341
    LCD_String(20, 8, "DHT11温湿度传感器", 24, WHITE, BLACK);   // 预先显示固定的屏显内容，不用重复刷新占用芯片资源
    LCD_Line(0, 40, 239, 40, WHITE);
    LCD_String(5, 65, "当前温度值：", 16, WHITE, BLACK);
    LCD_String(5, 85, "当前湿度值：", 16, WHITE, BLACK);   

    while (1)                                                   // while函数死循环，不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(500);                                          // 延时500ms
        LED_RED_TOGGLE;                                         // 红色LED，间隔亮灭，以判断系统正常工作
            
        printf("DHT11  ");
            
        temperature = DHT11_GetTemperature(GPIOC, GPIO_Pin_3);  // 获取DHT11温度值, 注意，DHT11的精度，小数部分无效
        printf("温度:%4.1f℃   ", temperature);                  // 把数据输出到电脑串口软件，方便观察
        sprintf(strTem, "%4.1f℃  ", temperature);              // 格式化，把float值存储为字符数组
        LCD_String(100, 65, strTem, 16, GREEN, BLACK);          // 温度值显示在显示屏上    
            
        humidity =  DHT11_GetHumidity(GPIOC, GPIO_Pin_3);       // 获取DHT11湿度值, 注意，DHT11的精度，小数部分无效
        printf("湿度:%4.1fRH%%\r\n", humidity);                 // 把数据输出到电脑串口软件，方便观察
        sprintf(strTem, "%4.1f℃  ", humidity);                 // 格式化，把float值存储为字符数组
        LCD_String(100, 85, strTem, 16, GREEN, BLACK);          // 温度值显示在显示屏上  
    }
}




// 注意：每个代码文件的末尾，要保留一个空行
