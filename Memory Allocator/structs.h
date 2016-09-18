#ifndef __STRUCTS_H
#define __STRUCTS_H
typedef struct _POOL Pool;
typedef struct _BLOCK
{
	int size;
	int offset;
	void *memory;
	struct _BLOCK *nextBlock;
	struct _BLOCK *PreviousBlock;
	int id;
} Block;

typedef struct _EmptyBlock {
	int size;
	int offset;
	struct _EmptyBlock *nextBlock;
	struct _EmptyBlock *PreviousBlock;
}EmptyBlock;

typedef struct _BlockMapNode {
	int size;
	int Blocktoken;
	struct _BlockMapNode* nextBlock;
	struct _BlockMapNode* previousBlock;
	struct _BLOCK *realblock;
	int allocationtime;//a TimeStap in allocations For when this block was created
}BlockMapNode;
struct BlockAlloc {
	Block* firstBlock;
	Block* lastBlock;
	EmptyBlock* FirstEmptyblock;
	EmptyBlock* LastEmptyblock;
	BlockMapNode* FirstMapNode;
	BlockMapNode* LastMapNode;
	Pool* poolptr;
};
struct _POOL
{
	int size;
	void* memory;
	struct BlockAlloc* Blockalloc;
};


#endif