#include "scheduler.h"
#include "hardware.h"



#if 1  // 任务调度所用到的 变量、函数， 用#if折叠起来方便查看
static u16 usTick_1ms     =0;
static u16 usTick_2ms     =0;
static u16 usTick_5ms     =0;
static u16 usTick_10ms    =0;
static u16 usTick_50ms    =0;
static u16 usTick_100ms   =0;
static u16 usTick_500ms   =0;
static u16 usTick_1000ms = 0;

void vTask_1ms(void);
void vTask_2ms(void);
void vTask_5ms(void);
void vTask_10ms(void);
void vTask_50ms(void);
void vTask_100ms(void);
void vTask_500ms(void);
void vTask_1000ms(void);


void Scheduler_TickCnt(void)
{
    ++usTick_1ms;   
    ++usTick_2ms;   
    ++usTick_5ms;   
    ++usTick_10ms;  
    ++usTick_50ms;  
    ++usTick_100ms; 
    ++usTick_500ms; 
    ++usTick_1000ms;
}



/*****************************************************************************
 * 函  数：vScheduler_Run
 * 功  能：任务调试器; while中不断调用本函数，判断是否有任务要执行
 * 参  数：
 * 返回值：
 * 备  注： 魔女开发板团队 2020年04月21日
*****************************************************************************/
void Scheduler_Run(void)
{
    if( usTick_1ms   >=1)    { usTick_1ms=0;      vTask_1ms();   }   // 1ms   执行1次的任务
    if( usTick_2ms   >=2)    { usTick_2ms=0;      vTask_2ms();   }   // 2ms   执行1次的任务
    if( usTick_5ms   >=5)    { usTick_5ms=0;      vTask_5ms();   }   // 5ms   执行1次的任务
    if( usTick_10ms  >=10)   { usTick_10ms=0;     vTask_10ms();  }   // 10ms  执行1次的任务
    if( usTick_50ms  >=50)   { usTick_50ms=0;     vTask_50ms();  }   // 50ms  执行1次的任务
    if( usTick_100ms >=100)  { usTick_100ms=0;    vTask_100ms(); }   // 100ms 执行1次的任务
    if( usTick_500ms >=500)  { usTick_500ms=0;    vTask_500ms(); }   // 500ms 执行1次的任务
    if( usTick_1000ms>=1000) { usTick_1000ms=0;   vTask_1000ms();}   // 1000ms执行1次 的任务
}
#endif



/*****************************************************************************
 ** 每 1ms 间隔执行一次
 ****************************************************************************/
void vTask_1ms(void)
{   
    // 每1ms执行一次的代码     
    
}

/*****************************************************************************
 ** 每 2ms 间隔执行一次
 ****************************************************************************/
void vTask_2ms(void)
{  
    // 每2ms执行一次的代码  
    
    
}

/*****************************************************************************
 ** 每 5ms 间隔执行一次
 ****************************************************************************/
void vTask_5ms(void)
{    
    // 每5ms执行一次的代码 
    
}

/*****************************************************************************
 ** 每 10ms 间隔执行一次
 ****************************************************************************/
void vTask_10ms(void)
{    
    // 每10ms执行一次的代码 
    
}

/*****************************************************************************
 ** 每 50ms 间隔执行一次
 ****************************************************************************/
void vTask_50ms(void)
{   
    // 每50ms执行一次的代码     

}

/*****************************************************************************
 ** 每 100ms 间隔执行一次
 ****************************************************************************/
void vTask_100ms(void)
{   
    // 每100ms执行一次的代码 
    
}

/*****************************************************************************
 ** 每 500ms 间隔执行一次
 ****************************************************************************/
void vTask_500ms(void)
{
    // 每500ms执行一次的代码     

}

/*****************************************************************************
 ** 每 1000ms 间隔执行一次
 ****************************************************************************/
void vTask_1000ms(void)
{    
    // 每1000ms执行一次的代码 
    
}


