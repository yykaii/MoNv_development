#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "system_f103.h"





void Key_Init(void)
{
    GPIO_InitTypeDef G;                     // ���ã��������Ź���ģʽ 
    NVIC_InitTypeDef NVIC_InitStruct ;      // ���ã��������ȼ�
    EXTI_InitTypeDef EXTI_InitStruct ;      // ���ã����������жϷ�ʽ
    
    // ʹ��AFIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE) ;    // EXTI��ʱ��Ҫ����AFIO�Ĵ��� 
    
    // ʹ��GPIOʱ��, Ϊ���ٵ��Թ�������ʹ��ʱ�Ӷ����������GPIO�˿�ʱ�ӣ���ʹ����;
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF ; 
   
    // KEY_1
    // �������Ź���ģʽ
//    G.GPIO_Pin  = KEY_1_PIN;                                   // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
//    G.GPIO_Mode = GPIO_Mode_IPD ;                              // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
//    G.GPIO_Speed = GPIO_Speed_10MHz ;                          // �����ƽ��ת�ٶ�;������״̬ʱ��Ч����GPIO_Init������Ҫ�õ�;
//    GPIO_Init(KEY_1_GPIO, &G);                                 // ��ʼ�����������Ź���ģʽ���ú���
//    // �����жϵ����ȼ�
//    NVIC_InitStruct.NVIC_IRQChannel = KEY_1_IRQN ;             // �жϺ�
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;    // ������ռ���ȼ�     
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0 ;           // ���������ȼ�     
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;              // ʹ���ж�ͨ��    
//    NVIC_Init(&NVIC_InitStruct) ;                              // ��ʼ�����������ȼ����ú��� 
//    // �����жϵķ�ʽ      
//    GPIO_EXTILineConfig( KEY_1_GPIOSOURCE , KEY_1_PINSOURCE);  // ѡ����ΪEXTI�ߵ�GPIO����
//    EXTI_InitStruct.EXTI_Line    = KEY_1_EXTI_LINE ;           // �����ж�or�¼���     
//    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;       // ģʽ���жϣ�EXTI_Mode_Interrupt���¼���EXTI_Mode_Event
//    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising ;       // ���ش����� ������EXTI_Trigger_Rising ���½���EXTI_Trigger_Falling �����գ�EXTI_Trigger_Rising_Falling
//    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                    // ʹ��EXTI��
//    EXTI_Init(&EXTI_InitStruct) ;                              // ��ʼ���������ж������ú���  
    
    // KEY_2
    // �������Ź���ģʽ
    G.GPIO_Pin  = KEY_2_PIN;                                   // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
    G.GPIO_Mode = GPIO_Mode_IPU ;                              // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
    G.GPIO_Speed = GPIO_Speed_10MHz ;                          // �����ƽ��ת�ٶ�;������״̬ʱ��Ч����GPIO_Init������Ҫ�õ�;
    GPIO_Init(KEY_2_GPIO, &G);                                 // ��ʼ�����������Ź���ģʽ���ú���
    // �����жϵ����ȼ�
    NVIC_InitStruct.NVIC_IRQChannel = KEY_2_IRQN ;             // �жϺ�
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;    // ������ռ���ȼ�     
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0 ;           // ���������ȼ�     
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;              // ʹ���ж�ͨ��    
    NVIC_Init(&NVIC_InitStruct) ;                              // ��ʼ�����������ȼ����ú��� 
    // �����жϵķ�ʽ      
    GPIO_EXTILineConfig( KEY_2_GPIOSOURCE , KEY_2_PINSOURCE);  // ѡ����ΪEXTI�ߵ�GPIO����
    EXTI_InitStruct.EXTI_Line    = KEY_2_EXTI_LINE ;           // �����ж�or�¼���     
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;       // ģʽ���жϣ�EXTI_Mode_Interrupt���¼���EXTI_Mode_Event
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling ;      // ���ش����� ������EXTI_Trigger_Rising ���½���EXTI_Trigger_Falling �����գ�EXTI_Trigger_Rising_Falling
    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                    // ʹ��EXTI��
    EXTI_Init(&EXTI_InitStruct) ;                              // ��ʼ���������ж������ú���  
        
    // KEY_3
    // �������Ź���ģʽ
    G.GPIO_Pin  = KEY_3_PIN;                                   // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
    G.GPIO_Mode = GPIO_Mode_IPU ;                              // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
    G.GPIO_Speed = GPIO_Speed_10MHz ;                          // �����ƽ��ת�ٶ�;������״̬ʱ��Ч����GPIO_Init������Ҫ�õ�;
    GPIO_Init(KEY_3_GPIO, &G);                                 // ��ʼ�����������Ź���ģʽ���ú���
    // �����жϵ����ȼ�
    NVIC_InitStruct.NVIC_IRQChannel = KEY_3_IRQN ;             // �жϺ�
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1 ;    // ������ռ���ȼ�     
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0 ;           // ���������ȼ�     
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE ;              // ʹ���ж�ͨ��    
    NVIC_Init(&NVIC_InitStruct) ;                              // ��ʼ�����������ȼ����ú��� 
    // �����жϵķ�ʽ      
    GPIO_EXTILineConfig( KEY_3_GPIOSOURCE , KEY_3_PINSOURCE);  // ѡ����ΪEXTI�ߵ�GPIO����
    EXTI_InitStruct.EXTI_Line    = KEY_3_EXTI_LINE ;           // �����ж�or�¼���     
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt ;       // ģʽ���жϣ�EXTI_Mode_Interrupt���¼���EXTI_Mode_Event
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling ;      // ���ش����� ������EXTI_Trigger_Rising ���½���EXTI_Trigger_Falling �����գ�EXTI_Trigger_Rising_Falling
    EXTI_InitStruct.EXTI_LineCmd = ENABLE ;                    // ʹ��EXTI��
    EXTI_Init(&EXTI_InitStruct) ;                              // ��ʼ���������ж������ú���  
        
    printf("���� ��ʼ��           �������\r");  
}


// KEY_1 �жϷ�����
//void KEY_1_IRQHANDLER(void)
//{           
//    if(EXTI->PR & KEY_1_PIN)
//    {  
//        EXTI->PR |= KEY_1_PIN  ;                  
//        USART1_SendString("\r�� 1 ������������, ���Ʒ�ת\r");         // ħŮ������İ���ʹ�õ��ݽ���Ӳ������,������ʹ�������ʱ���� 
//        LED_BLUE_TOGGLE;  
//    }        
//}

// KEY_2 �жϷ�����
void KEY_2_IRQHANDLER(void)
{           
    if(EXTI->PR & KEY_2_PIN)
    {  
        EXTI->PR |= KEY_2_PIN  ;                  
        USART1_SendString("\r�� 2 ������������, ���Ʒ�ת\r");         // ħŮ������İ���ʹ�õ��ݽ���Ӳ������,������ʹ�������ʱ���� 
        LED_BLUE_TOGGLE;  
        //System_Standby();
            
        	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //ʹ��PWR����ʱ��
	
	PWR_ClearFlag(PWR_FLAG_WU);                         //���Wake-up ��־
	
	PWR_WakeUpPinCmd(ENABLE);  //ʹ�ܻ��ѹܽ�	ʹ�ܻ���ʧ�ܻ��ѹܽŹ���
	
	
	PWR_EnterSTANDBYMode();//�������ģʽ
            
            
            
    }        
}
// KEY_3 �жϷ�����
void KEY_3_IRQHANDLER(void)
{           
    if(EXTI->PR & KEY_3_PIN)
    {  
        EXTI->PR |= KEY_3_PIN  ;                  
        USART1_SendString("\r�� 3 ������������, ���Ʒ�ת\r");         // ħŮ������İ���ʹ�õ��ݽ���Ӳ������,������ʹ�������ʱ���� 
        LED_BLUE_TOGGLE;  
    }        
}




