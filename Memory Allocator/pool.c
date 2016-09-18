#include "pool.h"

Pool* allocatePool(int n)
{
	Pool* pool = NULL;
	if (n > 0)
	{
		pool = (Pool*)malloc(sizeof(Pool));
		if (pool != NULL)
		{
			pool->Blockalloc = 0;
			pool->memory = malloc(sizeof(char)*n);
			if (pool->memory)
			{
				pool->size = n;
			}
			else
			{
				free(pool);
				pool = NULL;
			}
		}
	}
	return pool;
}

void freePool(Pool* pool)
{
	if (pool)
	{
		free(pool->memory);
		free(pool);
	}
}

void store(Pool* pool, int offset, int size, void *object)
{
	if (pool && offset >= 0 && offset + size <= pool->size)
	{
		memcpy((char*)pool->memory + offset, object, size);
	}
	else 
	{
		printf("pool: Failed to store in pool\n");
	}
}

void *retrieve(Pool* pool, int offset, int size)
{
	if (pool && offset >= 0 && offset + size <= pool->size)
	{
		return (char*)pool->memory + offset;
	}
	return NULL;
}