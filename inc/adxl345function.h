/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：adxl345function.c                                              **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现adxl345 3轴数字加速度计的操作，采用I2C或SPI通讯        **/
/**           CS引脚上拉至VDD，I2C模式使能。                                 **/
/**           SPI模式下， CS引脚由总线主机控制。                             **/
/**           采用SPI通讯接口时，最大SPI时钟速度为5 MHz，时序方案按照时钟极  **/
/**           性(CPOL)= 1、时钟相位(CPHA)= 1执行。                           **/
/**           采用I2C通讯接口时，ALT ADDRESS引脚处于高电平，器件的7位I2C地址 **/
/**           是0x1D，随后为R/W位。这转化为0x3A写入，0x3B读取。              **/
/**           通过ALT ADDRESS引脚(引脚12)接地，可以选择备用I2C地址0x53(随    **/
/**           后为R/W位)。这转化为0xA6写入，0xA7读取。根据应用实际配置       **/
/**           寄存器地址分配如下：                                           **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |十六进制|十进制|     名称     |类型| 复位值 | 描述                   | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x00  |   0  |    DEVID     | R  |11100101|器件ID                  | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x1D  |  29  |  THRESH_TAP  | R/W|00000000|敲击阈值                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x1E  |  30  |     OFSX     | R/W|00000000|X轴偏移                 | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x1F  |  31  |     OFSY     | R/W|00000000|Y轴偏移                 | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x20  |  32  |     OFSZ     | R/W|00000000|Z轴偏移                 | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x21  |  33  |     DUR      | R/W|00000000|敲击持续时间            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x22  |  34  |    Latent    | R/W|00000000|敲击延迟                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x23  |  35  |    Window    | R/W|00000000|敲击窗口                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x24  |  36  |  THRESH_ACT  | R/W|00000000|活动阈值                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x25  |  37  | THRESH_INACT | R/W|00000000|静止阈值                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x26  |  38  |  TIME_INACT  | R/W|00000000|静止时间                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x27  |  39  | ACT_INACT_CTL| R/W|00000000|轴使能控制活动和静止检测| **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x28  |  40  |   THRESH_FF  | R/W|00000000|自由落体阈值            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x29  |  41  |   TIME_FF    | R/W|00000000|自由落体时间            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2A  |  42  |   TAP_AXES   | R/W|00000000|单击/双击轴控制         | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2B  |  43  |ACT_TAP_STATUS| R  |00000000|单击/双击源             | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2C  |  44  |   BW_RATE    | R/W|00001010|数据速率及功率模式控制  | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2D  |  45  |  POWER_CTL   | R/W|00000000|省电特性控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2E  |  46  |  INT_ENABLE  | R/W|00000000|中断使能控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2F  |  47  |   INT_MAP    | R/W|00000000|中断映射控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x30  |  48  |   INT_SOURCE | R  |00000010|中断源                  | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x31  |  49  | DATA_FORMAT  | R/W|00000000|数据格式控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x32  |  50  |    DATAX0    | R  |00000000|X轴数据0                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x33  |  51  |    DATAX1    | R  |00000000|X轴数据1                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x34  |  52  |    DATAY0    | R  |00000000|Y轴数据0                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x35  |  53  |    DATAY1    | R  |00000000|Y轴数据1                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x36  |  54  |    DATAZ0    | R  |00000000|Z轴数据0                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x37  |  55  |    DATAZ1    | R  |00000000|Z轴数据1                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x38  |  56  |   FIFO_CTL   | R/W|00000000|FIFO控制                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x39  |  57  |  FIFO_STATUS | R  |00000000|FIFO状态                | **/
/**  +--------+------+-------------------+--------+------------------------+ **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.1.0  2019-08-07          木南              修改为基于对象的操作   **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __ADXL345FUNCTION_H
#define __ADXL345FUNCTION_H

#include "stdint.h"

/*寄存器地址定义*/
#define REG_DEVID               0x00    //器件ID
#define REG_THRESH_TAP          0x1D    //敲击阈值
#define REG_OFSX                0x1E    //X轴偏移
#define REG_OFSY                0x1F    //Y轴偏移
#define REG_OFSZ                0x20    //Z轴偏移
#define REG_DUR                 0x21    //敲击持续时间
#define REG_Latent              0x22    //敲击延迟
#define REG_Window              0x23    //敲击窗口
#define REG_THRESH_ACT          0x24    //活动阈值
#define THRESH_INACT            0x25    //静止阈值
#define REG_TIME_INACT          0x26    //静止时间
#define REG_ACT_INACT_CTL       0x27    //轴使能控制活动和静止检测
#define REG_THRESH_FF           0x28    //自由落体阈值
#define REG_TIME_FF             0x29    //自由落体时间
#define REG_TAP_AXES            0x2A    //单击/双击轴控制
#define REG_ACT_TAP_STATUS      0x2B    //单击/双击源
#define REG_BW_RATE             0x2C    //数据速率及功率模式控制
#define REG_POWER_CTL           0x2D    //省电特性控制
#define REG_INT_ENABLE          0x2E    //中断使能控制
#define REG_INT_MAP             0x2F    //中断映射控制
#define REG_INT_SOURCE          0x30    //中断源
#define REG_DATA_FORMAT         0x31    //数据格式控制
#define REG_DATAX0              0x32    //X轴数据0
#define REG_DATAX1              0x33    //X轴数据1
#define REG_DATAY0              0x34    //Y轴数据0
#define REG_DATAY1              0x35    //Y轴数据1
#define REG_DATAZ0              0x36    //Z轴数据0
#define REG_DATAZ1              0x37    //Z轴数据1
#define REG_FIFO_CTL            0x38    //FIFO控制
#define REG_FIFO_STATUS         0x39    //FIFO状态

/* 定义片选信号枚举 */
typedef enum Adxl345CS
{
  ADXL345CS_Enable,
  ADXL345CS_Disable
}Adxl345CSType;

/* 定义BMP280通讯接口枚举 */
typedef enum Adxl345Port {
  ADXL345_I2C,
  ADXL345_SPI
}Adxl345PortType;

/*定义ADXL345三轴数据结构*/
typedef struct Adxl345Object {
  uint8_t devAddress;
  uint8_t devID;
  Adxl345PortType port;
  int16_t incidence_X;
  int16_t incidence_Y;
  int16_t incidence_Z;
	
  float incidence_Xf;
  float incidence_Yf;
  float incidence_Zf;
	
  void (*ReadBytes)(struct Adxl345Object *adxl,uint8_t regAdd,uint8_t *rData,uint16_t rSize);	//读ADXL345寄存器操作
  void (*WriteBytes)(struct Adxl345Object *adxl,uint8_t regAdd,uint8_t *wData,uint16_t wSize);//写ADXL345寄存器操作
  void (*ChipSelect)(Adxl345CSType en);	//使用SPI接口时，片选操作
  void (*Delayus)(volatile uint32_t nTime);      //实现us延时操作
}Adxl345ObjectType;

typedef void (*Adxl345ReadBytesType)(struct Adxl345Object *adxl,uint8_t regAdd,uint8_t *rData,uint16_t rSize);	//读ADXL345寄存器操作
typedef void (*Adxl345WriteBytesType)(struct Adxl345Object *adxl,uint8_t regAdd,uint8_t *wData,uint16_t wSize);//写ADXL345寄存器操作
typedef void (*Adxl345ChipSelectType)(Adxl345CSType en);	//使用SPI接口时，片选操作
typedef void (*Adxl345DelayType)(volatile uint32_t nTime);      /*实现us延时操作*/

/*读取数据值，分辨率(3.9 mg/LSB)*/
void GetValueFromAdxl345(Adxl345ObjectType *adxl);

/*对ADXL345进行初始化配置*/
void Adxl345Initialization(Adxl345ObjectType *adxl,uint8_t devAdd,
                           Adxl345PortType port,
                           Adxl345ReadBytesType read,
                           Adxl345WriteBytesType write,
                           Adxl345ChipSelectType cs,
                           Adxl345DelayType delay);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
