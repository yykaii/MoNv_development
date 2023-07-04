/**==================================================================================================================
 **���ļ����ơ�  main.c
 **�����ܲ��ԡ�  ESP8266֮���ֻ�APPͨ��
 **==================================================================================================================
 **��ʵ��ƽ̨��  STM32F103 + KEIL5.27 + ESP8266
 **
 **��ʵ�������  �ֻ�APP��(��׿�ֻ�):
 **                1_�ֻ�����WiFi;
 **                2_����APP��"TCP����";
 **                3_��APP������¼���涥����:IP��ַ���˿ں�
 **              ���������
 **                1_�޸�main�ļ������е�WiFi���ơ����룬��APP����ʾ��IP��ַ���˿ںţ������޸����޴�����¼��
 **                2_ESP8266��TXD��RXD��������������USARTx�ϵ�RX��TX; ���������õ���USART3,��PB10,PB11���������޸�;
 **                3_���Զ˴򿪴���������������������صĴ���;
 **                4_�����尴��λ���������Զ���APP��������;
 **               ��������
 **                1����������У���ʾ����״̬��Ϣ����ʵʱ��ʾ�����շ�����;
 **                2�������ѿ���͸�����䣬�����������APPʵ������ʽ���ݴ���;
 **                3������APP���趨Լ�����ַ��������������յ���Щ���ݣ������жϲ���, ��LED���أ�������Ƶ�
 **
 **���� �� �㡿  1_ESP8266���ߣ�VCC��3.3V, IO0��EN��RST��3���Ž�3.3V��IO15��GND��2���Ž�GND, TXD��RXD��2���Žӿ������USART;
 **              2_ESP8266�ź�: ��ģ��ʹ�õ��ǰ������ߣ��źűȽ�����������·������Զ���ʺ�8���ڲ���;
 **
 **�����¼�¼��  2021-12-26  ���ƴ���ṹ��ע��
 **              2021-12-03  ����
 **
 **����ע˵����  ��Ȩ��ħŮ�Ƽ����У��������ã�лл��
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>            // ͷ�ļ�����(��׼��); �ںˡ�оƬ����....;(stm32f10x.conf.h, �Ա�׼��ͷ�ļ����е���)     
#include "stm32f10x_conf.h"       // ͷ�ļ�����(��׼��); �ںˡ�оƬ����....;(stm32f10x.conf.h, �Ա�׼��ͷ�ļ����е���) 
#include "system_f103.h"
#include "bsp_led.h"              // LEDָʾ��
#include "bsp_key.h"              // ����
#include "bsp_usart.h"            // USART1��2��3��UART4��5
#include "bsp_ESP8266.h"




static void delay_ms(uint32_t ms)                     // ����һ��ms��ʱ������������ֲʱ���ⲿ�ļ�����; ����������������ʱʹ�ã������Ǿ�׼��ʱ;
{
    ms = ms * 11993;                                  // ע�⣺�˲ο�ֵ������������ Options/ c++ / One ELF Section per Function
    for (uint32_t i = 0; i < ms; i++);                // 72MHzϵͳʱ���£���Լ���ٸ���ѭ����ʱ1ms
}


// ������
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   // �жϷ��飬��2:��ռ��0~3,�����ȼ�0~3 ; ȫ��ֻ����һ�Σ������������۵ĵط�
    System_SwdMode();                                 // ����оƬ���Է�ʽ(SWD); �ر�JTAGֻ����SWD; Ŀ��:�ͷ�PB3��PB4��PA15��ֻ��PA13��PA14

    USART1_Init(115200);                              // ����1��ʼ��; �����봮�����ͨ�ţ�����������; USART1(115200-N-8-1), �ҹ����Ѱ�printf�ض�����USART1���

    Led_Init();                                       // LED ��ʼ��
    LED_BLUE_ON ;                                     // ��������
    LED_RED_ON;                                       // �������

    Key_Init();                                       // ���� ��ʼ��

    delay_ms(2000);
    ESP8266_Init(UART4, 115200);                      // ��ʼ��;    ���������ڶ˿ڣ�������     ע�⣺������Ĭ��Ϊ115200
    ESP8266_JoinAP("CMCC-zml", "55025865502586");     // ����WiFi;  ����: SSID, Password;      ע�⣺ESP8266���ֻ��������Wifi��ͬһ��AP(·��)
    ESP8266_TCPConnect("192.168.1.2", 1234);          // ��������;  ����: Ŀ��IP��ַ���˿ں�;  ע�⣺���IP�Ͷ˿ںţ���Ŀ��APP��TCP���ӡ���������Ϸ�����ʾ
    ESP8266_EnablePassThrough();                      // ��ʼ͸������

    while (1)                                         // while������ѭ����������main�������н�������������Ӳ������
    {
        delay_ms(200);                                // �����ʱ

        // ��죺���Դ�������������������ݣ����жϴ���
        if (xUSART.USART1ReceivedNum)
        {
            ESP8266_SendString((char *)xUSART.USART1ReceivedBuffer);                   // ����λ�������������ݣ����͵�ESP8266
            printf("\r\n��������>>>��%s", (char *)xUSART.USART1ReceivedBuffer);        // �������͵����ݣ������������λ��������۲�
            xUSART.USART1ReceivedNum = 0;                                              // ��մ���1�Ľ��ձ�־
        }

        // ��죺ESP8266�������������ݣ����жϴ���
        if (ESP8266_CheckReceivedNum())                                                                  // ���ESP8266���õĴ����Ƿ��յ�����
        {
            printf("\r\nESP8266�յ� %d �ֽ�����<<<��%s", xESP8266.ReceivedNum, xESP8266.ReceivedBuffer); // �ѽ����յ������ݣ������������λ��������۲�
            // �ж�������ִ�ж���
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_RED_ON"))   LED_RED_ON;                     // �ж�APP�����������ݰ����Ƿ�����ַ�����LED_RED_ON
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_RED_OFF"))   LED_RED_OFF;                   // �ж�APP�����������ݰ����Ƿ�����ַ�����LED_RED_OFF
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_BLUE_ON"))   LED_BLUE_ON;                   // �ж�APP�����������ݰ����Ƿ�����ַ�����LED_BLUE_ON
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_BLUE_OFF"))   LED_BLUE_OFF;                 // �ж�APP�����������ݰ����Ƿ�����ַ�����LED_BLUE_OFF
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_ALL_ON"))
            {
                LED_RED_ON;     // �ж�APP�����������ݰ����Ƿ�����ַ�����LED_ALL_ON
                LED_BLUE_ON;
            }
            if (strstr((char *)xESP8266.ReceivedBuffer, "LED_ALL_OFF"))
            {
                LED_RED_OFF;    // �ж�APP�����������ݰ����Ƿ�����ַ�����LED_ALL_OFF
                LED_BLUE_OFF;
            }

            if (strstr((char *)xESP8266.ReceivedBuffer, "CLOSED"))         printf("ESP8266��Ŀ���ͨ�����ӣ��ѶϿ���\r\n"); // ��ESP8266���������ѶϿ�ʱ���ᷴ���������֡
            ESP8266_CleanReceivedFlag();              // �����������ESP8266�Ľ��ջ��棬������һ֡���ݵĽ���;
        }
    }
}




