#include "Virtualmemory.h"
#include <stdio.h>
Fileblocks* fileblocks;

void Save() {
	Block* itor = safe_malloc_set(sizeof(Block));
	itor->nextBlock = 0;
	FILE *fp;
	fp = fopen("Memory.txt", "a");
	if (fp) {
		while (itor->nextBlock != NULL) {
			fwrite(itor, sizeof(Block), 1, fp);
			itor = itor->nextBlock;
		}
		fclose(fp);
	}

}
void Load() {
	Block* itor = safe_malloc_set(sizeof(Block));
	FILE *fp;
	fp = fopen("Memory.txt", "a");

	if (fp) {
		int i = 0;
		for (i = 0; i < fileblocks->elements; i++) {
			fwrite(itor, sizeof(Block), 1, fp);
			itor = itor->nextBlock;
		}
		fclose(fp);
		fileblocks->firstblock = itor;
	}

}
Block* FindBlock(int size, struct BlockAlloc* Alloc) {
	Block* Conductor = Alloc->firstBlock;
	while (Conductor->nextBlock != NULL)
	{
		if (Conductor->size == size) {
			return Conductor;
		}
		Conductor = Conductor->nextBlock;
	}
	return NULL;
}
void SwapBlocks(int size, struct BlockAlloc* Alloc) {
	Block* block = FindBlock(size, Alloc);
	BlockMapNode* node = GetMapNode(block);
	SendBlockTodisk(block);
	node->realblock = NULL;
	if (block->PreviousBlock != NULL) {
		block->PreviousBlock->nextBlock = block->nextBlock;
	}
	if (block->nextBlock != NULL) {
		block->nextBlock->PreviousBlock = block->PreviousBlock;
	}
	if (block == Alloc->firstBlock) {
		Alloc->firstBlock = block->nextBlock;
	}
	if (block == Alloc->lastBlock) {
		Alloc->lastBlock = block->PreviousBlock;
	}
	AddEmptyBlock(CreateEmptyBlock(size, block->offset));

}

void SendBlockTodisk(Block * block)
{
	if (fileblocks == NULL) {
		fileblocks = safe_malloc_set(sizeof(Block));
		fileblocks->firstblock = 0;
	}
	//WARNING: for speed no Checking that the block does not already exist is performed
	if (fileblocks->firstblock) {
		fileblocks->firstblock->nextBlock = block;
		//Previous block is Unused Right Now
	}
	else
	{
		fileblocks->firstblock = block;
	}
	Save();
}
Block * GetBlockFromDisk(int id, struct BlockAlloc* alloc)
{
	Load();
	Block* itor = safe_malloc_set(sizeof(Block));
	itor->nextBlock = 0;
	Block* preitor = 0;
	itor = fileblocks->firstblock;
	preitor = fileblocks->firstblock;
	while (itor->nextBlock != NULL) {
		if (itor->id == id) {
			preitor->nextBlock = itor->nextBlock;
			//remove this block from the link
			alloc->lastBlock->nextBlock = itor;
			itor->PreviousBlock = alloc->lastBlock;
			//and add it to the end of blocks
			printf("retuning block %p of id %d size: %d offset: %d\n", itor, itor->id, itor->size, itor->offset);
			return itor;
		}
		preitor = itor;
		itor = itor->nextBlock;
	}
	return NULL;
}
