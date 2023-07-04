#include "bsp_tim.h"


xTIM_TypeDef  xTIM;         // 声明为全局变量,方便记录信息、状态


/*****************************************************************************
 * 函  数： TIM6_Init
 * 功  能： 配置基本定时器TIM6
 * 参  数： uint16_t PSC :  预分频器数值,16位; 用于设置计数器1脉冲的时长(时钟频率);
 *          uint16_t ARR :  自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期);
 *          FunctionalState NewState ：定时器开关; 初始化后，是打开还是关闭
 * 返回值： 无
 * 重  要： 1: F103系列，TIM的时钟源频率为72000000
 *          2: TIM_TimeBaseInitTypeDef结构体里面有5个成员，
 *             TIM6和TIM7基本定时的配置只需用到2个，另外3个是配置通用、高级定时器所用。
 *             typedef struct
 *             {
 *                 TIM_Prescaler ;          // 所有定时器 都有
 *                 TIM_CounterMode ;        // 基本定时器TIMx,x[6,7]没有，其他都有
 *                 TIM_Period ;             // 所有定时器 都有
 *                 TIM_ClockDivision ;      // 基本定时器TIMx,x[6,7]没有，其他都有
 *                 TIM_RepetitionCounter ;  // TIMx,x[1,8,15,16,17]才有
 *             }TIM_TimeBaseInitTypeDef;
*****************************************************************************/
void TIM6_Init(uint16_t PSC, uint16_t ARR, FunctionalState NewState)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        // 设置中断组为2, 这个设置，放在了main()函数的第1行，以方便统一设置所有中断的分组
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;           // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // 设置抢占优先级为2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 允许中断*/
    NVIC_Init(&NVIC_InitStructure);

#if 1                                                     // 标准库写法
#include "stm32f10x_tim.h"

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  // 开启TIM6时钟,即内部时钟CK_INT=72M
    TIM_Cmd(TIM6, DISABLE);                               // 在配置前，先关闭计数器
    TIM_TimeBaseStructure.TIM_Prescaler = PSC - 1;        // 预分频器数值,16位; 计数器的时钟频率CK_CNT等于f CK_PSC /(PSC[15:0]+1)。在每一次更新事件时，PSC的数值被传送到实际的预分频寄存器中。
    TIM_TimeBaseStructure.TIM_Period    = ARR;            // 自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期)。如果自动重装载数值为0，则计数器停止。
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);       // 初始化定时器

    TIM_ClearFlag(TIM6, TIM_FLAG_Update);                 // 清除计数器中断标志位
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);            // 开启更新中断(只是开启功能，未工作）
    TIM_Cmd(TIM6, NewState);                              // 是否使能计数器(使能了就会立即开始工作)

#else                                                     // 寄存器写法, 优点：方便移植、调试
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN ;                  // 开启TIM6时钟,
    TIM6->CR1  = 0;                                       // 在配置前，先关闭计数器, 并清0 CR1的配置
    TIM6->CR2  = 0;                                       // 清0 CR2的配置
    TIM6->DIER = 0x01 ;                                   // 开启更新中断(只是开启功能，未工作）, 关闭DMA清求
    TIM6->SR   = 0;                                       // 清0 SR寄存器, 本寄存器只有一个功能: 中断标志位。
    TIM6->CNT  = 0;                                       // 清0 计数器的值
    TIM6->PSC  = PSC - 1;                                 // 预分频器数值,16位; 计数器的时钟频率CK_CNT等于f CK_PSC /(PSC[15:0]+1)。在每一次更新事件时，PSC的数值被传送到实际的预分频寄存器中。
    TIM6->ARR  = ARR;                                     // 自动重装载数值,16位; 即多少个脉冲产生一个更新或中断(1周期)。如果自动重装载数值为0，则计数器停止。
    if (NewState)   TIM6->CR1 |= 0x01;                    // 是否使能计数器(使能了就会立即开始工作)
#endif

    xTIM.TIM6_CNT =0;
}



/*****************************************************************************
 * 函  数： TIM6_IRQHandler
 * 功  能： 超过0.03秒，则判断
 * 参  数： 无
 * 返回值： 无
 * 重  要： 1: TIM定时器，必须先清中断，后干其它事！！否则可能出现莫名错误
 *          2: 为了方便代码移植，中断服务函数的位置，放置于定时器配置函数下放
 *          3: 本函数的功能：当超过0.03ms时，Echo还没收到低电平结束信号，则停止测量
*****************************************************************************/
void  TIM6_IRQHandler(void)
{
    TIM6->SR &= (uint16_t)~0x01;   // 清0更新中断标志位
    xTIM.TIM6_CNT ++ ;             // 计数+1
}

/*****************************************************************************
 * 函  数： TIM6_IRQHandler
 * 功  能： 超过0.03秒，则判断
 * 参  数： 无
 * 返回值： 无
 * 重  要： 1: TIM定时器，必须先清中断，后干其它事！！否则可能出现莫名错误
 *          2: 为了方便代码移植，中断服务函数的位置，放置于定时器配置函数下放
 *          3: 本函数的功能：当超过0.03ms时，Echo还没收到低电平结束信号，则停止测量
*****************************************************************************/
//float TIM6_Start(void)
//{
//    while (ECHO == 1); // 等待Echo引脚低电平
//    /********1.启动信号:10US以上的脉冲**********/
//    TRIG_1;
//    delay_us(20);
//    TRIG_0;
//    /********2.启动TIM，记录ECHO高电平时长**********/
//    TIM6->CNT = 0;        // 狗血日常：调试过程无意中删了这一行，然后剧情他妈的精彩。排查一轮后，心力交瘁躺平发呆，刚眯上眼才来的灵感：是不是CNT计数器没清0！！
//    while (ECHO == 0);    // 等待高电平开始(SR04工作后，ECHO引脚将返回声波来回的等长高电平时长）
//    TIM6->CR1 |= 0x01;    // TIM开始工作
//    while (ECHO == 1);    // 等待高电平结束
//    TIM6->CR1 = 0x00;     // TIM停止工作
//    /********3.计算距离,单位:CM**********/
//    xSR04.CM = TIM6->CNT * 0.0173;  // 摄氏25度时音速每秒346米，因为TIM初始化成1us脉冲，换算成：0.0346厘米/1us; Echo引脚的高电平时长，是声波从发出到接收的时间，所以要除以2

//    return xSR04.CM;
//}

