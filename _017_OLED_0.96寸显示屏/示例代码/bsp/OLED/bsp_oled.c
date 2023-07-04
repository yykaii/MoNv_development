#include "bsp_oled.h"
#include "oledfont.h"
#include "bmp.h"



/*****************************************************************************
 ** �궨��
 *****************************************************************************/
#define SCL_0   (OLED_SCL_GPIO->BRR   = OLED_SCL_PIN)  // SCL
#define SCL_1   (OLED_SCL_GPIO->BSRR  = OLED_SCL_PIN)

#define SDA_0   (OLED_SDA_GPIO ->BRR  = OLED_SDA_PIN)  // SDA
#define SDA_1   (OLED_SDA_GPIO ->BSRR = OLED_SDA_PIN)



static char strTemp[30];



/*****************************************************************************
 ** �ڲ�����
****************************************************************************/
// �򵥵�ʱ���ڲ���ʱ
static void delay(void)
{
    uint8_t i;
    for (i = 0; i < 3; i++);    // ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz
}

// ��ʱms
static void delay_ms(uint32_t ms)           // ����һ��ms��ʱ������������ֲʱ���ⲿ�ļ�����; ����������������ʱʹ�ã������Ǿ�׼��ʱ;
{
    ms = ms * 11993;                        // ע�⣺�˲ο�ֵ������������ Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);      // 72MHzϵͳʱ���£���Լ���ٸ���ѭ����ʱ1ms
}

// ��ʼ�ź�
static void start(void)
{
    SCL_0 ;   //
    SDA_1;
    delay();

    SCL_1;     // SCL�ߵ�ƽ�ڼ䣬SDA��������
    SDA_0;

    delay();
    SCL_0;
    delay();
}

// ֹͣ�ź�
static void stop(void)
{
    SCL_0 ;
    SDA_0 ;
    delay();

    SCL_1 ;   // SCL�ߵ�ƽ�ڼ䣬SDA��������
    SDA_1;
    delay();
}

// Ӧ���ź� 
static void waitAck(void)
{
    SCL_1 ;
    delay();
    SCL_0;
    delay();
}

// ���߲���1�ֽ�����ʱ��
static void sendByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        SCL_0 ;
        delay();
        if (data & 0x80)
            SDA_1 ;
        else
            SDA_0 ;
        data <<= 1;
        delay();
        SCL_1 ;
        delay();   // ��Ҫ������Ҫ��һ����ʱ����Ȼ����ͨ����
        SCL_0 ;
    }
}

// д����
static void writeCmd(uint8_t IIC_Command)
{
    start();
    sendByte(0x78);            // �豸��ַ
    waitAck();
    sendByte(0x00);            // ����
    waitAck();
    sendByte(IIC_Command);
    waitAck();
    stop();
}

// д����
static void writeData(uint8_t IIC_Data)
{
    start();
    sendByte(0x78);           //D/C#=0; R/W#=0
    waitAck();
    sendByte(0x40);           //write data
    waitAck();
    sendByte(IIC_Data);
    waitAck();
    stop();
}

// �������꣬x:�������ص㣬y:����ڼ�ҳ(��)
static void OLED_SetPos(uint8_t x, uint8_t y)
{
    writeCmd(0xb0 + y);
    writeCmd(((x & 0xf0) >> 4) | 0x10);
    writeCmd((x & 0x0f));
}



/*****************************************************************************
* ��  ���� OLED_Init
* ��  �ܣ� ��ʼ��OLED
* ��  ��: 
* ����ֵ�� ��
* ��  ע�� 
*****************************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;   // ����һ��GPIO_InitTypeDef���͵Ľṹ��

    // ʹ��LED_RED�������Ŷ˿�ʱ��;ʹ�ö˿��жϵķ���ʹ��ʱ��, ��ʹ������ֲ������
    if (OLED_SCL_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (OLED_SCL_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (OLED_SCL_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (OLED_SCL_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (OLED_SCL_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (OLED_SCL_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (OLED_SCL_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    // ʹ��LED_RED�������Ŷ˿�ʱ��;ʹ�ö˿��жϵķ���ʹ��ʱ��, ��ʹ������ֲ������
    if (OLED_SDA_GPIO == GPIOA)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (OLED_SDA_GPIO == GPIOB)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    if (OLED_SDA_GPIO == GPIOC)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    if (OLED_SDA_GPIO == GPIOD)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (OLED_SDA_GPIO == GPIOE)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    if (OLED_SDA_GPIO == GPIOF)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    if (OLED_SDA_GPIO == GPIOG)  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

    // ����LED_RED���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin   = OLED_SCL_PIN;           // ѡ��Ҫ���Ƶ�GPIO����
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // ��������ģʽΪͨ���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������Ϊ50MHz
    GPIO_Init(OLED_SCL_GPIO, &GPIO_InitStructure);          // ���ÿ⺯������ʼ��GPIO

    // ����LED_RED���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin   = OLED_SDA_PIN;           // ѡ��Ҫ���Ƶ�GPIO����
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // ��������ģʽΪͨ���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������Ϊ50MHz
    GPIO_Init(OLED_SDA_GPIO, &GPIO_InitStructure);          // ���ÿ⺯������ʼ��GPIO

    stop();

    delay_ms(200);  // ��Ҫ����Ҫɾ

    writeCmd(0xAE); // OLED����
    writeCmd(0x00); // ���õ��е�ַ
    writeCmd(0x10); // ���ø��е�ַ
    writeCmd(0x40); // ������ʼ��ַ��
    writeCmd(0xB0); // set page address
    writeCmd(0x81); // ���öԱȶ�
    writeCmd(0xFF); // 128
    writeCmd(0xA1); // 0xa0���ҷ��� 0xa1����
    writeCmd(0xA6); // normal / reverse
    writeCmd(0xA8); // ���ö�·����(1 to 64)
    writeCmd(0x3F); // 1/32 duty
    writeCmd(0xC8); // Com scan direction
    writeCmd(0xD3); // ������ʾ��ƫ��ӳ���ڴ������
    writeCmd(0x00); //

    writeCmd(0xD5); // ������ʾʱ�ӷ�Ƶ��/����Ƶ��
    writeCmd(0x80); // ���÷�Ƶ������ʱ������Ϊ100֡/��

    writeCmd(0xD8); // set area color mode off
    writeCmd(0x05); //

    writeCmd(0xD9); // Ԥ���ʱ��
    writeCmd(0xF1); // Ԥ���Ϊ15�����壬�ͷ�Ϊ1������

    writeCmd(0xDA); // ��������Ӳ������
    writeCmd(0x12); //

    writeCmd(0xDB); // ����VCOM��ƽ
    writeCmd(0x30); //

    OLED_On();      // ����
}



/*****************************************************************************
* ��  ���� OLED_Fill
* ��  �ܣ� ��������, �����������ĻΪ0
* ��  ��: 
* ����ֵ�� ��
* ��  ע�� 
*****************************************************************************/
void OLED_Fill(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        writeCmd(0xb0 + i);  //����ҳ��ַ��0~7��
        writeCmd(0x00);      //������ʾλ�á��е͵�ַ
        writeCmd(0x10);      //������ʾλ�á��иߵ�ַ
        for (uint8_t n = 0; n < 128; n++)
            writeData(0);   // Ҫ��������
    }
}



/*****************************************************************************
* ��  ���� OLED_On
* ��  �ܣ� �������л���
* ��  ��: 
* ����ֵ�� ��
* ��  ע�� 
*****************************************************************************/
void OLED_On(void)
{
    writeCmd(0X8D); // ���õ�ɱ�
    writeCmd(0X14); // ������ɱ�
    writeCmd(0XAF); // OLED����
}



/*****************************************************************************
* ��  ���� OLED_Off
* ��  �ܣ� ����ģʽ��,OLED���Ĳ���10uA
* ��  ��: 
* ����ֵ�� ��
* ��  ע�� 
*****************************************************************************/
void OLED_Off(void)
{
    writeCmd(0X8D); // ���õ�ɱ�
    writeCmd(0X10); // �رյ�ɱ�
    writeCmd(0XAE); // OLED����
}



/*****************************************************************************
* ��  ���� OLED_ShowString
* ��  �ܣ� ��ʾoledfont.h�е�ASCII�ַ�
* ��  ��:  x, y : ��ʼ������(x0:0~127, y0:0~7)
*          chr* : Ҫ��ʾ���ַ���;
*          size : �ַ���С(��ѡ��8, 16)
* ����ֵ�� ��
* ��  ע�� 
*****************************************************************************/
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t size)
{
    uint8_t i = 0;
    uint8_t c = 0;

    if (size == 16)
    {
        while (chr[i] != '\0')
        {
            c = chr[i++] - ' ';
            OLED_SetPos(x, y);
            for (uint8_t j = 0; j < 8; j++)
                writeData(F8X16[c * 16 + j]);
            OLED_SetPos(x, y + 1);
            for (uint8_t j = 0; j < 8; j++)
                writeData(F8X16[c * 16 + j + 8]);

            x += 8;
            if (x > 120)
            {
                x = 0;
                y += 2;
            }
        }
    }
    else
    {
        while (chr[i] != '\0')
        {
            c = chr[i++] - ' ';
            OLED_SetPos(x, y);
            for (uint8_t j = 0; j < 6; j++)
                writeData(F6x8[c][j]);

            x += 8;
            if (x > 120)
            {
                x = 0;
                y += 1;
            }
        }
    }
}

/*****************************************************************************
* ��  ���� OLED_ShowCHinese
* ��  �ܣ� ��ʾ oledfont.h �еĺ���,16*16����
* ��  ��:  x,y: ��ʼ������(x:0~127, y:0~7)
*          N: ������ oledfont.h �����е�����
* ����ֵ�� ��
* ��  ע�� ��ģ�����PCtoLCD2002, ���������롢����ʽ��16*16������(������ͼ��)
*          ����չʾ��
*          OLED_ShowCHinese( 0,0,0);  // ��
*          OLED_ShowCHinese(18,0,2);  // ��
*****************************************************************************/
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t N)
{
    uint8_t wm = 0;
    unsigned int  adder = 32 * N;

    OLED_SetPos(x, y);
    for (wm = 0; wm < 16; wm++)
    {
        writeData(CH16[adder]);
        adder += 1;
    }

    OLED_SetPos(x, y + 1);
    for (wm = 0; wm < 16; wm++)
    {
        writeData(CH16[adder]);
        adder += 1;
    }
}

static  

/*****************************************************************************
* ��  ���� OLED_DrawBMP
* ��  �ܣ� ��ʾBMPλͼ
* ��  ��:  x0,y0 :��ʼ������(x0:0~127, y0:0~7)
*          x1,y1 : ���Խ���(������)������(x1:1~128,y1:1~8)
* ����ֵ�� ��
* ��  ע�� ͼƬ���ɵ��ֱ�ϴ󣬻�ռ�ý϶�ռ䣬ע��FLASH�ռ��Ƿ�������ͼƬ��С
*          ����չʾ�� 
*          OLED_DrawBMP(0, 0, 128, 8, BMP1);
*****************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
    uint32_t j = 0;
    uint8_t x, y;

    if (y1 % 8 == 0) y = y1 / 8;
    else y = y1 / 8 + 1;

    for (y = y0; y < y1; y++)
    {
        OLED_SetPos(x0, y);
        for (x = x0; x < x1; x++)
        {
            writeData(BMP[j++]);
        }
    }
}



void OLED_GUI(void)
{
    // ��ʾ����
    OLED_ShowCHinese(20, 0, 0);                   // ��, ����ģ�����еĵ�0����ģ
    OLED_ShowCHinese(38, 0, 1);                   // ʪ, ����ģ�����еĵ�1����ģ
    OLED_ShowCHinese(56, 0, 2);                   // ��, ����ģ�����еĵ�2����ģ
    OLED_ShowCHinese(74, 0, 3);                   // ��, ����ģ�����еĵ�3����ģ
    OLED_ShowCHinese(92, 0, 4);                   // ��, ����ģ�����еĵ�4����ģ
    // �¶Ȳɼ���ʾ
    OLED_ShowCHinese(2,  3, 0);                   // ��, ����ģ�����еĵ�0����ģ
    OLED_ShowCHinese(20, 3, 2);                   // ��, ����ģ�����еĵ�2��ģ
    OLED_ShowString(38, 3, ":", 16);              // :, ע�⣬�����Ӣ���ַ�
    float temperature = 0.0;                      //
    sprintf(strTemp, "%2.1f", temperature);       // ��Ҫ���ܶ�����ѯ, ��ΰѸ����������ͣ�ת��Ϊ�ַ���������ʾ����ʾ��. ��ٶ� sprintf() �ľ����÷���
    OLED_ShowString(54, 3, strTemp, 16);          // ��ʾ��ֵ����ʾ����
    OLED_ShowCHinese(92, 3, 5);                   // ��
    // ʪ�Ȳɼ���ʾ
    OLED_ShowCHinese(2,  6, 1);                   // ʪ, ����ģ�����еĵ�n����ģ
    OLED_ShowCHinese(20, 6, 2);                   // ��
    OLED_ShowString(38, 6, ":", 16);              // :, ע�⣬�����Ӣ���ַ�
    float humidity = 0.0;                         //
    sprintf(strTemp, "%2.1f", humidity);          // ��Ҫ���ܶ�����ѯ, ��ΰѸ����������ͣ�ת��Ϊ�ַ���������ʾ����ʾ��. ��ٶ� sprintf() �ľ����÷���
    OLED_ShowString(54, 6, strTemp, 16);          // ����ֵ��ֻ����ʾ�����ʾ��ֵ����ʾ����
    OLED_ShowString(92, 6, "%", 16);              // %
}




