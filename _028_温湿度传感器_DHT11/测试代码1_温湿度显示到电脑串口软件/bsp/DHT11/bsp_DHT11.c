/***********************************************************************************************************************************
 **���������ӡ�  ħŮ�Ƽ�    https://demoboard.taobao.com
 **�����·���  QȺ�ļ���
 ***********************************************************************************************************************************
 **���ļ����ơ�  bsp_DHT11.c
 **���ļ����ܡ�
 **������ƽ̨��  STM32F103 + ��׼��v3.5 + keil5
 **
 **
 **�����¼�¼��  2021-12-23  �޸ĳ�ʼ��������ʹ�ļ���ֲ������: DHT11_Init(GPIOx, GPIO_Pin_x);
 **              2021-10-26  �޸�c��h�ļ��ļ���ʽ
 **              2021-10-26  �޸�DHT11_GetData()����
 **              2021-05-20  �����ļ�
 **
************************************************************************************************************************************/
#include "bsp_DHT11.h"





xDHT11_TypeDef       xDHT11;             // ����ȫ�ֽṹ��, ���ڼ�¼��Ϣ



static GPIO_TypeDef *DHT11_GPIOx = 0;    // ���Ŷ˿�
static uint32_t      DHT11_PINx  = 0;    // ���ű��



#define  DHT11_BUS_HIGH     (DHT11_GPIOx->BSRR = (uint32_t)DHT11_PINx)           // DAT���� �øߵ�ƽ
#define  DHT11_BUS_LOW      (DHT11_GPIOx->BSRR = ((uint32_t)DHT11_PINx) << 16)   // DAT���� �õ͵�ƽ
#define  DHT11_BUS_READ     ((DHT11_GPIOx->IDR & DHT11_PINx) ? 1: 0)             // ��ȡ���ŵĵ�ƽ



#if DELAY_MODE_TIM2
#include "stm32f10x_tim.h"
#include "misc.h"
// ��ʱ����ʼ������
// ��ʹ���ж�
void Tim2_Init(uint16_t psc, uint16_t arr, FunctionalState NewState)
{
    TIM_TimeBaseInitTypeDef tim;                         // �ṹ��

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // ����TIMʱ��
    TIM_DeInit(TIM2);
    tim.TIM_ClockDivision = TIM_CKD_DIV1;                // ������Ƶ
    tim.TIM_CounterMode   = TIM_CounterMode_Up;          // ���ϼ���
    tim.TIM_Period        = arr;                         // �Զ���װ�ؼĴ�����ֵ
    tim.TIM_Prescaler     = psc - 1;                     // ʱ��Ԥ��Ƶ
    TIM_TimeBaseInit(TIM2, &tim);                        // ��ʼ���ṹ��
    TIM2->SR = (uint16_t)~((uint16_t)0x01);              // ������±�־ 
    TIM2->CNT = 0;                                       // ��0������
    TIM_Cmd(TIM2, NewState);                             // �Ƿ�ʼ����
}



static void delay_us(uint32_t us)
{
    Tim2_Init(72, us, ENABLE);
    while ((TIM2->SR & 1) == 0);
    TIM_Cmd(TIM2, DISABLE);
}



static void delay_ms(uint32_t ms)
{
    for (uint16_t i = 0; i < ms; i++)
    {
        Tim2_Init(72, 990, ENABLE);
        while ((TIM2->SR & 1) == 0);
        TIM_Cmd(TIM2, DISABLE);
    }
}
#else

// ����US������ʱ������������ֲʱ���ⲿ�ļ�������
static void delay_us(uint32_t us)
{
    uint16_t i = 0;
    while (us--)
    {
        i = 7;
        while (i--);
    }
}

// ����MS������ʱ������������ֲʱ���ⲿ�ļ�������
static void delay_ms(uint32_t ms)
{
    uint32_t i = 0;
    while (ms--)
    {
        i = 12000;
        while (i--);
    }
}
#endif // ������ʱ



static void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DHT11_PINx ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIOx, &GPIO_InitStructure);
}

static void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT11_PINx ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIOx, &GPIO_InitStructure);
}

static void DHT11_Init(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    DHT11_GPIOx = GPIOx;
    DHT11_PINx = PINx;

    // ʱ��ʹ�ܣ����Ŷ˿�;���ж϶˿ڵķ�ʽʹ��ʱ����, ������ֲʱ�Ĺ���
    if (GPIOx == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (GPIOx == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (GPIOx == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (GPIOx == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (GPIOx == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (GPIOx == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (GPIOx == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    DHT11_Mode_Out_PP();
    DHT11_BUS_HIGH;
}


// ��DHT11��ȡһ���ֽڣ�MSB����
// 1��ÿbit��50us�͵�ƽ���ÿ�ʼ��
// 2����26~28us�ĸߵ�ƽ��ʾ��0��
// 3����70us�ߵ�ƽ��ʾ��1��
// 4: ͨ�����Ӹߵ�ƽ���� x us��ĵ�ƽ��������������״̬
static uint8_t readByte(void)
{
    uint8_t  temp = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        while (DHT11_BUS_READ == 0);             // ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����
        delay_us(40);                             // ��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��
        temp <<= 1;
        if (DHT11_BUS_READ == 1)                 // x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1��
        {
            while (DHT11_BUS_READ == Bit_SET);  // �ȴ�����1�ĸߵ�ƽ����
            temp |= 1;                           // λ�á�1��
        }
    }
    return temp;
}


/******************************************************************************
 * ��  ���� DHT11_GetData
 * ��  �ܣ� ��DHT11�ж�ȡ����
 * ��  ���� 1:
 * ˵  ���� ���ú󣬻�ȡ�������ݣ����浽�ṹ��xDHT11��
 *          �¶�ֵ��xDHT11.Temperature (��Ч��Χ:0~50��)
 *          ʪ��ֵ: xDHT11.Humidity    (��Ч��Χ:20%~90%)
 * ����ֵ�� 0-ʧ��; 1-����
 ******************************************************************************/
uint8_t DHT11_GetData(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    static uint8_t humiInt = 0;        // ʪ�ȵ���������
    static uint8_t humiDec = 0;        // ʪ�ȵ�С������
    static uint8_t TempInt = 0;        // �¶ȵ���������
    static uint8_t TempDec = 0;        // �¶ȵ�С������
    static uint8_t sum = 0;            // У���

    DHT11_Init(GPIOx, PINx);

    // 1:����������ʼ�ź�
    DHT11_Mode_Out_PP();               // ���ģʽ
    DHT11_BUS_LOW;                     // �������͵�ƽ
    delay_ms(25);                      // ��ʱ18~30ms
    // 2���������ߵȴ�
    DHT11_BUS_HIGH;                    // ��������
    delay_us(50);                      // ��ʱ20~40us����������50������ΪҪֱ�ӽ�����һ��ʱ�򣨵�ƽ״̬�����Է�����

    // 3: �ӻ�������Ӧ��׼���ź�
    DHT11_Mode_IPU();                  // ������Ϊ���� �жϴӻ���Ӧ�ź�

    if (DHT11_BUS_READ == 0)           // �жϴӻ��Ƿ������Ӧ�ź�_�͵�ƽ, �粻��Ӧ������
    {
        while (DHT11_BUS_READ == 0);   // �ȴ���Ӧ�źŽ������͵�ƽ����Լ80us
        while (DHT11_BUS_READ == 1);   // �ȴ������źŽ������ߵ�ƽ����Լ80us
        // 4: �ӻ��������5�ֽ�����
        humiInt = readByte();          // ʪ�ȵ���������// ��ʼ��������
        humiDec = readByte();          // ʪ�ȵ�С������
        TempInt = readByte();          // �¶ȵ���������
        TempDec = readByte();          // �¶ȵ�С������
        sum     = readByte();          // У���

        DHT11_Mode_Out_PP();           // ��ȡ���������Ÿ�Ϊ���ģʽ
        DHT11_BUS_HIGH;                // ��������

        // 5: ����ȡ�������Ƿ���ȷ
        if (sum == (humiInt + humiDec + TempInt + TempDec))
        {
            xDHT11.Humidity = humiInt + humiDec;
            xDHT11.Temperature = (float)TempInt  + (float)TempDec / 10;
            return SUCCESS;            // У����ȷ, ���أ�SUCCESS=1
        }
        return ERROR;                  // У����󣬷��أ�ERROR=0
    }
    return ERROR;                      // ͨ�Ŵ��󣬷��أ�ERROR=0
}




/******************************************************************************
 * ��  ���� DHT11_GetTemperature
 * ��  �ܣ� ��DHT11�ж�ȡ�¶�ֵ
 * ��  ���� GPIO_TypeDef *GPIOx: GPIO�˿ںţ�ȡֵ��Χ��GPIOA ~ GPIOG
 *          uint32_t     PINx  : ���ű�ţ�  ȡֵ��Χ��GPIO_Pin_0 ~ GPIO_Pin_15
 * ˵  ���� �¶�ֵ��Ч��Χ:0~50��; ���ȡ�2��C; С��������Ч
 * ����ֵ�� 0-ʧ�ܣ���0ֵ-ʪ��ֵ
 ******************************************************************************/
float DHT11_GetTemperature(GPIO_TypeDef *GPIOx, uint32_t PINx)
{ 
    DHT11_GetData(GPIOx, PINx);
    return xDHT11.Temperature;
}


/******************************************************************************
 * ��  ���� DHT11_GetHumidity
 * ��  �ܣ� ��DHT11�ж�ȡʪ��ֵ
 * ��  ���� GPIO_TypeDef *GPIOx: GPIO�˿ںţ�ȡֵ��Χ��GPIOA ~ GPIOG
 *          uint32_t     PINx  : ���ű�ţ�  ȡֵ��Χ��GPIO_Pin_0 ~ GPIO_Pin_15
 * ˵  ���� ʪ��ֵ��Ч��Χ:20%~90%; ���ȡ�5%; С��������Ч
 * ����ֵ�� 0-ʧ�ܣ���0ֵ-ʪ��ֵ
 ******************************************************************************/
float DHT11_GetHumidity(GPIO_TypeDef *GPIOx, uint32_t PINx)
{  
    DHT11_GetData(GPIOx, PINx);
    return xDHT11.Humidity;
}
