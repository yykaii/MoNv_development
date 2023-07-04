#include "bsp_tim.h"


xTIM_TypeDef  xTIM;         // ����Ϊȫ�ֱ���,�����¼��Ϣ��״̬


/*****************************************************************************
 * ��  ���� TIM6_Init
 * ��  �ܣ� ���û�����ʱ��TIM6
 * ��  ���� uint16_t PSC :  Ԥ��Ƶ����ֵ,16λ; �������ü�����1�����ʱ��(ʱ��Ƶ��);
 *          uint16_t ARR :  �Զ���װ����ֵ,16λ; �����ٸ��������һ�����»��ж�(1����);
 * ����ֵ�� ��
 * ��  Ҫ�� 1: F103ϵ�У�TIM��ʱ��ԴƵ��Ϊ72000000
 *          2: TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��
 *             TIM6��TIM7������ʱ������ֻ���õ�2��������3��������ͨ�á��߼���ʱ�����á�
 *             typedef struct
 *             {
 *                 TIM_Prescaler ;          // ���ж�ʱ�� ����
 *                 TIM_CounterMode ;        // ������ʱ��TIMx,x[6,7]û�У���������
 *                 TIM_Period ;             // ���ж�ʱ�� ����
 *                 TIM_ClockDivision ;      // ������ʱ��TIMx,x[6,7]û�У���������
 *                 TIM_RepetitionCounter ;  // TIMx,x[1,8,15,16,17]����
 *             }TIM_TimeBaseInitTypeDef;
*****************************************************************************/
void TIM6_Init(uint16_t PSC, uint16_t ARR)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        // �����ж���Ϊ2, ������ã�������main()�����ĵ�1�У��Է���ͳһ���������жϵķ���
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;           // �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ���������ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // ������ռ���ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // �����ж�
    NVIC_Init(&NVIC_InitStructure);

#if 0                                                         // ��׼��д��
#include "stm32f10x_tim.h"
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);      // ����TIM6ʱ��,���ڲ�ʱ��CK_INT=72M
    TIM_Cmd(TIM6, DISABLE);                                   // ������ǰ���ȹرռ�����
    TIM_TimeBaseStructure.TIM_Prescaler = PSC - 1;            // Ԥ��Ƶ����ֵ,16λ; ��������ʱ��Ƶ��CK_CNT����f CK_PSC /(PSC[15:0]+1)����ÿһ�θ����¼�ʱ��PSC����ֵ�����͵�ʵ�ʵ�Ԥ��Ƶ�Ĵ����С�
    TIM_TimeBaseStructure.TIM_Period    = ARR - 1;            // �Զ���װ����ֵ,16λ; �����ٸ��������һ�����»��ж�(1����)������Զ���װ����ֵΪ0���������ֹͣ��
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);           // ��ʼ����ʱ��

    TIM_ClearFlag(TIM6, TIM_FLAG_Update);                     // ����������жϱ�־λ
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                // ���������ж�(ֻ�ǿ������ܣ�δ������
#else                                                         // �Ĵ���д��, �ŵ㣺������ֲ������
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN ;                      // ����TIM6ʱ��,
    TIM6->CR1  = 0;                                           // ������ǰ���ȹرռ�����, ����0 CR1������
    TIM6->CR2  = 0;                                           // ��0 CR2������
    TIM6->SR   = 0;                                           // ��0 SR�Ĵ���, ���Ĵ���ֻ��һ������: �жϱ�־λ��
    TIM6->CNT  = 0;                                           // ��0 ��������ֵ
    TIM6->DIER = 0x01 ;                                       // ���������ж�(ֻ�ǿ������ܣ�δ������, �ر�DMA����
    TIM6->CR1 |= 1 << 7;                                      // ����Ԥװ��
    TIM6->PSC  = PSC - 1;                                     // Ԥ��Ƶ����ֵ,16λ; ��������ʱ��Ƶ��CK_CNT����f CK_PSC /(PSC[15:0]+1)����ÿһ�θ����¼�ʱ��PSC����ֵ�����͵�ʵ�ʵ�Ԥ��Ƶ�Ĵ����С�
    TIM6->ARR  = ARR - 1;                                     // �Զ���װ����ֵ,16λ; �����ٸ��������һ�����»��ж�(1����)������Զ���װ����ֵΪ0���������ֹͣ��
#endif
}

/*****************************************************************************
 * ��  ���� TIM6_SetARR
 * ��  �ܣ� ����TIM7���Զ���װ��ֵ
 * ��  ���� uint16_t ARR: �Զ���װ��ֵ����Ƶ������ֵ(���ٸ���Ƶ������壬���һ����)
 * ����ֵ�� ��
*****************************************************************************/
void TIM6_SetARR(uint16_t ARR)
{
    TIM6->ARR= ARR-1;    
}

/*****************************************************************************
 * ��  ���� TIM6_Start
 * ��  �ܣ� ����TIM6: ʹ��TIM6_Init���������ò���
 * ��  ���� ��
 * ����ֵ�� ��
*****************************************************************************/
void TIM6_Start(void)
{
    xTIM.TIM6Counter = 0;       // �жϼ�����0
    TIM6->CNT=0;                // �����Ĵ�����0
    TIM6->CR1 |= 0x01;          // ʹ��TIM6, ������ʼ����
}

/*****************************************************************************
 * ��  ���� TIM6_Stop
 * ��  �ܣ� ֹͣTIM6, ԭ���ò�����0;
 * ��  ���� ��
 * ����ֵ�� ��
*****************************************************************************/
void TIM6_Stop(void)
{
    TIM6->CR1 &= ~0x01;         // ֹͣTIM6
}

/*****************************************************************************
 * ��  ���� TIM6_IRQHandler
 * ��  �ܣ� �жϷ�����
 * ��  ���� ��
 * ����ֵ�� ��
*****************************************************************************/
void  TIM6_IRQHandler(void)
{
    TIM6->SR &= (uint16_t)~0x01;    // ��0�����жϱ�־λ
    xTIM.TIM6Counter ++ ;           // ����+1
}
