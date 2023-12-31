#ifndef __BSP_DAC_H
#define __BSP_DAC_H     
/***********************************************************************************************************************************
  *【文件名称】  dac.h
  *【编写人员】  魔女开发板团队-老周
  *【更新分享】  如有更新，分享在Q群文件夹  262901124
  *【淘    宝】  魔女开发板  https://demoboard.taobao.com
  *【最后修改】  2020-12-15
  **********************************************************************************************************************************
  *【代码适用】  已于STM32F103xx中验证运行
  *
  *【移植使用】  引脚修改：无需修改；
  *              因为STM32F103只有两个DAC通道，且固定于PA4、PA5引脚  ； 
  *              要使用哪个通道，只要初始化、配置通道输出值，即可                                         
  *       
  *              
  *【更新记录】  2021-04-21  创建; 参考原子哥、野火大神的代码作编写
  *
***********************************************************************************************************************************/
#include <stm32f10x.h>  
#include "stm32f10x_dac.h"



/*****************************************************************************
 ** 全局变量 (无要修改)
****************************************************************************/
typedef struct 
{
    uint8_t   Channel1InitFlag;      // 初始化标记; 0=未初始化, 1=已初始化
    uint8_t   Channel2InitFlag;      // 初始化标记; 0=未初始化, 1=已初始化    
}xDAC_TypeDef;
extern xDAC_TypeDef  xDAC;           // 声明为全局变量,方便记录信息、状态
    




/*****************************************************************************
 ** 声明全局函数
 ** 魔女科技 --- 代码分享Q群：262901124
****************************************************************************/
void DAC_InitChannel1(void);           // 初始化DAC    通道1(PA4)     
void DAC_InitChannel2(void);           // 初始化DAC    通道2(PA5)    
void DAC_PA4SetVoltage(u16 mA);        // 设置DAC通道1(PA4)的输出电压值，单位：mA; 值域: 0~3300; 
void DAC_PA5SetVoltage(u16 mA);        // 设置DAC通道2(PA5)的输出电压值，单位：mA; 值域: 0~3300; 
#endif

