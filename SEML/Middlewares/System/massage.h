/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : massage.h
 * @author : SY7_yellow
 * @brief  : 消息列表模块
 * @date   : 2023-8-8
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-08-08     <td>0.1         <td>SY7_yellow  <td>实现消息列表
 * <tr><td>2023-08-28     <td>0.2         <td>SY7_yellow  <td>使用hash和链表对消息列表进行重构
 * <tr><td>2023-08-30     <td>0.3         <td>SY7_yellow  <td>实现消息缓存
 * <tr><td>2023-09-16     <td>0.4-alpha   <td>SY7_yellow  <td>将消息列表数据分发改成动态分配的形式
 * </table>
 * @todo    : 消息缓存增加对DMA的支持
 * @todo    : 消息缓存实现半满中断（基于定时处理）
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * ** 消息列表 **
 * 1. 消息列表主要是为了解决消息分发问题,可用于串口,CAN,任务间通讯等需要两个使用
 *    通讯的时候使用,可以使两个通讯单元进行完全解耦,无需关注驱动层如何实现.若接收
 *    回调使用消息缓存即成为消息队列，可以让两个线程不丢包进行异步运行.
 * 2. 使用Massage_List_Init初始化消息队列,申请的缓存长度只需要满足使用且有一定冗
 *    余即可.
 * 3. 若需要接收某个ID的消息需要进行注册才能接收到该ID的数据，注册的时候传入格式
 *    为:void massage_callback(void *config, uint16_t id, uint16_t cmd, void *data)
 *    的回调函数,其中config,data的数据类型类型,名称可以自己定义,但是顺序必须一致.
 *    config为该回调函数的句柄，可以自由定义.
 * 4. 更新数据需要调用Massage_Update函数,传入数据和id即可.
 * 5. 若需要获取当前存在的id可以使用Get_Massage_Links,返回超时时间内仍有通讯的数
 *    据,去除未注册超时数据,定期调用可以提高性能.
 * 6. 该文件内函数线程安全,涉及写入操作会进入临界区,待写入完毕后才退出临界区
 * 7. 消息列表出现错误会返回错误码,并且在errno_code中也会设置错误码.
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
#ifndef _MASSAGE_H_
#define _MASSAGE_H_
#include "SEML_common.h"
#include <string.h>
#include <stdlib.h>

/**
 * @addtogroup 系统库
 * @{
 */
/**
 * @addtogroup 消息列表模块
 * @{
 */
/**
 * @brief 消息列表哈希表大小
 */
#ifndef MASSAGE_LIST_HASH_NUM
#define MASSAGE_LIST_HASH_NUM 5
#endif
#define Massage_List_Realloc(ptr, size) realloc(ptr, size)
/**
 * @brief 消息列表错误码
 */
typedef enum
{
	massage_no_error,				/**< 无错误 */
	massage_mult_Register,	/**< 重复注册 */
	massage_list_full,			/**< 消息列表满 */
	massage_not_find,				/**< 未查找到 */
	massage_memory_failure, /**< 内存分配失败 */
} massage_error_code_t;
typedef uint32_t Cmd_t;
typedef struct
{
	void (*fun)(void *config, void const *const massage_pack); /**< 更新回调函数 */
	void *config;																							 /**< 更新回调函数配置结构体 */
} massage_callback_t;

typedef struct
{
	void *data;						/**< 数据指针 */
	uint16_t data_longth; /**< 数据长度 */
	Cmd_t cmd;						/**< 控制码 */
} Massage_Data_Pack_t;

/**
 * @brief 消息包结构体
 */
typedef struct
{
	uint16_t ID;									 /**< id号 */
	uint16_t callback_longth : 8;	 /**< 回调函数长度 */
	uint16_t data_alloc : 1;			 /**< 数据指针是否为动态分配 */
	uint16_t pack_register : 1;		 /**< 消息包是否注册 */
	Massage_Data_Pack_t data_pack; /**< 消息数据包 */
	massage_callback_t *callback;	 /**< 回调函数句柄 */
	uint32_t update_tick;					 /**< 更新时间戳 */
	void *next;
} Massage_Pack_t;

/**
 * @brief 消息队列回调函数
 * @param config 回调函数配置句柄
 * @param massage_pack 消息包
 */
typedef void (*massage_callback_fun_t)(void *config, Massage_Pack_t const *const massage_pack);

/**
 * @brief 消息队列结构体
 */
typedef struct
{
	Massage_Pack_t *list;															/**< 列表指针 */
	uint16_t size;																		/**< 列表大小 */
	uint16_t longth;																	/**< 队尾位置 */
	uint32_t timeout;																	/**< 超时时间 */
	SEML_LockType_t Lock : 1;													/**< 互斥锁 */
	massage_error_code_t error_code : 7;							/**< 错误码 */
	Massage_Pack_t *hash_list[MASSAGE_LIST_HASH_NUM]; /**< hash表 */
} Massage_List_t;

/**
 * @brief 初始化消息队列
 * @param[out] massage_ID_list 消息列表句柄
 * @param[in] id_buffer id缓存数组
 * @param[in] size 缓存大小
 * @param[in] timeout 超时时间
 */
void Massage_List_Init(Massage_List_t *massage_list, Massage_Pack_t *pack_buffer, uint16_t size, uint32_t timeout);

/**
 * @brief 在消息列表中进行注册
 * @param[in,out] massage_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] fun 消息回调函数指针
 * @param[in] config 消息回调函数句柄
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 重复注册
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef Massage_List_Register(Massage_List_t *massage_list, uint16_t ID, void (*fun)(void *config, Massage_Pack_t const *const massage_pack), void *config);

/**
 * @brief 在消息列表中注销消息包
 * @param[in,out] massage_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] fun 要注销的回调函数，传入NULL注销整个消息包
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef Massage_List_Logout(Massage_List_t *massage_list, uint16_t ID, massage_callback_fun_t fun);

/**
 * @brief 更新消息队列
 * @param[in,out] massage_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] cmd 控制码
 * @param[in] data 数据指针
 * @param[in] data_longth 数据长度
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 消息列表满
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef Massage_List_Update(Massage_List_t *massage_list, uint16_t ID, Cmd_t cmd, void *data, uint16_t data_longth);

/**
 * @brief 更新消息包控制码
 * @param[in,out] massage_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] cmd 控制码
 */
#define Massage_List_Update_Cmd(massage_list, id, cmd) Massage_List_Update(massage_list, id, cmd, NULL, 0)

/**
 * @brief 更新消息包数据
 * @param[in,out] massage_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] data 数据指针
 * @param[in] data_longth 数据长度
 */
#define Massage_List_Update_Data(massage_list, id, data, longth) Massage_List_Update(massage_list, id, 0x00, data, longth)

/**
 * @brief 获取现存的消息包id
 * 将超时的未注册消息包从消息列表删除，向existing_buffer输出未超时的消息包id
 * @param[in,out] massage_list 消息列表句柄
 * @param[out] existing_buffer 现存消息包id数组,若不需要可以传入NULL
 * @return 现存链接个数
 */
uint16_t Get_Massage_List_Links(Massage_List_t *massage_list, uint16_t *existing_buffer);

/**
 * @brief 获得当前消息列表长度
 * @param[in] massage_ID_list 消息列表句柄
 * @return 消息列表长度
 */
uint16_t Get_Massage_List_Num(Massage_List_t *massage_list);

/**
 * @brief 获取消息包更新时间戳
 * @param[in] massage_pack 消息包句柄
 * @return 该消息包更新时间戳
 */
uint32_t Get_Massage_Pack_Timestamp(Massage_Pack_t const *const massage_pack);

/**
 * @brief 获取消息包ID
 * @param[in] massage_pack 消息包句柄
 * @return 该消息包ID
 */
uint16_t Get_Massage_Pack_ID(Massage_Pack_t const *const massage_pack);

/**
 * @brief 获取消息包控制码
 * @param[in] massage_pack 消息包句柄
 * @return 该消息包控制码
 */
Cmd_t Get_Massage_Pack_Cmd(Massage_Pack_t const *const massage_pack);

/**
 * @brief 获取消息包数据包
 * @param[in] massage_pack 消息包句柄
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
uint16_t Get_Massage_Pack_Data(Massage_Pack_t const *const massage_pack, void **data);

/**
 * @brief 获取特定id的消息包更新时间戳
 * @param[in] massage_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包更新时间戳
 */
uint32_t Get_Massage_ID_Timestamp(Massage_List_t *massage_list, uint16_t id);

/**
 * @brief 获取特定id的消息包控制码
 * @param[in] massage_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包控制码
 */
Cmd_t Get_Massage_ID_Cmd(Massage_List_t *massage_list, uint16_t id);

/**
 * @brief 获取特定id的消息包数据包
 * @param[in] massage_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
uint16_t Get_Massage_ID_Data(Massage_List_t *massage_list, uint16_t id, void **data);

/// @} 消息列表模块结束

/**
 * @addtogroup 消息缓存模块
 * @{
 */
/**
 * @brief 消息缓存事件回调函数
 */
typedef void (*massage_buffer_callback_fun_t)(void *config, void *massage_buffer_handle);

/**
 * @brief 消息缓存事件类型
 */
typedef enum
{
	buffer_full_callback,
	buffer_half_full_callback
} massage_buffer_callback_type_t;
/// @brief 是否为消息缓存事件类型
#define IS_MASSAGE_BUFFER_CALLBACK_TYPE(a) ((a) == buffer_full_callback || (a) == buffer_half_full_callback)

/**
 * @brief 消息缓存模块结构体
 */
typedef struct
{
	uint8_t *buffer;																						 /**< 消息缓存数组指针 */
	uint16_t rear;																							 /**< 消息缓存队尾指针 */
	uint16_t froat;																							 /**< 消息缓存队头指针 */
	uint16_t size;																							 /**< 消息缓存数组大小 */
	void *buffer_full_callback_config;													 /**< 消息缓存全满事件回调函数配置 */
	massage_buffer_callback_fun_t buffer_full_callback_fun;			 /**< 消息缓存全满事件 */
	void *buffer_half_full_callback_config;											 /**< 消息缓存半满事件回调函数配置 */
	massage_buffer_callback_fun_t buffer_half_full_callback_fun; /**< 消息缓存半满事件回调函数 */
} massage_buffer_t;

/**
 * @brief 消息缓存初始化
 * @param[out] massage_buffer_handle 消息缓存句柄
 * @param[in] buffer 缓存数组
 * @param[in] size 缓存数组大小
 */
void Massage_Buffer_Init(massage_buffer_t *massage_buffer_handle, uint8_t *buffer, uint16_t size);

/**
 * @brief 消息缓存清空复位
 * @param[in,out] massage_buffer_handle 消息缓存句柄
 */
void Massage_Buffer_Reset(massage_buffer_t *massage_buffer_handle);

/**
 * @brief 消息缓存事件回调注册
 * @param[out] massage_buffer_handle 消息缓存句柄
 * @param[in] type 消息缓存事件类型
 * @param[in] callback_fun 消息缓存回调函数
 * @param[in] callback_config 消息缓存回调函数配置
 */
void Massage_Buffer_IT_Register(massage_buffer_t *massage_buffer_handle, massage_buffer_callback_type_t type,
																massage_buffer_callback_fun_t callback_fun, void *callback_config);

/**
 * @brief 设置已缓存数据长度
 * @param[in,out] massage_buffer_handle 消息缓存句柄
 * @param[in] new_longth 新长度
 */
void Massage_Buffer_SetLongth(massage_buffer_t *massage_buffer_handle, uint16_t new_longth);

/**
 * @brief 获取消息缓存长度
 * @param[in] massage_buffer_handle 消息缓存句柄
 * @return 当前缓存长度
 */
uint16_t Massage_Buffer_GetLongth(massage_buffer_t *massage_buffer_handle);

/**
 * @brief 获取消息缓存队头数据
 * @param[in] massage_buffer_handle 消息缓存句柄
 * @param[out] data 输出数据数组
 * @param[in] longth 读取长度
 * @return 实际读取长度
 */
uint16_t Massage_Buffer_GetFront(massage_buffer_t *massage_buffer_handle, uint8_t *data, uint16_t longth);

/**
 * @brief 获取消息缓存队尾数据
 * @param[in] massage_buffer_handle 消息缓存句柄
 * @param[out] data 输出数据数组
 * @param[in] longth 读取长度
 * @return 实际读取长度
 */
uint16_t Massage_Buffer_GetRear(massage_buffer_t *massage_buffer_handle, uint8_t *data, uint16_t longth);

/**
 * @brief 向消息缓存队尾写入数据
 * @param[in] massage_buffer_handle 消息缓存句柄
 * @param[out] data 写入数据数组
 * @param[in] longth 写入长度
 */
void Massage_Buffer_EnQueue(massage_buffer_t *massage_buffer_handle, uint8_t *data, uint16_t longth);

/**
 * @brief 消息缓存队头出队
 * @param[in] massage_buffer_handle 消息缓存句柄
 * @param[out] data 输出数据数组(可传NULL只出队，不读取)
 * @param[in] longth 读取长度
 * @return 实际读取长度
 */
uint16_t Massage_Buffer_DeQueue(massage_buffer_t *massage_buffer_handle, uint8_t *data, uint16_t longth);
/// @} 消息缓存模块结束
/// @} 系统库结束
#endif
