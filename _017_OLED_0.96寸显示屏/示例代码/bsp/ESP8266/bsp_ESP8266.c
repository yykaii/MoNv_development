#include "bsp_ESP8266.h"

xESP8266_TypeDef  xESP8266;


// ����US������ʱ������������ֲʱ���ⲿ�ļ�����;
#if 0
static void delayUS(__IO uint32_t times)
{
    times = times * 7;  //  10us����7;
    while (--times)
        __nop();
}
#endif

#if 1
// ms��ʱ������������ֲʱ���ⲿ�ļ�����;
static void delayMS(uint32_t ms)
{
    ms = ms * 6500;
    for (uint32_t i = 0; i < ms; i++); // 72MHzϵͳʱ���£����ٸ���ѭ��Լ��ʱ1ms
}
#endif


// ��ESP8266ģ�鷢��AT����
// ����: char* cmdString :     AT�����ַ�����ַ
//       char* answerString :  �ȴ����ص�����ȷ���ַ�����ַ
//       uint32_t waitTimesMS: ���������ȴ���ʱ�䣬���루��׼ȷʱ��ֵ��
uint8_t ESP8266_CMD(char *cmdString, char *answerString, uint32_t waitTimesMS)
{
    uint16_t CNT = 0;                   // �ȴ���ʱ����
    ESP8266_CleanReceivedFlag();        // ���ձ�ʾ��0
    ESP8266_SendString(cmdString);      // ����ATָ��
    while (1)                           // �ȴ�ָ���ִ�����
    {
        if (ESP8266_CheckReceivedNum())
        {
            if (strstr((char *)xESP8266.ReceivedBuffer, answerString)) // �յ�����ȷ��
                return 1;
        }
        delayMS(1);
        if (++CNT > waitTimesMS)       // ��ʱδ�յ���ȷ���ݣ�
        {
            return 0;                  // ���ش���ֵ��0
        }
    }
}


// ESP8266��ʼ��
uint8_t ESP8266_Init(USART_TypeDef *USARTx, uint32_t baudrate)
{
    printf("\r\nESP8266 ��ʼ���ü�����......\r\n");
    delayMS(300);                                    // ��Ҫ���ϵ�󣬱�������ʱ�Եȴ�8266�ȶ����ɹ���

    if (USARTx == USART1)
    {
        USART1_Init(baudrate);
    }
    if (USARTx == USART2)
    {
        USART2_Init(baudrate);
    }
    if (USARTx == USART3)
    {
        USART3_Init(baudrate);
    }
#ifdef STM32F10X_HD
    if (USARTx == UART4)
    {
        UART4_Init(baudrate);
    }
    if (USARTx == UART5)
    {
        UART5_Init(baudrate);
    }
#endif

    xESP8266.FlagOkay = 0;           // ��ʼ��״̬
    xESP8266.USARTx   = USARTx;      // ��¼���ô��ڶ˿�
    xESP8266.Baudrate = baudrate;    // ��¼���õĲ�����

    ESP8266_CMD("AT\r\n",   "OK", 1500)  ? (xESP8266.FlagOkay = 1) : (xESP8266.FlagOkay = 0) ; // �����Ƿ�������ESP8266
    xESP8266.FlagOkay ? printf("ģ�����Ӳ���:  �ɹ�\r\n") : printf("ģ�����Ӳ���:  ʧ��\r\n");   // �������״��

    return xESP8266.FlagOkay;      // ���س�ʼ��״̬��0_ʧ�ܣ�1_����
}


void ESP8266_SendString(char *str)
{
    if (xESP8266.USARTx == USART1)
    {
        USART1_SendString(str);
    }
    if (xESP8266.USARTx == USART2)
    {
        USART2_SendString(str);
    }
    if (xESP8266.USARTx == USART3)
    {
        USART3_SendString(str);
    }
#ifdef STM32F10X_HD
    if (xESP8266.USARTx == UART4)
    {
        UART4_SendString(str);
    }
    if (xESP8266.USARTx == UART5)
    {
        UART5_SendString(str);
    }
#endif
}

// ����Ƿ��յ�ESP8266�����������ݣ���־
// ���ؽ��յ����ֽ�����
uint16_t ESP8266_CheckReceivedNum(void)
{
    if ((xESP8266.USARTx == USART1) && (xUSART.USART1ReceivedNum))
    {
        xESP8266.ReceivedNum = xUSART.USART1ReceivedNum;
        memcpy(xESP8266.ReceivedBuffer, xUSART.USART1ReceivedBuffer, xESP8266.ReceivedNum);
        xUSART.USART1ReceivedNum = 0;
    }
    if ((xESP8266.USARTx == USART2) && (xUSART.USART2ReceivedNum))
    {
        xESP8266.ReceivedNum = xUSART.USART2ReceivedNum;
        memcpy(xESP8266.ReceivedBuffer, xUSART.USART2ReceivedBuffer, xESP8266.ReceivedNum);
        xUSART.USART2ReceivedNum = 0;
    }
    if ((xESP8266.USARTx == USART3) && (xUSART.USART3ReceivedNum))
    {
        xESP8266.ReceivedNum = xUSART.USART3ReceivedNum;
        memcpy(xESP8266.ReceivedBuffer, xUSART.USART3ReceivedBuffer, xESP8266.ReceivedNum);
        xUSART.USART3ReceivedNum = 0;
    }
#ifdef STM32F10X_HD
    if ((xESP8266.USARTx == UART4)  && (xUSART.UART4ReceivedNum))
    {
        xESP8266.ReceivedNum = xUSART.UART4ReceivedNum;
        memcpy(xESP8266.ReceivedBuffer, xUSART.UART4ReceivedBuffer, xESP8266.ReceivedNum);
        xUSART.UART4ReceivedNum = 0;
    }
    if ((xESP8266.USARTx == UART5)  && (xUSART.UART5ReceivedNum))
    {
        xESP8266.ReceivedNum = xUSART.UART5ReceivedNum;
        memcpy(xESP8266.ReceivedBuffer, xUSART.UART5ReceivedBuffer, xESP8266.ReceivedNum);
        xUSART.UART5ReceivedNum = 0;
    }
#endif

    return xESP8266.ReceivedNum;
}


// ����ESP8266�Ľ��ջ��棬�������ճ��ȱ��������ݴ�Ż���
void ESP8266_CleanReceivedFlag(void)
{
    if (xESP8266.USARTx == USART1)
    {
        xUSART.USART1ReceivedNum = 0;
    }
    if (xESP8266.USARTx == USART2)
    {
        xUSART.USART2ReceivedNum = 0;
    }
    if (xESP8266.USARTx == USART3)
    {
        xUSART.USART3ReceivedNum = 0;
    }
#ifdef STM32F10X_HD
    if (xESP8266.USARTx == UART4)
    {
        xUSART.UART4ReceivedNum = 0;
    }
    if (xESP8266.USARTx == UART5)
    {
        xUSART.UART5ReceivedNum = 0;
    }
#endif

    xESP8266.ReceivedNum = 0;                                         // ��0�����ճ���
    memset(xESP8266.ReceivedBuffer, 0, ESP8266_RX_BUF_SIZE);          // ���㣬���ջ���
}


/******************************************************************************
 * ��  ���� ESP8266_GetLinkStatus
 * ��  �ܣ� ��ʼ��USART��GPIO��ͨ�Ų������á��ж����ȼ�
 *          (8λ���ݡ���У�顢1��ֹͣλ)
 * ��  ���� uint32_t baudrate  ͨ�Ų�����
 * ����ֵ:  0_��ȡ״̬ʧ��
 *          2_���ip
 *          3_��������
 *          4_ʧȥ����
 ******************************************************************************/
uint8_t ESP8266_GetLinkStatus(void)
{
    if (ESP8266_CMD("AT+CIPSTATUS\r\n", "OK", 10000))
    {
        if (strstr((char *)xESP8266.ReceivedBuffer, "STATUS:2"))
        {
            ESP8266_CleanReceivedFlag();
            return 2;
        }
        if (strstr((char *)xESP8266.ReceivedBuffer, "STATUS:3"))
        {
            ESP8266_CleanReceivedFlag();
            return 3;
        }

        if (strstr((char *)xESP8266.ReceivedBuffer, "STATUS:4"))
        {
            ESP8266_CleanReceivedFlag();
            return 4;
        }
        if (strstr((char *)xESP8266.ReceivedBuffer, "STATUS:5"))
        {
            ESP8266_CleanReceivedFlag();
            return 5;
        }
    }
    
    ESP8266_CleanReceivedFlag();
    return 0;
}



/******************************************************************************
 * ��  ���� ESP8266_JoinAP
 * ��  �ܣ� ����AP
 * ��  ���� char* SSID       WiFi����
 *          char* passWord   WiFi����
 * �� �� ֵ: 0_����ʧ��
 *           1_���ӳɹ�
 ******************************************************************************/
uint8_t ESP8266_JoinAP(char *SSID, char *passWord)
{
    char strTemp[60];
    uint8_t linkStatus = 0;

    // ��ESP8266�������ó�SATģʽ
    ESP8266_CMD("AT+RESTORE\r\n", "ready", 3000)  ? printf("�ָ���������:  �ɹ�\r\n") : printf("�ָ���������: ʧ��\r\n");    // �ָ�ģ��ĳ�������
    ESP8266_CMD("AT+CWMODE=1\r\n",    "OK", 3000)  ? printf("���� STAģʽ:  �ɹ�\r\n") : printf("���� STAģʽ: ʧ��\r\n");   // ����ģʽ��1_STA, 2_AP, 3_STA+AP
    ESP8266_CMD("AT+RST\r\n",      "ready", 3000)  ? printf("���� ESP8266:  �ɹ�\r\n") : printf("���� ESP8266: ʧ��\r\n");   // ����ģ��: ���ù���ģʽ������������Ч
    ESP8266_CMD("AT+CIPMUX=0\r\n",    "OK", 3000)  ? printf("������ģʽ  :  �ɹ�\r\n") : printf("������ģʽ  : ʧ��\r\n");   // �� �� ��: 0_�����ӣ�1_������
    //������ָ��WiFi�ȵ�
    printf("׼������SSID�� %s, %s\r\n", SSID, passWord);
    sprintf(strTemp, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, passWord);
    printf("��ʼ����AP ... ");
    ESP8266_CMD(strTemp,        "OK\r\n", 20000)  ? printf("�ɹ�\r\n") : printf("ʧ��\r\n");
    // �������״̬
    printf("��ȡ����״̬��");
    linkStatus = ESP8266_GetLinkStatus();
    if (linkStatus == 0)
    {
        printf(" ʧ�ܣ�ԭ�򣺻�ȡʧ�ܣ�\r\n");
        return 0;
    }
    if (linkStatus == 2)
    {
        printf(" �ɹ����ѻ��IP\r\n");
        return 1;
    }
    if (linkStatus == 3)
    {
        printf(" ʧ�ܣ�ԭ�������ӣ���δ���IP��\r\n");
        return 0;
    }
    if (linkStatus == 4)
    {
        printf(" ʧ�ܣ�ԭ��ʧȥ���ӣ�\r\n");
        return 0;
    }
    if (linkStatus == 5)
    {
        printf(" ʧ�ܣ�ԭ��û������\r\n");
        return 0;
    }

    return 0;

}

// ��ģ�����ó�APģʽ
uint8_t ESP8266_SetAP(char *SSID, char *passWord)
{
    char strTemp[60];

    printf("׼������SSID��%s, %s\r\n", SSID, passWord);
    // ��ESP8266�������ó�APģʽ
    ESP8266_CMD("AT+RESTORE\r\n", "ready", 1000)  ? printf("�ָ���������: �ɹ�\r\n") : printf("�ָ���������: ʧ��\r\n");   // �ָ�ģ��ĳ�������
    ESP8266_CMD("AT+CWMODE=2\r\n",   "OK", 3000)  ? printf("����ΪAPģʽ: �ɹ�\r\n") : printf("���� STAģʽ: ʧ��\r\n");   // ����ģʽ��1_STA, 2_AP, 3_STA+AP
    ESP8266_CMD("AT+RST\r\n",     "ready", 3000)  ? printf("���� ESP8266: �ɹ�\r\n") : printf("���� ESP8266: ʧ��\r\n");   // ����ģ��: ���ù���ģʽ������������Ч
    // ����WiFi�ȵ�
    sprintf(strTemp, "AT+CWSAP=\"%s\",\"%s\",11,0\r\n", SSID, passWord);
    printf("��ʼ����AP... ");
    if (ESP8266_CMD(strTemp,        "OK\r\n", 10000))
    {
        printf("�ɹ�\r\n");
        return 1;
    }
    else
    {
        printf("ʧ��\r\n");
        return 0;
    }
}


uint8_t ESP8266_TCPConnect(char *IP, uint16_t port)
{
    char strTemp[100];
    uint8_t flag = 0;

    printf("����TCPͨ��... ");
    sprintf(strTemp, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", IP, port);
    ESP8266_CMD(strTemp, "OK\r\n", 20000)  ? (flag = 1) : (flag = 0) ;
    flag ? printf("�ɹ�\r\n") : printf("ʧ��, ����APP��IP��ַ���˿ںš��Ƿ��ѶϿ��ɵ�����״̬\r\n");
    ESP8266_CleanReceivedFlag();
    return flag;
}

// ���ã������ӣ�0_�����ӣ�1_������;��ע��͸������AT+CIPMUX=0;
uint8_t   ESP8266_SetCIPMux(uint8_t value)
{
    uint8_t flag = 0;
    char strTemp[50];
    sprintf(strTemp, "AT+CIPMUX=%d\r\n", value);
    ESP8266_CMD(strTemp, "OK", 3000)  ? (flag = 1) : (flag = 0) ;
    flag ? printf("�������ӷ�ʽ:  �ɹ�\r\n") : printf("�������ӷ�ʽ: ʧ��\r\n");   // �� �� ��: 0_�����ӣ�1_������
    ESP8266_CleanReceivedFlag();
    return flag;
}


// ���ã����䷽ʽ��0_��ͨ���䣬1_͸������
uint8_t   ESP8266_SetCIPMode(uint8_t value)
{
    uint8_t flag = 0;
    char strTemp[50];
    sprintf(strTemp, "AT+CIPMODE=%d\r\n", value);
    ESP8266_CMD(strTemp, "OK", 3000)  ? (flag = 1) : (flag = 0) ;
    flag ? printf("���ô��䷽ʽ:  �ɹ�\r\n") : printf("���ô��䷽ʽ: ʧ��\r\n");   // ���䷽ʽ��0_��ͨ���䣬1_͸������
    ESP8266_CleanReceivedFlag();
    return flag;
}

// ����͸��
uint8_t ESP8266_EnablePassThrough(void)
{
    uint8_t status = 1;
    status &= ESP8266_CMD("AT+CIPMODE=1\r\n", "OK", 3000);   // ���䷽ʽ��0_��ͨ���䣬1_͸������
    status &= ESP8266_CMD("AT+CIPSEND\r\n",    ">", 3000);   // ���ݴ��䣬��������͸��ʱ���������

    ESP8266_CleanReceivedFlag();                             // ������ձ�ǡ�����

    status ? printf("��͸������:  �ɹ�\r\n") : printf("��͸������:  ʧ��\r\n");
    return status;
}

// ֹͣ͸��
uint8_t ESP8266_DisablePassThrough(void)
{
    ESP8266_SendString("+++");
    printf("�ѹر�͸�����䣡\r\n");
    return 0;
}
