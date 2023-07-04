#include "bsp_key.h"
#include "bsp_led.h"





void Key_Init(void)
{
    GPIO_InitTypeDef G;                     // 作用：配置引脚工作模式 
    NVIC_InitTypeDef NVIC_InitStruct ;      // 作用：配置优先级
    EXTI_InitTypeDef EXTI_InitStruct ;      // 作用：配置引脚中断方式
    
    // 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE) ;    // EXTI的时钟要设置AFIO寄存器 
    
    // 使能GPIO时针, 为减少调试过程忘了使能时钟而出错，把相关GPIO端口时钟，都使能了；
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF ; 
   
    // KEY_1
    // 配置引脚工作模式
    G.GPIO_Pin  = KEY_1_PIN;                                   // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    G.GPIO_Mode = GPIO_Mode_IPD ;                              // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    G.GPIO_Speed = GPIO_Speed_10MHz ;                          // 输出电平反转速度；在输入状态时无效，但GPIO_Init函数需要用到；
    GPIO_Init(KEY_1_GPIO, &G);                                 // 初始化，调用引脚工作模式配置函数
    // 配置中断的优先级
    NVIC_InitStruct.NVIC_IRQChannel = KEY_1_IRQN ;             // 中断号
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;    // 配置抢占优先级     
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0 ;           // 配置子优先级     
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;              // 使能中断通道    
    NVIC_Init(&NVIC_InitStruct) ;                              // 初始化，调用优先级配置函数 
    // 配置中断的方式      
    GPIO_EXTILineConfig( KEY_1_GPIOSOURCE , KEY_1_PINSOURCE);  // 选择作为EXTI线的GPIO引脚
    EXTI_InitStruct.EXTI_Line    = KEY_1_EXTI_LINE ;           // 配置中断or事件线     
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;       // 模式：中断：EXTI_Mode_Interrupt、事件：EXTI_Mode_Event
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising ;       // 边沿触发： 上升：EXTI_Trigger_Rising 、下降：EXTI_Trigger_Falling 、浮空：EXTI_Trigger_Rising_Falling
    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                    // 使能EXTI线
    EXTI_Init(&EXTI_InitStruct) ;                              // 初始化，调用中断线配置函数  
    
    // KEY_2
    // 配置引脚工作模式
    G.GPIO_Pin  = KEY_2_PIN;                                   // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    G.GPIO_Mode = GPIO_Mode_IPU ;                              // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    G.GPIO_Speed = GPIO_Speed_10MHz ;                          // 输出电平反转速度；在输入状态时无效，但GPIO_Init函数需要用到；
    GPIO_Init(KEY_2_GPIO, &G);                                 // 初始化，调用引脚工作模式配置函数
    // 配置中断的优先级
    NVIC_InitStruct.NVIC_IRQChannel = KEY_2_IRQN ;             // 中断号
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;    // 配置抢占优先级     
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0 ;           // 配置子优先级     
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;              // 使能中断通道    
    NVIC_Init(&NVIC_InitStruct) ;                              // 初始化，调用优先级配置函数 
    // 配置中断的方式      
    GPIO_EXTILineConfig( KEY_2_GPIOSOURCE , KEY_2_PINSOURCE);  // 选择作为EXTI线的GPIO引脚
    EXTI_InitStruct.EXTI_Line    = KEY_2_EXTI_LINE ;           // 配置中断or事件线     
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;       // 模式：中断：EXTI_Mode_Interrupt、事件：EXTI_Mode_Event
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling ;      // 边沿触发： 上升：EXTI_Trigger_Rising 、下降：EXTI_Trigger_Falling 、浮空：EXTI_Trigger_Rising_Falling
    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                    // 使能EXTI线
    EXTI_Init(&EXTI_InitStruct) ;                              // 初始化，调用中断线配置函数  
        
    // KEY_3
    // 配置引脚工作模式
    G.GPIO_Pin  = KEY_3_PIN;                                   // 引脚KEY_1, 闲时下拉(重要)， 按下时被置高电平
    G.GPIO_Mode = GPIO_Mode_IPU ;                              // 闲时电平状态(使用芯片内部电阻进行电平上下拉)
    G.GPIO_Speed = GPIO_Speed_10MHz ;                          // 输出电平反转速度；在输入状态时无效，但GPIO_Init函数需要用到；
    GPIO_Init(KEY_3_GPIO, &G);                                 // 初始化，调用引脚工作模式配置函数
    // 配置中断的优先级
    NVIC_InitStruct.NVIC_IRQChannel = KEY_3_IRQN ;             // 中断号
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;    // 配置抢占优先级     
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0 ;           // 配置子优先级     
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;              // 使能中断通道    
    NVIC_Init(&NVIC_InitStruct) ;                              // 初始化，调用优先级配置函数 
    // 配置中断的方式      
    GPIO_EXTILineConfig( KEY_3_GPIOSOURCE , KEY_3_PINSOURCE);  // 选择作为EXTI线的GPIO引脚
    EXTI_InitStruct.EXTI_Line    = KEY_3_EXTI_LINE ;           // 配置中断or事件线     
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;       // 模式：中断：EXTI_Mode_Interrupt、事件：EXTI_Mode_Event
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling ;      // 边沿触发： 上升：EXTI_Trigger_Rising 、下降：EXTI_Trigger_Falling 、浮空：EXTI_Trigger_Rising_Falling
    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                    // 使能EXTI线
    EXTI_Init(&EXTI_InitStruct) ;                              // 初始化，调用中断线配置函数  
}



/******************************************************************************
 * 函  数： Key_Scan
 * 功  能： 扫描按键状态
 * 参  数： GPIOx：     GPIO端口 
 *          PINx :      Pin引脚编号
 *          downStatus: 按下时的电平
 *
 * 返回值： KEY_ON : 按下
 *          KEY_OFF：无动作
 *     
 * 说  明： 1: 本函数检测方式为“while不断扫描”，发现按键按下且等待按键松开，为一次有效按下动作;   
 *          2：本方法建议只用示例使用，实际项目中，建议使用中断方法检测;
 ******************************************************************************/ 
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t PINx , uint8_t downStatus)
{	
    if(GPIO_ReadInputDataBit(GPIOx, PINx)==downStatus)
    {   
        while(GPIO_ReadInputDataBit(GPIOx, PINx)==downStatus);
        return KEY_ON;
    }		
    return KEY_OFF;	     
}



// KEY_1 中断服务函数
void KEY_1_IRQHANDLER(void)
{           
    if(EXTI->PR & KEY_1_PIN)
    {  
        EXTI->PR |= KEY_1_PIN  ;                  
        LED_BLUE_TOGGLE;  
    }        
}



// KEY_2 中断服务函数
void KEY_2_IRQHANDLER(void)
{           
    if(EXTI->PR & KEY_2_PIN)
    {  
        EXTI->PR |= KEY_2_PIN  ;                  
        LED_BLUE_TOGGLE;  
    }        
}



// KEY_3 中断服务函数
void KEY_3_IRQHANDLER(void)
{           
    if(EXTI->PR & KEY_3_PIN)
    {  
        EXTI->PR |= KEY_3_PIN  ;                  
        LED_BLUE_TOGGLE;  
    }        
}




