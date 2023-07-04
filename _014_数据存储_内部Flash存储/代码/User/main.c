/**==================================================================================================================
 **���ļ����ơ�  main.c
 **�����ܲ��ԡ�  ���ݴ洢-STM32F103оƬ�ڲ�Flash�洢
 **==================================================================================================================
 **��ʵ��ƽ̨��  STM32F103 + KEIL5.27
 **
 **��ʵ�������  Ӳ�����ߣ��������
 **
 **              ���������
 **              1_���뱣����system_f103.c�ļ��У�����Ҫ#include "system_f103.h"
 **              2_Ϊ���������ֲ���������������ͳһΪ������������������д������
 **                    �����ݣ�W25qxx_ReadBuffer (uint32_t addr, uint8_t *buf, uint16_t num)
 **                    д���ݣ�W25qxx_WriteBuffer(uint32_t addr, uint8_t *buf, uint16_t num)
 **
 **              ��������
 **              1���򿪵��ԵĴ��������������Ӧ�Ĵ���;
 **              2����¼����󣬿��ڴ�������������д������ݣ��Ͷ�ȡ��������;
 **
 **���� �� �㡿  1_�����ݴ洢��STM32оƬ�ڲ�, ֻ�ʺ��������ݣ���1K; ���ʺϼ�ʮK������;
 **              2_���ݴ洢�ĵ�ַ��Χ��(0x8000000+�������) < ��ַ < (0x8000000+оƬFlash��С��; 
 **              3_������� = Code + ROdata + RWdata
 **              4_����оƬFlash��С����ַ���ֵ��
 **                    STM32F103C6:   32K   0x0800 0000 + 0x00 8000 = 0x0800 8000
 **                    STM32F103C8:   64K   0x0800 0000 + 0x01 0000 = 0x0801 0000
 **                    STM32F103RC:  256K   0x0800 0000 + 0x04 0000 = 0x0804 0000
 **                    STM32F103VE:  512K   0x0800 0000 + 0x08 0000 = 0x0808 0000
 **                    STM32F103ZE:  512K   0x0800 0000 + 0x08 0000 = 0x0808 0000
 **                    STM32F103ZG: 1024K   0x0800 0000 + 0x10 0000 = 0x0810 0000
 **                      
 **
 **�����¼�¼��  2022-03-20  ����ע��
 **
 **����ע˵����  ��Ȩ��ħŮ�Ƽ����У��������ã�лл��
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>                               // ��Ҫ�ĺ궨���ļ��������˸��ֵ�ֵַ
#include "stm32f10x_conf.h"                          // ͷ�ļ�����(��׼��); �ںˡ�оƬ����....��(stm32f10x.conf.h, �Ա�׼��ͷ�ļ����е���)  
#include "system_f103.h"                             // ϵͳ���ù���, �紮��USART1����115200-0-8-1, SysTic����Ϊ1ms�ж�, 
#include "bsp_led.h"                                 // LEDָʾ��
#include "bsp_usart.h"                               // USART1��2��3��UART4��5



// ��ʱ����
static void delay_ms(uint32_t ms)                    // �򵥵ĺ�����ʱ����
{
    ms = ms * 6500;                                  // 72MHzϵͳʱ���£����ٸ���ѭ��Լ��ʱ1ms
    for (uint32_t i = 0; i < ms; i++);               
}



// ������
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // �жϷ��飬��2:��ռ��0~3,�����ȼ�0~3 ; ȫ��ֻ����һ�Σ������������۵ĵط�

    USART1_Init(115200);                             // ���ڳ�ʼ����USART1(115200-N-8-1), �ҹ����Ѱ�printf�ض�����USART1���

    Led_Init();                                      // LED ��ʼ��
    LED_RED_ON ;                                     // ������ɫLED
    LED_BLUE_ON ;                                    // ������ɫLED

    printf("\r\n>>>>оƬ�ڲ�Flash������д��\r");
    // ��ָ��λ��д��ָ����������
    uint8_t writeBuf[50] = "Test, �����ܺã�";
    System_WriteInteriorFlash(0x08000000+60*1024, writeBuf, 20);                       // д�����ݣ���ַ�������׵�ַ���ֽ���;  ˵����Ϊ���⸲�Ǵ������ݣ���Flash�ĵ�60K��ַλ��д��
    printf("д�������: %s\r\n", writeBuf);
    // ��ָ��λ�ö�ȡָ����������
    uint8_t readBuf[50];
    System_ReadInteriorFlash(0x08000000+60*1024, readBuf, 20);                         // ��ȡ���ݣ���ַ�������׵�ַ������
    printf("��ȡ������: %s\r\n\r\n", readBuf);                    

        
    printf("\r\n>>>>���Բ�ͬ��ַ�Ķ�д�������������͵�ת����\r\n");         
    // ����1���ַ�
    uint32_t addrChar   = 0x08000000+38*1024;                                          // ע���ַ������ż�����ҵ�ַ�ξ���������FLASHβ�������⸲���˴��������
    char writeChar='F';            
    System_WriteInteriorFlash(addrChar, (uint8_t*)&writeChar, 1);                      // �������ݣ��������Զ�������������д��
    char readChar=0;                                 
    System_ReadInteriorFlash(addrChar, (uint8_t*)&readChar, 1);                        // ��ȡ���ݣ��������Զ������ֶ�ȡ
    printf("����char     ��ַ:0x%X    д��:%c       ��ȡ:%c \r\n", addrChar, writeChar, readChar);
   
   // ����2�������ŵ�16λ   
    uint32_t addrShort  = 0x08000000+155*1024-2;                                       // ע���ַ������ż�����ҵ�ַ�ξ���������FLASHβ�������⸲���˴��������
    short writeShort=-888;             
    System_WriteInteriorFlash(addrShort, (uint8_t*)&writeShort, sizeof(writeShort));   // �������ݣ��������Զ�������������д��
    short readShort=0;       
    System_ReadInteriorFlash(addrShort, (uint8_t*)&readShort, sizeof(writeShort));     // ��ȡ���ݣ��������Զ������ֶ�ȡ
    printf("����short    ��ַ:0x%X    д��:%d    ��ȡ:%d \r\n", addrShort, writeShort, readShort);
   
   // ����3���޷��ŵ�32λ
    uint32_t addrUint   = 0x08000000+254*1024;                                         // ע���ַ������ż�����ҵ�ַ�ξ���������FLASHβ�������⸲���˴��������
    uint32_t writeUint= 0x12345678;
    System_WriteInteriorFlash(addrUint, (uint8_t*)&writeUint, 4);                      // �������ݣ��������Զ�������������д��
    uint32_t readUint = 0;
    System_ReadInteriorFlash(addrUint, (uint8_t*)&readUint, 4);                        // ��ȡ���ݣ��������Զ������ֶ�ȡ
    printf("����uint     ��ַ:0x%X    д��:0x%X     ��ȡ:0x%X \r\n", addrUint, writeUint, readUint);
    
    // ����4��������
    uint32_t addrFloat  = 0x08000000+122*1024;                                         // ע���ַ������ż�����ҵ�ַ�ξ���������FLASHβ�������⸲���˴��������
    float writeFloat= -123.456;     
    System_WriteInteriorFlash(addrFloat, (uint8_t*)&writeFloat, 4);                    // �������ݣ��������Զ�������������д��
    float readFloat;
    System_ReadInteriorFlash(addrFloat, (uint8_t*)&readFloat, 4);                      // ��ȡ���ݣ��������Զ������ֶ�ȡ
    printf("����float    ��ַ:0x%X��  д��:%f    ��ȡ:%f \r\n", addrFloat, writeFloat, readFloat);
    
    // ����5���ַ���, Cû���ַ����������, ���ַ�����
    uint32_t addrString   = 0x08000000+166*1024-6;                                     // ע���ַ������ż�����ҵ�ַ�ξ���������FLASHβ�������⸲���˴��������
    char writeString[100]="��߹! ȥ����?";     
    System_WriteInteriorFlash(addrString, (uint8_t*)writeString, sizeof(writeString)); // �������ݣ��������Զ�������������д��
    char readString[100];
    System_ReadInteriorFlash(addrString, (uint8_t*)readString, sizeof(readString));    // ��ȡ���ݣ��������Զ������ֶ�ȡ
    printf("����string   ��ַ:0x%X��  д��:%s  ��ȡ:%s \r\n", addrString, writeString, readString);
       
                
    while (1)                                                     // while������ѭ�������ã�������main�������н�������������Ӳ������
    {
        delay_ms(500);                                            // ��ʱ���
        LED_RED_TOGGLE ;                                          // ��ת��ɫLED; ��ʾ����LED���ɵ���˸���ɷ���۲�����Ƿ��ܷɻ���
    }
}




