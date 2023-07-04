/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "bsp_usb.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "string.h"       // C标准库头文件: 字符数组常用：strcpy()、strncpy()、strcmp()、strlen()、strnset()


/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5


/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    : 处理主机的输入，即串口的输出
*                : 这个函数，是被帧首中断服务函数SOF_Callback()所调用的。
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
    uint16_t USB_Tx_ptr;
    uint16_t txLen;

    if (xUSB.TxFifoReadPointer == xUSB.TxFifoWritePointer)         // 无任何数据要发送,直接退出
    {
        return;
    }

    if (xUSB.TxFifoReadPointer < xUSB.TxFifoWritePointer)          // 没有超过数组,读指针<写指针
    {
        txLen = xUSB.TxFifoWritePointer - xUSB.TxFifoReadPointer;  // 得到要发送的数据长度
    }
    else                                                           // 超过数组了 读指针>写指针
    {
        txLen = USB_TXFIFO_SIZE - xUSB.TxFifoReadPointer;          // 得到要发送的数据长度
    }

    if (txLen > VIRTUAL_COM_PORT_DATA_SIZE)                        // 超过64字节
    {
        txLen = VIRTUAL_COM_PORT_DATA_SIZE;                        // 此次发送数据量
    }

    USB_Tx_ptr = xUSB.TxFifoReadPointer;                           // 发送起始地址
    xUSB.TxFifoReadPointer += txLen;                               // 读指针偏移
    if (xUSB.TxFifoReadPointer >= USB_TXFIFO_SIZE)                 // 读指针归零
    {
        xUSB.TxFifoReadPointer = 0;
    }

    UserToPMABufferCopy(&xUSB.TxFifoBuffer[USB_Tx_ptr], ENDP1_TXADDR, txLen);  // 把需要发送的数据，复制到USB缓存区中
    SetEPTxCount(ENDP1, txLen);                                                // 设置端点需要发送的字节数量
    SetEPTxValid(ENDP1);                                                       // 使能端点
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    : 处理主机的输出，即串口的输入
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
    memset(xUSB.ReceivedBuffer ,0, USB_RXBUFF_SIZE);                // 清零缓存旧数据，即新数据覆盖旧数据
    xUSB.ReceivedNum = USB_SIL_Read(EP3_OUT, xUSB.ReceivedBuffer);  // 得到USB接收到的数据及其长度
    SetEPRxValid(ENDP3);                                            // 使能端点3的数据接收
    
    xUSB.ReceivedBuffer[xUSB.ReceivedNum]=0;                        // ?：当每帧数据长度为单数时(长度正确)，缓存区会莫名的多了一个字节。暂时未发现原因，先把最后这一字节清0
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    : 帧首信号回调函数，
*                : SOF是帧首，对于全速设备来说，每1毫秒有1个帧首信号;
*                : 因此，每1毫秒会调用1次这个帧首中断回调函数。
*                : 在这里，这个中断用做定时，每1毫秒×VCOMPORT_IN_FRAME_INTERVAL的间隔时间，判断是否有从USART收到的数据要通过USB发到主机。
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
    static uint32_t FrameCount = 0;

    if (bDeviceState == CONFIGURED)   // 判断是否已配置成功
    {
        if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
        {
            FrameCount = 0;          // 重设帧计数器
            EP1_IN_Callback();       // 通过EP1_IN_Callback函数实现TX数据发送给USB
        }
    }
}


