#include "bsp_adc.h" 



static uint8_t FLAG_ADC1InitOK=0;   // ADC1��ʼ����ǣ�0��δ��ʼ����1���ѳ�ʼ��



// ����US������ʱ������������ֲʱ���ⲿ�ļ�������
#if 1
static void delayUS( __IO u32 times)
{
    times=times*7;      //  10us����7;
    while(--times)         
        __nop();  
}
#endif 



//��ʼ��ADC
static void  configADC1(void)
{       
    RCC->APB2ENR  |= 1<<9;             // ADC1ʱ��ʹ��      
    RCC->APB2RSTR |= 1<<9;             // ADC1��λ
    RCC->APB2RSTR &= ~(1<<9);          // ��λ����     

    RCC->CFGR &= ~(3<<14);             // ��Ƶ��������    
    RCC->CFGR |= 2<<14;                // SYSCLK/DIV2=72/6=12M ADCʱ������Ϊ12M,ADC���ʱ�Ӳ��ܳ���14M! 

    ADC1->CR1  = 0 ;                   // ��0 , CR1���ù���ģʽ
    ADC1->CR1 |= 0<<8;                 // ɨ��ģʽ(���ͨ��)  0:�ر�   1��ʹ��
    ADC1->CR1 |= 0<<16;                // ��������ģʽ  

    ADC1->CR2  = 0;                    // ��0, CR2���ù�������
    ADC1->CR2 |= 0<<1;                 // ����ת��   0������ת��   1������ת��
    ADC1->CR2 |= 0<<11;                // �Ҷ���    
    ADC1->CR2 |= 7<<17;                // �������ת��  
    ADC1->CR2 |= 1<<20;                // ʹ�����ⲿ����(SWSTART)!!!    ����ʹ��һ���¼�������
    ADC1->CR2 |= 0<<23;                // ʹ���ڲ�оƬ�ڲ��¶�ͨ��16���ڲ�Vrefintͨ��17

    ADC1->SQR1 = 0 ;                   // ��0
    ADC1->SQR1|= 0<<20;                // ��Ҫת����ͨ������ �״�����=N+1           

    ADC1->SMPR1 = 0x00FDB6DB;          // ͨ��17~10�������ڣ�ͨ��16��17�Ĳ���ʱ������Ϊ239.5���ڣ���(1-12MHz)*239.5����=19.9us(�ֲ��Ƽ�ֵ17us), ͨ��10~15����ʱ������Ϊ28.5���ڣ���2.3us 
    ADC1->SMPR2 = 0x1B6DB6DB;          // ͨ�� 9~ 0��������, ͨ��9~0����ʱ������Ϊ28.5���ڣ���2.3us 
    
    ADC1->CR2  |= 1<<0;                // ����ADת����     
    ADC1->CR2  |= 1<<3;                // ʹ�ܸ�λУ׼  
    while(ADC1->CR2&1<<3);             // �ȴ�У׼����, ��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ�������         
    ADC1->CR2  |= 1<<2;                // ����ADУ׼       
    while(ADC1->CR2&1<<2);             // �ȴ�У׼����, ��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ�������
    
    delayUS(50);                       // ����ʱ�������ȶ�
    FLAG_ADC1InitOK =1;                // ��ʼ���󣬱�־��1
}



// ��ʼ������Ҫ������
static void adc_GPIOInit(GPIO_TypeDef* GPIOx, uint32_t PINx)
{
    GPIO_InitTypeDef GPIO_InitStructure;               // ���ã��������Ź���ģʽ 
    
    // ʹ���������Ŷ˿�ʱ�ӣ�ʹ�ö˿��жϵķ���ʹ��ʱ��, ��ʹ������ֲ������
    if(GPIOx == GPIOA)  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
    if(GPIOx == GPIOB)  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
    if(GPIOx == GPIOC)  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; 
    if(GPIOx == GPIOD)  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN; 
    if(GPIOx == GPIOE)  RCC->APB2ENR |= RCC_APB2ENR_IOPEEN; 
    #if defined (STM32F10X_HD)
    if(GPIOx == GPIOF)  RCC->APB2ENR |= RCC_APB2ENR_IOPFEN; 
    if(GPIOx == GPIOG)  RCC->APB2ENR |= RCC_APB2ENR_IOPGEN; 
    #endif
        
    // ����LED_RED���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin   = PINx;              // ѡ��Ҫ���Ƶ�����       
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;     // ��������ģʽΪģ������   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // ������������Ϊ50MHz; ����������ģʽʱ����ֵ����Ч            
    GPIO_Init(GPIOx , &GPIO_InitStructure);            // ���ÿ⺯������ʼ��GPIO     
};



// ������ADC1��2��ͨ��GPIO�˿ڡ����źţ�����ͨ����
// ���أ�������0~15��ʧ�ܣ�99
static uint8_t adc_PinConvertChannel(GPIO_TypeDef* GPIOx, uint32_t PINx)
{
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_0))    return  0;  // PA0 = ͨ�� 0
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_1))    return  1;  // PA1 = ͨ�� 1
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_2))    return  2;  // PA2 = ͨ�� 2
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_3))    return  3;  // PA3 = ͨ�� 3
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_4))    return  4;  // PA4 = ͨ�� 4
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_5))    return  5;  // PA5 = ͨ�� 5
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_6))    return  6;  // PA6 = ͨ�� 6
    if((GPIOx==GPIOA) && (PINx==GPIO_Pin_7))    return  7;  // PA7 = ͨ�� 7
    if((GPIOx==GPIOB) && (PINx==GPIO_Pin_0))    return  8;  // PB0 = ͨ�� 8
    if((GPIOx==GPIOB) && (PINx==GPIO_Pin_1))    return  9;  // PB1 = ͨ�� 9
    if((GPIOx==GPIOC) && (PINx==GPIO_Pin_0))    return 10;  // PC0 = ͨ��10
    if((GPIOx==GPIOC) && (PINx==GPIO_Pin_1))    return 11;  // PC1 = ͨ��11
    if((GPIOx==GPIOC) && (PINx==GPIO_Pin_2))    return 12;  // PC2 = ͨ��12
    if((GPIOx==GPIOC) && (PINx==GPIO_Pin_3))    return 13;  // PC3 = ͨ��13
    if((GPIOx==GPIOC) && (PINx==GPIO_Pin_4))    return 14;  // PC4 = ͨ��14
    if((GPIOx==GPIOC) && (PINx==GPIO_Pin_5))    return 15;  // PC5 = ͨ��15     
    return 99; // ���û�ҵ� ƥ��Ĳ������ͷ��ش����       
}


// Ϊ��ֲʱ��������ṹ�������ù�ϵ������������Ϊȫ�ֺ���
// ��ͨ�����ж�β�ֵ��������ƽ��ֵ
// ������  uint8_t channel: ͨ��ֵ����ѡ��Χ0~17
//         uint8_t times:   ��ֲ����
// ����ֵ����β�ֵ���ƽ��ֵ��ֵ��Χ��0~4095
static uint16_t ADC1_GetADC1Value(uint8_t channel, uint8_t times)
{
    uint32_t valueSUM = 0;                        // ADC�Ķ��ȡֵ�ۼӽ��  
    
    if(FLAG_ADC1InitOK==0)  configADC1();         // ����ADC1�Ĺ���ģʽ
    
    if(channel ==16 || channel ==17)              // ����ǵ�16��17ͨ������ʹ��TSVREFEλ
        ADC1->CR2 |= 1<<23;                       // ʹ�� �¶ȴ�����
        
        
    ADC1->SQR3 = channel;                         // ��1��Ҫת����ͨ��,�ڲ��ο���ѹת��ֵͨ��
    
    for(u8 t=0; t<times; t++)                     // �����ֵ�����ۼӳɽ��ֵ
    {   
        ADC1->CR2 |= 1<<22;                       // ��������ת��ͨ�� 
        while(!(ADC1->SR & 1<<1));                // �ȴ�����ת������            
        valueSUM = valueSUM + ADC1->DR;           // ���ӱ��β�ֵ
        delayUS(2);                               // ��Ϊ��ʱһ��
    }   
    
    return valueSUM / times;                      // ����ƽ��ֵ       
}
     


/******************************************************************************
 * ��  ���� ADC1_GetInteriorTemperature
 * ��  �ܣ� ��ȡоƬ�ڲ��¶�ֵ����ADC1ͨ��16
 *          ��������ֱ�ӵ��ü�����������������ǰ��ADC��ʼ��
 * ��  ���� 
 * ����ֵ�� ���϶�ֵ
 *     
 * ˵  ���� 1: ��������ֵ��ʽΪ��while���ȡ���ÿ����һ�κ�������ʱԼ: 19us*5�� = 100US����;   
 *             ��ϵͳʵʱ��Ҫ��ϸߣ�������жϷ�ʽ��
 *          2���¶�ֵ��Χ��-40~125�ȣ����ȣ��� 1.5������
 *          3��ADC�¶�ͨ���Ĳ���ʱ�䣬оƬ�ֲ����Ƽ�Ϊ17.1us, ԶС�ڴ�ֵʱ���ݳ���
 ******************************************************************************/ 
float  ADC1_GetInteriorTemperature(void)
{
    uint16_t adcValue;
    float TEMP=0.0;
    
    adcValue = ADC1_GetADC1Value(16, 5);;    // ���ض�β�ֵ���ƽ��ֵ   
    TEMP = (float)adcValue * (3.3/4095);     // ���ѹ
    TEMP = (1.43-TEMP)/0.0043 +25;           // ��ѹת�������ϵ�
    return TEMP;
}



/******************************************************************************
 * ��  ���� ADC1_GetInteriorVrefint
 * ��  �ܣ� ��ȡоƬ�ڲ��¶�ֵ����ADC1ͨ��17
 *          ��������ֱ�ӵ��ü�����������������ǰ��ADC��ʼ��
 * ��  ���� 
 * ����ֵ�� �ڲ��ο���ѹ1.20V��ADCֵ
 *     
 * ˵  ���� 1: ��������ֵ��ʽΪ��while���ȡ���ÿ����һ�κ�������ʱԼ: 19us*5�� = 100US����;   
 *             ��ϵͳʵʱ��Ҫ��ϸߣ�������жϷ�ʽ��
 *          2���ڲ��ο���ѹ��ADCֵ�����������ⲿ��ѹ�����ο�����ʽ���£�
 *             ������ѹֵ = 1.20*(����ADC����ֵ/�ڲ���ѹADC����ֵ);
 ******************************************************************************/ 
uint16_t ADC1_GetInteriorVrefint(void)
{              
    return  ADC1_GetADC1Value( 17, 5);;           // ���ض�β�ֵ���ƽ��ֵ      
}



/******************************************************************************
 * ��  ���� ADC1_GetGPIOValue
 * ��  �ܣ� ��ȡĳ�����ϵ�ADCֵ
 *  
 * ��  ���� GPIO_TypeDef* GPIOx   GPIO�˿�, ��ѡ������GPIOA ~ GPIOG
 *          uint32_t PINx         ���ţ�    ��ѡ������GPIO_Pin_0 ~ GPIO_Pin_15
 * ����ֵ�� ʧ�ܣ�8888�������������  
 *          ������0~4095 
 *
 * ˵  ���� 1: ��������ֵ��ʽΪ��while���ȡ���ÿ����һ�κ�������ʱԼ: 2.3us*10=23us; 
 *             ��ϵͳʵʱ��Ҫ��ϸߣ�������жϷ�ʽ��
 *          2����������ֱ�ӵ��ü�����������������ǰ��ADC��GPIO��ʼ��
 ******************************************************************************/ 
uint16_t ADC1_GetGPIOValue(GPIO_TypeDef* GPIOx, uint32_t PINx)
{        
    uint8_t  channel  = 0;                        // ͨ����
    
    channel = adc_PinConvertChannel(GPIOx, PINx); // �������ţ��ж�����ADC1��ͨ��
    if(channel == 99)  return 8888;               // ����������ź������˳�������0
        
    adc_GPIOInit(GPIOx, PINx);                    // ��ʼ������Ϊģ������ģʽ
    
    return  ADC1_GetADC1Value( channel, 10);      // ���ض�β�ֵ���ƽ��ֵ   
} 




