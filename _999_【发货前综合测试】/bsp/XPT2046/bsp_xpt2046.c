#include "bsp_xpt2046.h"


/******************************* 声明相关的全局变量 ***************************/
xXPT2046_TypeDey xXPT2046;    // 用于存放XPT2046的各种信息，全局变量，在h中extern


/******************************* 声明相关的静态变量 ***************************/



#define DURIATION_TIME      2            // 触摸消抖阈值
#define XPT2046_CHANNEL_X   0x90         // 控制字：检测通道Y+电压值    
#define XPT2046_CHANNEL_Y   0xd0         // 控制字：检测通道X+电压值

#define  IRQ_READ           GPIO_ReadInputDataBit ( XPT2046_IRQ_GPIO, XPT2046_IRQ_PIN )
#define  CS_HIGH            (XPT2046_CS_GPIO->BSRR = XPT2046_CS_PIN)
#define  CS_LOW             (XPT2046_CS_GPIO->BRR  = XPT2046_CS_PIN)
#define  CLK_HIGH           (XPT2046_CLK_GPIO->BSRR = XPT2046_CLK_PIN)
#define  CLK_LOW            (XPT2046_CLK_GPIO->BRR  = XPT2046_CLK_PIN)
#define  MOSI_1             (XPT2046_MOSI_GPIO->BSRR = XPT2046_MOSI_PIN)
#define  MOSI_0             (XPT2046_MOSI_GPIO->BRR  = XPT2046_MOSI_PIN)
#define  MISO               (((XPT2046_MISO_GPIO->IDR) & XPT2046_MISO_PIN) ? 1 : 0 )




/******************************* 声明 XPT2046 相关的静态函数 ***************************/
static void      delayUS(__IO uint32_t ulCount);     // 粗略的延时函数, 为方便移植, 不使用外部延时函数
static void      sendCMD(uint8_t cmd);               // 发送命令字
static uint16_t  receiveData(void);                  // 读取返回值
static uint8_t   touchDetect(void);                  // 判断触摸屏是否已按下且稳定，0_已稳定地按下，1_释放
static int16_t   readADC_X(void);                    // 读取X的ADC值
static int16_t   readADC_Y(void);                    // 读取Y的ADC值
static uint8_t   readAdcXY(void);                    // 读取X、Y的ADC值并滤波，存放到全局结构体变量xXPT2046中



static uint8_t   waitForFlashBSY(uint32_t timeOut);  // 用于把校准数据存放到内部FLASH中:等待FLASH->SR:BSY空闲;
static uint8_t   readInteriorFlash(uint32_t readAddr, uint8_t *pBuffer, uint16_t numToRead);          // 用于把校准数据存放到内部FLASH中: 读取内部FLASH数据
static uint8_t   writeInteriorFlash(uint32_t writeAddr, uint8_t *writeToBuffer, uint16_t numToWrite); // 用于把校准数据存放到内部FLASH中: 写入内部FLASH数据



// 校准数据存放到内部FLASH, 共三个函数
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 下面这三个定义，用于存取内部FLASH数据，F103系列都不用修改
#ifdef   STM32F10X_HD
    #define  STM32_FLASH_SECTOR_SIZE       2048                  // 内部FLASH页大小, 单位：bytes 　(注：STM32F10xx系列下，小中容量存储器扇区为1K, 大容量存储器扇区为2K）
#else
    #define  STM32_FLASH_SECTOR_SIZE       1024             　　 // 内部FLASH页大小, 单位：bytes 　(注：STM32F10xx系列下，小中容量存储器扇区为1K, 大容量存储器扇区为2K）
#endif

#define  STM32_FLASH_ADDR_BASE   0x08000000                  // 芯片内部FLASH基址(这个基本不用修改）
static   uint8_t sectorbufferTemp[STM32_FLASH_SECTOR_SIZE];  // 开辟一段内存空间，作用：在内部FLASH写入时作数据缓冲


// 作用：内部FLASH写入时，等待空闲，BSY位标志:0闲1忙
static uint8_t waitForFlashBSY(uint32_t timeOut)
{
    while ((FLASH->SR & 0x01) && (timeOut-- != 0x00)) ; // 等待BSY标志空闲
    if (timeOut == 0)    return 1;                     // 失败，返回1, 等待超时；　

    return 0;                                          // 正常，返回０
}

/******************************************************************************
 * 函　数:  readInteriorFlash
 * 功  能： 在芯片的内部FLASH里，读取指定长度数据
 * 参  数： uint32_t  readAddr     数据地址
 *          uint8_t  *pBuffer      读出后存放位置
 *          uint16_t  numToRead    读取的字节数量
 * 返回值： 0_成功
 *          1_失败，地址小于FLASH基址
 *          2_失败，地址大于FLASH最大值
 * 备  注：
 ******************************************************************************/
static uint8_t  readInteriorFlash(uint32_t readAddr, uint8_t *pBuffer, uint16_t numToRead)
{
    uint16_t flashSize = *(uint16_t *)(0x1FFFF7E0);                       // 读取芯片FLASH大小;本寄存器值为芯片出厂前写入的FLASH大小，只读单位：KByte

    // 判断地址有效性
    if (readAddr < STM32_FLASH_ADDR_BASE)    return 1;                    // 如果读的地址，小于FLASH的最小地址，则退出
    if (readAddr > (STM32_FLASH_ADDR_BASE + (flashSize * 1024)))  return 2; // 如果读的地址，超出FLASH的最大地址，则退出

    // 开始复制
    while (numToRead--)
    {
        *pBuffer = *(__IO uint8_t *)readAddr;
        pBuffer++;        // 指针后移一个数据长度
        readAddr++;       // 偏移一个数据位
    }

    return 0;             // 成功，返回0;
}


/******************************************************************************
 * 函　数:  writeInteriorFlash
 * 功  能： 在芯片的内部FLASH里，写入指定长度数据
 * 参  数： uint32_t  writeAddr        重要：写入的目标地址必须是偶数!!!
 *          uint8_t  *writeToBuffer
 *          uint16_t  numToWrite       重要: 写入的数量, 必须是偶数!!!因为内部FLASH每次是16位写入.如果目标数据量是单数,侧追加缓存位置的后一字节补位存入.
 *
 * 返回值：0_成功，
 *         1_失败，地址范围不正确
 *         2_失败，FLASH->SR:BSY忙超时
 *         3_失败，擦除超时
 * 备  注：
 ******************************************************************************/
static uint8_t writeInteriorFlash(uint32_t writeAddr, uint8_t *writeToBuffer, uint16_t numToWrite)
{
    uint16_t flashSize = *(uint16_t *)(0x1FFFF7E0);               // 读取芯片FLASH大小;本寄存器值为芯片出厂前写入的FLASH大小，只读，单位：KByte

    uint32_t addrOff    = writeAddr - STM32_FLASH_ADDR_BASE;      // 去掉0x08000000后的实际偏移地址
    uint32_t secPos     = addrOff / STM32_FLASH_SECTOR_SIZE;;     // 扇区地址,即起始地址在第几个扇区
    uint16_t secOff     = addrOff % STM32_FLASH_SECTOR_SIZE ;     // 开始地始偏移字节数: 数据在扇区的第几字节存放
    uint16_t secRemain  = STM32_FLASH_SECTOR_SIZE - secOff;       // 本扇区需要写入的字节数 ,用于判断够不够存放余下的数据

    // 判断地址有效性
    if (writeAddr < STM32_FLASH_ADDR_BASE)    return 1;                    // 如果读的地址，小于FLASH的最小地址，则退出，返回1_地址失败
    if (writeAddr > (STM32_FLASH_ADDR_BASE + (flashSize * 1024)))    return 1; // 如果读的地址，超出FLASH的最大地址，则退出, 返回1_地址失败

    // 0_解锁FLASH
    FLASH->KEYR = ((uint32_t)0x45670123);
    FLASH->KEYR = ((uint32_t)0xCDEF89AB);

    if (numToWrite <= secRemain)    secRemain = numToWrite;
    while (1)
    {
        // 1_读取当前页的数据
        if (waitForFlashBSY(0x00888888))   return 2;                        // 失败，返回:2, 失败原因：FLASH->SR:BSY忙超时
        readInteriorFlash(secPos * STM32_FLASH_SECTOR_SIZE + STM32_FLASH_ADDR_BASE, sectorbufferTemp, STM32_FLASH_SECTOR_SIZE);   // 读取扇区内容到缓存

        // 2_擦险指定页(扇区)
        if (waitForFlashBSY(0x00888888))   return 2;                        // 失败，返回:2, 失败原因：FLASH->SR:BSY忙超时
        FLASH->CR |= 1 << 1;                                                // PER:选择页擦除；位2MER为全擦除
        FLASH->AR = STM32_FLASH_ADDR_BASE + secPos * STM32_FLASH_SECTOR_SIZE; // 填写要擦除的页地址
        FLASH->CR |= 0x40;                                                  // STRT:写1时触发一次擦除运作　
        if (waitForFlashBSY(0x00888888))   return 2;                        // 失败，返回:３, 失败原因：擦除超时
        FLASH->CR &= ((uint32_t)0x00001FFD);                                // 关闭页擦除功能

        for (uint16_t i = 0; i < secRemain ; i++)                           // 原始数据写入缓存
            sectorbufferTemp[secOff + i] = writeToBuffer[i];

        for (uint16_t i = 0; i < STM32_FLASH_SECTOR_SIZE / 2 ; i++)         // 缓存数据写入芯片FLASH
        {
            if (waitForFlashBSY(0x00888888))   return 2;                    // 失败，返回:2, 失败原因：FLASH->SR:BSY忙超时
            FLASH->CR |= 0x01 << 0;                                         // PG: 编程
            *(uint16_t *)(STM32_FLASH_ADDR_BASE + secPos * STM32_FLASH_SECTOR_SIZE + i * 2) = (sectorbufferTemp[i * 2 + 1] << 8) | sectorbufferTemp[i * 2] ; // 缓存数据写入设备

            if (waitForFlashBSY(0x00888888))   return 2;                    // 失败，返回:2, 失败原因：FLASH->SR:BSY忙超时
            FLASH->CR &= ((uint32_t)0x00001FFE) ;                           // 关闭编程
        }

        if (secRemain == numToWrite)
        {
            break;                                                          // 已全部写入
        }
        else                                                                // 未写完
        {
            writeToBuffer += secRemain ;                                    // 原始数据指针偏移
            secPos ++;                                                      // 新扇区
            secOff = 0;                                                     // 新偏移位,扇区内数据起始地址
            numToWrite -= secRemain ;                                       // 剩余未写字节数
            secRemain = (numToWrite > STM32_FLASH_SECTOR_SIZE) ? (STM32_FLASH_SECTOR_SIZE) : numToWrite; // 计算新扇区写入字节数
        }
    }
    FLASH->CR |= 1 << 7 ;                                                   // LOCK:重新上锁

    return 0;
}



// 本地US粗略延时函数，减少移植时对外部文件依赖；
static void delayUS(__IO uint32_t us)
{
    for (uint32_t i = 0; i < us; i++)
    {
        uint8_t uc = 12;     //设置值为12，大约延1微秒
        while (uc --);       //延1微秒
    }
}



// 写入命令字
// Cmd ：0x90_通道Y+的选择控制字, 0xd0_通道X+的选择控制字
// XPT2046时序要求：CLK闲时低电平，上升沿采样数据，下降沿改变数据
// 注意：发送命令字后，返回的数据，与命令字，是同一组数据内(CS低、高电平期间)
static void sendCMD(uint8_t cmd)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        ((cmd >> (7 - i)) & 0x01) ? MOSI_1 : MOSI_0; // 高位先行
        delayUS(1);
        CLK_HIGH  ;
        delayUS(1);
        CLK_LOW   ;
    }
}



// 读取返回数据(紧跟sendCMD函数后面);
// 返回读取到的数据
static uint16_t receiveData(void)
{
    uint16_t usBuf = 0;

    CLK_HIGH;              // 给一个时钟，清除BUSY；这个时序是跟在发送命令字后面的，AD转换需要大约6US
    delayUS(5);
    CLK_LOW ;
    delayUS(5);

    for (uint8_t i = 0; i < 12; i++)
    {
        usBuf = usBuf << 1;
        CLK_HIGH;
        delayUS(1);
        usBuf |= MISO ;    // 高位先行
        CLK_LOW;
        delayUS(1);
    }

    return usBuf;
}



// 选择一个模拟通道后，启动ADC，并返回ADC采样结果
// 0x90 :通道Y+的选择控制字
// 0xd0 :通道X+的选择控制字
static int16_t readADC_X(void)
{
    if (xXPT2046.dir == 0)    sendCMD(XPT2046_CHANNEL_Y);
    if (xXPT2046.dir == 1)    sendCMD(XPT2046_CHANNEL_X);
    return  receiveData();
}



static int16_t readADC_Y(void)
{
    if (xXPT2046.dir == 0)    sendCMD(XPT2046_CHANNEL_X);
    if (xXPT2046.dir == 1)    sendCMD(XPT2046_CHANNEL_Y);
    return  receiveData();
}



/******************************************************************************
 * 函数名： readAdcXY
 * 功  能： 获取触摸屏按下时X、Y的ADC值，并滤波
 * 参  数：
 * 返  回： 1  获取成功，已存放到传入的结构体中
 *          0  失败
 ******************************************************************************/
static uint8_t readAdcXY()
{
    uint8_t cnt = 0, i;
    uint16_t xSum = 0, ySum = 0;
    int16_t xyArray [2] [10] = {{0}, {0}};    // 临时二维数组，用于存放坐标X、Y的10次采样
    int32_t xMin, xMax, yMin, yMax;           // 存储采样中的最小值、最大值;　采样多次后，去头去尾求平均值

    while ((IRQ_READ == 0) && (cnt < 10))     // 循环采样10次; 条件：TP_INT_IN信号为低(屏幕初按下), 且 cnt<10
    {
        xyArray[0] [cnt] = readADC_X();
        xyArray[1] [cnt] = readADC_Y();
        cnt ++;
    }

    // 开始求平均值
    if (cnt == 10)
    {
        // 筛选等会要去掉的最小值、最大值
        xMax = xMin = xyArray [0] [0];
        yMax = yMin = xyArray [1] [0];
        for (i = 1; i < 10; i++)
        {
            if (xyArray[0] [i] < xMin)    xMin = xyArray [0] [i];   // 求x的10次采样最小ADC值
            if (xyArray[0] [i] > xMax)    xMax = xyArray [0] [i];   // 求x的10次采样最大ADC值

            if (xyArray[1] [i] < yMin)    yMin = xyArray [1] [i];   // 求y的10次采样最小ADC值
            if (xyArray[1] [i] > yMax)    yMax = xyArray [1] [i];   // 求y的10次采样最小ADC值
        }
        // 去除最小值和最大值之后求平均值
        for (i = 0; i < 10; i++)
        {
            xSum = xSum + xyArray[0][i];
            ySum = ySum + xyArray[1][i];
        }

        xXPT2046.adcX = (xSum - xMin - xMax) >> 3;
        xXPT2046.adcY = (ySum - yMin - yMax) >> 3;

        return 1;
    }

    return 0;
}



// 把电压值换算成对应的LCD坐标值
static void adcXYToLcdXY(void)
{
    static int16_t lcdX = 0;
    static int16_t lcdY = 0;
    // 计算比例系数
    lcdX = xXPT2046.adcX * xXPT2046.xfac + xXPT2046.xoff ;
    lcdY = xXPT2046.adcY * xXPT2046.yfac + xXPT2046.yoff ;
    // 限制坐标值范围
    // if(lcdX<0)  lcdX=0;
    if (lcdX > xXPT2046.lcdWidth)  lcdX = xXPT2046.lcdWidth;
    if (lcdY < 0)  lcdY = 0;
    if (lcdY > xXPT2046.lcdHeight)  lcdY = xXPT2046.lcdHeight;
    // 经换算, 及限值后的坐标值, 转存到结构体, 随时可调用
    xXPT2046.lcdX = lcdX;
    xXPT2046.lcdY = lcdY;
}


/******触摸状态机相关******/
typedef enum
{
    _STATUS_0_FREE  = 0,                   // 触摸释放
    _STATUS_2_WAITING,                     // 等待稳定
    _STATUS_3_PRESSED,                     // 触摸按下
} eflagTouchStatus;

/******************************************************************************
 * 函数名： touchDetect
 * 功  能： 触摸屏检测状态机
 * 参  数：
 * 返  回： 1   触摸释放中
 *          0   触摸按下中
 ******************************************************************************/
static uint8_t touchDetect(void)
{
    static eflagTouchStatus  eTouchStatus = _STATUS_0_FREE;  //  触摸释放 XPT2046_STATE_FREE=0， 等待稳定 XPT2046_STATE_WAITING=1， 触摸按下 XPT2046_STATE_PRESSED=2
    static uint32_t timeCNT = 1;

    switch (eTouchStatus)
    {
    case _STATUS_0_FREE:
        if (IRQ_READ  == 0)                             // 第一次出现触摸低电平信号
            eTouchStatus = _STATUS_2_WAITING;           //
        else
            eTouchStatus = _STATUS_0_FREE;              // 无触摸
        break;

    case _STATUS_2_WAITING:
        if (IRQ_READ  == 0)
        {
            timeCNT++;
            //等待时间大于阈值则认为触摸被按下
            //消抖时间 = DURIATION_TIME * 本函数被调用的时间间隔
            //如在定时器中调用，每10ms调用一次，则消抖时间为：DURIATION_TIME*10ms
            if (timeCNT > DURIATION_TIME)
            {
                timeCNT = 1;
                eTouchStatus = _STATUS_3_PRESSED;
            }
            else                                        // 等待时间累加
                eTouchStatus = _STATUS_2_WAITING;
        }
        else                                            // 等待时间值未达到阈值就为无效电平，当成抖动处理
        {
            timeCNT = 1;
            eTouchStatus = _STATUS_0_FREE;
        }
        break;

    case _STATUS_3_PRESSED:
        if (IRQ_READ  == 0)                             // 触摸持续按下
            eTouchStatus = _STATUS_3_PRESSED;
        else                                            // 触摸释放
            eTouchStatus = _STATUS_0_FREE;
        break;

    default:
        eTouchStatus = _STATUS_0_FREE;
        break;
    }

    // 当触摸屏按下并稳定后，返回0, 否则返回1
    return (eTouchStatus == _STATUS_3_PRESSED) ? 0 : 1 ;
}





// 上面的是本地函数
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 下面的是全局函数





/******************************************************************************
 * 函数名： XPT2046_Init
 * 功  能： 初始化
 * 参  数： uint16_t lcdWidth     LCD宽度像素
 *          uint16_t lcdHeight    LCD宽度像素
 *          uint8_t dir           显示方向    0-正竖屏，3-倒竖屏，5-正横屏, 6-倒横屏
 * 返  回：
 ******************************************************************************/
void XPT2046_Init(uint16_t lcdWidth, uint16_t lcdHeight, uint8_t dir)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    uint8_t flashDATA[50] = {0};
    uint32_t flashSize = *(uint16_t *)(0x1FFFF7E0);           // 读取芯片FLASH大小;本寄存器值为芯片出厂前写入的FLASH大小，只读单位：KByte
    xXPT2046.dataAddr  = 0x08000000 + (flashSize - 1) * 1024; // 校准数据的存放位置: 内部FLASH的最后1K的开头
    xXPT2046.lcdWidth  = lcdWidth;
    xXPT2046.lcdHeight = lcdHeight;
    xXPT2046.dir = dir;

    // 开启GPIO时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN | RCC_APB2ENR_IOPFEN ;

    // 模拟SPI GPIO初始化
    GPIO_InitStructure.GPIO_Pin   = XPT2046_CS_PIN;      // CS
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(XPT2046_CS_GPIO, &GPIO_InitStructure);

    CS_HIGH;                                             // 拉高片选，防止误操作

    GPIO_InitStructure.GPIO_Pin   = XPT2046_CLK_PIN;     // CLK
    GPIO_Init(XPT2046_CLK_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = XPT2046_MOSI_PIN;    // MOSI
    GPIO_Init(XPT2046_MOSI_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = XPT2046_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;       // MISO, 上拉输入
    GPIO_Init(XPT2046_MISO_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = XPT2046_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        // 触摸屏触摸信号指示引脚，不使用中断
    GPIO_Init(XPT2046_IRQ_GPIO, &GPIO_InitStructure);

    CLK_LOW ;                                            // XPT2046时序要求：CLK 闲时低电平，上升沿采样数据，下降沿改变数据
    MOSI_0;                                              // XPT2046时序要求：MOSI闲时低电平
    CS_LOW;                                              // 拉低片选，使XTP2046开始通信

    // 通过读取旧数据, 检查是否已校准
    readInteriorFlash(xXPT2046.dataAddr, flashDATA, 20);
    if ((flashDATA[0] == 'O') && (flashDATA[1] == 'K'))  // 存有已校准的数据
    {
        xXPT2046.xfac = *(float *)(flashDATA + 2);
        xXPT2046.xoff = *(short *)(flashDATA + 6);
        xXPT2046.yfac = *(float *)(flashDATA + 10);
        xXPT2046.yoff = *(short *)(flashDATA + 14);
    }
    else                                                 // 没有校准数据
    {
        XPT2046_ReCalibration();                         // 重新校准
    }
}


/******************************************************************************
 * 函数名： XPT2046_ReCalibration
 * 功  能： 重新校准触摸屏,
 *          并把校准的数据存入内部FLASH, 以方便下次调用
 * 参  数： 取消原来的参数输入, 直接在全局结构体中获得:xXPT2046
 *          uint16_t lcdWidth     LCD宽度像素
 *          uint16_t lcdHeight    LCD宽度像素
 *          uint8_t dir           显示方向    0-正竖屏，3-倒竖屏，5-正横屏, 6-倒横屏
 * 返  回： 0_校准成功
 *          1_校准失败
 ******************************************************************************/
uint8_t  XPT2046_ReCalibration(void)
{
    uint16_t pixelOff = 30;   // 偏移像素,用来画十字
    uint16_t adcX1, adcX2, adcX3, adcX4, adcY1, adcY2, adcY3, adcY4; // 记录校准过程中的坐标值
    float xfac = 0;
    float yfac = 0;
    short xoff = 0;
    short yoff = 0;
    uint16_t crossX = 0;      // 用于画十字线
    uint16_t crossY = 0;      // 用于画十字线
    char strTemp[30];
    uint16_t lcdWidth  = xXPT2046.lcdWidth;
    uint16_t lcdHeight = xXPT2046.lcdHeight;

    printf("\r\n触摸屏校准中....\r\n");
    LCD_Fill(0, 0, lcdWidth, lcdHeight, BLACK);
    LCD_String(45, 110, "ReCalibration......", 16, WHITE, BLACK);
    LCD_String(52, 130, "Click the Cross!!", 16, WHITE, BLACK);


    // 左上角
    crossX = pixelOff;
    crossY = pixelOff;
    xXPT2046.adcX = 0;
    xXPT2046.adcY = 0;
    XPT2046_Cmd(ENABLE);                                              // 打开触屏检测(代码)
    LCD_Cross(crossX, crossY, 20, YELLOW);                            // 画十字
    while ((readAdcXY() == 0) || (xXPT2046.adcX == 0));               // 等待按下
    XPT2046_Cmd(DISABLE);                                             // 关闭触屏检测(代码)
    adcX1 = xXPT2046 .adcX;                                           // 记录下取得的adc值
    adcY1 = xXPT2046.adcY;                                            // 记录下取得的adc值
    LCD_Cross(crossX, crossY, 20, BLACK);                             // 抹去十字
    sprintf(strTemp, "X:%d", adcX1);
    LCD_String(crossX - 12, crossY - 16, strTemp, 12, YELLOW, BLACK); // 提示
    sprintf(strTemp, "Y:%d", adcY1);
    LCD_String(crossX - 12, crossY, strTemp, 12, YELLOW, BLACK);      // 提示
    delayUS(400000);

    // 右上角
    crossX = lcdWidth - pixelOff;
    crossY = pixelOff;
    xXPT2046.adcX = 0;
    xXPT2046.adcY = 0;
    XPT2046_Cmd(ENABLE);                                              // 打开触屏检测(代码)
    LCD_Cross(crossX, crossY, 20, YELLOW);                            // 画十字
    while ((readAdcXY() == 0) || (xXPT2046.adcX == 0));               // 等待按下
    XPT2046_Cmd(DISABLE);                                             // 关闭触屏检测(代码)
    adcX2 = xXPT2046 .adcX;                                           // 记录下取得的adc值
    adcY2 = xXPT2046.adcY;                                            // 记录下取得的adc值
    LCD_Cross(crossX, crossY, 20, BLACK);                             // 抹去十字
    sprintf(strTemp, "X:%d", adcX2);
    LCD_String(crossX - 12, crossY - 16, strTemp, 12, YELLOW, BLACK); // 提示
    sprintf(strTemp, "Y:%d", adcY2);
    LCD_String(crossX - 12, crossY, strTemp, 12, YELLOW, BLACK);      // 提示
    delayUS(400000);

    // 左下角
    crossX = pixelOff;
    crossY = lcdHeight - pixelOff;
    xXPT2046.adcX = 0;
    xXPT2046.adcY = 0;
    XPT2046_Cmd(ENABLE);                                              // 打开触屏检测(代码)
    LCD_Cross(crossX, crossY, 20, YELLOW);                            // 画十字
    while ((readAdcXY() == 0) || (xXPT2046.adcX == 0));               // 等待按下
    XPT2046_Cmd(DISABLE);                                             // 关闭触屏检测(代码)
    adcX3 = xXPT2046 .adcX;                                           // 记录下取得的adc值
    adcY3 = xXPT2046.adcY;                                            // 记录下取得的adc值
    LCD_Cross(crossX, crossY, 20, BLACK);                             // 抹去十字
    sprintf(strTemp, "X:%d", adcX3);
    LCD_String(crossX - 12, crossY - 16, strTemp, 12, YELLOW, BLACK); // 提示
    sprintf(strTemp, "Y:%d", adcY3);
    LCD_String(crossX - 12, crossY, strTemp, 12, YELLOW, BLACK);      // 提示
    delayUS(400000);

    // 右下角
    crossX = lcdWidth - pixelOff;
    crossY = lcdHeight - pixelOff;
    xXPT2046.adcX = 0;
    xXPT2046.adcY = 0;
    XPT2046_Cmd(ENABLE);                                              // 打开触屏检测(代码)
    LCD_Cross(crossX, crossY, 20, YELLOW);                            // 画十字
    while ((readAdcXY() == 0) || (xXPT2046.adcX == 0));               // 等待按下
    XPT2046_Cmd(DISABLE);                                             // 关闭触屏检测(代码)
    adcX4 = xXPT2046 .adcX;                                           // 记录下取得的adc值
    adcY4 = xXPT2046.adcY;                                            // 记录下取得的adc值
    LCD_Cross(crossX, crossY, 20, BLACK);                             // 抹去十字
    sprintf(strTemp, "X:%d", adcX4);
    LCD_String(crossX - 12, crossY - 16, strTemp, 12, YELLOW, BLACK); // 提示
    sprintf(strTemp, "Y:%d", adcY4);
    LCD_String(crossX - 12, crossY, strTemp, 12, YELLOW, BLACK);      // 提示
    delayUS(400000);

    //timeCNT=0;
    // 取adcX和adcY的平均值; 如果不取平均值, 在对角画两个十字线即可
    adcX1 = (adcX1 + adcX3) / 2;
    adcX2 = (adcX2 + adcX4) / 2;

    adcY1 = (adcY1 + adcY2) / 2;
    adcY2 = (adcY3 + adcY4) / 2;

    xfac = (float)(pixelOff - (lcdWidth - pixelOff)) / (adcX1 - adcX2); // 触摸屏与LCD的坐标比例系数,  xfac=(float)(20-320)/(t1x-t2x);
    yfac = (float)(pixelOff - (lcdHeight - pixelOff)) / (adcY1 - adcY2);
    xoff = (lcdWidth - xfac * (adcX1 + adcX2)) / 2;   // 像素点偏移值, xoff=(320-xfac*(t1x+t2x))/2;
    yoff = (lcdHeight - yfac * (adcY1 + adcY2)) / 2;

    // 存入内部FLASH中
    uint8_t err = 0;
    uint16_t flag = 'O' | ('K' << 8);
    err |= writeInteriorFlash(xXPT2046.dataAddr, (uint8_t *)&flag, 2);
    err |= writeInteriorFlash(xXPT2046.dataAddr + 2, (uint8_t *)&xfac, 4);
    err |= writeInteriorFlash(xXPT2046.dataAddr + 6, (uint8_t *)&xoff, 4);
    err |= writeInteriorFlash(xXPT2046.dataAddr + 10, (uint8_t *)&yfac, 4);
    err |= writeInteriorFlash(xXPT2046.dataAddr + 14, (uint8_t *)&yoff, 4);

    if (err > 0)
    {
        printf(">>>校准完成, 但保存参数时失败!\r\n");
        return err;
    }

    xXPT2046.xfac = xfac;
    xXPT2046.xoff = xoff;
    xXPT2046.yfac = yfac;
    xXPT2046.yoff = yoff;

    printf(">>>校准完成! 比例系数、偏移值已存入内部FLASH, 地址:0x%X\r\n", xXPT2046.dataAddr);

    SCB->AIRCR = 0X05FA0000 | (uint32_t)0x04; // 重新上电
    return 0;
}



/******************************************************************************
 * 函数名： XPT2046_Cmd
 * 功  能： 触摸检测开关
 *          由于检测比较耗时, 在不使用触屏的状态下, 可以关闭检测以节省芯片资源
 *          此开关状态, 只作用于XPT2046_TouchHandler();
 * 参  数： 0_关闭触摸屏的检测,以节省资源
 *          1_打开触摸检测
 * 返  回：
 ******************************************************************************/
void XPT2046_Cmd(uint8_t status)
{
    if (status != 0)
    {
        xXPT2046 .EN = 1;
    }
    else
    {
        xXPT2046.EN = 0;
    }
}



/******************************************************************************
 * 函数名： XPT2046_GetXY
 * 功  能： 获取当前按下位置的坐标值
 * 参  数： uint16_t* X   x存入变量
 *          uint16_t* Y   y存入变量
 * 返  回： 无
 ******************************************************************************/
void  XPT2046_GetXY(uint16_t *X, uint16_t *Y)
{
    *X = xXPT2046.lcdX;
    *Y = xXPT2046.lcdY;
}



/******************************************************************************
 * 函数名： XPT2046_TouchHandler
 * 功  能： 触摸中断处理函数
 *          本函数需要在while循环里被调用，也可使用定时器定时调用, 调用间隔1ms最合适
 * 参  数： 无
 * 返  回： 无
 ******************************************************************************/
void XPT2046_TouchHandler(void)
{
    // 如果允许触摸检测(0_关闭, 1_打开), 且触摸屏已按下(0_按下且已稳定，1_未按下)
    if ((xXPT2046.EN == 1) && (touchDetect() == 0))
    {
        // 获取按下时的位置电压值
        if (readAdcXY() == 0)
            return ;
        adcXYToLcdXY();

        // 这里调用屏幕按下的函数(用户自己的功能定义)
        XPT2046_TouchDown();
    }
    else
    {
        // 这里调用屏幕没有按下的函数(用户自己的功能定义)
        XPT2046_TouchUp();
    }
}



/******************************************************************************
 * 函数名： XPT2046_TouchDown
 * 功  能： 触摸按下时的处理
 *          空白函数, 用户自行编写代码
 * 参  数：
 *
 * 返  回：
 ******************************************************************************/
void XPT2046_TouchDown(void)
{
    // 示例如何获取触摸时的LCD坐标
    //sprintf(strTemp, "%4d,%4d", xXPT2046.lcdX, xXPT2046.lcdY);
    //LCD_String(180,290,strTemp, 12, BLUE , WHITE);              // 在LCD上显示触摸坐标

    // 示例在触摸点画一个颜色点
    LCD_DrawPoint(xXPT2046.lcdX, xXPT2046.lcdY, YELLOW);
}



/******************************************************************************
 * 函数名： XPT2046_TouchUp
 * 功  能： 触摸按下时的处理
 *          空白函数, 用户自行编写代码
 * 参  数：
 *
 * 返  回：
 ******************************************************************************/
void XPT2046_TouchUp(void)
{


}




