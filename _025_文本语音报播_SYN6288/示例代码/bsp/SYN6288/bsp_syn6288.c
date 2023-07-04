#include "bsp_syn6288.h"
#include "stdarg.h"



xSYN6288_TypeDef  xSYN6288;                // ȫ�ֱ����ṹ��



static void delay_ms(uint32_t ms)          // ����һ��ms��ʱ������������ֲʱ���ⲿ�ļ�����; ����������������ʱʹ�ã������Ǿ�׼��ʱ;
{
    ms = ms * 11993;                       // ע�⣺�˲ο�ֵ������������ Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);     // 72MHzϵͳʱ���£���Լ���ٸ���ѭ����ʱ1ms
}



//Music:ѡ�񱳾����֡�0:�ޱ������֣�1~15��ѡ�񱳾�����
// SYN6288_SendFrameInfo(0, "[v10][m1][t5]������ Ϊ32.8Ԫ");
// ������ 0~15  :  �������֣�0_�ޱ������֣�1~15_�������ֿ�ѡ
//       [V0~16]:  �ı��ʶ�������0_��С��16_���
//       [m0~16]:  ��������������0_��С��16_���
//       [t0~5]:   �ʶ����٣�0_������5_���
//       ���������ù�����ο������ֲ�
static void SYN6288_SendFrameInfo(uint8_t Music, uint8_t *HZdata)
{
    /****************��Ҫ���͵��ı�**********************************/
    unsigned  char  Frame_Info[50];
    unsigned  char  HZ_Length;
    unsigned  char  ecc  = 0;             //����У���ֽ�
    unsigned  int i = 0;
    HZ_Length = strlen((char *)HZdata);   //��Ҫ�����ı��ĳ���

    /*****************֡�̶�������Ϣ**************************************/
    Frame_Info[0] = 0xFD ;                //����֡ͷFD
    Frame_Info[1] = 0x00 ;                //�������������ȵĸ��ֽ�
    Frame_Info[2] = HZ_Length + 3;        //�������������ȵĵ��ֽ�
    Frame_Info[3] = 0x01 ;                //���������֣��ϳɲ�������
    Frame_Info[4] = 0x01 | Music << 4 ;   //����������������������趨

    /*******************У�������***************************************/
    for (i = 0; i < 5; i++)               //���η��͹���õ�5��֡ͷ�ֽ�
    {
        ecc = ecc ^ (Frame_Info[i]);      //�Է��͵��ֽڽ������У��
    }

    for (i = 0; i < HZ_Length; i++)       //���η��ʹ��ϳɵ��ı�����
    {
        ecc = ecc ^ (HZdata[i]);          //�Է��͵��ֽڽ������У��
    }
    /*******************����֡��Ϣ***************************************/
    memcpy(&Frame_Info[5], HZdata, HZ_Length);
    Frame_Info[5 + HZ_Length] = ecc;

    if (xSYN6288.USARTx == USART1)   USART1_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == USART2)   USART2_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == USART3)   USART3_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == UART4)    UART4_SendData(Frame_Info, 5 + HZ_Length + 1);
    if (xSYN6288.USARTx == UART5)    UART5_SendData(Frame_Info, 5 + HZ_Length + 1);
}



/***********************************************************
* ��    �ƣ� SYN6288_Set(uint8_t *Info_data)
* ��    �ܣ� ������ �������
* ��ڲ����� *Info_data:�̶���������Ϣ����
* ���ڲ�����
* ˵    �����������������ã�ֹͣ�ϳɡ���ͣ�ϳɵ����� ��Ĭ�ϲ�����9600bps��
* ���÷�����ͨ�������Ѿ�������������������á�
**********************************************************/
static void SYN6288_Set(uint8_t *Info_data)
{
    uint8_t Com_Len;
    Com_Len = strlen((char *)Info_data);
    UART5_SendData(Info_data, Com_Len);
}



/******************************************************************************
 * ��  ���� SYN6288_Say
 * ��  �ܣ� ����ϳ�����
 * ��  ���� ��ʽ����������printf������һ�����÷�
 * ����ֵ�� ��
 * ʾ  ���� SYN6288_Say("�����?");
*******************************************************************************/
void SYN6288_Say(char *fmt, ...)
{
    static char  str_1[200];   // ������1��ģ��ÿ�ο�ת��200�ֽ�
    static char  str_2[200];   // ������2��ģ��ÿ�ο�ת��200�ֽ�

    va_list ap;
    va_start(ap, fmt);
    vsprintf(str_1, fmt, ap);
    va_end(ap);

    sprintf(str_2, "[d][V12][m15][t5]%s", str_1);  // [d]�ָ�Ĭ��״̬��[V12]�ʶ�����0~16��[m15]��������0~16��[t5]����0~5
    SYN6288_SendFrameInfo(0, (uint8_t *)str_2); // �ޱ�������
}


/******************************************************************************
 * ��  ���� SYN6288_Init
 * ��  �ܣ� ��ʼ�����ô���, ģ��Ĭ��ͨ�Ų�����9600
 * ��  ���� ����-USARTx
 * ����ֵ�� ��
 * ʾ  ���� SYN6288_Init(USART1);
*******************************************************************************/
void SYN6288_Init(USART_TypeDef *USARTx)
{
    uint16_t baudrate = 9600;      // Ĭ�ϲ�����9600bps��

    delay_ms(200);                 // �ϵ��������ʱ���ȴ�ģ������ȶ�״̬

    if (USARTx == USART1)    USART1_Init(baudrate);
    if (USARTx == USART2)    USART2_Init(baudrate);
    if (USARTx == USART3)    USART3_Init(baudrate);
#ifdef STM32F10X_HD
    if (USARTx == UART4)     UART4_Init(baudrate);
    if (USARTx == UART5)     UART5_Init(baudrate);
#endif
    xSYN6288.FlagOkay = 0;         // ��ʼ��״̬
    xSYN6288.USARTx   = USARTx;    // ��¼���ô��ڶ˿�
}
