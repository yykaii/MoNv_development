/*==================================================================================================================
  *【文件名称】  main.c
  *【编写人员】  魔女开发板团队
  *【淘    宝】  魔女开发板  https://demoboard.taobao.com
 ===================================================================================================================
 **【实验名称】  温度传感器-DS18B20
 **
 **【实验平台】  STM32F103 + KEIL5.27 + W25Q128
 **
 **【实验操作】  硬件接线：
 **              1_本实验所使用的是DS18B20完整模块，非单独DS18B20元件；
 **              2_使用用杜邦线，连接至开发板上排针, 接线方式如下
 **                    VCC --- 接3.3V 或 5.0V
 **                    DQ  --- 接PC0
 **                    GND --- 接GND
 **
 **              文件移植：
 **              1-复制文件夹下的bsp_DS18B20.c和h两个文件，到目标工程文件夹下；
 **              2-引用文件：在目标工程keil文件管理器中，双击某文件夹，以添加引用bsp_DS18B20.c文件
 **              3-添加文件路径：双击魔术棒工具按钮，在c/c++选项页中，添加刚才存放bsp_DS18B20.h的文件夹位置;
 **              4-在需要获取数据代码位置，调用函数DS18B20_GetData(GPIOx, PINx), 返回float类型温度数据值；
 **
 **              代码使用：
 **              1_为方便代码移植，对外操作函数，已统一为1个函数DS18B20_GetDtat()，无需另行初始化;
 **              2_获取数据：DS18B20_GetDtat(GPIO_TypeDef *GPIOx, uint32_t PINx); 只需调用这个函数，传入所用的GPIOx, GPIO_Pin_x，即返回float类型温度值;
 **
 **              测试现象：
 **              1：打开电脑的串口软件，并打开相应的串口;
 **              2：烧录代码后，可在串口软件中输出：实时温度值;
 **
 **【重点重点】  为支持总线机制，代码使用了开漏模式，所以：数据线端必须外部上拉(3.3V、4.7K电阻)；
 **              如果使用模块，那基本已有上述上拉电阻；如果直接使用元件，须自行接上拉电阻；
 **              没有上拉的反应：数据输出为0.0；
 **              直接使用元件且不想接上拉电阻的情况，可使用原子哥的DS18B20代码，其引脚使用推挽模式；
 **
 **【推荐视频】  https://www.bilibili.com/video/BV1CV411o7Di?from=search&seid=1639361392451575575
 **
 **【更新记录】  2021-05-12  创建；
 **              2021-05-14  完善DS18B20_GetTemp()逻辑，完善文件注释；
 **              2022-03-20  完善DS18B20_GetData()函数；
 **
 **【备注说明】  版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
===================================================================================================================*/
#include <stm32f10x.h>
#include "stm32f10x_conf.h"                           // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)  
#include "system_f103.h"
#include "bsp_led.h"              // LED指示灯
#include "bsp_key.h"              // 按键
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

    W25qx_Init();                                                // 设备W25Q128： 引脚初始化，SPI初始化，测试连接, 测试是否存在字库

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




