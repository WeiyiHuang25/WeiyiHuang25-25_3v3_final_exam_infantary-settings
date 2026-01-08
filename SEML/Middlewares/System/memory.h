/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : memory.h
 * @author : SY7_yellow
 * @brief  : 内存管理模块
 * @note   : 在RT-Thread的内存管理模块基础上修改而来
 * @see    : https://blog.csdn.net/fhqlongteng/article/details/104057299
 * @see    : https://www.rt-thread.org/document/api/group___m_m.htmls
 * @date   : 2023-9-12
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-09-12     <td>1.0         <td>SY7_yellow  <td>实现malloc,free,mem_realloc
 * <tr><td>2023-09-20     <td>1.1         <td>SY7_yellow  <td>使其能够支持其他堆的使用
 * </table>
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 *
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
 * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 * In the case of simultaneous editing by multiple threads, use a mutex to
 * prevent one process from being accessed by other processes while it is
 * editing. \n
 ******************************************************************************
 */
#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "SEML_common.h"
/**
 * @addtogroup 内存管理模块
 * @{
*/
/**
 * @addtogroup 小内存管理模块
 * @{
*/

/// @brief 最小分配尺寸
#define MEM_MIN_SIZE 4
/// @brief 内存对齐尺寸
#define MEM_ALIGN_SIZE 4
/// @brief 使用内存对齐
#define USE_MEM_ALIGN 1
/// @brief 内存大小变量定义
typedef uint16_t mem_size_t;

/**
 * @brief 内存数据复制
 * @param[out] des 目标地址
 * @param[in] src 源地址
 * @param[in] n 复制字节数
 */
void mem_copy(void *des, const void *src, mem_size_t n);

/**
 * @brief 内存数据填充
 * @param[out] des 目标地址
 * @param[in] c 填充字符
 * @param[in] n 填充字节数
 */
void mem_set(void *des, const uint8_t c, mem_size_t n);

/**
 * @brief 初始化内存管理
 * @param[in,out] buffer 缓存数组
 * @param[in] size 缓存数组字节数
 * @return 初始化后的堆句柄
 */
uint32_t mem_init(void *buffer, mem_size_t size);

/**
 * @brief 申请内存
 * @param[in] size 申请的字节数
 * @return 申请得到的内存指针,申请失败返回NULL
 */
void *mem_malloc(mem_size_t size);

/**
 * @brief 重新申请内存
 * @param[in] ptr 要重新申请内存指针
 * @param[in] size 重新申请的内存大小
 * @return 申请得到的内存指针,申请失败或者被释放返回NULL
 */
void *mem_realloc(void *ptr, mem_size_t size);

/**
 * @brief 释放内存
 * @param[in] ptr 要释放的指针
 */
void mem_free(void *ptr);

/**
 * @brief 初始化内存管理
 * @param[in,out] buffer 缓存数组
 * @param[in] size 缓存数组字节数
 * @return 初始化后的堆句柄
 */
uint32_t _mem_init(void *buffer, mem_size_t size);

/**
 * @brief 裁切空闲内存块
 * @param[in] hander 堆句柄
 * @param[in] ptr 内存管理节点所在偏移量
 * @param[in] size 分配尺寸
 */
static void _mem_crop_free(uint32_t hander, mem_size_t ptr, mem_size_t size);

/**
 * @brief 申请内存
 * @param[in] hander 堆句柄
 * @param[in] size 申请的字节数
 * @return 申请得到的内存指针,申请失败返回NULL
 */
void *_mem_malloc(uint32_t hander, mem_size_t size);

/**
 * @brief 释放内存
 * @param[in] hander 堆句柄
 * @param[in] ptr 要释放的指针
 */
void _mem_free(uint32_t hander, void *ptr);

/**
 * @brief 重新申请内存
 * @param[in] hander 堆句柄
 * @param[in] ptr 要重新申请内存指针
 * @param[in] newsize 重新申请的内存大小
 * @return 申请得到的内存指针,申请失败或者被释放返回NULL
 */
void *_mem_realloc(uint32_t hander, void *ptr, mem_size_t newsize);

/// @} 小内存管理结束
/// @} 内存管理结束
#endif
