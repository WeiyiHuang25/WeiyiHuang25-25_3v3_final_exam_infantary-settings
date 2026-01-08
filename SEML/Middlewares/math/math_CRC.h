/**
  ******************************************************************************
  * @copyright (c) 2023 - ~, Singularity
  * @file   : math_CRC.h
  * @see : https://blog.csdn.net/weixin_46672094/article/details/117136044
  * @brief  : CRC校验模块
  * @date   : 2023-7-6
  * @par Change Log：
  * <table>
  * <tr><th>Date           <th>Version     <th>Author      <th>Description
  * <tr><td>2023-8-26      <td>0.1         <td>SY7_yellow  <td>将crc代码移植过来
  * </table>
	* @todo   : 建立查表版本
  *@details :
  * ============================================================================
  *                       How to use this driver  
  * ============================================================================
  * 1. 直接调用GetCRC传入CrcInfoType_t的crc配置即可
	* 2. 非标准crc可以自己声明个CrcInfoType_t进行配置.
  ******************************************************************************
  * @attention:
  * 
  * 文件编码：UTF-8,出现乱码请勿上传! \n
  * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
  * 勿合并到master. \n
  * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
  * 不要包含太多不相关的头文件. \n
  * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
  * 够准确定位修复. \n
  * 优先兼容arm_math.h的格式，使用第三方的库的时候注意检查。\n
  * 浮点类型固定为float,如果需要使用double请调用"math.h"或使用其他第三方库。
  * (Cortex-M系列的fpu只有硬件单精度计算，双精度的依然是软件计算)\n
  * File encoding:UTF-8,Do not upload garbled code!\n
  * Please remember to change the version number. If you have not been tested, 
  * please use the suffix alpha,beta... And do not merge to master. \n
  * Defensive programming, where input arguments are checked for validity and
  * an error number is returned. \n
  * Don't include too many irrelevant headers. \n
  * If you find a bug, file an issue with a detailed description of the 
  * phenomenon (required) and conditions for reoccurrence (optional) so that 
  * the appropriate owner can locate the correct fix. \n
  * Compatibility with arm_math.h format is preferred. When using third-party 
  * libraries, pay attention to check. \n
  * The floating-point type is fixed to float, so if you need to use double call
  * "math.h" or use another third-party library. 
  * (The Cortex-M fpu only has hardware single precision calculation, the double
  * precision is still software calculation)\n
  ******************************************************************************
  */
#ifndef _MATH_CRC_H_
#define _MATH_CRC_H_
#include "main.h"
#include "math_common.h"

typedef struct
{
	uint8_t Width;					/**< 宽度，即CRC比特数 */
	uint32_t Poly;					/**< 生成多项式 */
	uint32_t CrcInit;				/**< CRC初始值 */
	uint32_t XorOut;				/**< 计算结果与此参数异或后得到最终的CRC值 */
	FunctionalState RefIn;	/**< 待测数据的每个字节是否按位反转 */
	FunctionalState RefOut; /**< 在计算后之后，异或输出之前，整个数据是否按位反转 */
} CrcInfoType_t;

extern const CrcInfoType_t CRC4_ITU;
extern const CrcInfoType_t CRC5_EPC;
extern const CrcInfoType_t CRC5_ITU;
extern const CrcInfoType_t CRC5_USB;
extern const CrcInfoType_t CRC6_ITU;
extern const CrcInfoType_t CRC7_MMC;
extern const CrcInfoType_t CRC8;
extern const CrcInfoType_t CRC8_ITU;
extern const CrcInfoType_t CRC8_ROHC;
extern const CrcInfoType_t CRC8_MAXIM;
extern const CrcInfoType_t CRC16_IBM;
extern const CrcInfoType_t CRC16_MAXIM;
extern const CrcInfoType_t CRC16_USB;
extern const CrcInfoType_t CRC16_MODBUS;
extern const CrcInfoType_t CRC16_CCITT;
extern const CrcInfoType_t CRC16_CCITT_FALSE;
extern const CrcInfoType_t CRC16_X25;
extern const CrcInfoType_t CRC16_XMODEM;
extern const CrcInfoType_t CRC16_DNP;
extern const CrcInfoType_t CRC32;
extern const CrcInfoType_t CRC32_MPEG2;

/**
 * @brief   获取 CRC(循环冗余校验) 函数
 * @param   [in] crc_info		- CRC 类型
 * @param   [in] buf				- 数据块指针
 * @param   [in] bufLen			- 数据长度
 * @return  校验码
 */
uint32_t GetCRC(const CrcInfoType_t *crc_info, uint8_t *buf, uint32_t bufLen);

#endif