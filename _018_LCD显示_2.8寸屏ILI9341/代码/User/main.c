/*==================================================================================================================
  *【文件名称】  main.c
  *【编写人员】  魔女开发板团队
  *【淘    宝】  魔女开发板  https://demoboard.taobao.com
 ===================================================================================================================
 **【实验名称】  2.8寸显示屏-ILI9341
 **
 **【实验平台】  STM32F103 + KEIL5.27 + 2.8寸显示屏(ILI9341驱动芯片) + DS18B20温度传感器
 **
 **【实验操作】  硬件接线：
 **              1_显示屏接线：无需另外接线，直接压入排母中即可;
 **              1_温度传感器接线：
 **                    VCC --- 接3.3V 或 5.0V
 **                    DQ  --- 接PC0
 **                    GND --- 接GND 

 **              测试现象：
 **              1：打开电脑的串口软件，并打开相应的串口;
 **              2：烧录代码后，可在显示屏上，和串口软件输出：实时温度值;
 **
 **【重点重点】  
 **              如果使用模块，那基本已有上述上拉电阻；如果直接使用元件，须自行接上拉电阻；
 **              没有上拉的反应：数据输出为0.0；
 **              直接使用元件且不想接上拉电阻的情况，可使用原子哥的DS18B20代码，其引脚使用推挽模式；
 **
 **
 **【更新记录】  2021-05-12  创建；
 **
 **【备注说明】  版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
===================================================================================================================*/
#include <stm32f10x.h>
#include "stm32f10x_conf.h"                           // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)  
#include "system_f103.h"
#include "bsp_led.h"              // LED指示灯
#include "bsp_w25qxx.h"           // Flash存储器
#include "bsp_usart.h"            // USART1、2、3，UART4、5
#include "bsp_lcd_ili9341.h"      // LCD
#include "bsp_DS18B20.h"          // 温度传感器



char   strTem[100];



// 延时函数
static void delay_ms(uint32_t ms)
{
    ms = ms * 6500;                                  // 72MHz系统时钟下，多少个空循环约耗时1ms
    for (uint32_t i = 0; i < ms; i++);               // 不建议通过SysTick实现delay，以减少代码移植时出现冲突
}



// 主函数
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);              // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方

    USART1_Init(115200);                                         // 串口初始化：USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出
    System_SwdMode();                                            // 设置芯片调试方式(SWD); 关闭JTAG只保留SWD; 目的:释放PB3、PB4、PA15，只需PA13、PA14

    Led_Init();                                                  // LED 初始化
    LED_RED_ON ;                                                 // 打开红色LED
    LED_BLUE_ON ;                                                // 打开蓝色LED

    W25qx_Init();                                                // 设备W25Q128（外部FLASH, 16M空间，前10M用户使用，后6M存有字模数据)
    LCD_Init();                                                  // 显示屏初始化

    LCD_String(20, 8, "DS18B20温度传感器", 24, WHITE, BLACK);    // 预先显示固定的屏显内容，不用重复刷新占用芯片资源
    LCD_Line(0, 40, 239, 40, WHITE);
    LCD_String(5, 65, "当前温度值：", 16, WHITE, BLACK);


    while (1)                                                    // while函数死循环; 作用：不让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(500);                                           // 延时间隔
        LED_RED_TOGGLE ;                                         // 反转红色LED; 提示：让LED规律地闪烁，可方便观察程序是否跑飞或卡死

        float DS18B20Value = DS18B20_GetDtat(GPIOC, GPIO_Pin_0); // 获取DS18B20温度值

        printf("DS18B20测量温度: %4.1f℃ \r\n", DS18B20Value);    // 温度值显示在电脑串口软件上

        sprintf(strTem, "%4.1f℃  ", DS18B20Value);              // 格式化，把float值存储为字符数组
        LCD_String(100, 65, strTem, 16, GREEN, BLACK);           // 温度值显示在显示屏上
    }
}




