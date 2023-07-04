#ifndef __BSP_USB_H
#define __BSP_USB_H
/**==================================================================================================================
 ** 【文件名称】  bsp_usb.h
 **
 ** 【文件功能】  USB虚拟串口
 **==================================================================================================================
 **
 ** 【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 ** 【移植说明】  1- 复制工程文件夹下USB文件夹，到目标位置;
 **               2- 在目标工程中，引用如本示例: 用户配置CONFIG文件夹6个c文件，及无需修改的核心文件6个c文件;
 **               3- 在所需位置：#include "bsp_usb.h" ，即可调相关函数
 **               
 **
 ** 【划 重 点】  1-WIN7 需要安装驱动，不然无法识别; WIN10 无需安装驱动;
 **               2-适合电路：PA12经1.5K电阻恒上拉到3.3V的电路; 不适合PA12上拉电阻经三级管控制的电路;
 **               3-F103系列芯片中，USB与CAN共用同一芯片缓存，不能同时使用。
 **               4-同上，检查引脚，是否有跳线帽切换：USB/CAN。
 **               5-重中之重：每帧数据，最大有效长度：64字节; 超过这个长度，需要另行编写代码实现分包处理;
 **               6-USB虚拟串口的枚举过程: 由端点0中断根据已设置好的描述符，自动完成
 **               7-USB虚拟串口的发送处理，由端点1中断处理;(只要把数据和长度，写入缓存区，然后使能端点1即可自动发送)
 **               8-USB虚拟串口的接收处理，由端点3中断处理;(发生中断时，读取相应缓存区，即可得到所接收的有效长度，和数据)
 **
 **
 ** 【代码说明】  1-重中之重：每帧数据，最大有效长度：64字节; 超过这个长度，需要另行编写代码实现分包处理;
 **
 **               2-初始化：  本函数，适合PA12经1.5K电阻恒上拉的电路;
 **                           ---USB_Config();                                // 调用后，即可完成所有配置工作
 **
 **               3-发送数据：通过下面两个函数，把数据存入发送缓存，端点1中断回调函数自行实现被主机读取数据;
 **                           ---USB_SendDatas(uint8_t *buf, uint16_t len);   // 发送指定长度的数据
 **                           ---USB_SendString(char *fmt, ...);              // 发送字符串; 使用方式如同printf
 **
 **               4-接收数据：数据的接收由端点3中断回调函数自动实现，我们只需处理接收到的数据即可;
 **                           ---数据长度 xUSB.ReceivedNum > 0                // xUSB.ReceivedNum，为最后一帧数据的负载长度，由 EP3_OUT_Callback() 刷新; 数据使用后，需手动清0
 **                           ---数据缓存 xUSB.ReceivedBuffer                 // xUSB.ReceivedBuffer[], 为最后一帧数据的负载缓存，由 EP3_OUT_Callback() 刷新; 注意，未做超64字节的分包处理
 **
 **
 ** 【更新记录】  2022-06-02  参考移植于正点原子示例
 **
 **
************************************************************************************************************************************/
#include "stm32f10x.h"
#include "usb_type.h"
#include "usb_pwr.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"




#define USB_TXFIFO_SIZE      1024              // USB虚拟串口发送FIFO大小       
#define USB_RXBUFF_SIZE       256              // USB串口接收缓冲区最大字节数; USB每帧数据，有效负载64字节，超过64字节，需要另行编写代码进行分包处理



//定义一个USB USART FIFO结构体
typedef struct
{
    // 状态
    uint8_t   status ;                         // 连接状态
    // 发送FIFO
    uint8_t   TxFifoBuffer[USB_TXFIFO_SIZE];   // 发送缓存; 解释：
    uint16_t  TxFifoWritePointer;              // 写指针  ; ----当TxFifoWritePointer!=TxFifoReadPointer时，就表示有数据需要发送;
    uint16_t  TxFifoReadPointer;               // 读指针  ; ----当相等时，表示无需发送; 发送处理，是在EP1_IN_Callback函数中处理
    // 接收
    uint16_t  ReceivedNum;                     // 接收长度, 单位:字节
    uint8_t   ReceivedBuffer[USB_RXBUFF_SIZE]; // 接收缓冲, 最大USB_RXBUFF_SIZE个字节
} xUSB_TypeDef;
extern xUSB_TypeDef xUSB;                      // USB收发结构体



/*****************************************************************************
 ** 声明全局函数
****************************************************************************/
// 三个主要函数
void USB_Config(void);                                           // 初始化; 当使用电路:PA12经1.5K电阻恒上拉到3.3V，无需作任何修改，调用后即可自动枚举、收发数据
void USB_SendDatas(uint8_t *buf, uint16_t len);                  // 发送指定长度的数据
void USB_SendString(char *fmt, ...);                             // 发送字符串，参数格式如同printf
// 正常收发数据时，无需调用; 移植于正点原子的功能函数
bool USART_Config(void);                                         // USB COM口的配置信息,通过此函数打印出来.
void Enter_LowPowerMode(void);                                   // 进入低功耗模式; 需自行编写代码实现
void Leave_LowPowerMode(void);                                   // 退出低功耗模式; 需自行编写代码实现
void USB_Cable_Config(FunctionalState NewState);                 // 打开三极管: 当PA12由三极管控制1.5K上拉电阻时，编写代码实现具体通断控制
void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len);   // 将32位的值转换成unicode
void Get_SerialNum(void);                                        // 获取STM32的唯一ID，用于USB配置信息



#endif


