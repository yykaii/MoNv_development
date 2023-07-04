#ifndef __DHT11_H
#define __DHT11_H
/***********************************************************************************************************************************
 **【购买链接】  魔女科技   https://demoboard.taobao.com
 **【更新分享】  Q群文件夹
 ***********************************************************************************************************************************
 **【文件名称】  bsp_DHT11.h
 **【功能描述】  定义引脚、定义全局结构体、声明全局函数
 **
 **【适用平台】  STM32F103 + 标准库v3.5 + keil5
 **
 **【划 重 点】  1-电压范围：3.3V~5.0V
 **              2-注意判断使用的DHT11器件，是单独的DHT11元件，还是完整的DHT11模块
 **                单独的DHT11元件：蓝色塑料主体、三个引脚，不带上拉电阻；适合低成本工程方案使用，建议测试时引脚使用推挽输出模式；
 **                完整的DTH11模块：蓝色塑料主体、三个引脚、PCB底板、上拉电阻、电源指示LED，适合方案搭建；
 **              3-本代码适合DHT11模块；为支持总线机制，代码使用了开漏模式GPIO_Mode_Out_OD(完整模块已带上拉电阻3.3V、4.7K)；
 **                当使用本代码+单独的DHT11元件时，需要把DATA通信引脚改为GPIO_Mode_Out_PP, 或者在DATA引脚接上拉电阻，两者修改其一即可；
 **              4-响应时间：上电6S后数据稳定，采集间隔建议2S以上(DHT11变化反应有点慢）
 **              5-精度误差：DHT11分旧版和新版，10元内的绝大部分是旧版，新版约12元左右；
 **                          旧版数据稳定，但只有整数部分，新版有小数部分，但数据跳动稍大；
 **                          温度0-50°C; 精度±2°C; 小数部份无效
 **                          湿度20-80%; 精度±5%; 小数部分无效
 **
 **
 **【文件移植】  步骤1-复制文件：可复制bsp_DHT11.c和bsp_DHT11h两个文件，或复制DHT11文件夹，保存到所需工程目录文件夹下；
 **              步骤2-添加文件：在keil工各程左侧文件管理器中，双击某文件夹，以添加bsp_DHT11.c文件;
 **              步骤3-添加路径：点击魔术棒工具按钮，在“c/c++"选项页中，点击”Include Path"后面的按键，以添加文件存放所在路径(是文件夹，不是文件);
 **　　　　　    步骤4-添加引脚：在所需DHT11功能的代码文件头部，添加：#include "bsp_DHT11.h";
 **
 **
 **【函数使用】  函数1-DHT11_Init(GPIOx, GPIO_Pin_x); // 初始化DHT11所用的芯片引脚
 **              函数2-DHT11_GetData(void);           // 获取数据, 获取的数据存放于结构体xDHT11中，具体使用方法，可参考示例代码
 **
 **
 **【更新记录】  2021-12-23  修改初始化函数，使文件移植更方便: DHT11_Init(GPIOx, GPIO_Pin_x);
 **              2021-10-26  修改c和h文件文件格式
 **              2021-10-26  修改DHT11_GetData()函数
 **              2021-05-20  创建文件
 **
***********************************************************************************************************************************/
#include <stm32f10x.h>
#include <stdio.h>



/*****************************************************************************
 ** 声明 全局变量
****************************************************************************/
// 声明本地变量
typedef struct
{
    GPIO_TypeDef  *GPIOx;            // 所用的GPIO端口
    uint32_t       Pinx;             // 所用的Pin引脚
    float          Temperature;      // 温度值：在调用DHT11_GetTemp()函数后，获取到的温度值；
    float          Humidity;         // 湿度值：在调用DHT11_GetTemp()函数后，获取到的温度值；
} xDHT11_TypeDef;
extern xDHT11_TypeDef  xDHT11;       // 声明全局结构体, 用于记录信息



/*****************************************************************************
 ** 声明  全局函数
****************************************************************************/
void     DHT11_Init(GPIO_TypeDef *GPIOx, uint32_t PINx);           // 初始化DHT11, 返回值：0-失败，1-正常
uint8_t  DHT11_GetData(void);                                      // 读取DHT11数据, 温度值保存到xDHT11.temperature, 湿度值保存到xDHT11.humidity
float    DHT11_GetTemperature(GPIO_TypeDef *GPIOx, uint32_t PINx); // 获取DHT11的温度数据; 本函数为方便新手小白使用而写，老司机直接调用DHT11_Init()和DHT11_GetData()效率更高
float    DHT11_GetHumidity(GPIO_TypeDef *GPIOx, uint32_t PINx);    // 获取DHT11的湿度数据; 本函数为方便新手小白使用而写，老司机直接调用DHT11_Init()和DHT11_GetData()效率更高

#endif

