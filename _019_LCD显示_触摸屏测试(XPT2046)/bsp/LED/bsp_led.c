/***********************************************************************************************************************************
 ** 【文件名称】  led.c
 ** 【编写人员】  魔女开发板团队
 ** 【淘    宝】  魔女开发板      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** 【文件功能】  实现LED指示灯常用的初始化函数、功能函数
 **                 
 ** 【移植说明】  
 **             
 ** 【更新记录】  
 **
***********************************************************************************************************************************/  
#include "bsp_led.h"


    

void Led_Init(void)
{    
    GPIO_InitTypeDef GPIO_InitStructure;                    // 定义一个GPIO_InitTypeDef类型的结构体
    
    // 使能LED_RED所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if(LED_RED_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(LED_RED_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(LED_RED_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(LED_RED_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(LED_RED_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(LED_RED_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(LED_RED_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    // 使能LED_RED所用引脚端口时钟；使用端口判断的方法使能时钟, 以使代码移植更方便
    if(LED_BLUE_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(LED_BLUE_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(LED_BLUE_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(LED_BLUE_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(LED_BLUE_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(LED_BLUE_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(LED_BLUE_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    
    // 配置LED_RED引脚工作模式
    GPIO_InitStructure.GPIO_Pin   = LED_RED_PIN;            // 选择要控制的GPIO引脚       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 设置引脚模式为通用推挽输出   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚速率为50MHz            
    GPIO_Init(LED_RED_GPIO , &GPIO_InitStructure);          // 调用库函数，初始化GPIO  
    
    // 配置LED_RED引脚工作模式
    GPIO_InitStructure.GPIO_Pin   = LED_BLUE_PIN;           // 选择要控制的GPIO引脚       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 设置引脚模式为通用推挽输出   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设置引脚速率为50MHz            
    GPIO_Init(LED_BLUE_GPIO , &GPIO_InitStructure);         // 调用库函数，初始化GPIO    
    
    LED_RED_GPIO -> BSRR = LED_RED_PIN ;                    // 点亮LED_RED， 低电平点亮
    LED_BLUE_GPIO ->BSRR = LED_BLUE_PIN ;                   // 点亮LED_BLUE，低电平点亮
                               
    printf("LED 指示灯            配置完成\r");     
}



