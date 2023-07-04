#include "bsp_key.h"






void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                    // ���ã��������Ź���ģʽ 
    
    // ʹ��GPIOʱ��, Ϊ���ٵ��Թ�������ʹ��ʱ�Ӷ����������GPIO�˿�ʱ�ӣ���ʹ����;
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF ; 
   
    // KEY_1
    GPIO_InitStructure.GPIO_Pin  = KEY_1_PIN;               // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;          // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;      // �����ƽ��ת�ٶ�;������״̬ʱ��Ч����GPIO_Init������Ҫ�õ�;
    GPIO_Init(KEY_1_GPIO, &GPIO_InitStructure);             // ��ʼ�����������Ź���ģʽ���ú���
    
    // KEY_2
    GPIO_InitStructure.GPIO_Pin  = KEY_2_PIN;               // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;          // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;      // �����ƽ��ת�ٶ�;������״̬ʱ��Ч����GPIO_Init������Ҫ�õ�;
    GPIO_Init(KEY_2_GPIO, &GPIO_InitStructure);             // ��ʼ�����������Ź���ģʽ���ú���

    // KEY_3
    GPIO_InitStructure.GPIO_Pin  = KEY_3_PIN;               // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;          // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;      // �����ƽ��ת�ٶ�;������״̬ʱ��Ч����GPIO_Init������Ҫ�õ�;
    GPIO_Init(KEY_3_GPIO, &GPIO_InitStructure);             // ��ʼ�����������Ź���ģʽ���ú���
}



/******************************************************************************
 * ��  ���� Key_Scan
 * ��  �ܣ� ɨ�谴��״̬
 * ��  ���� GPIOx��     GPIO�˿� 
 *          PINx :      Pin���ű��
 *          downStatus: ����ʱ�ĵ�ƽ
 *
 * ����ֵ�� KEY_ON : ����
 *          KEY_OFF���޶���
 *     
 * ˵  ���� 1: ��������ⷽʽΪ��while����ɨ�衱�����ְ��������ҵȴ������ɿ���Ϊһ����Ч���¶���;   
 *          2������������ֻ��ʾ��ʹ�ã�ʵ����Ŀ�У�����ʹ���жϷ������;
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




