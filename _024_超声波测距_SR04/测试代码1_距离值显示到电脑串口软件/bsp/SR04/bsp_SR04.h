#ifndef _BSP_SR04_H 
#define _BSP_SR04_H
/***********************************************************************************************************************************
 **【购买链接】  魔女科技   https://demoboard.taobao.com
 **【更新分享】  Q群文件夹
 ***********************************************************************************************************************************
 **【文件名称】  bsp_SR04.h
 **【功能描述】  定义引脚、定义全局结构体、声明全局函数
 **
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 **【划 重 点】  1-工作电压：5.0V
 **              2-最短距离：2cm
 **              3-最远距离：400cm
 **              4-有效角度：15度
 **              5-工作原理
 **                启动信号：Trig引脚，给一个10us以上的脉冲信号(置低->置高15us->置低), 模块即开始工作，自动产生40kHz的声波; 
 **                返回信号：当发出的声波接到障碍特返回被接收到时，SR04模块产在Echo引脚产生一个高电平脉冲，脉冲时长是发出声波到接收到声波的时长;
 **                          上面的过程，是自动的。我们只要监测Echo的高电平时长，配合音速就可以转换为需要的距离;
 **              3-音速问题：声音在空气(15℃) 中的速度340m/s。温度每上升1度，速度增加0.6米/秒; 
 **
 **【文件移植】  步骤1-复制文件：可复制bsp_SR04.c和bsp_SR04h两个文件，或复制SR04文件夹，保存到所需工程目录文件夹下；
 **              步骤2-添加文件：在keil工各程左侧文件管理器中，双击某文件夹，以添加bsp_SR04.c文件;
 **              步骤3-添加路径：点击魔术棒工具按钮，在“c/c++"选项页中，点击”Include Path"后面的按键，以添加文件存放所在路径(是文件夹，不是文件);
 **　　　　　    步骤4-添加引脚：在所需SR04功能的代码文件头部，添加：#include "bsp_SR04.h";
 **
 **【函数使用】  初始化：  SR04_Init(void);     // SR04 初始化：引脚、TIM  
 **              获取距离：SR04_GetCM(void);    // 获取SR04的数据，并转化成厘米值; 
 **
 **【更新记录】  2022-05-04  完成测试
 **
 **【备注说明】  代码版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
***********************************************************************************************************************************/
#include <stm32f10x.h>
#include <stdio.h>




//移植参数区 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define  SR04_TRIG_GPIO          GPIOC
#define  SR04_TRIG_PIN           GPIO_Pin_0

#define  SR04_ECHO_GPIO          GPIOC
#define  SR04_ECHO_PIN           GPIO_Pin_2
#define  SR04_ECHO_IRQN          EXTI2_IRQn             // 中断向量编号
#define  SR04_ECHO_IRQHANDLER    EXTI2_IRQHandler       // 中断服务函数
#define  SR04_ECHO_GPIOSOURCE    GPIO_PortSourceGPIOC   // 中断线端口
#define  SR04_ECHO_EXTI_LINE     EXTI_Line2             // 中断线编号
#define  SR04_ECHO_PINSOURCE     GPIO_PinSource2   





/*****************************************************************************
 ** 声明 全局变量
****************************************************************************/
typedef struct
{
    uint8_t   flagInitOK;       // 初始化标志：0-未初始化，1-已完成初始化
    float     CM;               // 距离值，单位:厘米; 当值为0时，表示无数据
} xSR04_TypeDef;
extern xSR04_TypeDef  xSR04;    // 声明全局结构体, 用于记录信息



/*****************************************************************************
 ** 声明  全局函数
****************************************************************************/
void    SR04_Init(void);        // 初始化SR04: 配置引脚、中断、TIM6
float   SR04_GetCM(void);       // 获得结果值; 本函数返回xSR04.CM值;
void    SR04_Start(void);       // SR04开始工作; 注意，本函数在SR04_Init()工作时，默认调用了。也可以单独调用;
void    SR04_Stop(void);        // SR04停止工作;
#endif

