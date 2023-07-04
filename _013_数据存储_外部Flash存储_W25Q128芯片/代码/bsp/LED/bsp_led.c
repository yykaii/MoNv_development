/***********************************************************************************************************************************
 ** ���ļ����ơ�  led.c
 ** ����д��Ա��  ħŮ�������Ŷ�
 ** ����    ����  ħŮ������      https://demoboard.taobao.com
 ***********************************************************************************************************************************
 ** ���ļ����ܡ�  ʵ��LEDָʾ�Ƴ��õĳ�ʼ�����������ܺ���
 **                 
 ** ����ֲ˵����  
 **             
 ** �����¼�¼��  
 **
***********************************************************************************************************************************/  
#include "bsp_led.h"


    

void Led_Init(void)
{    
    GPIO_InitTypeDef GPIO_InitStructure;                    // ����һ��GPIO_InitTypeDef���͵Ľṹ��
    
    // ʹ��LED_RED�������Ŷ˿�ʱ�ӣ�ʹ�ö˿��жϵķ���ʹ��ʱ��, ��ʹ������ֲ������
    if(LED_RED_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(LED_RED_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(LED_RED_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(LED_RED_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(LED_RED_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(LED_RED_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(LED_RED_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    // ʹ��LED_RED�������Ŷ˿�ʱ�ӣ�ʹ�ö˿��жϵķ���ʹ��ʱ��, ��ʹ������ֲ������
    if(LED_BLUE_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    if(LED_BLUE_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    if(LED_BLUE_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    if(LED_BLUE_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
    if(LED_BLUE_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
    if(LED_BLUE_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    if(LED_BLUE_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
    
    // ����LED_RED���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin   = LED_RED_PIN;            // ѡ��Ҫ���Ƶ�GPIO����       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // ��������ģʽΪͨ���������   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������Ϊ50MHz            
    GPIO_Init(LED_RED_GPIO , &GPIO_InitStructure);          // ���ÿ⺯������ʼ��GPIO  
    
    // ����LED_RED���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin   = LED_BLUE_PIN;           // ѡ��Ҫ���Ƶ�GPIO����       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // ��������ģʽΪͨ���������   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������Ϊ50MHz            
    GPIO_Init(LED_BLUE_GPIO , &GPIO_InitStructure);         // ���ÿ⺯������ʼ��GPIO    
    
    LED_RED_GPIO -> BSRR = LED_RED_PIN ;                    // ����LED_RED�� �͵�ƽ����
    LED_BLUE_GPIO ->BSRR = LED_BLUE_PIN ;                   // ����LED_BLUE���͵�ƽ����
}



