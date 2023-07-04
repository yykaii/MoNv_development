/**==================================================================================================================
 **【文件名称】  main.c
 **【功    能】  ADC获取外部电压
 ===================================================================================================================
 **
 **【划 重 点】 危险：绝对不要采集超过Vref或VDDA引脚电压的外部电压(即，不要超过3.3V)
 **             易错: 对外部供电电路采集时，要共地; 
 **             易错: 当采集引脚悬空时，将采集到约为2000左右的跳动值；估计处在悬空时，引脚内部两个钳位二极管存在微弱的漏电流，导致IO口电压约等于供电电压的一半；
 **
 **
 **【操作说明】 1：示例代码中，通过PA1引脚采集电压，可修改成需要的引脚(需要有ADC功能的引脚)：
 **             2：运行代码后，杜邦线一头接有ADC功能的引脚, 另一头可连接至GND、3.3V等引脚进行测试，结果会输出到串口软件中； 
 **
 **【文件移植】 1：复制bsp文件夹下的ADC文件夹，粘贴到相应的工程文件夹里。ADC文件夹内有两个文件：bsp_adc.c、bsp_adc.h；
 **             2：在keil左侧工程文件管理器Project中，双击某文件夹，以添加文件bsp_adc.c
 **             3: 在keil里添加路径：魔术棒->c/c++->include path...增加(刚才粘贴文件夹的位置)； 
 **
 **【代码使用】 1：在要使用ADC功能的文件中，添加：#include “bsp_adc.h";
 **             2: 调用ADC1_GetAdcValue(GPIOx, GPIO_Pin_x), 即可返回指定引脚的ADC转换值，返回值范围:0~4095;
 **             3: 注意，为方便代码使用和移植，直接调用ADC1_GetAdcValue()就可以，无需其它ADC初始化操作，函数内部自动判断并做相关初始化；
 **         
 **
 **【更    新】 2022-01-25  完善示例格式
 **             2021-12-02  完成函数重写，把常用ADC单通道采集，封装成仅一个函数
 **             2021-11-30  修改文件名称
 **             2020-12-18  完善代码结构、完善注释格式
 **
 **【备注说明】 版权归魔女科技所有，请勿商用，谢谢！ 
 **             https://demoboard.taobao.com 
===================================================================================================================*/
#include "system_f103.h"                              // 调用system_f103.h以简化代码工作
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_key.h"



uint16_t gpioADC =0;
uint16_t InteriorADC=0;
float    voltage =0.0;



static void delay_ms(uint32_t ms)                     // 定义一个ms延时函数，减少移植时对外部文件依赖;
{
    ms = ms * 6500;
    for (uint32_t i = 0; i < ms; i++);                // 72MHz系统时钟下，大约6500个空循环耗时1ms
}




int main(void)
{  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // 中断分组，组2:抢占级0~3,子优先级0~3 ; 全局只设置一次，尽量放在显眼的地方
           
    USART1_Init(115200);                              // 串口初始化：USART1(115200-N-8-1), 且工程已把printf重定向至USART1输出

    Led_Init();                                       // LED 初始化
    LED_BLUE_ON ;
    LED_RED_ON ;  
        
 
    while(1)                                          // 死循环，不能让main函数运行结束，否则会产生硬件错误
    {      
        delay_ms(500);                                // 上面已初始化SysTick, 其后就可以直接使用System_DelayMS()、System_DelayMS()
        LED_RED_TOGGLE;                               // 规律闪烁红色LED，以方便观察系统是否正常工作        
        
        #if 0        
        // 方式1：直接换算
        gpioADC=ADC1_GetValue(GPIOA, GPIO_Pin_1);                             // 获取通道的ADC转换值; 每获取一次用时约23us
        voltage  =(float)gpioADC*(3.3/4095);                                  // 转换成电压值      
        printf("采集到ADC值=%4d    转换为电压值=%4.2fV\r", gpioADC, voltage); // 把结果输出到串口软件 
        #else
        // 方式2：使用内部参考电压作参考，更精准，但耗时
        gpioADC=ADC1_GetValue(GPIOA, GPIO_Pin_1);                             // 获取通道的ADC转换值
        InteriorADC=ADC1_GetInteriorVrefint();                                // 获取内部参考电压1.20V的ADC值
        voltage  =1.20*( gpioADC/(float)InteriorADC);                         // 转换成电压值      
        printf("通道引脚ADC值=%4d    内部参考电压ADC值=%4d    转换后电压值=%4.2fV\r", gpioADC, InteriorADC, voltage); // 把结果输出到串口软件 
        
        gpioADC=ADC1_GetValue(GPIOA, GPIO_Pin_4);                             // 获取通道的ADC转换值
        InteriorADC=ADC1_GetInteriorVrefint();                                // 获取内部参考电压1.20V的ADC值
        voltage  =1.20*( gpioADC/(float)InteriorADC);                         // 转换成电压值      
        printf("通道引脚ADC值=%4d    内部参考电压ADC值=%4d    转换后电压值=%4.2fV\r\r", gpioADC, InteriorADC, voltage); // 把结果输出到串口软件 
        #endif        
    }          
}

