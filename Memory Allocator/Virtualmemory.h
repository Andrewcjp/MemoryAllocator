#ifndef INC_VM_H
#define INC_VM_H
#include "block.h"
void SendBlockTodisk(Block* block);
Block * GetBlockFromDisk(int id, struct BlockAlloc* alloc);
typedef struct _FileBlocks {
	Block* firstblock;
	Block* lastblock;
	int elements;
}Fileblocks;
void SwapBlocks(int size, struct BlockAlloc* Alloc);
//The Virtual Memory system
//This allows blocks to be written to disk and retreved again
//Currently it picks the first block in the block list
//However it could be upgraded to use the allocation time to decide which block is best to remove
#endif
