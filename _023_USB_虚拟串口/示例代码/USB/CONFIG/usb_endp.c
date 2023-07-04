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
#include "string.h"       // C��׼��ͷ�ļ�: �ַ����鳣�ã�strcpy()��strncpy()��strcmp()��strlen()��strnset()


/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5


/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    : �������������룬�����ڵ����
*                : ����������Ǳ�֡���жϷ�����SOF_Callback()�����õġ�
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
    uint16_t USB_Tx_ptr;
    uint16_t txLen;

    if (xUSB.TxFifoReadPointer == xUSB.TxFifoWritePointer)         // ���κ�����Ҫ����,ֱ���˳�
    {
        return;
    }

    if (xUSB.TxFifoReadPointer < xUSB.TxFifoWritePointer)          // û�г�������,��ָ��<дָ��
    {
        txLen = xUSB.TxFifoWritePointer - xUSB.TxFifoReadPointer;  // �õ�Ҫ���͵����ݳ���
    }
    else                                                           // ���������� ��ָ��>дָ��
    {
        txLen = USB_TXFIFO_SIZE - xUSB.TxFifoReadPointer;          // �õ�Ҫ���͵����ݳ���
    }

    if (txLen > VIRTUAL_COM_PORT_DATA_SIZE)                        // ����64�ֽ�
    {
        txLen = VIRTUAL_COM_PORT_DATA_SIZE;                        // �˴η���������
    }

    USB_Tx_ptr = xUSB.TxFifoReadPointer;                           // ������ʼ��ַ
    xUSB.TxFifoReadPointer += txLen;                               // ��ָ��ƫ��
    if (xUSB.TxFifoReadPointer >= USB_TXFIFO_SIZE)                 // ��ָ�����
    {
        xUSB.TxFifoReadPointer = 0;
    }

    UserToPMABufferCopy(&xUSB.TxFifoBuffer[USB_Tx_ptr], ENDP1_TXADDR, txLen);  // ����Ҫ���͵����ݣ����Ƶ�USB��������
    SetEPTxCount(ENDP1, txLen);                                                // ���ö˵���Ҫ���͵��ֽ�����
    SetEPTxValid(ENDP1);                                                       // ʹ�ܶ˵�
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    : ��������������������ڵ�����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
    memset(xUSB.ReceivedBuffer ,0, USB_RXBUFF_SIZE);                // ���㻺������ݣ��������ݸ��Ǿ�����
    xUSB.ReceivedNum = USB_SIL_Read(EP3_OUT, xUSB.ReceivedBuffer);  // �õ�USB���յ������ݼ��䳤��
    SetEPRxValid(ENDP3);                                            // ʹ�ܶ˵�3�����ݽ���
    
    xUSB.ReceivedBuffer[xUSB.ReceivedNum]=0;                        // ?����ÿ֡���ݳ���Ϊ����ʱ(������ȷ)����������Ī���Ķ���һ���ֽڡ���ʱδ����ԭ���Ȱ������һ�ֽ���0
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    : ֡���źŻص�������
*                : SOF��֡�ף�����ȫ���豸��˵��ÿ1������1��֡���ź�;
*                : ��ˣ�ÿ1��������1�����֡���жϻص�������
*                : ���������ж�������ʱ��ÿ1�����VCOMPORT_IN_FRAME_INTERVAL�ļ��ʱ�䣬�ж��Ƿ��д�USART�յ�������Ҫͨ��USB����������
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
    static uint32_t FrameCount = 0;

    if (bDeviceState == CONFIGURED)   // �ж��Ƿ������óɹ�
    {
        if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
        {
            FrameCount = 0;          // ����֡������
            EP1_IN_Callback();       // ͨ��EP1_IN_Callback����ʵ��TX���ݷ��͸�USB
        }
    }
}


