/***********************************************************************************************************************************
 **���ļ����ơ�  bsp_usb.c
 **
 **���ļ����ܡ�  USB���⴮��ʵ��
 **
 **������ƽ̨��  STM32F103 + ��׼��v3.5 + keil5
 **
 **������˵���� 
 **                                 
 **���� �� ����  �޸�������
 **              �ְ��շ�����
 **              ����������Ϊ����ʱ�����ջ���Ī����ĩβ�������һ�ֽڣ�Ŀǰ�����ڶ˵�3�жϻص������У���0�������һ�ֽ�
 **
 **�����¼�¼��  
 **              2022-06-3  
 **    
************************************************************************************************************************************/
#include "bsp_usb.h"



xUSB_TypeDef xUSB;                            // ȫ������; �շ����ݻ�����
extern LINE_CODING linecoding;                // USB���⴮��������Ϣ



// ���غ�������
static void USB_Port_Set(uint8_t enable);
static void Set_USBClock(void);
static void USB_Interrupts_Config(void);      // �����жϡ����ȼ�



// ��ʱ����
// ����� Options/ c++ / One ELF Section per Function
static void delay_ms(uint32_t ms)                        // ����һ��ms��ʱ������������ֲʱ���ⲿ�ļ�����; ����������������ʱʹ�ã������Ǿ�׼��ʱ;
{
    ms = ms * 10260;                                     // ע�⣺�˲ο�ֵ������������ Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);                   // 72MHzϵͳʱ���£���Լ���ٸ���ѭ����ʱ1ms
}



// �жϷ�����-USB����
void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);  // ���USB�����жϹ���λ
}



// �жϷ�����-USB�����ȼ��ж�
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();                          // ʵ��USB���������������շ�
}



//USB����͹���ģʽ
//��USB����suspendģʽʱ,MCU����͹���ģʽ
//��������ӵ͹��Ĵ���(�����ʱ�ӵ�)
void Enter_LowPowerMode(void)
{
    // printf("usb enter low power mode\r\n");
    bDeviceState = SUSPENDED; // ����ͣ
}



//USB�˳��͹���ģʽ
//�û��������������ش���(������������ʱ�ӵ�)
void Leave_LowPowerMode(void)
{
    DEVICE_INFO *pInfo = &Device_Info;
    // printf("leave low power mode\r\n");
    if (pInfo->Current_Configuration != 0)
        bDeviceState = CONFIGURED;
    else
        bDeviceState = ATTACHED;  // ��������
}



// USB�ӿ�����
// ���ܣ����ڴ�PA12�ϵ�������/MOS��
// ��PA12����������1.5K���������裬�˺�����������
// ����: DISABLE��_����, ENABLE_����
void USB_Cable_Config(FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        // printf("usb pull up enable\r\n");
    }
    else
    {
        // printf("usb pull up disable\r\n");
    }
}



// USB��ʼ������_1
// USB����
// ����: 0_�Ͽ�, 1_����
static void USB_Port_Set(uint8_t enable)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // ʹ��PORTAʱ��
    if (enable)
    {
        _SetCNTR(_GetCNTR() & (~(1 << 1)));                // �˳��ϵ�ģʽ; USBͨ�üĴ���CNTR[1]��0_�˳��ϵ�ģʽ��1_����ϵ�ģʽ
    }
    else
    {
        _SetCNTR(_GetCNTR() | (1 << 1));                   // �ϵ�ģʽ
        GPIOA->CRH &= 0XFFF00FFF;                          // ����USB��D+����PA12����ģʽ
        GPIOA->CRH |= 0X00033000;                          // GPIO_Mode_Out_OD
        GPIOA->BRR  = GPIO_Pin_12;                         // ����
    }
}



// USB��ʼ������_2
// USBʱ�����ú���,USBclk=48Mhz@HCLK=72Mhz
static void Set_USBClock(void)
{
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);     // USBclk=PLLclk/1.5=48Mhz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);  // USBʱ��ʹ��
}



// USB��ʼ������_3
// ����USB�жϼ����ȼ�
static void USB_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_ClearITPendingBit(EXTI_Line18);                       // Configure the EXTI line 18 connected internally to the USB IP
    //  ������18�ϵ��ж�
    EXTI_InitStructure.EXTI_Line = EXTI_Line18;                // USB resume from suspend mode
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;     // line 18���¼��������ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /****** ����USB�����ȼ��ж���Ӧ���ȼ�******/
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  //��2�����ȼ���֮
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

      /****** ����USB�����ж���Ӧ���ȼ�******/
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;        // ��2�����ȼ����
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}



//��ȡSTM32��ΨһID
//����USB������Ϣ
void Get_SerialNum(void)
{
    uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
    Device_Serial0 = *(uint32_t *)(0x1FFFF7E8);
    Device_Serial1 = *(uint32_t *)(0x1FFFF7EC);
    Device_Serial2 = *(uint32_t *)(0x1FFFF7F0);
    Device_Serial0 += Device_Serial2;
    if (Device_Serial0 != 0)
    {
        IntToUnicode(Device_Serial0, &Virtual_Com_Port_StringSerial[2], 8);
        IntToUnicode(Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
    }
}



//��32λ��ֵת����unicode.
//value,Ҫת����ֵ(32bit)
//pbuf:�洢��ַ
//len:Ҫת���ĳ���
void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
    uint8_t idx = 0;
    for (idx = 0 ; idx < len ; idx ++)
    {
        if (((value >> 28)) < 0xA)
        {
            pbuf[ 2 * idx] = (value >> 28) + '0';
        }
        else
        {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }
        value = value << 4;
        pbuf[ 2 * idx + 1] = 0;
    }
}



//USB COM�ڵ�������Ϣ,ͨ���˺�����ӡ����.
bool USART_Config(void)
{
    xUSB.TxFifoReadPointer = 0;  // ��ն�ָ��
    xUSB.TxFifoWritePointer = 0; // ���дָ��
#if 0                          
    printf("linecoding.format:%d\r\n", linecoding.format);            // ��ӡ������Ϣ, ���ڵ��Գ���
    printf("linecoding.paritytype:%d\r\n", linecoding.paritytype);
    printf("linecoding.datatype:%d\r\n", linecoding.datatype);
    printf("linecoding.bitrate:%d\r\n", linecoding.bitrate);
#endif
    return (TRUE);
}



// ����TxFifoBuffer���黺������ݣ��ᱻ�жϼ�鲢����
// hw_config.c:USB_LP_CAN1_RX0_IRQHandler() >> usb_istr.c:USB_Istr() >> usb_endp.c:SOF_Callback() >> usb_endp.c:EP1_IN_Callback()
void USB_SendDatas(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        xUSB.TxFifoBuffer[xUSB.TxFifoWritePointer] = buf[i];
        xUSB.TxFifoWritePointer++;
        if (xUSB.TxFifoWritePointer == USB_TXFIFO_SIZE)       // ������FIFO��С, ��������
        {
            xUSB.TxFifoWritePointer = 0;
        }
    }
}



//usb���⴮��,printf ����
//ȷ��һ�η������ݲ���USB_RXBUFF_SIZE�ֽ�
void USB_SendString(char *fmt, ...)
{
    static char  strTemp[USB_RXBUFF_SIZE]; // USB_SendString���ͻ�����

    va_list ap;
    va_start(ap, fmt);
    vsprintf(strTemp, fmt, ap);
    va_end(ap);    
   
    USB_SendDatas((uint8_t*)strTemp, strlen((const char *)strTemp));
}



// ��ʼ��; ��ʹ�õ�·:PA12��1.5K�����������3.3V���������κ��޸ģ����ú󼴿��Զ�ö�١��շ�����
void USB_Config(void)
{
    USB_Port_Set(0);        // USB�Ͽ�
    delay_ms(100);
    USB_Port_Set(1);        // USB�ٴ�����
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
}

