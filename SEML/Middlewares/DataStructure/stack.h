#ifndef _STACK_H
#define _STACK_H
/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : stack.h
 * @author : SY7_yellow
 * @brief  : 栈模块
 * @date   : 2023-7-16
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-05-30     <td>1.0         <td>SY7_yellow  <td>实现泛型队列
 * <tr><td>2023-07-16     <td>1.1-alpha   <td>SY7_yellow  <td>对其重新封装
 * <tr><td>2023-8-25      <td>1.2         <td>SY7_yellow  <td>使其线程安全
 * </table>
 * @details:
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 此栈采用泛型编程,对任何数据都可以直接求其类型大小后使用.\n
 * 2. 由于采用泛型编程,对于诸如"[警告]用于算术的void *类型的指针"是正常现象.\n
 * 3. 初始化时候选择满栈处理类型,会根据配置来执行对应操作：
 *    - 报错：返回满栈错误码
 *    - 重新申请内存：自动扩大队列大小
 * 4. 在不需要使用该栈时(包括函数内调用后)需要执行DeinitStack来释放栈内存,
 *    防止内存溢出.\n
 * 5. 对于单片机，由于其栈区很小，可分配容量很小，使用时候一般传入一个静态
 *    (全局)数组进去，并且满队处理不能使用重新申请内存。
 * 6. 该文件内函数线程安全,在多个线程同时访问的情况下失去竞争的会返回SEML_BUSY.
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
#include "SEML_common.h"
/**
 * @addtogroup 数据结构库
 * @{
 */

/**
 * @addtogroup 栈
 * @{
 */
/******************
	参数设置
******************/
// 重新扩展大小
#define StackReapplySize 10
// 最大扩展大小
#define StackSizeMax 1000

typedef enum
{
	stack_no_error = 0,	 /**< 堆栈无错误*/
	stack_full_error,	 /**< 堆栈满*/
	stack_empty_error,	 /**< 堆栈空*/
	stack_reapply_error, /**< 堆栈内存再申请出错*/
	stack_config_error,	 /**< 堆栈配置出错*/
} stack_error_code_t;

/**
 * @brief 满队处理方式
 */
typedef enum
{
	stack_full_hander_error = 0, /**< 报错*/
	stack_full_hander_reapply,	 /**< 重新申请内存*/
} stack_full_hander_t;
// @brief 处理方式断言
#define IS_STACK_FULL_HANDER(x) ((x) == stack_full_hander_error || (x) == stack_full_hander_reapply)

typedef struct
{
	void *address;												 /**< 栈初始地址			*/
	uint16_t top;													 /**< 栈顶指针			*/
	uint16_t size;												 /**< 栈大小				*/
	uint16_t elem_size;										 /**< 元素大小			*/
	stack_full_hander_t full_hander : 1;	 /**< 满栈处理方式		*/
	stack_error_code_t error_code : 3;		 /**< 栈错误码			*/
	FunctionalState use_extern_buffer : 1; /**< 使用外部缓存数组	*/
	SEML_LockType_t Lock : 1;
} s_stack;

/**
 * @brief 初始化栈
 * @param[out] stack 栈结构体指针
 * @param[in] elem_size 元素大小
 * @param[in] size 栈大小
 * @param[in] buffer 存数组，使用栈区自动分配可传NULL,传入大小需比申请尺寸大1
 * @param[in] full_hander 满栈处理方式
 * @return 栈执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 申请内存错误
 */
SEML_StatusTypeDef InitStack(s_stack *stack, uint16_t elem_size, uint16_t size, void *buffer, stack_full_hander_t full_hander);

/**
 * @brief 删除堆栈
 * @param[in,out] stack 栈结构体指针
 */
void DeinitStack(s_stack *stack);

/**
 * @brief 数据弹栈
 * @param[in,out] stack 栈结构体指针
 * @param[out] data 栈顶数据
 * @return 栈执行状态
 */
SEML_StatusTypeDef Push(s_stack *stack, void *data);

/**
 * @brief 数据弹栈
 * @param[in,out] stack 栈结构体指针
 * @param[in] data 栈顶数据
 * @return 栈执行状态
 */
SEML_StatusTypeDef Pop(s_stack *stack, void *data);

/**
 * @brief 访问栈顶但不弹栈
 * @param[in] stack 栈结构体指针
 * @param[out] data 栈顶数据
 * @return 栈执行状态
 */
SEML_StatusTypeDef GetStackTop(s_stack *stack, void *data);
/**
 * @} 栈结束
 */

/**
 * @} 数据结构库结束
 */
#endif

/*******************End Of File*****************************/