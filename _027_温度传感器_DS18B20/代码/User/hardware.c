#include "hardware.h"



/*****************************************************************************
*��  HardWare_NVICManage
*��  �ܣ��ⲿ�ж����ȼ������ţ� ������� ���������ȼ���ϵ
*��  ����
*����ֵ��
*****************************************************************************/
void HardWare_NVICManage(void)
{

}



/*****************************************************************************
*��  Hardware_Init
*��  �ܣ���ʼ��Ӳ��
*��  ����
*����ֵ��
*****************************************************************************/
u8 Hardware_Init(void)
{
    USART1_Init(115200);                                      // ���ڳ�ʼ����USART1(115200-N-8-1), �ҹ����Ѱ�printf�ض�����USART1���
    System_SwdMode();                                         // ����оƬ���Է�ʽ(SWD); �ر�JTAGֻ����SWD; Ŀ��:�ͷ�PB3��PB4��PA15��ֻ��PA13��PA14
    System_SysTickInit();                                     // ��ʼ��systickʱ�ӣ�1ms�ж�1�Σ����ã�ʹSystem_DelayMS()��System_DelayUS()��������
    SystemCoreClockUpdate();                                  // ��ȡ��ǰʱ��Ƶ�ʣ� ����ȫ�ֱ��� SystemCoreClockֵ
    printf("ϵͳ����ʱ��          %d Hz\r", SystemCoreClock); // ϵͳʱ��Ƶ����Ϣ , SystemCoreClock��system_stm32f4xx.c�ж���
    printf("оƬ�ڲ�FLASH         ");                         // ��ȡ��оƬFLASH��С��
    printf("%d Kbytes\n", *(uint16_t *)(0x1FFFF7E0));         // ����ַ�Σ������оƬ����ʱ��д��FLASH��С;��������һ�����ã�ʾ��ʹ��ָ���ȡ����

    Led_Init();                                               // LED ��ʼ��
    LED_RED_ON ;
    LED_BLUE_ON ;

    Key_Init();                                               // ���� ��ʼ��

    W25qx_Init();                                             // �豸W25Q128�� ���ų�ʼ����SPI��ʼ������������, �����Ƿ�����ֿ�

    LCD_Init();                                               // ��ʾ����ʼ��
    XPT2046_Init(xLCD.width, xLCD.height, xLCD.dir);          // ��������ʼ��, �������, �߶�����, ����:0_����, 1_����
    //XPT2046_ReCalibration();                                // ����У׼��������
    XPT2046_Cmd(ENABLE);                                      // �򿪴������
    LCD_GUI();                                                // ����չʾ����

    CAN1_Config();

    // ���ڡ�ESP8266
    USART3_Init(115200);



    return 0;
}

