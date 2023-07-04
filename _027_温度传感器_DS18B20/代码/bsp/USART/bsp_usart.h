#ifndef __BSP__USART_H
#define __BSP__USART_H
/***********************************************************************************************************************************
 ** �������д��  ħŮ�������Ŷ�
 ** ��������¡�  QȺ�ļ���       1126717453
 ** ����    ����  ħŮ������      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** ���ļ����ơ�  bsp_usart.h
 **
 ** ���ļ����ܡ�  �������š�����ȫ�ֽṹ�塢����ȫ�ֺ���
 **               ���ļ��򻯵�USART�ĳ�ʼ�������������ա������ܺ�������ʼ������ú�������ʹ�ã�
 **
 ** ������ƽ̨��  STM32F103 + ��׼��v3.5 + keil5
 **
 ** ����ֲ˵����  1- ���Ʊ����̵�USART�ļ��У���Ŀ�깤���ļ�����;
 **               2- ��c�ļ���ӵ����̹�����;
 **               3- ����ļ����·��;
 **               4- ����Ҫ���ڹ��ܵ��ļ��У�#include "usart_f103.h����
 **     
 ** ��Ӳ���ص㡿  ���ø�USARTx_Init()����ʱ����������Ĭ�����Ž��г�ʼ����
 **               1- USART1 PA9, PA10
 **               2- USART2 PA2, PA3
 **               3- USART3 PB10,PB11
 **               4- UART4  PC10,PC11
 **               5- UART5  PC12,PD2
 **
 ** ������˵����  ���ļ����շ�����, ������޸�, �ѱȽ�����. 
 **               ��ʼ��: ֻ����ã�USARTx_Init(������), ���������������ż�ʱ�����ã� 
 **               �� �� : ��������, �ַ���: USARTx_SendString (char* stringTemp)�� ����: USARTx_SendData (uint8_t* buf, uint8_t cnt);
 **               �� �� : ��ʽ1��ͨ��ȫ�ֺ���USARTx_GetBuffer (uint8_t* buffer, uint8_t* cnt);����������������ʾ���˽��ջ��ƣ� 
 **                       ��ʽ2��ͨ���ж�xUSART.USARTxReceivedFla==1;
 **                              ����while�в�����ѯ�������κ�һ����Ҫ�ĵط��жϽ��ձ�־��ʾ����
 **                              while(1){
 **                                  if(xUSART1.USART1ReceivedFlag==1){
 **                                      printf((char*)xUSART.USART1ReceivedBuffer);          // ʾ��1: ���������ַ���
 **                                      uint16_t GPS_Value = xUSART.USART1ReceivedBuffer[1]; // ʾ��2: ��ζ�д����ĳ����Ա������
 **                                      xUSART1.USART1ReceivedFlag == 0;                     // ��Ҫ�����������ݺ�
 **                                  }  
 **                              }     
 **   
 ** �����¼�¼��
 **              2021-11-03  ���ƽ��պ�������ֵ����
 **              2021-08-14  ���Ӻ궨�壺���ջ�������С�趨ֵ��ʹ�ռ�ռ�ø��ɿأ�
 **              2021-08-14  �޸ķ��ͺ������ƣ�USARTx_Printf()���޸�Ϊ��USARTx_SendString();
 **              2020-09-02  �ļ�������USART1�����жϡ������жϡ������жϡ�DMA�շ�
 **              2021-06-04  USART1��2��3��UART4��5 ���շ����ƴ���
 **              2021-06-09  �����ļ���ʽ��ע��  
 **              2021-06-22  ����ע��˵��
 **    
************************************************************************************************************************************/
#include <stm32f10x.h>  
#include <stdio.h>
#include "string.h"       // C��׼��ͷ�ļ�: �ַ����鳣�ã�strcpy()��strncpy()��strcmp()��strlen()��strnset()





/*****************************************************************************
 ** ��ֲ����
****************************************************************************/
// ���ĸ���������λ��ͨ��
#define USARTx_DEBUG            USART1              // �����ض���printf, ʹprintfͨ��USARTx��������
// ���ݽ��ջ�������С���󲿷�����¶������޸�
#define U1_RX_BUF_SIZE             256              // ����ÿ��USARTx���ջ������Ĵ�С(�ֽ���)�������ж���Ļ����С����xUSART�ṹ����Ļ�������С
#define U2_RX_BUF_SIZE             256              // --- ��ÿ֡���յ��������ֽ�����С�ڴ�ֵʱ������������ʣ��ռ�����Ϊ: 0
#define U3_RX_BUF_SIZE             256              // --- ��ÿ֡���յ��������ֽ�����������ֵʱ���������֣������ж���ֱ�����ᣬֱ�����ս���(���������ж�); 
#define U4_RX_BUF_SIZE             256              // --- ��Ч���򣺽��ڸ���USARTx���жϷ������й�����
#define U5_RX_BUF_SIZE             256              // --- ��Ҫ����:  1:��Ͽ����жϽ�������֡;  2:������û����С;  3:��ֹ�������!!!!



/*****************************************************************************
 ** ȫ�ֱ��� 
****************************************************************************/
typedef struct 
{
    uint8_t   USART1InitFlag;                       // ��ʼ�����; 0=δ��ʼ��, 1=�ѳ�ʼ��
    uint8_t   USART1ReceivedFlag;                   // ���յ��µ�һ֡���ݣ�0-�ޣ�1-�������ݣ��ڴ������ݺ󣬱��Ϊ0
    uint16_t  USART1ReceivedCNT;                    // ���յ����ٸ��ֽ�����
    uint8_t   USART1ReceivedBuffer[U1_RX_BUF_SIZE]; // ���յ����ݵĻ���
    
    uint8_t   USART2InitFlag;                       // ��ʼ�����; 0=δ��ʼ��, 1=�ѳ�ʼ��
    uint8_t   USART2ReceivedFlag;                   // ���յ��µ�һ֡���ݣ�0-�ޣ�1-�������ݣ��ڴ������ݺ󣬱��Ϊ0
    uint16_t  USART2ReceivedCNT;                    // ���յ����ٸ��ֽ�����
    uint8_t   USART2ReceivedBuffer[U2_RX_BUF_SIZE]; // ���յ����ݵĻ���
    
    uint8_t   USART3InitFlag;                       // ��ʼ�����; 0=δ��ʼ��, 1=�ѳ�ʼ��
    uint8_t   USART3ReceivedFlag;                   // ���յ��µ�һ֡���ݣ�0-�ޣ�1-�������ݣ��ڴ������ݺ󣬱��Ϊ0
    uint16_t  USART3ReceivedCNT;                    // ���յ����ٸ��ֽ�����
    uint8_t   USART3ReceivedBuffer[U3_RX_BUF_SIZE]; // ���յ����ݵĻ��� 
    
    uint8_t   UART4InitFlag;                        // ��ʼ�����; 0=δ��ʼ��, 1=�ѳ�ʼ��
    uint8_t   UART4ReceivedFlag;                    // ���յ��µ�һ֡���ݣ�0-�ޣ�1-�������ݣ��ڴ������ݺ󣬱��Ϊ0
    uint16_t  UART4ReceivedCNT;                     // ���յ����ٸ��ֽ�����
    uint8_t   UART4ReceivedBuffer[U4_RX_BUF_SIZE];  // ���յ����ݵĻ���
    
    uint8_t   UART5InitFlag;                        // ��ʼ�����; 0=δ��ʼ��, 1=�ѳ�ʼ��
    uint8_t   UART5ReceivedFlag;                    // ���յ��µ�һ֡���ݣ�0-�ޣ�1-�������ݣ��ڴ������ݺ󣬱��Ϊ0
    uint16_t  UART5ReceivedCNT;                     // ���յ����ٸ��ֽ�����
    uint8_t   UART5ReceivedBuffer[U5_RX_BUF_SIZE];  // ���յ����ݵĻ���
    
    uint16_t  testCNT;                              // �����ڲ���
    
}xUSATR_TypeDef;

extern xUSATR_TypeDef  xUSART;                      // ����Ϊȫ�ֱ���,�����¼��Ϣ��״̬
    




/*****************************************************************************
 ** ����ȫ�ֺ���
****************************************************************************/
// USART1
void    USART1_Init (uint32_t baudrate);                      // ��ʼ�����ڵ�GPIO��ͨ�Ų������á��ж����ȼ�; (�����ʿ��衢8λ���ݡ���У�顢1��ֹͣλ)
uint8_t USART1_GetBuffer (uint8_t* buffer, uint8_t* cnt);     // ��ȡ���յ�������
void    USART1_SendData (uint8_t* buf, uint8_t cnt);          // ͨ���жϷ������ݣ��ʺϸ�������
void    USART1_SendString (char* stringTemp);                 // ͨ���жϷ����ַ������ʺ��ַ�����������256�������ڵ�
void    USART1_printfForDMA (char* stringTemp) ;              // ͨ��DMA�������ݣ��ʺ�һ�ι������������ر����ַ�����ʡ��ռ���жϵ�ʱ��
// USART2
void    USART2_Init (uint32_t baudrate);                      // ��ʼ�����ڵ�GPIO��ͨ�Ų������á��ж����ȼ�; (�����ʿ��衢8λ���ݡ���У�顢1��ֹͣλ)
uint8_t USART2_GetBuffer (uint8_t* buffer, uint8_t* cnt);     // ��ȡ���յ�������
void    USART2_SendData (uint8_t* buf, uint8_t cnt);          // ͨ���жϷ������ݣ��ʺϸ�������
void    USART2_SendString (char* stringTemp);                 // ͨ���жϷ����ַ������ʺ��ַ�����������256�������ڵ�
// USART3
void    USART3_Init (uint32_t baudrate);                      // ��ʼ�����ڵ�GPIO��ͨ�Ų������á��ж����ȼ�; (�����ʿ��衢8λ���ݡ���У�顢1��ֹͣλ)
uint8_t USART3_GetBuffer (uint8_t* buffer, uint8_t* cnt);     // ��ȡ���յ�������
void    USART3_SendData (uint8_t* buf, uint8_t cnt);          // ͨ���жϷ������ݣ��ʺϸ�������
void    USART3_SendString (char* stringTemp);                 // ͨ���жϷ����ַ������ʺ��ַ�����������256�������ڵ�
// USART4
void    UART4_Init (uint32_t baudrate);                       // ��ʼ�����ڵ�GPIO��ͨ�Ų������á��ж����ȼ�; (�����ʿ��衢8λ���ݡ���У�顢1��ֹͣλ)
uint8_t UART4_GetBuffer (uint8_t* buffer, uint8_t* cnt);      // ��ȡ���յ�������
void    UART4_SendData (uint8_t* buf, uint8_t cnt);           // ͨ���жϷ������ݣ��ʺϸ�������
void    UART4_SendString (char* stringTemp);                  // ͨ���жϷ����ַ������ʺ��ַ�����������256�������ڵ�
// USART5
void    UART5_Init (uint32_t baudrate);                       // ��ʼ�����ڵ�GPIO��ͨ�Ų������á��ж����ȼ�; (�����ʿ��衢8λ���ݡ���У�顢1��ֹͣλ)
uint8_t UART5_GetBuffer (uint8_t* buffer, uint8_t* cnt);      // ��ȡ���յ�������
void    UART5_SendData (uint8_t* buf, uint8_t cnt);           // ͨ���жϷ������ݣ��ʺϸ�������
void    UART5_SendString (char* stringTemp);                  // ͨ���жϷ����ַ������ʺ��ַ�����������256�������ڵ�


#endif

