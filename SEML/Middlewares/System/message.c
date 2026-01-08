#include "message.h"

/**
 * @brief 消息列表哈希函数
 */
#define message_Hash(a) ((a ^ (a << 4)) % (message_LIST_HASH_NUM))

/**
 * @brief 消息包是否注册
 */
#define Is_message_Pack_Register(pack) ((pack)->pack_register)

/**
 * @brief 遍历消息包回调函数
 * @param pack 消息包句柄
 */
#define scan_message_callback(pack)                                                                \
	do                                                                                               \
	{                                                                                                \
		for (uint16_t __tmp = 0; __tmp < (pack)->callback_longth && (pack)->callback != NULL; __tmp++) \
			if ((pack)->callback[__tmp].fun != NULL)                                                     \
				(pack)->callback[__tmp].fun((pack)->callback[__tmp].config, (pack));                       \
	} while (0)

/**
 * @brief 申请消息包回调函数内存
 * @param list 消息列表句柄
 * @param pack 消息包句柄
 * @param opt 空间大小操作符(增加1单位:++,减少1单位:--,设为0:=0,...)
 */
#define message_callback_alloc(list, pack)                                                                            \
	do                                                                                                                  \
	{                                                                                                                   \
		message_callback_t *__mc_temp = NULL;                                                                             \
		__mc_temp = message_List_Realloc((void *)(pack)->callback, (pack)->callback_longth * sizeof(message_callback_t)); \
		if (__mc_temp == NULL && (pack)->callback_longth != 0)                                                            \
		{                                                                                                                 \
			(list)->error_code = message_memory_failure;                                                                    \
			assert_param(0);                                                                                                \
		}                                                                                                                 \
		(pack)->callback = __mc_temp;                                                                                     \
	} while (0)

/**
 * @brief 更新消息包数据
 * @param pack 消息包句柄
 * @param _data_pack 数据包
 * @param _update_tick 更新时间戳
 * @param _data_alloc 数据是否为动态申请
 */
#define message_Pack_Update(pack, _update_tick) \
	do                                            \
	{                                             \
		(pack)->update_tick = (_update_tick);       \
	} while (0)
	
/**
 * @brief 消息数据包更新函数
 *
 * @param message_pack 消息包
 * @param cmd 控制码
 * @param data 数据内容
 * @param data_longth 数据长度
 */
static SEML_StatusTypeDef message_Data_Update(message_Pack_t *message_pack, Cmd_t cmd, void *data, uint16_t data_longth)
{
	void *mem;
	SEML_StatusTypeDef ret;
	// 静态内存
	if (message_pack->data_alloc == 0)
	{
		mem = message_List_Realloc(NULL, data_longth);
		if (mem != NULL)
		{
			message_pack->data_pack.data = mem;
			memcpy(mem, data, data_longth);
			message_pack->data_alloc = 1;
			ret = SEML_OK;
		}
		else
		{
			message_pack->data_pack.data = data;
			if (data_longth == 0)
				ret = SEML_OK;
			else
				ret = SEML_ERROR;
		}
	}
	// 动态内存
	else
	{
		mem = message_List_Realloc(message_pack->data_pack.data, data_longth);
		if (mem != NULL)
		{
			message_pack->data_pack.data = mem;
			memcpy(mem, data, data_longth);
			ret = SEML_OK;
		}
		else
		{
			// 释放内存 检查之前是否释放过，防止重复释放
			if (data_longth != 0)
			{
				message_List_Realloc(message_pack->data_pack.data, 0);
				ret = SEML_ERROR;
			}
			else
				ret = SEML_OK;
			message_pack->data_pack.data = data;
			message_pack->data_alloc = 0;
		}
	}
	message_pack->data_pack.data_longth = data_longth;
	message_pack->data_pack.cmd = cmd;
	return ret;
}
/**
 * @brief 向消息列表添加新元素
 * @param[in,out] message_list 消息列表句柄
 * @param[out] message_pack 消息包句柄
 * @param[in] ID id号
 * @param[in] callback 回调函数
 * @param[in] config 回调函数句柄
 * @param[in] update_tick 更新时间戳
 */
static void Add_message_Pack(message_List_t *message_list, message_Pack_t *message_pack, uint16_t ID,
														 message_callback_fun_t fun, void *config, uint32_t update_tick)
{
	message_Pack_t *message_Pack_temp = message_list->hash_list[message_Hash(ID)];
	message_pack->ID = ID;
	message_pack->update_tick = update_tick;
	// 更新回调函数
	if (fun != NULL)
	{
		message_pack->callback_longth++;
		message_callback_alloc(message_list, message_pack);
		message_pack->callback[message_pack->callback_longth - 1].fun = (void (*)(void *, void const *const))fun;
		message_pack->callback[message_pack->callback_longth - 1].config = config;
	}
	if (message_Pack_temp != NULL)
	{
		while (message_Pack_temp->next != NULL)
			message_Pack_temp = message_Pack_temp->next;
		// 防止出现自循环
		if (message_Pack_temp != message_pack)
			message_Pack_temp->next = message_pack;
	}
	else
		message_list->hash_list[message_Hash(ID)] = message_pack;
}

/**
 * @brief 从消息列表删除元素
 * @param[in,out] message_list 消息列表句柄
 * @param[out] message_pack 消息包句柄
 * @param[in] fun 要删除的回调函数(传入NULL则删除整个消息包)
 */
static void Del_message_Pack(message_List_t *message_list, message_Pack_t *message_pack, message_callback_fun_t fun)
{
	uint16_t ID = message_pack->ID;
	message_Pack_t *message_pack_temp = message_list->hash_list[message_Hash(ID)];
	// 删除回调
	if (fun != NULL && message_pack->callback != NULL)
	{
		for (int i = 0, temp = 0; i < message_pack->callback_longth; i++)
		{
			if (i == message_pack->callback_longth - 1)
			{
				message_pack->callback_longth--;
				message_callback_alloc(message_list, message_pack);
			}

			else if (message_pack->callback[i].fun == (void (*)(void *, void const *const))fun)
				temp = 1;
			if (temp != 0)
				message_pack->callback[i] = message_pack->callback[i + 1];
		}
	}
	// 删除整个数据包
	else
	{
		// 从链表剔除数据
		if (message_pack_temp->ID == ID)
		{
			message_list->hash_list[message_Hash(ID)] = (message_Pack_t *)message_pack_temp->next;
		}
		else
			while (message_pack_temp != NULL)
			{
				if (((message_Pack_t *)message_pack_temp->next)->ID == ID)
				{
					message_pack_temp->next = ((message_Pack_t *)message_pack_temp->next)->next;
					break;
				}
				message_pack_temp = message_pack_temp->next;
			}

		message_pack->next = NULL;
		message_pack->ID = 0x0000;
		message_Data_Update(message_pack, 0x00, NULL, 0);
		message_Pack_Update(message_pack, 0);
		message_pack->callback_longth = 0;
		message_callback_alloc(message_list, message_pack);
	}
}

/**
 * @brief 获取消息号ID所在位置
 * @param[in] message_list 消息列表句柄
 * @param[in] ID ID号
 * @param[out] message_Pack 消息包句柄地址
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 未查找到该ID
 */
static SEML_StatusTypeDef message_ID_Search(message_List_t *message_list, uint16_t ID, message_Pack_t **message_Pack)
{
	assert_param(message_list != NULL);
	message_Pack_t *message_Pack_temp;
	message_Pack_temp = message_list->hash_list[message_Hash(ID)];
	while (message_Pack_temp != NULL)
	{
		if (message_Pack_temp->ID == ID)
		{
			*message_Pack = message_Pack_temp;
			return SEML_OK;
		}
		message_Pack_temp = message_Pack_temp->next;
	}
	return SEML_ERROR;
}

/**
 * @brief 最久远未注册消息包所在位置
 * @param[in] message_list 消息列表句柄
 * @param[out] message_Pack 消息包句柄地址
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 未查找到该ID
 */
static SEML_StatusTypeDef message_Oldest_Serach(message_List_t *message_list, message_Pack_t **message_Pack)
{
	uint16_t temp = 0;
	for (int i = 0; i < message_list->size; i++)
	{
		if (message_list->list[temp].update_tick < message_list->list[i].update_tick && message_list->list[i].callback == NULL)
			temp = i;
		else if (i == message_list->size - 1)
			return SEML_ERROR;
	}
	if (SEML_GetTick() - message_list->list[temp].update_tick > message_list->timeout)
	{
		*message_Pack = &message_list->list[temp];
		return SEML_OK;
	}
	return SEML_ERROR;
}

/**
 * @brief 空闲消息包所在位置
 * @param[in] message_list 消息列表句柄
 * @param[out] message_Pack 消息包句柄地址
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 未查找到该ID
 */
static SEML_StatusTypeDef message_Empty_Serach(message_List_t *message_list, message_Pack_t **message_Pack)
{
	for (int i = 0; i < message_list->size; i++)
	{
		if (message_list->list[i].ID == 0x0000)
		{
			*message_Pack = &message_list->list[i];
			return SEML_OK;
		}
	}
	return SEML_ERROR;
}

/**
 * @brief 初始化消息队列
 * @param[out] message_ID_list 消息列表句柄
 * @param[in] id_buffer id缓存数组
 * @param[in] size 缓存大小
 * @param[in] timeout 超时时间
 */
void message_List_Init(message_List_t *message_list, message_Pack_t *pack_buffer, uint16_t size, uint32_t timeout)
{
	assert_param(message_list != NULL);
	assert_param(pack_buffer != NULL);
	message_Data_Pack_t data_pack;

	message_list->list = pack_buffer;
	message_list->size = size;
	message_list->longth = 0;
	message_list->timeout = timeout;
	message_list->Lock = SEML_UNLOCKED;

	data_pack.cmd = 0;
	data_pack.data = NULL;
	data_pack.data_longth = 0;

	for (int i = 0; i < message_LIST_HASH_NUM; i++)
		message_list->hash_list[i] = NULL;
	for (uint16_t i = 0; i < size; i++)
	{
		pack_buffer[i].data_pack = data_pack;
		message_Pack_Update(&pack_buffer[i], 0);
		pack_buffer[i].callback = NULL;
		pack_buffer[i].callback_longth = 0;
		pack_buffer[i].next = NULL;
	}
}

/**
 * @brief 获得当前消息列表长度
 * @param[in] message_ID_list 消息列表句柄
 * @return 消息列表长度
 */
uint16_t Get_message_List_Num(message_List_t *message_list)
{
	assert_param(message_list != NULL);
	return message_list->longth;
}

/**
 * @brief 获取消息包更新时间戳
 * @param[in] message_pack 消息包句柄
 * @return 该消息包更新时间戳
 */
inline uint32_t Get_message_Pack_Timestamp(message_Pack_t const *const message_pack)
{
	assert_param(message_pack != NULL);
	return message_pack->update_tick;
}

/**
 * @brief 获取消息包ID
 * @param[in] message_pack 消息包句柄
 * @return 该消息包ID
 */
inline uint16_t Get_message_Pack_ID(message_Pack_t const *const message_pack)
{
	assert_param(message_pack != NULL);
	return message_pack->ID;
}

/**
 * @brief 获取消息包控制码
 * @param[in] message_pack 消息包句柄
 * @return 该消息包控制码
 */
inline Cmd_t Get_message_Pack_Cmd(message_Pack_t const *const message_pack)
{
	assert_param(message_pack != NULL);
	return message_pack->data_pack.cmd;
}

/**
 * @brief 获取消息包数据包
 * @param[in] message_pack 消息包句柄
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
inline uint16_t Get_message_Pack_Data(message_Pack_t const *const message_pack, void **data)
{
	assert_param(message_pack != NULL);
	*data = message_pack->data_pack.data;
	return message_pack->data_pack.data_longth;
}

/**
 * @brief 获取特定id的消息包更新时间戳
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包更新时间戳
 */
inline uint32_t Get_message_ID_Timestamp(message_List_t *message_list, uint16_t id)
{
	message_Pack_t *pack;
	assert_param(message_list != NULL);
	if (message_ID_Search(message_list, id, &pack) == SEML_OK)
		return pack->update_tick;
	return 0;
}

/**
 * @brief 获取特定id的消息包控制码
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @return 该消息包控制码
 */
inline Cmd_t Get_message_ID_Cmd(message_List_t *message_list, uint16_t id)
{
	message_Pack_t *pack;
	assert_param(message_list != NULL);
	if (message_ID_Search(message_list, id, &pack) == SEML_OK)
		return pack->data_pack.cmd;
	return 0;
}

/**
 * @brief 获取特定id的消息包数据包
 * @param[in] message_ID_list 消息列表句柄
 * @param[in] id 消息包id
 * @param[out] data 数据指针地址
 * @return 该数据包大小
 */
inline uint16_t Get_message_ID_Data(message_List_t *message_list, uint16_t id, void **data)
{
	message_Pack_t *pack;
	assert_param(message_list != NULL);
	if (message_ID_Search(message_list, id, &pack) == SEML_OK)
	{
		*data = pack->data_pack.data;
		return pack->data_pack.data_longth;
	}
	*data = NULL;
	return 0;
}

/**
 * @brief 在消息列表中进行注册
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] fun 消息回调函数指针
 * @param[in] config 消息回调函数句柄
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 重复注册
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef message_List_Register(message_List_t *message_list, uint16_t ID, message_callback_fun_t fun, void *config)
{
	message_Pack_t *message_pack_temp;
	assert_param(message_list != NULL);
	assert_param(message_list->list != NULL);
	// 若不适用回调函数使用空白回调,避免被清理
	// 首先遍历是否在列表中
	if (message_ID_Search(message_list, ID, &message_pack_temp) == SEML_OK)
	{
		// 存在于列表且已注册
		if (Is_message_Pack_Register(message_pack_temp))
		{
			for (int i = 0; i <= message_pack_temp->callback_longth; i++)
			{
				if (message_pack_temp->callback[i].fun == (void (*)(void *, void const *const))fun)
				{
					message_list->error_code = message_mult_Register;
					return SEML_ERROR;
				}
			}
		}
		// 重新注册
		__SEML_ENTER_CRITICAL_SECTION();
		if (fun != NULL)
		{
			message_pack_temp->callback_longth++;
			message_callback_alloc(message_list, message_pack_temp);
			message_pack_temp->callback[message_pack_temp->callback_longth - 1].fun = (void (*)(void *config, void const *const message_pack))fun;
			message_pack_temp->callback[message_pack_temp->callback_longth - 1].config = config;
			message_pack_temp->pack_register = 1;
		}
		__SEML_LEAVE_CRITICAL_SECTION();
		return SEML_OK;
	}
	// 查找不到进行添加
	if (message_list->longth >= message_list->size)
	{
		// 消息列表满查找最久未注册数据进行替代
		if (message_Oldest_Serach(message_list, &message_pack_temp) != SEML_OK)
		{
			message_list->error_code = message_list_full;
			return SEML_ERROR;
		}
		__SEML_ENTER_CRITICAL_SECTION();
		Add_message_Pack(message_list, message_pack_temp, ID, fun, config, 0);
		message_pack_temp->pack_register = 1;
		__SEML_LEAVE_CRITICAL_SECTION();
	}
	else
	{
		// 未满寻找空闲位置添加
		message_Empty_Serach(message_list, &message_pack_temp);
		message_list->longth++;
		__SEML_ENTER_CRITICAL_SECTION();
		Add_message_Pack(message_list, message_pack_temp, ID, fun, config, 0);
		message_pack_temp->pack_register = 1;
		__SEML_LEAVE_CRITICAL_SECTION();
	}
	return SEML_OK;
}

/**
 * @brief 在消息列表中注销消息包
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] fun 要注销的回调函数，传入NULL注销整个消息包
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef message_List_Logout(message_List_t *message_list, uint16_t ID, message_callback_fun_t fun)
{
	message_Pack_t *message_pack_temp;

	assert_param(message_list != NULL);
	assert_param(message_list->list != NULL);
	// 首先遍历是否在列表中
	if (message_ID_Search(message_list, ID, &message_pack_temp) == SEML_OK)
	{
		__SEML_ENTER_CRITICAL_SECTION();
		Del_message_Pack(message_list, message_pack_temp, fun);
		message_pack_temp->pack_register = 0;
		__SEML_LEAVE_CRITICAL_SECTION();
		// 更新队列大小
		message_list->longth--;
	}
	return SEML_OK;
}

/**
 * @brief 消息列表调用回调
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 无此id
 */
SEML_StatusTypeDef message_List_Callback(message_List_t *message_list, uint16_t ID)
{
	message_Pack_t *message_pack_temp;
	assert_param(message_list != NULL);
	assert_param(message_list->list != NULL);

	if (message_ID_Search(message_list, ID, &message_pack_temp) == SEML_OK)
	{
		if (message_pack_temp->callback != NULL) // 调用自身回调
			scan_message_callback(message_pack_temp);
		return SEML_OK;
	}
	return SEML_ERROR;
}

/**
 * @brief 更新消息队列
 * @param[in,out] message_list 消息列表句柄
 * @param[in] ID 消息包ID
 * @param[in] cmd 控制码
 * @param[in] data 数据指针
 * @param[in] data_longth 数据长度
 * @return 函数执行状态
 * @retval SEML_OK 无错误
 * @retval SEML_ERROR 消息列表满
 * @retval SEML_BUSY 有其他进程正在编辑该句柄
 */
SEML_StatusTypeDef message_List_Update(message_List_t *message_list, uint16_t ID, Cmd_t cmd, void *data, uint16_t data_longth)
{
	message_Pack_t *message_pack_temp;
	assert_param(message_list != NULL);
	assert_param(message_list->list != NULL);

	if (message_ID_Search(message_list, ID, &message_pack_temp) == SEML_OK)
	{
		message_Data_Update(message_pack_temp, cmd, data, data_longth);
		message_Pack_Update(message_pack_temp, SEML_GetTick());
		if (message_pack_temp->callback != NULL) // 调用自身回调
			scan_message_callback(message_pack_temp);
		return SEML_OK;
	}
	// 查找不到进行添加 如果消息列表已满找到最久远的未注册消息进行替代
	if (message_list->longth >= message_list->size)
	{
		if (message_Oldest_Serach(message_list, &message_pack_temp) != SEML_OK)
		{
			message_list->error_code = message_list_full;
			return SEML_ERROR;
		}
	}
	else
	{
		// 遍历到空节点
		message_Empty_Serach(message_list, &message_pack_temp);
		message_list->longth++;
	}
	message_Pack_Update(message_pack_temp, SEML_GetTick());
	message_Data_Update(message_pack_temp, cmd, data, data_longth);
	__SEML_ENTER_CRITICAL_SECTION();
	Add_message_Pack(message_list, message_pack_temp, ID, NULL, NULL, SEML_GetTick());
	__SEML_LEAVE_CRITICAL_SECTION();
	return SEML_OK;
}

/**
 * @brief 获取现存的消息包id
 * 将超时的未注册消息包从消息列表删除，向existing_buffer输出未超时的消息包id
 * @param[in,out] message_list 消息列表句柄
 * @param[out] existing_buffer 现存消息包id数组,若不需要可以传入NULL
 * @return 现存链接个数
 */
uint16_t Get_message_List_Links(message_List_t *message_list, uint16_t *existing_buffer)
{
	uint16_t j = 0, old_longth = message_list->longth;
	assert_param(message_list != NULL);
	assert_param(message_list->list != NULL);
	// 对消息列表进行遍历
	for (int i = 0; i < old_longth; i++)
	{
		if (SEML_GetTick() - message_list->list[i].update_tick >= message_list->timeout)
		{

			if (Is_message_Pack_Register(&message_list->list[i]) == 0 && message_list->list[i].ID != 0x0000)
			{
				__SEML_ENTER_CRITICAL_SECTION();
				Del_message_Pack(message_list, &message_list->list[i], NULL);
				message_list->longth--;
				__SEML_LEAVE_CRITICAL_SECTION(message_list);
			}
			continue;
		}
		// 输出现存队列id
		if (existing_buffer != NULL)
			existing_buffer[j++] = message_list->list[i].ID;
	}
	return j;
}

/**
 * @brief 消息缓存根节点
 */
message_buffer_t *message_buffer_root;

/**
 * @brief 消息缓存事件回调
 * @param _handle 消息缓存句柄
 * @param _callback 消息缓存事件类型
 */
#define message_Buffer_Callback(_handle, _callback)                   \
	do                                                                  \
	{                                                                   \
		if (_handle->_callback##_fun != NULL)                             \
			_handle->_callback##_fun(_handle->_callback##_config, _handle); \
	} while (0)

/**
 * @brief 消息缓存初始化
 * @param[out] message_buffer_handle 消息缓存句柄
 * @param[in] buffer 缓存数组
 * @param[in] size 缓存数组大小
 */
void message_Buffer_Init(message_buffer_t *message_buffer_handle, uint8_t *buffer, uint16_t size)
{
	assert_param(message_buffer_handle != NULL);
	assert_param(buffer != NULL);
	message_buffer_handle->buffer = buffer;
	message_buffer_handle->size = size;
	message_buffer_handle->froat = 0;
	message_buffer_handle->rear = 0;
}

/**
 * @brief 复位消息缓存
 * @param[in,out] message_buffer_handle 消息缓存句柄
 */
void message_Buffer_Reset(message_buffer_t *message_buffer_handle)
{
	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);
	message_buffer_handle->froat = 0;
	message_buffer_handle->rear = 0;
}

/**
 * @brief 消息缓存事件回调注册
 * @param[out] message_buffer_handle 消息缓存句柄
 * @param[in] type 消息缓存事件类型
 * @param[in] callback_fun 消息缓存回调函数
 * @param[in] callback_config 消息缓存回调函数配置
 */
void message_Buffer_IT_Register(message_buffer_t *message_buffer_handle, message_buffer_callback_type_t type,
																message_buffer_callback_fun_t callback_fun, void *callback_config)
{
	assert_param(message_buffer_handle != NULL);
	assert_param(IS_message_BUFFER_CALLBACK_TYPE(type));
	switch (type)
	{
	case buffer_full_callback:
		message_buffer_handle->buffer_full_callback_fun = callback_fun;
		message_buffer_handle->buffer_full_callback_config = callback_config;
		break;
	case buffer_half_full_callback:
		message_buffer_handle->buffer_half_full_callback_fun = callback_fun;
		message_buffer_handle->buffer_half_full_callback_config = callback_config;
		break;
	default:
		break;
	}
}
/**
 * @brief 设置已缓存数据长度
 * @param[in,out] message_buffer_handle 消息缓存句柄
 * @param[in] new_longth 新长度
 */
void message_Buffer_SetLongth(message_buffer_t *message_buffer_handle, uint16_t new_longth)
{
	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);
	message_buffer_handle->rear = (message_buffer_handle->froat + new_longth) % message_buffer_handle->size;
}
/**
 * @brief 获取消息缓存长度
 * @param[in] message_buffer_handle 消息缓存句柄
 * @return 当前缓存长度
 */
uint16_t message_Buffer_GetLongth(message_buffer_t *message_buffer_handle)
{
	uint16_t longth;
	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);
	if (message_buffer_handle->froat > message_buffer_handle->rear)
		longth = message_buffer_handle->size + message_buffer_handle->rear - message_buffer_handle->froat;
	else
		longth = message_buffer_handle->rear - message_buffer_handle->froat;
	return longth;
}

/**
 * @brief 获取消息缓存队头数据
 * @param[in] message_buffer_handle 消息缓存句柄
 * @param[out] data 输出数据数组
 * @param[in] longth 读取长度
 * @return 实际读取长度
 */
uint16_t message_Buffer_GetFront(message_buffer_t *message_buffer_handle, uint8_t *data, uint16_t longth)
{
	uint16_t temp;

	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);

	if (message_buffer_handle->froat == message_buffer_handle->rear)
		return 0;
	temp = message_Buffer_GetLongth(message_buffer_handle);
	if (longth > temp)
		longth = temp;
	if (message_buffer_handle->froat + longth >= message_buffer_handle->size)
	{
		temp = message_buffer_handle->size - message_buffer_handle->froat;
		if (data != NULL)
		{
			memcpy(data, &message_buffer_handle->buffer[message_buffer_handle->froat], temp);
			memcpy(&data[temp], message_buffer_handle->buffer, longth - temp);
		}
	}
	else if (data != NULL)
		memcpy(data, &message_buffer_handle->buffer[message_buffer_handle->froat], longth);
	return longth;
}

/**
 * @brief 获取消息缓存队尾数据
 * @param[in] message_buffer_handle 消息缓存句柄
 * @param[out] data 输出数据数组
 * @param[in] longth 读取长度
 * @return 实际读取长度
 */
uint16_t message_Buffer_GetRear(message_buffer_t *message_buffer_handle, uint8_t *data, uint16_t longth)
{
	uint16_t temp;

	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);

	if (message_buffer_handle->froat == message_buffer_handle->rear)
		return 0;
	temp = message_Buffer_GetLongth(message_buffer_handle);
	if (longth > temp)
		longth = temp;
	if ((int32_t)message_buffer_handle->rear - longth < 0)
	{
		temp = message_buffer_handle->rear;
		if (data != NULL)
		{
			memcpy(data, &message_buffer_handle->buffer, temp);
			memcpy(&data[temp], &message_buffer_handle->buffer[message_buffer_handle->size - longth + temp], longth - temp);
		}
	}
	else if (data != NULL)
		memcpy(data, &message_buffer_handle->buffer[message_buffer_handle->rear - longth], longth);
	return longth;
}

/**
 * @brief 向消息缓存队尾写入数据
 * @param[in] message_buffer_handle 消息缓存句柄
 * @param[out] data 写入数据数组
 * @param[in] longth 写入长度
 */
void message_Buffer_EnQueue(message_buffer_t *message_buffer_handle, uint8_t *data, uint16_t longth)
{
	uint16_t old_longth, temp, rep_longth = 0;

	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);
	assert_param(data != NULL);
	assert_param(rep_longth >= message_buffer_handle->size);

	old_longth = message_Buffer_GetLongth(message_buffer_handle);
	// 剩余长度不足以写入
	if (old_longth + longth >= message_buffer_handle->size)
	{
		// 将缓存填满
		if (message_buffer_handle->rear > message_buffer_handle->froat)
		{
			temp = message_buffer_handle->size - message_buffer_handle->rear;
			memcpy(&message_buffer_handle->buffer[message_buffer_handle->rear], data, temp);
			rep_longth += temp;
			message_buffer_handle->rear = message_buffer_handle->froat - 1;
			memcpy(message_buffer_handle->buffer, &data[rep_longth], message_buffer_handle->rear);
			rep_longth += message_buffer_handle->rear;
		}
		else
		{
			temp = message_buffer_handle->froat - message_buffer_handle->rear - 1;
			memcpy(&message_buffer_handle->buffer[message_buffer_handle->rear], data, temp);
			rep_longth += temp;
			message_buffer_handle->rear += temp;
		}
		// 调用消息缓存全满事件回调
		message_Buffer_Callback(message_buffer_handle, buffer_full_callback);
		// 无论是否处理，直接覆盖
		longth = longth - rep_longth;
		old_longth = message_Buffer_GetLongth(message_buffer_handle);
		if (old_longth + longth >= message_buffer_handle->size)
		{
			message_buffer_handle->froat += old_longth + longth - message_buffer_handle->size + 1;
			if (message_buffer_handle->froat >= message_buffer_handle->size)
				message_buffer_handle->froat -= message_buffer_handle->size;
		}
	}
	// 剩余长度足以写入
	// 处理非循环列表出现循环情况
	if (message_buffer_handle->rear >= message_buffer_handle->froat && message_buffer_handle->rear + longth >= message_buffer_handle->size)
	{
		temp = message_buffer_handle->size - message_buffer_handle->rear;
		memcpy(&message_buffer_handle->buffer[message_buffer_handle->rear], data, temp);
		message_buffer_handle->rear = longth - temp;
		memcpy(message_buffer_handle->buffer, &data[temp], message_buffer_handle->rear);
		message_buffer_handle->rear = longth - temp;
	}
	else
	{
		memcpy(&message_buffer_handle->buffer[message_buffer_handle->rear], data, longth);
		message_buffer_handle->rear += longth;
	}
}

/**
 * @brief 消息缓存队头出队
 * @param[in] message_buffer_handle 消息缓存句柄
 * @param[out] data 输出数据数组(可传NULL只出队，不读取)
 * @param[in] longth 读取长度
 * @return 实际读取长度
 */
uint16_t message_Buffer_DeQueue(message_buffer_t *message_buffer_handle, uint8_t *data, uint16_t longth)
{
	assert_param(message_buffer_handle != NULL);
	assert_param(message_buffer_handle->buffer != NULL);

	longth = message_Buffer_GetFront(message_buffer_handle, data, longth);
	if (longth == 0)
		return 0;
	message_buffer_handle->froat += longth;
	if (message_buffer_handle->froat >= message_buffer_handle->size)
		message_buffer_handle->froat -= message_buffer_handle->size;
	return longth;
}
