/**==================================================================================================================
 **【文件名称】  main.c
 **【功能测试】  数据存储-STM32F103芯片内部Flash存储
 **==================================================================================================================
 **【实验平台】  STM32F103 + KEIL5.27
 **
 **【实验操作】  硬件接线：无需接线
 **
 **              代码操作：
 **              1_代码保存在system_f103.c文件中，所以要#include "system_f103.h"
 **              2_为方便代码移植，对外操作函数已统一为两个函数：读函数、写函数：
 **                    读数据：W25qxx_ReadBuffer (uint32_t addr, uint8_t *buf, uint16_t num)
 **                    写数据：W25qxx_WriteBuffer(uint32_t addr, uint8_t *buf, uint16_t num)
 **
 **              测试现象：
 **              1：打开电脑的串口软件，并打开相应的串口;
 **              2：烧录代码后，可在串口软件中输出：写入的数据，和读取到的数据;
 **
 **【划 重 点】  1_把数据存储于STM32芯片内部, 只适合少量数据，如1K; 不适合几十K的数据;
 **              2_数据存储的地址范围：(0x8000000+代码体积) < 地址 < (0x8000000+芯片Flash大小）; 
 **              3_代码体积 = Code + ROdata + RWdata
 **              4_常用芯片Flash大小及地址最大值：
 **                    STM32F103C6:   32K   0x0800 0000 + 0x00 8000 = 0x0800 8000
 **                    STM32F103C8:   64K   0x0800 0000 + 0x01 0000 = 0x0801 0000
 **                    STM32F103RC:  256K   0x0800 0000 + 0x04 0000 = 0x0804 0000
 **                    STM32F103VE:  512K   0x0800 0000 + 0x08 0000 = 0x0808 0000
 **                    STM32F103ZE:  512K   0x0800 0000 + 0x08 0000 = 0x0808 0000
 **                    STM32F103ZG: 1024K   0x0800 0000 + 0x10 0000 = 0x0810 0000
 **                      
 **
 **【更新记录】  2022-03-20  完善注释
 **
 **【备注说明】  版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>                               // 重要的宏定义文件，定义了各种地址值
#include "stm32f10x_conf.h"                          // 头文件引用(标准库); 内核、芯片外设....；(stm32f10x.conf.h, 对标准库头文件进行调用)  
#include "system_f103.h"                             // 系统常用功能, 如串口USART1配置115200-0-8-1, SysTic配置为1ms中断, 
#include "bsp_led.h"                                 // LED指示灯
#include "bsp_usart.h"                               // USART1、2、3，UART4、5



// 延时函数
static void delay_ms(uint32_t ms)                    // 简单的毫秒延时函数
{
    ms = ms * 6500;                                  // 72MHz系统时钟下，多少个空循环约耗时1ms
    for (uint32_t i = 0; i < ms; i++);               
}



// 主函数
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方

    USART1_Init(115200);                             // 串口初始化：USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出

    Led_Init();                                      // LED 初始化
    LED_RED_ON ;                                     // 点亮红色LED
    LED_BLUE_ON ;                                    // 点亮蓝色LED

    printf("\r\n>>>>芯片内部Flash基本读写：\r");
    // 在指定位置写入指定长度数据
    uint8_t writeBuf[50] = "Test, 天气很好！";
    System_WriteInteriorFlash(0x08000000+60*1024, writeBuf, 20);                       // 写入数据：地址，数据首地址，字节数;  说明：为避免覆盖代码数据，从Flash的第60K地址位置写入
    printf("写入的数据: %s\r\n", writeBuf);
    // 在指定位置读取指定长度数据
    uint8_t readBuf[50];
    System_ReadInteriorFlash(0x08000000+60*1024, readBuf, 20);                         // 读取数据：地址，缓存首地址，长度
    printf("读取的数据: %s\r\n\r\n", readBuf);                    

        
    printf("\r\n>>>>尝试不同地址的读写，及各数据类型的转换：\r\n");         
    // 测试1：字符
    uint32_t addrChar   = 0x08000000+38*1024;                                          // 注意地址必须是偶数，且地址段尽量安排在FLASH尾部，避免覆盖了代码段数据
    char writeChar='F';            
    System_WriteInteriorFlash(addrChar, (uint8_t*)&writeChar, 1);                      // 存入数据，函数内自动解锁，擦除，写入
    char readChar=0;                                 
    System_ReadInteriorFlash(addrChar, (uint8_t*)&readChar, 1);                        // 读取数据，函数内自动按半字读取
    printf("测试char     地址:0x%X    写入:%c       读取:%c \r\n", addrChar, writeChar, readChar);
   
   // 测试2：带符号的16位   
    uint32_t addrShort  = 0x08000000+155*1024-2;                                       // 注意地址必须是偶数，且地址段尽量安排在FLASH尾部，避免覆盖了代码段数据
    short writeShort=-888;             
    System_WriteInteriorFlash(addrShort, (uint8_t*)&writeShort, sizeof(writeShort));   // 存入数据，函数内自动解锁，擦除，写入
    short readShort=0;       
    System_ReadInteriorFlash(addrShort, (uint8_t*)&readShort, sizeof(writeShort));     // 读取数据，函数内自动按半字读取
    printf("测试short    地址:0x%X    写入:%d    读取:%d \r\n", addrShort, writeShort, readShort);
   
   // 测试3：无符号的32位
    uint32_t addrUint   = 0x08000000+254*1024;                                         // 注意地址必须是偶数，且地址段尽量安排在FLASH尾部，避免覆盖了代码段数据
    uint32_t writeUint= 0x12345678;
    System_WriteInteriorFlash(addrUint, (uint8_t*)&writeUint, 4);                      // 存入数据，函数内自动解锁，擦除，写入
    uint32_t readUint = 0;
    System_ReadInteriorFlash(addrUint, (uint8_t*)&readUint, 4);                        // 读取数据，函数内自动按半字读取
    printf("测试uint     地址:0x%X    写入:0x%X     读取:0x%X \r\n", addrUint, writeUint, readUint);
    
    // 测试4：浮点型
    uint32_t addrFloat  = 0x08000000+122*1024;                                         // 注意地址必须是偶数，且地址段尽量安排在FLASH尾部，避免覆盖了代码段数据
    float writeFloat= -123.456;     
    System_WriteInteriorFlash(addrFloat, (uint8_t*)&writeFloat, 4);                    // 存入数据，函数内自动解锁，擦除，写入
    float readFloat;
    System_ReadInteriorFlash(addrFloat, (uint8_t*)&readFloat, 4);                      // 读取数据，函数内自动按半字读取
    printf("测试float    地址:0x%X　  写入:%f    读取:%f \r\n", addrFloat, writeFloat, readFloat);
    
    // 测试5：字符串, C没有字符串这个类型, 用字符数组
    uint32_t addrString   = 0x08000000+166*1024-6;                                     // 注意地址必须是偶数，且地址段尽量安排在FLASH尾部，避免覆盖了代码段数据
    char writeString[100]="早吖! 去哪呢?";     
    System_WriteInteriorFlash(addrString, (uint8_t*)writeString, sizeof(writeString)); // 存入数据，函数内自动解锁，擦除，写入
    char readString[100];
    System_ReadInteriorFlash(addrString, (uint8_t*)readString, sizeof(readString));    // 读取数据，函数内自动按半字读取
    printf("测试string   地址:0x%X　  写入:%s  读取:%s \r\n", addrString, writeString, readString);
       
                
    while (1)                                                     // while函数死循环，作用：不能让main函数运行结束，否则会产生硬件错误
    {
        delay_ms(500);                                            // 延时间隔
        LED_RED_TOGGLE ;                                          // 反转红色LED; 提示：让LED规律地闪烁，可方便观察程序是否跑飞或卡死
    }
}




