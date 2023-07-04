/***********************************************************************************************************************************
 **���������ӡ�  ħŮ�Ƽ�    https://demoboard.taobao.com
 **�����·���  QȺ�ļ���   887199504
 ***********************************************************************************************************************************
 **���ļ����ơ�  ds18b20.c
 **���ļ����ܡ�
 **������ƽ̨��  STM32F103 + ��׼��v3.5 + keil5
 **
 **���Ƽ���Ƶ��  https://www.bilibili.com/video/BV1CV411o7Di?from=search&seid=1639361392451575575
 **
 **������ָ�  DS18B20�Ĺ���ָ��
 **              �¶�ת��ָ��    0x44������Ds18b20����ת���¶�
 **              ���ݴ���ָ��    0xBE����ȡ�ݴ����еľ��ֽ�����
 **              д�ݴ�������    0x4E��������д���ݴ�����TH��TL
 **              ��ֵ�ݴ���      0x48�����ݴ����е�TH��TLд��EEPROM��
 **              ����Դ���緽ʽ  0xB4������Ds18b20,���͵�Դ���緽ʽ
 **              �ص�EEPROM      0xB8����EEPROM�е�TH��TL�����ݴ���
 **
 **
 **�����¼�¼��  2021-05-12  ����
 **
 **
************************************************************************************************************************************/
#include "bsp_DS18B20.h"



static GPIO_TypeDef *DS18B20_GPIO = 0;
static uint32_t      DS18B20_Pin  = 0;

#define DS18B20_BUS_HIGH    (DS18B20_GPIO->BSRR = DS18B20_Pin)
#define DS18B20_BUS_LOW     (DS18B20_GPIO->BSRR = DS18B20_Pin<<16)
#define DS18B20_BUS_READ   ((DS18B20_GPIO->IDR & DS18B20_Pin)?1:0)



xDS18B20_TypeDef  xDS18B20;              // ����ȫ�ֽṹ��, ���ڼ�¼��Ϣ



static void    delayUS(uint32_t times);  // ����US������ʱ������������ֲʱ���ⲿ�ļ�������
static void    writeByte(uint8_t byte);  // д��һ���ֽ�;
static uint8_t readByte(void);           // ����һ���ֽ�;
static uint8_t resetAndCheck(void);      // ������λ�źţ�������Ӧ



// ����US������ʱ������������ֲʱ���ⲿ�ļ�������
static void delayUS(u32 times)
{
    times = times * 8;
    while (--times)
        __nop();
}

// дһ���ֽڵ�DS18B20
// byte��Ҫд����ֽ�
// ע�⣺��λ����LSB
static void writeByte(u8 byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_BUS_LOW;        // ��������������
        delayUS(2);             // ����1US����

        if ((byte >> i) & 0x01)    DS18B20_BUS_HIGH; // д1: ��������1US���ͷ�����Ϊ�ߵ�ƽ����������60US����
        // д0: ��������1US�����߱����ڵ͵�ƽ����������60US����
        delayUS(60);

        DS18B20_BUS_HIGH;       // �ͷ�����, ʹʱ������
        delayUS(1) ;
    }
}

// ��DS18B20��ȡһ���ֽ�
// �Ƚ��յ����ǵ�λ
static uint8_t readByte(void)
{
    u8 data = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_BUS_LOW;         // ��������������
        delayUS(4);              // ����1US����
        DS18B20_BUS_HIGH;        // �����ͷ�����

        delayUS(20);             // DS18B20�ᱣ��һ������15US���ź�
        if (DS18B20_BUS_READ)   data = (data >> 1) | 0x80;
        else                    data = data >> 1;
        delayUS(100);            // �ʵ���ʱ������������60US

        DS18B20_BUS_HIGH;        // �ͷ�����, ʹʱ������
        delayUS(1) ;
    }
    return data;
}

// ������λ�źţ�������Ӧ
// 1-���������������ͣ�����480US���ͷ�����
// 2-��ʱ15~60US��
// 3-��60~240US�ڣ����DQ�Ƿ�Ϊ�͵�ƽ��
// 4-�ͷ����ߣ���ʱ240US��������ʼʱ�������
static uint8_t resetAndCheck(void)
{
    u32 timeOut = 0;

    // �����пɲ��õģ�ֻ��Ϊ��ʹ���ߵ�ƽ�ȶ�
    //DS18B20_BUS_HIGH;   // �ͷ����ߣ�Ϊ��ʼʱ����׼��
    //delayUS(500);

    // ��λʱ��
    DS18B20_BUS_LOW;    // ��������
    delayUS(600);       // ��������480US
    DS18B20_BUS_HIGH;   // �ͷ�����
    delayUS(40);        // �ȴ�15~60US

    // ������߻�Ӧ: ��60~240US�ڣ����DQ�Ƿ�Ϊ�͵�ƽ
    while (DS18B20_BUS_READ && (timeOut < 200))
    {
        timeOut++;
        delayUS(1);
    }
    if (timeOut >= 200)
        return 0;        // ��ʱ���أ�0-�������л�Ӧ����1-ʧ�ܣ��޻�Ӧ��
    else
        timeOut = 0;     // �л�Ӧ����ʱ�����声

    // ����Ӧ��������Ƿ��ͷţ���ʱ240US��������ʼʱ�������
    while ((!DS18B20_BUS_READ) && (timeOut < 240))
    {
        timeOut++;
        delayUS(1);
    }
    if (timeOut >= 240)
        return 0;        // ��ʱ���أ�0-�������л�Ӧ����1-ʧ�ܣ��޻�Ӧ��
    else
        return 1;        // ������ʱ���﷢�������ͷ�
}



//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����0:������
//����1:����
uint8_t DS18B20_Init(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    xDS18B20.initState = 0;    // ��ʼ��״̬������Ϊ0����δ��ʼ����
    DS18B20_GPIO = GPIOx;
    DS18B20_Pin = PINx;

    // ʱ��ʹ�ܣ����Ŷ˿�;���ж϶˿ڵķ�ʽʹ��ʱ����, ������ֲʱ�Ĺ���
    if (GPIOx == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (GPIOx == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (GPIOx == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (GPIOx == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (GPIOx == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (GPIOx == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (GPIOx == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);

    // ���ã����Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin   = PINx;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;          // ��©ģʽ�����ߣ�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    DS18B20_BUS_HIGH ;                                         // �ͷ����ߣ����ߣ�

    /*********** ����һ�Σ�Ϊ����ϵ���DS18B20�����ã��ɲ�Ҫ
    xDS18B20.initState = resetAndCheck();                      // ��ʼ��״̬��0-����; 1-ʧ��
    if (xDS18B20.initState == 0)                               // ��ʼ��״̬��0-����; 1-ʧ��
        printf("DS18B20 ���...       �����豸\r\n");
    else
        printf("DS18B20 ���...       û���ҵ��豸����\r\n");

    return xDS18B20.initState;
    *************/
    return 1;
}

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-55~125��
float   DS18B20_GetDtat(GPIO_TypeDef *GPIOx, uint32_t PINx)
{
    uint8_t TL, TH;
    short temp;

    //if (xDS18B20 .initState != 1)        // ���û�г�ʼ�����ȳ�ʼ������
    DS18B20_Init(GPIOx, PINx);

    // ����ת��
    resetAndCheck();    // ��λ
    writeByte(0xcc);    // skip rom
    writeByte(0x44);    // ����Ds18b20����ת���¶�

    // ��ȡ����
    resetAndCheck();
    writeByte(0xcc);    // skip rom
    writeByte(0xbe);    // ��ȡ�ݴ����еľ��ֽ�����
    TL = readByte();    // LSB
    TH = readByte();    // MSB

    temp = TH << 8 | TL;
    if (temp >= 127)    temp = (0 - (~temp));

    return (((float)temp * 0.625) / 10);
}





