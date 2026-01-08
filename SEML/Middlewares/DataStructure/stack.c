#include "stack.h"
#include <stdlib.h>
#include <string.h>

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
SEML_StatusTypeDef InitStack(s_stack *stack, uint16_t elem_size, uint16_t size,void* buffer, stack_full_hander_t full_hander)
{
	assert_param(IS_STACK_FULL_HANDER(full_hander));
	assert_param(stack != NULL);
	assert_param(elem_size > 0);
	assert_param(size > 0);

	stack->elem_size = elem_size;
	stack->size = size;
	stack->top = 0;
	stack->full_hander = full_hander;
	stack->error_code = stack_no_error;
	if(buffer != NULL)
	{
		stack->address = buffer;
		stack->use_extern_buffer = ENABLE;
	}
	else
	{
		stack->address = malloc(stack->size * stack->elem_size);
		stack->use_extern_buffer = DISABLE;
	}

	if (stack->address == NULL)
	{
		stack->error_code = stack_reapply_error;
		return SEML_ERROR;
	}
	return SEML_OK;
}

/**
 * @brief 删除堆栈
 * @param[in,out] stack 栈结构体指针
 */
void DeinitStack(s_stack *stack)
{
	assert_param(stack != NULL);
	assert_param(stack->address != NULL);
	stack->size = 0;
	stack->top = 0;
	stack->elem_size = 0;

	if(stack->use_extern_buffer != DISABLE)
		free(stack->address);
	stack->use_extern_buffer = DISABLE;
	stack->address = NULL;
}

/**
 * @brief 数据弹栈
 * @param[in,out] stack 栈结构体指针
 * @param[out] data 栈顶数据
 * @return 栈执行状态 
 */
SEML_StatusTypeDef Push(s_stack *stack, void *data)
{
	void *new_stack;
	assert_param(stack != NULL);
	assert_param(data != NULL);
	__SEML_LOCK(stack);
	//栈空间不足则重新分配
	if (stack->top >= stack->size)
	{
		switch (stack->full_hander)
		{
		case stack_full_hander_error:
			stack->error_code = stack_full_error;
			__SEML_UNLOCK(stack);
			return SEML_ERROR;
			break;
		case stack_full_hander_reapply:
			stack->size += StackReapplySize * stack->elem_size;
			new_stack = realloc(stack->address, stack->size * stack->elem_size);
			if (new_stack == NULL)
			{
				stack->error_code = stack_reapply_error;
				__SEML_UNLOCK(stack);
				return SEML_ERROR;
			}
			stack->address = new_stack;
			break;
		}
	}
	memcpy(data, (uint8_t *)stack->address + stack->top * stack->elem_size, stack->elem_size);
	stack->top += 1;
	__SEML_UNLOCK(stack);
	return SEML_OK;
}

/**
 * @brief 数据弹栈
 * @param[in,out] stack 栈结构体指针
 * @param[in] data 栈顶数据
 * @return 栈执行状态
 */
SEML_StatusTypeDef Pop(s_stack *stack, void *data)
{
	__SEML_LOCK(stack);
	if (stack->top == 0)//空栈弹栈处理
	{
		stack->error_code = stack_empty_error;
		__SEML_UNLOCK(stack);
		return SEML_ERROR;
	}
	if (data)//对数据为空指针不获取栈顶数据
	{
		memcpy(data, (uint8_t *)stack->address + (stack->top - 1) * stack->elem_size, stack->elem_size);
	}
	stack->top -= 1;//弹栈
	__SEML_UNLOCK(stack);
	return SEML_OK;
}


/**
 * @brief 访问栈顶但不弹栈
 * @param[in] stack 栈结构体指针
 * @param[out] data 栈顶数据
 * @return 栈执行状态
 */
SEML_StatusTypeDef GetStackTop(s_stack *stack, void *data)
{
	__SEML_LOCK(stack);
	if (stack->top == 0)//空栈弹栈处理
	{
		stack->error_code = stack_empty_error;
		__SEML_UNLOCK(stack);
		return SEML_ERROR;
	}
	memcpy(data, (uint8_t *)stack->address + (stack->top - 1) * stack->elem_size, stack->elem_size);
	__SEML_UNLOCK(stack);
	return SEML_OK;
}

/*******************End Of File*****************************/