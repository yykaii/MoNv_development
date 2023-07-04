/**==================================================================================================================
 **���ļ����ơ�  main.c
 **�����ܲ��ԡ�  �ÿ⺯���ķ�ʽ����LED_���ŵ�ƽ����
 **==================================================================================================================
 **��ʵ��ƽ̨��  STM32F103 + KEIL5.27
 **
 **��ʵ�������  USB�ߣ��嵽������CMSIS-DAP�ӿ���;
 **              ��ʹ�õ��ǰ��ص�LED����������������߼�����;
 **              ���������Ͻǵı��룬�����޴�󣬵��LOAD��������¼��������;
 **
 **���� �� �㡿  ��ɫLED��������оƬ��PC5���ţ����͵�ƽ��ͨ·;
 **              ��ɫLED��������оƬ��PB2���ţ����͵�ƽ��ͨ·;
 **              ��׼�⣬���⺯������������ڹ����ڣ�����ʹ��; ��๤���ļ���������FWLB�ļ��У���ŵľ��Ǳ�׼���֧���ļ�;
 **              Ҫʹ�ÿ⺯���������ڴ����ļ���������ص�ͷ�ļ���������������ļ�������stm32f10x_conf.h�ļ�����������ȡ������ļ���ע�ͷ���;
 **
 **�����¼�¼��  2022-01-25  ���ƴ���ṹ��ע��
 **              2021-12-03  ����
 **
 **����ע˵����  ��Ȩ��ħŮ�Ƽ����У��������ã�лл��
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <stm32f10x.h>            // ͷ�ļ�����(��׼��); �ںˡ�оƬ����....;(stm32f10x.conf.h, �Ա�׼��ͷ�ļ����е���)     
#include "stm32f10x_conf.h"       // ͷ�ļ�����(��׼��); �ںˡ�оƬ����....;(stm32f10x.conf.h, �Ա�׼��ͷ�ļ����е���) 




int main(void)                                              // ������, �������̵��û�������ʼ��
{
    GPIO_InitTypeDef GPIO_InitStructure;                    // ����GPIO���õĽṹ��

    // ���ú�ɫLED, PC5, �õ͵�ƽͨ·
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // ʹ��GPIOCʱ��
   
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;             // ѡ��Ҫ���Ƶ�GPIO����
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // ��������ģʽΪͨ���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);                  // ���ÿ⺯������ʼ��PC5

    // ������ɫLED, PB2, �õ͵�ƽͨ·
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // ʹ��GPIOBʱ��
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;             // ѡ��Ҫ���Ƶ�GPIO����
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // ��������ģʽΪͨ���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ������������Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  // ���ÿ⺯������ʼ��PB2

    GPIO_ResetBits(GPIOC, GPIO_Pin_5);                      // ������ɫLED��PC5�õ͵�ƽ
    GPIO_SetBits(GPIOB, GPIO_Pin_2);                        // Ϩ����ɫLED��PB2�øߵ�ƽ


    while (1)                                               // while������ѭ����������main�������н�������������Ӳ������
    {
    }
}




