#ifndef __BSP_CAN_H
#define __BSP_CAN_H
/***********************************************************************************************************************************
 **【购买链接】  魔女科技    https://demoboard.taobao.com
 **【更新分享】  Q群文件夹   1126717453
 ***********************************************************************************************************************************
 **【文件名称】  can_f103.h
 **【文件功能】  本文件USART的初始化，及完善了收、发功能函数；初始化后调用函数即可使用；
 **
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 **【使用说明】  移 植 ：复制CAN文件夹下c、h文件到目标文件夹，添加到工程管理器, 添加文件路径, 最后 #include "CAN_F103.h＂；
 **              初始化: 调用 CAN1_Config(), 函数内已做好引脚配置,； 
 **              发 送 : CAN_SendData
 **              接 收 : 通过接收标志xCan.RxFlag，判断是否收到新报文                      
 **                            
 **
 **【更新记录】  2021-07-27  文件建立
 **              2021-07-28  完善注释
 **    
************************************************************************************************************************************/

#include "stm32f10x.h"
#include "stdio.h"



#define CAN1_TX_GPIO   GPIOA
#define CAN1_TX_PIN    GPIO_Pin_12

#define CAN1_RX_GPIO   GPIOA
#define CAN1_RX_PIN    GPIO_Pin_11

#define RECIVE_ID       ((uint32_t)0x1234)     // 决定接收哪些ID发来的报文 




/*****************************************************************************
 ** 全局变量 
****************************************************************************/
typedef struct 
{
    uint8_t  RxFlag;   
    CanRxMsg RxData;   
    CanTxMsg TxData;                
    
}xCAN_InfoDef;
extern xCAN_InfoDef  xCAN;         // 声明为全局变量,方便记录信息、状态








/*****************************************************************************
 ** 声明全局函数
****************************************************************************/
void    CAN1_Config(void);
uint8_t CAN1_SendData(uint8_t* data, uint32_t targetID);    // 发送数据; data:数据缓存，targetID:ID号，　返回：发送所用的邮箱号
void    CAN1_ReceiveData(uint8_t* data);                    // 接收数据













#endif


