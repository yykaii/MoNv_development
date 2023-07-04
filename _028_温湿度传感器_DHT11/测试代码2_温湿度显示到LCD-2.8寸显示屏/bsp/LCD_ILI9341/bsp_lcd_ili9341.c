/***********************************************************************************************************************************
    *   @file     lcd_ili9341.h
    *   @date     2020-12-13        
    *   @author   ħŮ������    
    *   @brief    ��������     ����QȺ�ļ��У� 
    *   @taobao   �Ա�Ӳ��     ttps://demoboard.taobao.com/
    ********************************************************************************************************************************
    *    ��ʵ��ƽ̨�� ħŮ������_STM32F103RC + KEIL5.27 + 2.8����ʾ��_ILI9341
    *
    *   ����ֲ˵���� 
    *    1��������ʹ����F103RC�ϣ�ʹ��IOģ��FSMCͨ�ţ�ע�����ŵ��޸�
    *    2�����ֵ���ʾ��ʹ�ÿ������ϵ��ⲿFLASH���ֿ�
    *     
    *   �����¼�¼��
    *    2020-09-xx  ����
    *    2020-12-12  �����޸���Ӣ����ʾ���� 
    *    2020-12-xx  �´����ƣ��������������ֲ�޸�
    
************************************************************************************************************************************/
#include "bsp_lcd_ili9341.h" 
#include "font.h" 


 

/*****************************************************************************
 ** ��������
 *****************************************************************************/
_LCD xLCD;



/*****************************************************************************
 ** ��������
 ****************************************************************************/
void sendOrder (u16 data);                                    // ��LCD���ͣ��Ĵ�����ֵַ(ָ��), 8λ
void sendDataShort (u16 data);                                // ��LCD���ͣ����ݣ�8λ
u16  readData (void);                                         // ��ȡLCD���ص�����
void sendShort(u16 Data);                                     // ��Һ�������ͣ����ݣ�16λ
void setCursor(u16 xStart,u16 yStart,u16 xEnd,u16 yEnd);      // ������ʾ����
u16 readReg(u16 reg);



// ����US������ʱ������������ֲʱ���ⲿ�ļ�������
#if 0
static void delayUS( uintt32_t times)
{
    times=times*7;      //  10us����7;
    while(--times)         
        __nop();  
}
#endif 

// ms��ʱ������������ֲʱ���ⲿ�ļ�������
#if 1
static void delayMS(uintt32_t ms)
{
    ms=ms*6500;                  
    for(uintt32_t i=0; i<ms; i++);      // 72MHzϵͳʱ���£����ٸ���ѭ��Լ��ʱ1ms
}
#endif




/*****************************************************************************
*��  ����LCD_Init
*��  �ܣ���ʼ��lcd
*��  ����Ϊ�˼���⡢������ֲ������(��Ļ���ء���ʾ����)��h�ļ�������޸�
*����ֵ���� 
*��  ע��
*****************************************************************************/
void LCD_Init(void)
{ 
    RCC->APB2ENR|=1<<3;       // ��ʹ������PORTBʱ��
    RCC->APB2ENR|=1<<4;       // ��ʹ������PORTCʱ��

    // ʹ��GPIOʱ��, Ϊ���ٵ��Թ�������ʹ��ʱ�Ӷ����������GPIO�˿�ʱ�ӣ���ʹ���ˣ�
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD  ; 
    GPIO_InitTypeDef G;                     // ���ã��������Ź���ģʽ 
    G.GPIO_Pin  = GPIO_Pin_15;              // ����KEY_1, ��ʱ����(��Ҫ)�� ����ʱ���øߵ�ƽ
    G.GPIO_Mode = GPIO_Mode_Out_PP ;        // ��ʱ��ƽ״̬(ʹ��оƬ�ڲ�������е�ƽ������)
    G.GPIO_Speed = GPIO_Speed_10MHz ;       // �����ƽ��ת�ٶȣ�������״̬ʱ��Ч����GPIO_Init������Ҫ�õ���
    GPIO_Init(GPIOA, &G);                   // ��ʼ�����������Ź���ģʽ���ú���
    
    
    System_SwdMode ();
                                             
    //PORTC6~10�����������     
    GPIOC->CRH&=0XFFFFF000;
    GPIOC->CRH|=0X00000333;   // PB8 9 10     �������
    GPIOC->CRL&=0X00FFFFFF;
    GPIOC->CRL|=0X33000000;   // PB6 7
    GPIOC->ODR|=0X07C0;       // PB6 7 8 9 10 ȫ������
    //PORTB �������     
    GPIOB->CRH=0X33333333;    // PB0~15       �������
    GPIOB->CRL=0X33333333;      
    GPIOB->ODR=0XFFFF;        // PB0~15       ȫ������

    
    delayMS(100);            // delay 50 ms 

     //����9341 ID�Ķ�ȡ        
    sendOrder(0XD3);          // ָ���ID               
    readData();               // ��1��������dummy     
    readData();               // ��2��������IC�汾��
    xLCD.id=readData();       // ��3��������IC����(93)                                   
    xLCD.id<<=8;
    xLCD.id|=readData();      // ��4��������IC����(41)    

    printf("��ʾ�� ���...        %x\r\n",xLCD.id);  // ��ӡLCD ID  
    if(xLCD.id !=0X9341)                             // 9341��ʼ��ʧ��
        return;

    sendOrder(0xCF);  
    sendDataShort(0x00); 
    sendDataShort(0xC1); 
    sendDataShort(0X30); 
    sendOrder(0xED);  
    sendDataShort(0x64); 
    sendDataShort(0x03); 
    sendDataShort(0X12); 
    sendDataShort(0X81); 
    sendOrder(0xE8);  
    sendDataShort(0x85); 
    sendDataShort(0x10); 
    sendDataShort(0x7A); 
    sendOrder(0xCB);  
    sendDataShort(0x39); 
    sendDataShort(0x2C); 
    sendDataShort(0x00); 
    sendDataShort(0x34); 
    sendDataShort(0x02); 
    sendOrder(0xF7);  
    sendDataShort(0x20); 
    sendOrder(0xEA);  
    sendDataShort(0x00); 
    sendDataShort(0x00); 
    sendOrder(0xC0);      // Power control 
    sendDataShort(0x1B);  // VRH[5:0] 
    sendOrder(0xC1);      // Power control 
    sendDataShort(0x01);  // SAP[2:0];BT[3:0] 
    sendOrder(0xC5);      // VCM control 
    sendDataShort(0x30);  // 3F
    sendDataShort(0x30);  // 3C
    sendOrder(0xC7);      // VCM control2 
    sendDataShort(0XB7); 
    sendOrder(0x36);      // Memory Access Control 
    sendDataShort(0x48); 
    sendOrder(0x3A);   
    sendDataShort(0x55); 
    sendOrder(0xB1);   
    sendDataShort(0x00);   
    sendDataShort(0x1A); 
    sendOrder(0xB6);      // Display Function Control 
    sendDataShort(0x0A); 
    sendDataShort(0xA2); 
    sendOrder(0xF2);      // 3Gamma Function Disable 
    sendDataShort(0x00); 
    sendOrder(0x26);      // Gamma curve selected 
    sendDataShort(0x01); 
    sendOrder(0xE0);      // Set Gamma 
    sendDataShort(0x0F); 
    sendDataShort(0x2A); 
    sendDataShort(0x28); 
    sendDataShort(0x08); 
    sendDataShort(0x0E); 
    sendDataShort(0x08); 
    sendDataShort(0x54); 
    sendDataShort(0XA9); 
    sendDataShort(0x43); 
    sendDataShort(0x0A); 
    sendDataShort(0x0F); 
    sendDataShort(0x00); 
    sendDataShort(0x00); 
    sendDataShort(0x00); 
    sendDataShort(0x00);          
    sendOrder(0XE1);      // Set Gamma 
    sendDataShort(0x00); 
    sendDataShort(0x15); 
    sendDataShort(0x17); 
    sendDataShort(0x07); 
    sendDataShort(0x11); 
    sendDataShort(0x06); 
    sendDataShort(0x2B); 
    sendDataShort(0x56); 
    sendDataShort(0x3C); 
    sendDataShort(0x05); 
    sendDataShort(0x10); 
    sendDataShort(0x0F); 
    sendDataShort(0x3F); 
    sendDataShort(0x3F); 
    sendDataShort(0x0F); 
    sendOrder(0x2B); 
    sendDataShort(0x00);
    sendDataShort(0x00);
    sendDataShort(0x01);
    sendDataShort(0x3f);
    sendOrder(0x2A); 
    sendDataShort(0x00);
    sendDataShort(0x00);
    sendDataShort(0x00);
    sendDataShort(0xef);     
    sendOrder(0x11);      // Exit Sleep
    delayMS(120);
    sendOrder(0x29);      // display on    
    
    LCD_DisplayDir(LCD_DIR);   // ������ʾ����
    LCD_Fill(0, 0, xLCD.width-1, xLCD.height-1, BLACK);
    LCD_LED=1;                 // ��������
    xLCD.FlagInit =1;    
}          

// �ײ㺯��-1����LCD���ͼĴ�����ַ(ָ��)
void sendOrder(u16 data)
{ 
    
    LCD_RS_LOW;          // RS=0:ָ�RS=1:����
     LCD_CS_LOW;         // Ƭѡ�ź����ͣ�ѡ���豸
    GPIOB->ODR = data;   // ��ֵ����
    LCD_WR_LOW;          // WR=0���豸���Զ�ȡ���ݣ�WR=1���豸�ȴ���ȡ����
    LCD_WR_HIGH; 
     LCD_CS_HIGH;        // Ƭѡ�ź����ߣ�
}

// �ײ㺯��-2
// д���ݺ���
// �������sendDataShortX��,��ʱ�任�ռ�.
// data:�Ĵ���ֵ
#if 1
void sendDataShort(u16 data)
{
    LCD_RS_HIGH;        // RS��: ����ֵ�� RS��: �Ĵ�����ֵַ
    LCD_CS_LOW;         // Ƭѡ 
    GPIOB->ODR = data;  // PB0~15��Ϊ������
    LCD_WR_LOW;         // д����
    LCD_WR_HIGH;        // д���� 
    LCD_CS_HIGH;        // Ƭѡ
}
#else
//д���ݺ���
#define sendDataShort(data)\
        {\
            LCD_RS_HIGH;\
            LCD_CS_LOW;\
            GPIOB->ODR = data;\
            LCD_WR_LOW;\
            LCD_WR_HIGH;\
            LCD_CS_HIGH;\
            } 
#endif

// �ײ㺯��-3
// ��LCD����
// ����ֵ:������ֵ
u16 readData(void)
{
    // ��PB0~15����Ϊ����������
    GPIOB->CRL=0X88888888; //PB0-7  ��������
    GPIOB->CRH=0X88888888; //PB8-15 ��������
    GPIOB->ODR=0X0000;     //ȫ�����0

    //��ȡ���� (���Ĵ���ʱ,������Ҫ��2��)
    LCD_RS_HIGH;
    LCD_CS_LOW;    
    LCD_RD_LOW;
    //if(xLCD.id==0X8989) delay_us(2);//FOR 8989,��ʱ2us                       
    u16 reg = GPIOB->IDR;  
    LCD_RD_HIGH;
    LCD_CS_HIGH; 

    // ��PB0~15����Ϊ����������
    GPIOB->CRL=0X33333333; //PB0-7  �������
    GPIOB->CRH=0X33333333; //PB8-15 �������
    GPIOB->ODR=0XFFFF;     //ȫ�������
    
    return reg;  
}

//���Ĵ���
//LCD_Reg:�Ĵ������
//����ֵ:������ֵ
u16 readReg(u16 reg)
{                                           
     sendOrder (reg);    // �Ĵ���  
    return readData(); 
} 

//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
    u16  r,g,b,rgb;   
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;     
    rgb=(b<<11)+(g<<5)+(r<<0);         
    return(rgb);
}    
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(u8 i)
{
    while(i--);
}
//��ȡ��ĳ�����ɫֵ     
//x,y:����
//����ֵ:�˵����ɫ
u16 LCD_ReadPoint(u16 x,u16 y)
{
     u16 r,g,b;
    
    if(x>=xLCD.width||y>=xLCD.height)return 0;    //�����˷�Χ,ֱ�ӷ���           
    setCursor(x,y, x+1, y+1);
    sendOrder(0X2E);//9341/6804/3510/1963 ���Ͷ�GRAMָ��
    GPIOB->CRL=0X88888888;                             //PB0-7  ��������
    GPIOB->CRH=0X88888888;                             //PB8-15 ��������
    GPIOB->ODR=0XFFFF;                                 //ȫ�������

    LCD_RS_HIGH;
    LCD_CS_LOW;        
    //��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)    
    LCD_RD_LOW;           
    opt_delay(2);                                      //��ʱ        
    r=GPIOB->IDR;                                      //ʵ��������ɫ 
    LCD_RD_HIGH;

     //dummy READ
    LCD_RD_LOW;                       
    opt_delay(2);    // ��ʱ                       
     r=GPIOB->IDR;   // ʵ��������ɫ
    LCD_RD_HIGH;

    LCD_RD_LOW;                       
    opt_delay(2);    // ��ʱ               
    b=GPIOB->IDR;    // ��ȡ��ɫֵ        
    LCD_RD_HIGH;
    g=r&0XFF;        // ����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
    g<<=8;

    LCD_CS_HIGH;
    GPIOB->CRL=0X33333333;         // PB0-7  �������
    GPIOB->CRH=0X33333333;         // PB8-15 �������
    GPIOB->ODR=0XFFFF;             // ȫ�������  
    
    return (((r>>11)<<11)|((g>>10)<<5)|(b>>11)); // ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
    
}         
//LCD������ʾ
void LCD_DisplayOn(void)
{                       
    sendOrder(0X29);    //������ʾ
    LCD_LED =1;

}     
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{       
    sendOrder(0X28);    //�ر���ʾ
    LCD_LED =0;
}   

/*****************************************************************
 * ��  ����setCursor
 * ��  �ܣ�������ʾ�����ڴ�����д�������Զ�����
 * ��  ����xStart���������, yStart���������
 *         xEnd�����������㣬yEnd������������
 * ����ֵ����
 *
 * ��  ע�� ħŮ�������Ŷӱ�д   �Ա� https://demoboard.taobao.com
 * ��  �� ���ϸ�����QȺ�ļ��У�   ����޸�_2020��09��014��
******************************************************************/
void setCursor(u16 xStart,u16 yStart,u16 xEnd,u16 yEnd)
{             
    sendOrder (0X2A);                 // ����ָ�����x����
    sendDataShort(xStart>>8);
    sendDataShort(xStart&0xFF);
    sendDataShort(xEnd>>8);
    sendDataShort(xEnd&0xFF);     
    
    sendOrder (0X2B); 
    sendDataShort(yStart>>8);
    sendDataShort(yStart&0xFF);
    sendDataShort(yEnd>>8);
    sendDataShort(yEnd&0xFF);     

    // ����дGRAMָ��
    sendOrder(0X2C);    
 
}          
 

// ����LCD��ʾ����
// ������ʾ����0-��������3-��������5-������, 6-������
void LCD_DisplayDir(u8 scanDir)
{
    u16 regval=0;
    
    if(scanDir==0||scanDir==3)     // ����
    {
        xLCD.dir=0;          
        xLCD.width  = LCD_WIDTH;
        xLCD.height = LCD_HIGH ;
    }
    if(scanDir==5 || scanDir==6)   // ����
    {                      
        xLCD.dir=1;          
        xLCD.width  = LCD_HIGH;
        xLCD.height = LCD_WIDTH;
    }         
    
    if(scanDir==0) regval|=(0<<7)|(0<<6)|(0<<5); // ������,���ϵ���
    if(scanDir==3) regval|=(1<<7)|(1<<6)|(0<<5); // ���ҵ���,���µ���
    if(scanDir==5) regval|=(0<<7)|(1<<6)|(1<<5); // ���ϵ���,���ҵ���
    if(scanDir==6) regval|=(1<<7)|(0<<6)|(1<<5); // ���µ���,������           
    sendOrder (0X36);             // ��д������ɫģʽ
    sendDataShort(regval|0x08);          // 
}     

/*****************************************************************
 * ��  ����drawPoint
 * ��  �ܣ���һ����
 * ��  ����x���꣬y����, 16λ��ɫֵ
 * ����ֵ����
 * 
 * ��  ע�� ħŮ�������Ŷӱ�д   �Ա� https://demoboard.taobao.com
 * ��  �� ���ϸ�����QȺ�ļ��У�   ����޸�_2020��09��01��
******************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    setCursor(x,y, x+1, y+1);        //���ù��λ�� 
    sendDataShort(color); 
}    

//��ָ�����������ָ����ɫ
//�����С:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:Ҫ������ɫ
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{          
    uintt32_t CNT=(ex+1-sx)*(ey+1-sy);
    setCursor(sx, sy, ex, ey);                  // ���ù��λ�� 
    
    LCD_RS_HIGH;             // RS��: ����ֵ�� RS��: �Ĵ�����ֵַ
    LCD_CS_LOW;              // Ƭѡ 
    while(CNT--)
    {
        GPIOB->ODR = color;  // PB0~15��Ϊ������
        LCD_WR_LOW;          // д����
        LCD_WR_HIGH;         // д���� 
    }
    LCD_CS_HIGH;             // Ƭѡ
}  

//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance; 
    int incx,incy,uRow,uCol; 
    delta_x=x2-x1; //������������ 
    delta_y=y2-y1; 
    uRow=x1; 
    uCol=y1; 
    if(delta_x>0)incx=1; //���õ������� 
    else if(delta_x==0)incx=0;//��ֱ�� 
    else {incx=-1;delta_x=-delta_x;} 
    if(delta_y>0)incy=1; 
    else if(delta_y==0)incy=0;//ˮƽ�� 
    else{incy=-1;delta_y=-delta_y;} 
    if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
    else distance=delta_y; 
    for(t=0;t<=distance+1;t++ )//������� 
    {  
        LCD_DrawPoint(uRow,uCol, color);//���� 
        xerr+=delta_x ; 
        yerr+=delta_y ; 
        if(xerr>distance) 
        { 
            xerr-=distance; 
            uRow+=incx; 
        } 
        if(yerr>distance) 
        { 
            yerr-=distance; 
            uCol+=incy; 
        } 
    }  
}    
//������      
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_Line(x1,y1,x2,y1, color);
    LCD_Line(x1,y1,x1,y2, color);
    LCD_Line(x1,y2,x2,y2, color);
    LCD_Line(x2,y1,x2,y2, color); 
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Circle(u16 x0,u16 y0,u8 r, u16 color)
{
    int a,b;
    int di;
    a=0;b=r;      
    di=3-(r<<1);             //�ж��¸���λ�õı�־
    while(a<=b)
    {
        LCD_DrawPoint(x0+a,y0-b, color);             //5
        LCD_DrawPoint(x0+b,y0-a, color);             //0           
        LCD_DrawPoint(x0+b,y0+a, color);             //4               
        LCD_DrawPoint(x0+a,y0+b, color);             //6 
        LCD_DrawPoint(x0-a,y0+b, color);             //1       
        LCD_DrawPoint(x0-b,y0+a, color);             
        LCD_DrawPoint(x0-a,y0-b, color);             //2             
        LCD_DrawPoint(x0-b,y0-a, color);             //7                  
        a++;
        //ʹ��Bresenham�㷨��Բ     
        if(di<0)di +=4*a+6;      
        else
        {
            di+=10+4*(a-b);   
            b--;
        }                             
    }
}                                       



/******************************************************************
 * �������� drawAscii
 * ��  �ܣ� ��ָ��λ����ʾһ���ַ�    
 * ��  ���� u16 x,y     ��ʼ����
 *          u8  num     Ҫ��ʾ���ַ�:" "--->"~"
 *          u8  size    �����С 12/16/24/32
 *          uintt32_t fColor  ������ɫ
 *          uintt32_t bColor  ������ɫ
 * ��  ע�� �ο�ԭ�Ӹ��Ұ�����Ĵ�����޸�  ���ϴ��QȺ��  
 *****************************************************************/
void drawAscii(u16 x,u16 y,u8 num,u8 size,uintt32_t fColor, uintt32_t bColor)
{              
    //spiInit();                                      // ��ֹSPI�����������豸�޸���
     
    u8 temp;
    u16 y0=y;
    
    u8 csize=(size/8+((size%8)?1:0))*(size/2);           // �õ�����һ���ַ���Ӧ������ռ���ֽ���    
     num=num-' ';                                       // �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
    for(u8 t=0;t<csize;t++)
    {   
        if(size==12)         temp=asc2_1206[num][t];   // ����1206����
        else if(size==16)    temp=asc2_1608[num][t];   // ����1608����
        else if(size==24)    temp=asc2_2412[num][t];   // ����2412����
        else if(size==32)    temp=asc2_3216[num][t];   // ����3216����
        else return;                                   // û�е��ֿ�
        
        for(u8 t1=0; t1<8; t1++)
        {                
            if(temp&0x80) LCD_DrawPoint (x, y, fColor);// ���� ���� 
            else          LCD_DrawPoint (x, y, bColor);// ���� ����
            temp<<=1;
            y++;
            if(y>=xLCD.height)    return;              // ������Ļ�߶�(��)
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=xLCD.width) return;              // ������Ļ���(��)
                break;
            }
        }       
    }                     
} 

/******************************************************************
 * �������� drawGBK
 * ��  �ܣ� ��ָ��λ����ʾһ���ַ�    
 * ��  ���� u16 x,y     ��ʼ����
 *          u8  num     Ҫ��ʾ���ַ�:" "--->"~"
 *          u8  size    �����С 12/16/24/32
 *          uintt32_t fColor  ������ɫ
 *          uintt32_t bColor  ������ɫ
 * ��  ע�� �ο�ԭ�Ӹ��Ұ�����Ĵ�����޸�  ���ϴ��QȺ��  
 *****************************************************************/
void drawGBK( u16 x, u16 y, u8* font, u8 size, uintt32_t fColor, uintt32_t bColor)
{    
    u8 temp;
    u16 y0=y;
    u8 GBK[128];   
    u8 csize=(size/8+((size%8)?1:0))*(size); // �õ�����һ���ַ���Ӧ������ռ���ֽ���     
    W25qxx_ReadGBK(font, size, GBK);         // �õ���Ӧ��С�ĵ�������
    
    //spiInit();                             // ��ֹSPI�����������豸�޸���    
    for(u8 t=0; t<csize; t++)
    {                                                      
        temp = GBK[t];                       // �õ�GBK��������                          
        for(u8 t1=0; t1<8; t1++)
        {
            if(temp&0x80)   LCD_DrawPoint (x, y, fColor);    
            else            LCD_DrawPoint (x, y, bColor);            
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }       
    }      
}



/******************************************************************************
 * ��  ���� LCD_String
 * ��  �ܣ� ��LCD����ʾ�ַ���(֧��Ӣ�ġ�����)
 * ��  ���� Ӣ�ģ���ģ���ݱ�����font.h�������ʹ���һ�𱣴���оƬ�ڲ�Flash
 *          ���֣���ģ�������ⲿFlash�У��������ֿ���W25Q128��
 *                ħŮ��������W25Q128����¼����4���ֺŴ�С��ģ����
 * ��  ���� u16   x      �������Ͻ�X����
 *          u16   y      �������Ͻ�y����
 *          char* pFont  Ҫ��ʾ���ַ�������
 *          u8    size   �ֺŴ�С��12 16 24 32
 *          uintt32_t   fColor ������ɫ
 *          uintt32_t   bColor ������ɫ
 * ����ֵ:  ��
 * ��  ע�� ħŮ������   ���ϸ���QȺ��     ����޸�_2020��05��1����
 ******************************************************************************/  
void LCD_String(u16 x, u16 y, char* pFont, u8 size, uintt32_t fColor, uintt32_t bColor)
{
    if(xLCD .FlagInit ==0 ) return;
    
    u16 xStart = x;    

    if( size!=12 && size!=16 && size!=24 && size!=32 )       // �����С����
        size=24;    
    
    while(*pFont != 0)                // ������ȡ�ַ������ݣ�ֱ��'\0'ʱֹͣ
    {    
        if(x>(xLCD.width-size))       // ��λ���жϣ����������ĩ���Ͱѹ�껻��
        {
            x=xStart;
            y=y+size;
        }
        if(y>(xLCD.height - size))    // ��λ���жϣ����������ĩ���ͷ��أ��������
            return;        
        
        if(*pFont < 127)              // ASCII�ַ�
        {
            drawAscii (x, y, *pFont, size, fColor, bColor);            
            pFont++;
            x+=size/2;            
        }
        else                          // ������ʾ
        {     
            // ��Ҫ: ����õĲ���ħŮ��������ֿ�, ��Ҫ�޸Ļ�ע��������һ��, �����Ͳ�Ӱ��ASCIIӢ���ַ������            
            drawGBK(x, y, (u8*)pFont, size, fColor, bColor);     
            pFont = pFont+2;          // ��һ��Ҫ��ʾ���������ڴ��е�λ��              
            x=x+size;                 // ��һ��Ҫ��ʾ����������Ļ�ϵ�Xλ��    
        }
    }      
}



/******************************************************************
 * �������� LCD_Image
 * ��  �ܣ� ��ָ�����������ָ��ͼƬ����
 *          ͼƬ������font.h�ļ���.ֻ�ʺ�����ͼƬ����
 *          Image2Lcdת����ˮƽɨ�裬16λ���ɫ
 * ��  ���� u16 x,y     ���Ͻ���ʼ����
 *          u16 width   ͼƬ���
 *          u16 height  ͼƬ�߶�
 *          u8* image   ���ݻ����ַ
 *****************************************************************/
void LCD_Image(u16 x, u16 y, u16 width, u16 height, const u8 *image) 
{  
    //spiInit();                               // ��ֹSPI�����������豸�޸���
    
    u16 colorData=0;
    uintt32_t cnt=0;
    
     for(u16 i=0; i<height; i++)               // һ��һ�е���ʾ
    {
         setCursor(x, y+i, x+width, y+height); // �������ù��λ�� 
        for(u16 j=0; j<width; j++)             // һ���У������£�������ش���
        {            
            colorData=(image[cnt*2+1]<<8) | image[cnt*2]; 
            sendDataShort(colorData);          // д��16λ��ɫ���� 
            cnt++;    
        }
    }      
}  



/******************************************************************
 * �������� LCD_Cross
 * ��  �ܣ� ��ָ�����ϻ���ʮ���ߣ�����У׼������
 * ��  ���� uint16_t x  ��   ʮ���ߵ����ĵ�����x
 *          uint16_t y  ��   ʮ���ߵ����ĵ�����x
 *          uint16_t len     ʮ���ߵ����س���
 *          uint32_t fColor  ��ɫ
 * �����أ� ��
 * ��  ע�� 
 *****************************************************************/
void LCD_Cross( uint16_t x, uint16_t y, uint16_t len, uint32_t fColor)
{
    uint16_t temp=len/2;                
                
    LCD_Line(x-temp, y, x+temp, y, fColor);   
    LCD_Line(x, y-temp, x, y+temp, fColor);       
}



/******************************************************************
 * �������� LCD_GUI
 * ��  �ܣ� ���԰����豸�����LCD��ʾ����
 * ��  ����           
 * �����أ� 
 * ��  ע�� 
 *****************************************************************/
void LCD_GUI(void)
{
//    char strTemp[30];
//    
//    LCD_Fill(0, 0,  xLCD.width,  xLCD.height, BLACK);
//    //LCD_Image (0,0, 60, 60, imageLoge);  // ͼƬ��ʾ����
//    LCD_Line(  0, 49,  0,329,GREY);        // ����
//    LCD_Line(119, 70,119,329,GREY);        // ����
//    LCD_Line(239, 49,239,329,GREY);        // ����
//    LCD_Fill(  0,287,239,329,WHITE);       // �װ�
//    LCD_Line(0,303,239,303,BLACK);
//    LCD_Line(119,287,119,329,BLACK);
//    LCD_Fill (0,49, 239,70,WHITE);    
//    LCD_Line(119, 49,119, 70,BLACK);       // ����
//    LCD_Fill (119,168, 239,188,WHITE);    
//    
//    LCD_Printf( 8,  0,"STM32F103RCT6������", 24, WHITE, BLACK);    
//    LCD_Printf( 72, 28,"�豸�����", 16, GREY, BLACK);        
//    LCD_Printf(  6, 52,"�����豸", 16,  BLACK , WHITE);
//    LCD_Printf(125, 52,"WiFi����ͨ��", 16, BLACK , WHITE);
//    LCD_Printf(125,171,"CANͨ��" , 16, BLACK , WHITE);    
//    LCD_Printf(  6,290,"�ڲ��¶�", 12, BLACK , WHITE);     // �ڲ��¶�
//    LCD_Printf(  6,306,"��������", 12, BLACK , WHITE);     // ��������
//    LCD_Printf(125,290,"��������", 12, BLACK , WHITE);     // ��������
//    LCD_Printf(125,306,"����ʱ��", 12, BLACK , WHITE);     // ����ʱ��
//    sprintf(strTemp, "��%d��", xW25Qxx .StartupTimes);
//    LCD_Printf( 68,306,strTemp,12,BLUE,WHITE);
//    
//    u16 y=74;
//    // UASRT1
//    LCD_Printf( 6, y, "UART1����",  12, YELLOW ,BLACK);  
//    if(xUSART.USART1InitFlag ==1)  { LCD_Printf(90,y,"���", 12, GREEN ,BLACK); }   else    { LCD_Printf(90,y,"ʧ��", 12, RED ,BLACK); }  y=y+15;
//    // SystemClock
//    LCD_Printf( 6, y, "ϵͳʱ��",   12, YELLOW ,BLACK);   
//    sprintf(strTemp,"%d",SystemCoreClock/1000000);
//    LCD_Printf(84, y,strTemp,       12, GREEN ,BLACK);  
//    LCD_Printf(96, y, "MHz", 12, GREEN ,BLACK);   y=y+15;
//    // LEDָʾ��
//    LCD_Printf( 6, y, "LEDָʾ��",  12, YELLOW ,BLACK);   
//    LCD_Printf(90,y,"���", 12, GREEN ,BLACK);    y=y+15;
//    // �����ж�
//    LCD_Printf( 6, y, "�����ж�",   12, YELLOW ,BLACK);   
//    LCD_Printf(90,y,"���", 12, GREEN ,BLACK);    y=y+15;
//    // FLASH�洢��
//    LCD_Printf( 6, y, "FLASH�洢",  12, YELLOW ,BLACK);   
//    if(xW25Qxx.FlagInit  ==1)  { LCD_Printf(71,y,xW25Qxx.type, 12, GREEN ,BLACK); }   else    { LCD_Printf(90,y,"ʧ��", 12, RED ,BLACK); }  y=y+15;
//    // �����ֿ�
//    LCD_Printf( 6, y, "�����ֿ�",   12, YELLOW ,BLACK);  
//    if(xW25Qxx .FlagGBKStorage ==1)  { LCD_Printf(90,y,"����", 12, GREEN ,BLACK); }   else    { LCD_Printf(90,y,"ʧ��", 12, RED ,BLACK); }  y=y+15;
//    // ��ʾ��
//    LCD_Printf( 6, y, "��ʾ��оƬ", 12, YELLOW ,BLACK);  
//    sprintf(strTemp,"%X",xLCD.id);        
//    if(xLCD.FlagInit  ==1)  { LCD_Printf(90,y,strTemp, 12, GREEN ,BLACK); }   else    { LCD_Printf(90,y,"ʧ��", 12, RED ,BLACK); }  y=y+15;
//           
}

