/***********************************************************************************************************************************
 **���������ӡ�  ħŮ�Ƽ�   https://demoboard.taobao.com
 **�����·���  QȺ�ļ���  1126717453
 ***********************************************************************************************************************************
 **���ļ����ơ�  pwm.c
 **������������  ����ʵ��
 **
 **������ƽ̨��  STM32F103 + ��׼��v3.5 + keil5
 **                 
 **����ֲ˵����  �����޸ģ��ڱ�h�ļ����޸�
 **              TIM���� ���ڱ�h�ļ���
 **           
 **�����¼�¼��  2021-05-16  ����;
 **              2021-07-06  ����TIM1��TIM8�жϣ�����MOEλ;
 **              2021-07-06  ���ƺ������ƣ�����vǰꡣ���ʾ����void, ��ߴ���ɶ���;
 **
***********************************************************************************************************************************/
#include "bsp_pwm.h"


/*****************************************************************************
 ** ȫ�ֱ�������
 *****************************************************************************/



/******************************************************************************
 * �������� TIM_PwmInit
 * ��  �ܣ� PWM����
 * ��  ������GPIOx��GPIOA~F
 *         ��PINx�� GPIO_Pin_0~15
 *         ��TIMx�� TIM1~8
 *         ��CHx��  1~4
 *         ��psc��  72             // Ϊ�˷��㣬����72�󣬼�����������Ϊ:1us
 *         ��arr��  0~65535        // ����ʱ��(us)
 *         ��CCR��  0~65535     �� // �ߵ�ƽ����ʱ��(us)
 * ��  �أ� ��
 ******************************************************************************/  
void TIM_PwmInit(GPIO_TypeDef* GPIOx, u16 PINx, TIM_TypeDef* TIMx, u8 CHx, u16 psc, u16 arr, u16 ccr)
{    
    // TIMʱ��ʹ��
    if(TIMx==TIM1)    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    if(TIMx==TIM8)    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  
    if(TIMx==TIM2)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
    if(TIMx==TIM3)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
    if(TIMx==TIM4)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  
    if(TIMx==TIM5)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  
    if(TIMx==TIM6)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  
    if(TIMx==TIM7)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);      

    // GPIOʱ��ʹ��
    if(GPIOx==GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if(GPIOx==GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if(GPIOx==GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if(GPIOx==GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if(GPIOx==GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if(GPIOx==GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    
    // GPIO����
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = PINx;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
    
    // ʹ�üĴ�������
    #if 1
    // TIMʱ������
    TIMx->PSC  = psc-1;           // CNTԤ��Ƶֵ=72000000/PSC
    TIMx->ARR  = arr-1;           // �Զ���װ��ֵ, �����ٸ�CNT����Ϊһ����
    TIMx->CR1  = 0;               // ��λ��0
    TIMx->CR1 |= 0<< 4;           // DIR����, 0-����, 1-����
    TIMx->CR1 |= 1<< 7;           // ARRԤװ��ʹ��; 0-������,�����ı�ARRֵ; 1-����,�ȸ����¼�ʱ�ٸı�ARRֵ
 
    // ����Ƚ�ģʽ
    u16 regCCMR =0;
    regCCMR |= 0x00<<0;           // ���ģʽ
    regCCMR |= 0x01<<3;           // CCRx�Ĵ���Ԥװ��ʹ��
    regCCMR |= 0x06<<4;           // ���ģʽ; 6=PW1,���ϼ���ʱ,CNT<CCRʱΪ��Ч��ƽ; 7=PWM2, ���ϼ���ʱ, CNT<CCRʱΪ��Ч��ƽ
    if(CHx==1)  TIMx->CCMR1 = (TIMx->CCMR1 & (~((u16)0xFF<<0))) | (regCCMR<<0);
    if(CHx==2)  TIMx->CCMR1 = (TIMx->CCMR1 & (~((u16)0xFF<<8))) | (regCCMR<<8);
    if(CHx==3)  TIMx->CCMR2 = (TIMx->CCMR2 & (~((u16)0xFF<<0))) | (regCCMR<<0);
    if(CHx==4)  TIMx->CCMR2 = (TIMx->CCMR2 & (~((u16)0xFF<<8))) | (regCCMR<<8);

    // ͨ��ʹ������
    u16 regCCER = 0;
    regCCER |= 0<<1;              // ͨ��x��Ч��ƽ; 0-�ߵ�ƽ��Ч; 1-�͵�ƽ��Ч
    regCCER |= 1<<0;              // ͨ��xʹ��
    TIMx->CCER = (TIMx->CCER & (~(0xFF<<((CHx-1)*4)))) | (regCCER<<((CHx-1)*4));

    if((TIMx==TIM1) || (TIMx==TIM8))  TIMx->BDTR |= TIM_BDTR_MOE;  // �߼���ʱ������Ҫ����BDTR��MOEλ
    TIMx->CR1 |= 1<<0;            // ʹ�ܼ�����
    
    
    // ʹ�ñ�׼������
    #else
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    // TIMʱ������
    TIM_TimeBaseStructure.TIM_Prescaler= (psc-1);                   // ����CNT��������ʱ�� = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Period= (arr-1);                      // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;           // ʱ�ӷ�Ƶ���� ��������������ʱ�䣬û�õ�������
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;       // ����������ģʽ������Ϊ���ϼ���    
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;                  // �ظ���������ֵ��û�õ���������������
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);                 // ��ʼ����ʱ��        
    // ����Ƚ�ģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               // ����ΪPWMģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // ���ʹ��
    TIM_OCInitStructure.TIM_Pulse = ccr;                            // ����ռ�ձȴ�С
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        // ���ͨ����ƽ��������
    if(CHx==1)  TIM_OC1Init(TIMx, &TIM_OCInitStructure);
    if(CHx==2)  TIM_OC2Init(TIMx, &TIM_OCInitStructure);
    if(CHx==3)  TIM_OC3Init(TIMx, &TIM_OCInitStructure);
    if(CHx==4)  TIM_OC4Init(TIMx, &TIM_OCInitStructure);
    // CCRԤװ��
    if(CHx==1)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);    
    if(CHx==2)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    if(CHx==3)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    if(CHx==4)  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIMx, ENABLE);                                          // ʹ�ܼ�����
    //TIM_CtrlPWMOutputs(TIMx, ENABLE);                             // �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ  
    #endif
    
    // ���ó�ʼ�����ռ�ձ�ֵ
    TIM_SetCCR(TIMx, CHx, ccr);

}



/******************************************************************************
 * �������� TIM_SetCCR
 * ��  �ܣ� �ߵ�ƽ����ʱ��(us��
 * ��  ������TIMx��TIM1~8
 *         ��CHx�� 1~4
 *         ��CCR�� 0~65535;     // ��Init���÷�Ƶ�������ʱ��Ϊ:1us, ��CCRֵ���ȼ���usֵ
 * ��  �أ� ��
 ******************************************************************************/  
void TIM_SetCCR(TIM_TypeDef* TIMx, u8 CHx, u16 CCR) 
{       
    //if(CCR>TIM_PWM_ARR)    CCR=TIM_PWM_ARR;  // �������ʱ����������ʱ������ȡ���ֵ
    //if(CCR<0)         CCR=0;
                
    if(CHx==1)    TIMx->CCR1 = CCR;
    if(CHx==2)    TIMx->CCR2 = CCR;
    if(CHx==3)    TIMx->CCR3 = CCR;
    if(CHx==4)    TIMx->CCR4 = CCR;        
}









