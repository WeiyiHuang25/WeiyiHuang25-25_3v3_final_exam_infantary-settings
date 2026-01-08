#include "test_memory.h"

#include "main.h"
#include "memory.h"
uint8_t buffer[65535];
uint32_t handle;
uint8_t *ptr[100];

uint16_t alloc_size[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

void test_memory(void)
{

	handle = mem_init(buffer, 65536);
	for (int o = 0; o < 100; o++)
	{
		printf("memory test start loop %d\n", o);
		printf("memory alloc test start\n");
		for (int i = 0; i < 100; i++)
		{
			ptr[i] = (uint8_t *)mem_malloc(alloc_size[i % 10]);
			if (ptr[i] == NULL)
				printf("memory alloc error at %d\n", i);

			for (int j = 0; j < alloc_size[i % 10]; j++)
			{
				ptr[i][j] = i;
			}
		}
		for (int i = 0; i < 100; i++)
		{
			for (int j = 0; j < alloc_size[i % 10]; j++)
			{
				if (ptr[i][j] != i)
				{
					printf("memory read error occurred at %d %d\n", i, j);
				}
			}
		}
		printf("memory alloc test done\n");

		printf("memory free test start\n");
		for (int i = 1; i < 100; i += 2)
		{
			mem_free(ptr[i]);
			ptr[i] = NULL;
		}
		for (int i = 0; i < 100; i++)
		{
			for (int j = 0; j < alloc_size[i % 10]; j++)
			{
				if (ptr[i] == NULL)
					continue;
				if (ptr[i][j] != i)
				{
					printf("memory read error occurred at %d %d\n", i, j);
				}
			}
		}
		printf("memory free test done\n");

		printf("memory alloc test restart\n");
		for (int i = 1; i < 100; i += 2)
		{
			ptr[i] = (uint8_t *)mem_malloc(alloc_size[(i + 6) % 10]);
			if (ptr[i] == NULL)
				printf("memory alloc error\n");

			for (int j = 0; j < alloc_size[(i + 6) % 10]; j++)
			{
				ptr[i][j] = i + 3;
			}
		}
		for (int i = 1; i < 100; i += 2)
		{
			for (int j = 0; j < alloc_size[(i + 6) % 10]; j++)
			{
				if (ptr[i][j] != i + 3)
				{
					printf("memory read error occurred at %d %d,Originally %d read as%d\n", i, j, i + 3, ptr[i][j]);
				}
			}
		}
		printf("memory alloc test done\n");

		printf("memory realloc test start\n");
		uint8_t *temp = NULL;
		for (int i = 0; i < 100; i += 2)
		{
			temp = (uint8_t *)mem_realloc(ptr[i], alloc_size[i % 10] + 10);
			if (temp != NULL)
				ptr[i] = temp;
			else
				printf("memory realloc error at %d\n", i);
			for (int j = alloc_size[i % 10]; j < alloc_size[i % 10] + 10; j++)
			{
				ptr[i][j] = 5;
			}
			for (int j = 0; j < alloc_size[i % 10]; j++)
			{
				if (ptr[i][j] != i)
				{
					printf("memory read error occurred at %d %d,Originally %d read as%d\n", i, j, i + 3, ptr[i][j]);
				}
			}
			for (int j = alloc_size[i % 10]; j < alloc_size[i % 10] + 10; j++)
			{
				if (ptr[i][j] != 5)
				{
					printf("memory read error occurred at %d %d,Originally %d read as%d\n", i, j, i + 3, ptr[i][j]);
				}
			}
		}
		for (int i = 1; i < 100; i += 2)
		{
			for (int j = 0; j < alloc_size[(i + 6) % 10]; j++)
			{
				if (ptr[i][j] != i + 3)
				{
					printf("memory read error occurred at %d %d,Originally %d read as%d\n", i, j, i + 3, ptr[i][j]);
				}
			}
		}
		printf("memory realloc test done\n");
		printf("memory free test start\n");
		for (int i = 1; i < 100; i += 2)
		{
			mem_free(ptr[i]);
			ptr[i] = NULL;
		}
		for (int i = 0; i < 100; i++)
		{
			for (int j = 0; j < alloc_size[i % 10]; j++)
			{
				if (ptr[i] == NULL)
					continue;
				if (ptr[i][j] != i)
				{
					printf("memory read error occurred at %d %d\n", i, j);
				}
			}
		}
		printf("memory free test done\n");

		printf("memory alloc test restart\n");
		for (int i = 1; i < 100; i += 2)
		{
			ptr[i] = (uint8_t *)mem_malloc(alloc_size[(i + 6) % 10]);
			if (ptr[i] == NULL)
				printf("memory alloc error\n");

			for (int j = 0; j < alloc_size[(i + 6) % 10]; j++)
			{
				ptr[i][j] = i + 3;
			}
		}
		for (int i = 1; i < 100; i += 2)
		{
			for (int j = 0; j < alloc_size[(i + 6) % 10]; j++)
			{
				if (ptr[i][j] != i + 3)
				{
					printf("memory read error occurred at %d %d,Originally %d read as%d\n", i, j, i + 3, ptr[i][j]);
				}
			}
		}
		printf("memory alloc test done\n");
		for (int i = 0; i < 100; i ++)
		{
			mem_free(ptr[i]);
			ptr[i] = NULL;
		}
	}


	return 0;
}