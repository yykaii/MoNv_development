/***********************************************************************************************************************************
 **【文件名称】  bsp_usb.c
 **
 **【文件功能】  USB虚拟串口实现
 **
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 **【代码说明】 
 **                                 
 **【待 解 决】  修改描述符
 **              分包收发机制
 **              当接收数据为单数时，接收缓存莫名在末尾增加奇怪一字节，目前处理：在端点3中断回调函数中，用0覆盖最后一字节
 **
 **【更新记录】  
 **              2022-06-3  
 **    
************************************************************************************************************************************/
#include "bsp_usb.h"



xUSB_TypeDef xUSB;                            // 全局作用; 收发数据缓存区
extern LINE_CODING linecoding;                // USB虚拟串口配置信息



// 本地函数声明
static void USB_Port_Set(uint8_t enable);
static void Set_USBClock(void);
static void USB_Interrupts_Config(void);      // 配置中断、优先级



// 延时函数
// 建议打勾 Options/ c++ / One ELF Section per Function
static void delay_ms(uint32_t ms)                        // 定义一个ms延时函数，减少移植时对外部文件依赖; 本函数仅作粗略延时使用，而并非精准延时;
{
    ms = ms * 10260;                                     // 注意：此参考值运行条件：打勾 Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);                   // 72MHz系统时钟下，大约多少个空循环耗时1ms
}



// 中断服务函数-USB唤醒
void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);  // 清除USB唤醒中断挂起位
}



// 中断服务函数-USB低优先级中断
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();                          // 实现USB所有事务处理，包括收发
}



//USB进入低功耗模式
//当USB进入suspend模式时,MCU进入低功耗模式
//需自行添加低功耗代码(比如关时钟等)
void Enter_LowPowerMode(void)
{
    // printf("usb enter low power mode\r\n");
    bDeviceState = SUSPENDED; // 已暂停
}



//USB退出低功耗模式
//用户可以自行添加相关代码(比如重新配置时钟等)
void Leave_LowPowerMode(void)
{
    DEVICE_INFO *pInfo = &Device_Info;
    // printf("leave low power mode\r\n");
    if (pInfo->Current_Configuration != 0)
        bDeviceState = CONFIGURED;
    else
        bDeviceState = ATTACHED;  // 已连接上
}



// USB接口配置
// 功能：用于打开PA12上的三极管/MOS管
// 当PA12配置了配置1.5K恒上拉电阻，此函数不起作用
// 参数: DISABLE—_上拉, ENABLE_上拉
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



// USB初始化函数_1
// USB连接
// 参数: 0_断开, 1_连接
static void USB_Port_Set(uint8_t enable)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能PORTA时钟
    if (enable)
    {
        _SetCNTR(_GetCNTR() & (~(1 << 1)));                // 退出断电模式; USB通用寄存器CNTR[1]：0_退出断电模式，1_进入断电模式
    }
    else
    {
        _SetCNTR(_GetCNTR() | (1 << 1));                   // 断电模式
        GPIOA->CRH &= 0XFFF00FFF;                          // 配置USB的D+，即PA12引脚模式
        GPIOA->CRH |= 0X00033000;                          // GPIO_Mode_Out_OD
        GPIOA->BRR  = GPIO_Pin_12;                         // 下拉
    }
}



// USB初始化函数_2
// USB时钟配置函数,USBclk=48Mhz@HCLK=72Mhz
static void Set_USBClock(void)
{
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);     // USBclk=PLLclk/1.5=48Mhz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);  // USB时钟使能
}



// USB初始化函数_3
// 配置USB中断及优先级
static void USB_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_ClearITPendingBit(EXTI_Line18);                       // Configure the EXTI line 18 connected internally to the USB IP
    //  开启线18上的中断
    EXTI_InitStructure.EXTI_Line = EXTI_Line18;                // USB resume from suspend mode
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;     // line 18上事件上升降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /****** 配置USB低优先级中断响应优先级******/
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  //组2，优先级次之
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

      /****** 配置USB唤醒中断响应优先级******/
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;        // 组2，优先级最高
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}



//获取STM32的唯一ID
//用于USB配置信息
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



//将32位的值转换成unicode.
//value,要转换的值(32bit)
//pbuf:存储地址
//len:要转换的长度
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



//USB COM口的配置信息,通过此函数打印出来.
bool USART_Config(void)
{
    xUSB.TxFifoReadPointer = 0;  // 清空读指针
    xUSB.TxFifoWritePointer = 0; // 清空写指针
#if 0                          
    printf("linecoding.format:%d\r\n", linecoding.format);            // 打印连接信息, 用于调试程序
    printf("linecoding.paritytype:%d\r\n", linecoding.paritytype);
    printf("linecoding.datatype:%d\r\n", linecoding.datatype);
    printf("linecoding.bitrate:%d\r\n", linecoding.bitrate);
#endif
    return (TRUE);
}



// 填入TxFifoBuffer数组缓存的数据，会被中断检查并发送
// hw_config.c:USB_LP_CAN1_RX0_IRQHandler() >> usb_istr.c:USB_Istr() >> usb_endp.c:SOF_Callback() >> usb_endp.c:EP1_IN_Callback()
void USB_SendDatas(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        xUSB.TxFifoBuffer[xUSB.TxFifoWritePointer] = buf[i];
        xUSB.TxFifoWritePointer++;
        if (xUSB.TxFifoWritePointer == USB_TXFIFO_SIZE)       // 当超过FIFO大小, 计数归零
        {
            xUSB.TxFifoWritePointer = 0;
        }
    }
}



//usb虚拟串口,printf 函数
//确保一次发送数据不超USB_RXBUFF_SIZE字节
void USB_SendString(char *fmt, ...)
{
    static char  strTemp[USB_RXBUFF_SIZE]; // USB_SendString发送缓冲区

    va_list ap;
    va_start(ap, fmt);
    vsprintf(strTemp, fmt, ap);
    va_end(ap);    
   
    USB_SendDatas((uint8_t*)strTemp, strlen((const char *)strTemp));
}



// 初始化; 当使用电路:PA12经1.5K电阻恒上拉到3.3V，无需作任何修改，调用后即可自动枚举、收发数据
void USB_Config(void)
{
    USB_Port_Set(0);        // USB断开
    delay_ms(100);
    USB_Port_Set(1);        // USB再次连接
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
}

