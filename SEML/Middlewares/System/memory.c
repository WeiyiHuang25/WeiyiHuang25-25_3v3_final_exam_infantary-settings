#include "memory.h"

/**
 * @brief 内存数据复制
 * @param[out] des 目标地址
 * @param[in] src 源地址
 * @param[in] n 复制字节数
 */
void mem_copy(void *des, const void *src, mem_size_t n)
{
	uint16_t n_cpy64 = n >> 3, n_cpy8 = n & 0x07;
	void *des_cpy8 = (uint8_t *)des + n - n_cpy8;
	void *src_cpy8 = (uint8_t *)src + n - n_cpy8;
	for (int i = 0; i < n_cpy64; i++)
		((uint64_t *)des)[i] = ((uint64_t *)src)[i];
	for (int i = 0; i < n_cpy8; i++)
		((uint8_t *)des_cpy8)[i] = ((uint8_t *)src_cpy8)[i];
}

/**
 * @brief 内存数据填充
 * @param[out] des 目标地址
 * @param[in] c 填充字符
 * @param[in] n 填充字节数
 */
void mem_set(void *des, const uint8_t c, mem_size_t n)
{
	for (int i = 0; i < n; i++)
		((uint8_t *)des)[i] = c;
}

/**
 * @brief 内存块节点结构体
 */
typedef struct
{
	uint16_t magic;	 /**< 内存合法性检查变量 */
	mem_size_t prev; /**< 前继节点 */
	mem_size_t next; /**< 后继节点 */
} mem_heap_t;

/**
 * @brief 堆定义结构体
 */
typedef struct
{
	uint8_t *base;		/**< 堆基地址 */
	mem_heap_t *end;	/**< 堆尾节点地址 */
	mem_heap_t *free; /**< 堆空闲节点地址 */
	mem_size_t size;	/**< 堆可用分配字节数 */
} heap_t;
/// @brief 内存合法性魔数
#define HEAP_MAGIC 0x8b2e
/// @brief 内存使用中
#define MEM_USED() ((HEAP_MAGIC) | 0x1)
/// @brief 内存空闲
#define MEM_FREED() ((HEAP_MAGIC) | 0x0)
/// @brief 内存是否在使用中
#define MEM_ISUSED(_mem) (((mem_heap_t *)(_mem))->magic & (~HEAP_MAGIC))
/// @brief 内存合法性检查
#define MEM_CHACK(_mem) (((((mem_heap_t *)(_mem))->magic ^ HEAP_MAGIC) & 0xfffe) == 0x00)
#if USE_MEM_ALIGN != 0
/// @brief 内存是否对齐
#define MEM_IS_ALIGN(addr, align) ((!((uint32_t)addr & (align - 1))) && ((uint32_t)addr != NULL))
/// @brief 向高位取整到对齐尺寸
#define MEM_ALIGN(size, align) (((uint32_t)(size) + (align)-1) & ~((align)-1))
/// @brief 向低位取整到对齐尺寸
#define MEM_ALIGN_DOWN(size, align) ((uint32_t)(size) & ~((align)-1))
#else
/// @brief 内存是否对齐
#define MEM_IS_ALIGN(addr, align) (1U)
/// @brief 向高位取整到对齐尺寸
#define MEM_ALIGN(size, align) ((uint32_t)(size))
/// @brief 向低位取整到对齐尺寸
#define MEM_ALIGN_DOWN(size, align) ((uint32_t)(size))
#endif
/// @brief 内存块节点大小
#define STRUCT_HEAP_SIZE MEM_ALIGN(sizeof(mem_heap_t), MEM_ALIGN_SIZE)
/// @brief 最小已对齐分配尺寸
#define MEM_MIN_SIZE_ALIGN MEM_ALIGN(MEM_MIN_SIZE, MEM_ALIGN_SIZE)
/// @brief 获取内存偏移值
#define MEM_OFFSET(_mem) (mem_size_t)((uint8_t *)(_mem)-heap->base)
/// @brief 分配出去的内存大小
#define MEM_SIZE(_mem) \
	((mem_size_t)((mem_heap_t *)(_mem))->next - MEM_OFFSET(_mem) - STRUCT_HEAP_SIZE)
/// @brief 当前使用中的堆指针
static heap_t *_heap;

/**
 * @brief 初始化内存管理
 * @param[in,out] buffer 缓存数组
 * @param[in] size 缓存数组字节数
 * @return 初始化后的堆句柄
 */
uint32_t _mem_init(void *buffer, mem_size_t size)
{
	heap_t *heap = NULL;
	uint32_t start_addr, begin_align, end_align, mem_size;
	mem_heap_t *mem;
	__SEML_ENTER_CRITICAL_SECTION();
	// 分配给堆头
	heap = (heap_t *)MEM_ALIGN(buffer, MEM_ALIGN_SIZE);
	start_addr = (uint32_t)heap + sizeof(heap_t);
	// 进行内存对齐
	begin_align = MEM_ALIGN(start_addr, MEM_ALIGN_SIZE);
	end_align = MEM_ALIGN_DOWN((uint32_t)buffer + size, MEM_ALIGN_SIZE);
	// 对剩余内存进行检查是否足够分配堆管理链表
	if (end_align < begin_align + 2 * STRUCT_HEAP_SIZE)
	{
		__SEML_LEAVE_CRITICAL_SECTION();
		return NULL;
	}
	mem_size = end_align - begin_align - 2 * STRUCT_HEAP_SIZE;
	// 设置头管理节点
	mem = (mem_heap_t *)begin_align;
	mem->prev = 0;
	mem->next = mem_size + STRUCT_HEAP_SIZE;
	mem->magic = MEM_FREED();
	// 设置尾管理节点
	mem = (mem_heap_t *)(end_align - STRUCT_HEAP_SIZE);
	mem->prev = mem_size + STRUCT_HEAP_SIZE;
	mem->next = mem_size + STRUCT_HEAP_SIZE;
	mem->magic = MEM_USED();
	// 设置堆句柄
	heap->base = (uint8_t *)begin_align;
	heap->size = mem_size;
	heap->free = (mem_heap_t *)begin_align;
	heap->end = mem;
	__SEML_LEAVE_CRITICAL_SECTION();
	return (uint32_t)heap;
}

/**
 * @brief 裁切空闲内存块
 * @param[in] hander 堆句柄
 * @param[in] ptr 内存管理节点所在偏移量
 * @param[in] size 分配尺寸
 */
static void _mem_crop_free(uint32_t hander, mem_size_t ptr, mem_size_t size)
{
	heap_t *heap = (heap_t *)hander;
	mem_heap_t *mem = (mem_heap_t *)&heap->base[ptr], *mem2;
	mem_size_t ptr2;
	assert_param(!MEM_ISUSED(mem));
	// 剩余空间足够进行分割
	if (mem->next - (ptr + STRUCT_HEAP_SIZE) >= size + STRUCT_HEAP_SIZE + MEM_MIN_SIZE_ALIGN)
	{
		// 设置分割后的堆管理节点
		ptr2 = ptr + STRUCT_HEAP_SIZE + size;
		mem2 = (mem_heap_t *)&heap->base[ptr2];
		mem2->prev = (mem_size_t)ptr;
		mem2->next = mem->next;
		mem2->magic = MEM_FREED();
		// 向链表插入
		mem->next = MEM_OFFSET(mem2);
		if (mem2->next != heap->size + STRUCT_HEAP_SIZE)
			((mem_heap_t *)&heap->base[mem2->next])->prev = MEM_OFFSET(mem2);
	}
	mem->magic = MEM_USED();
	// 若空闲节点被分配,则设置到下一个空闲节点
	if (mem == heap->free)
	{
		while (MEM_ISUSED(heap->free) && heap->free != heap->end)
			heap->free = (mem_heap_t *)&heap->base[heap->free->next];
		assert_param(!MEM_ISUSED(heap->free));
	}
}

/**
 * @brief 申请内存
 * @param[in] hander 堆句柄
 * @param[in] size 申请的字节数
 * @return 申请得到的内存指针,申请失败返回NULL
 */
void *_mem_malloc(uint32_t hander, mem_size_t size)
{
	heap_t *heap = (heap_t *)hander;
	mem_heap_t *mem;
	mem_size_t ptr1;
	if (size == 0)
		return NULL;
	// 进行内存对齐
	size = MEM_ALIGN(size, MEM_ALIGN_SIZE);
	// 申请内存大于实际内存
	if (size > heap->size)
		return NULL;
	__SEML_ENTER_CRITICAL_SECTION();
	for (ptr1 = (mem_size_t)((uint8_t *)heap->free - heap->base);
			 ptr1 <= heap->size - size;
			 ptr1 = ((mem_heap_t *)&heap->base[ptr1])->next)
	{
		// 节点遍历与合法性检查
		mem = (mem_heap_t *)&heap->base[ptr1];
		// if(MEM_CHACK(mem))
		//     return NULL;
		assert_param(MEM_CHACK(mem));
		// 首次适配分配算法
		if (!MEM_ISUSED(mem) && (mem->next - (ptr1 + STRUCT_HEAP_SIZE)) >= size)
		{
			_mem_crop_free(hander, ptr1, size);
			__SEML_LEAVE_CRITICAL_SECTION();
			assert_param((uint8_t *)mem + STRUCT_HEAP_SIZE + size <= (uint8_t *)heap->end);
			assert_param(MEM_IS_ALIGN((uint8_t *)mem + STRUCT_HEAP_SIZE, MEM_ALIGN_SIZE));
			return (void *)((uint8_t *)mem + STRUCT_HEAP_SIZE);
		}
	}
	__SEML_LEAVE_CRITICAL_SECTION();
	return NULL;
}

/**
 * @brief 释放内存
 * @param[in] hander 堆句柄
 * @param[in] ptr 要释放的指针
 */
void _mem_free(uint32_t hander, void *ptr)
{
	heap_t *heap = (heap_t *)hander;
	mem_heap_t *mem = (mem_heap_t *)((uint8_t *)ptr - STRUCT_HEAP_SIZE);
	if (ptr == NULL)
		return;
	// 参数合法性检查
	assert_param(MEM_ISUSED(mem));
	// 内存合法性检查
	assert_param(ptr >= (void *)heap->base && ptr < (void *)heap->end);
	assert_param(MEM_CHACK(mem));
	assert_param(MEM_CHACK(&heap->base[mem->next]));
	__SEML_ENTER_CRITICAL_SECTION();
	mem->magic = MEM_FREED();
	// 后继节点空闲进行合并
	if (!MEM_ISUSED(&heap->base[mem->next]))
	{
		mem->next = ((mem_heap_t *)&heap->base[mem->next])->next;
		((mem_heap_t *)&heap->base[mem->next])->prev = MEM_OFFSET(mem);
	}
	// 前继节点空闲进行合并
	if (!MEM_ISUSED(&heap->base[mem->prev]))
	{
		((mem_heap_t *)&heap->base[mem->prev])->next = mem->next;
		((mem_heap_t *)&heap->base[mem->next])->prev = mem->prev;
		mem = (mem_heap_t *)&heap->base[mem->prev];
	}
	// 更新空闲节点指针
	if (heap->free > mem)
		heap->free = mem;
	__SEML_LEAVE_CRITICAL_SECTION();
}

/**
 * @brief 重新申请内存
 * @param[in] hander 堆句柄
 * @param[in] ptr 要重新申请内存指针
 * @param[in] newsize 重新申请的内存大小
 * @return 申请得到的内存指针,申请失败或者被释放返回NULL
 */
void *_mem_realloc(uint32_t hander, void *ptr, mem_size_t newsize)
{
	heap_t *heap = (heap_t *)hander;
	mem_heap_t *mem = (mem_heap_t *)((uint8_t *)ptr - STRUCT_HEAP_SIZE), *mem2;
	mem_size_t size;
	void *ptr1 = NULL;
	// 参数合法性检查
	assert_param(MEM_ISUSED(mem));
	// 内存合法性检查
	assert_param(ptr >= (void *)heap->base && ptr < (void *)heap->end);
	assert_param(MEM_CHACK(mem));
	assert_param(MEM_CHACK(&heap->base[mem->next]));
	size = MEM_SIZE(mem);
	if (newsize == 0)
	{
		_mem_free(hander, ptr);
		return NULL;
	}
	if (ptr == NULL)
		return _mem_malloc(hander, newsize);
	// 进行内存对齐
	newsize = MEM_ALIGN(newsize, MEM_ALIGN_SIZE);
	__SEML_ENTER_CRITICAL_SECTION();
	if (newsize <= size)
	{
		// 释放多余内存
		if (size - newsize >= STRUCT_HEAP_SIZE + MEM_MIN_SIZE_ALIGN)
		{
			mem2 = (mem_heap_t *)((uint8_t *)ptr + newsize);
			mem2->next = mem->next;
			mem2->prev = MEM_OFFSET(mem);
			mem2->prev = MEM_FREED();
			// 更新前后节点
			mem->next = MEM_OFFSET(mem2);
			((mem_heap_t *)&heap->base[mem2->next])->prev = mem->next;
		}
		__SEML_LEAVE_CRITICAL_SECTION();
		return ptr;
	}
	// 后面内存块可以进行分配
	if (!MEM_ISUSED(&heap->base[mem->next]) && MEM_SIZE(&heap->base[mem->next]) + STRUCT_HEAP_SIZE >= newsize - size)
	{
		// 这里会让内存多分配12字节,可以改成其他分配方式
		_mem_crop_free(hander, mem->next, newsize - size);
		// 合并后继节点
		mem->next = ((mem_heap_t *)&heap->base[mem->next])->next;
		((mem_heap_t *)&heap->base[mem->next])->prev = MEM_OFFSET(mem);
		__SEML_LEAVE_CRITICAL_SECTION();
		assert_param((uint8_t *)mem + STRUCT_HEAP_SIZE + newsize <= (uint8_t *)heap->end);
		assert_param(MEM_IS_ALIGN((uint8_t *)mem + STRUCT_HEAP_SIZE, MEM_ALIGN_SIZE));
		return ptr;
	}
	__SEML_LEAVE_CRITICAL_SECTION();
	// 重新申请内存,将数据拷贝到新内存
	ptr1 = _mem_malloc(hander,newsize);
	if (ptr1 != NULL)
	{
		mem_copy(ptr1, ptr, size);
		_mem_free(hander, ptr);
	}
	return ptr1;
}

/**
 * @brief 初始化内存管理
 * @param[in] size 缓存数组字节数
 * @return 初始化后的堆句柄
 */
uint32_t mem_init(void *buffer, mem_size_t size)
{
	_heap = (heap_t*)_mem_init(buffer,size);
	return (uint32_t)_heap;
}

/**
 * @brief 申请内存
 * @param[in] size 申请的字节数
 * @return 申请得到的内存指针,申请失败返回NULL
 */
void *mem_malloc(mem_size_t size)
{
	return _mem_malloc((uint32_t)_heap,size);
}

/**
 * @brief 重新申请内存
 * @param[in] ptr 要重新申请内存指针
 * @param[in] newsize 重新申请的内存大小
 * @return 申请得到的内存指针,申请失败或者被释放返回NULL
 */
void *mem_realloc(void *ptr, mem_size_t newsize)
{
	return _mem_realloc((uint32_t)_heap,ptr,newsize);
}

/**
 * @brief 释放内存
 * @param[in] ptr 要释放的指针
 */
void mem_free(void *ptr)
{
	_mem_free((uint32_t)_heap,ptr);
}
