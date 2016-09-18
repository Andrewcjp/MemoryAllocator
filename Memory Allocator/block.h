#ifndef INC_BLOCK_H
#define INC_BLOCK_H
#include "Pool.h"
#include <memory.h>
#include "Virtualmemory.h"
void *safe_malloc_set(int n);

/* Reserve a block of memory in a pool */
Block* allocate(Pool* pool, int size);

/* Write to a block of memory obtained via allocate() */
void write(Block* block, void *object, int objectSize, int offsetInBlock);

// Read from a block of memory obtained via allocate()
void* read(Block* block, int objectSize, int offsetInBlock);

// Free up a block of memory obtained via allocate()
void deallocate(Block* block);
// Additional Api Call for Defragmentation
void DefragmentDataBlocks();
//For Virtual memory
BlockMapNode* GetMapNode(Block* token);
void AddEmptyBlock(EmptyBlock* eblock);
EmptyBlock* CreateEmptyBlock(int size, int offset);
#endif