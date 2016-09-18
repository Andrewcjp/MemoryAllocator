#include "Block.h"
#define TRUE 1
#define FALSE 0
struct BlockAlloc* blockalloc;
int SizeOfPool = 0;
int AllocationTimer = 1;
// This Variable Enables the use of the Virtual Memory System (VMS)
int Usevms = TRUE;
void DefragmentEmpty();
void print_allocation_table();
void print_empty_allocation_table();
Block* GetBlock(Block* token);
void RemoveMapNode(Block* block);
void *safe_malloc(size_t n)
{
	void *p = malloc(n);
	if (p) {
		return p;
	}
	else {
		fprintf(stderr, "Fatal: Failed to allocate %zu bytes.\n", n);
		return NULL;
	}
}
void *safe_malloc_set(int n) {
	void* p = 0;
	if (n > 0) {
		p = safe_malloc(n);
		/*initilise all new memory to 0*/
		if (p) {
			memset(p, 0, n);
		}
	}
	else {
		printf("Fatal: Attempt to allocate a stupid number of Bytes\n");
		return NULL;
	}
	return p;
}
EmptyBlock* CreateEmptyBlock(int size, int offset) {
	EmptyBlock* EB = safe_malloc_set(sizeof(EmptyBlock));
	EB->offset = offset;
	EB->size = size;
	EB->nextBlock = 0;
	EB->PreviousBlock = 0;
	return EB;
}
void RemoveEmptyBlock(EmptyBlock* block) {
	if (block) {
		if (block == blockalloc->LastEmptyblock) {
			blockalloc->LastEmptyblock = 0;
		}
		if (block == blockalloc->FirstEmptyblock) {
			//this means we are the first block
			if (block->nextBlock == NULL) {
				blockalloc->FirstEmptyblock = NULL;
				blockalloc->LastEmptyblock = NULL;
			}
			else
			{
				blockalloc->FirstEmptyblock = block->nextBlock;
				block->nextBlock->PreviousBlock = 0;
			}
		}
		if (block == blockalloc->LastEmptyblock && block->nextBlock == NULL) {
			blockalloc->LastEmptyblock = 0;
			blockalloc->FirstEmptyblock = 0;
		}
		else if (block->nextBlock == NULL && block->PreviousBlock != NULL) {
			//end of the list
			block->PreviousBlock->nextBlock = 0;
			blockalloc->LastEmptyblock = block->PreviousBlock;
		}
		else if (block->nextBlock != NULL && block->PreviousBlock != NULL) {
			//We are in the centre of our list
			block->PreviousBlock->nextBlock = block->nextBlock;
			block->nextBlock->PreviousBlock = block->PreviousBlock;
		}
		free(block);
	}
}
void RemoveBlock(Block* block) {
	if (block == blockalloc->firstBlock) {
		//This means we are the first block
		if (block->nextBlock == NULL) {
			blockalloc->firstBlock = NULL;
			blockalloc->lastBlock = NULL;
		}
		else
		{
			blockalloc->firstBlock = block->nextBlock;
			block->nextBlock->PreviousBlock = 0;
		}
	}
	else if (block->nextBlock == NULL&& block->PreviousBlock != NULL) {
		//At the end of the list
		block->PreviousBlock->nextBlock = 0;
		blockalloc->lastBlock = block->PreviousBlock;
	}
	else if (block->nextBlock != NULL && block->PreviousBlock != NULL) {
		//We are in the centre of our list
		block->PreviousBlock->nextBlock = block->nextBlock;
		block->nextBlock->PreviousBlock = block->PreviousBlock;
	}
	block = NULL; //finally ensure that we have ensure that that block is dead, gone and turned to dust
}
void Initilise(Pool* pool) {
	//Initalise the block allocation listing for allocated and unallocated space
	pool->Blockalloc = safe_malloc_set(sizeof(struct BlockAlloc));
	pool->Blockalloc->FirstEmptyblock = CreateEmptyBlock(pool->size, 0);
	pool->Blockalloc->LastEmptyblock = pool->Blockalloc->FirstEmptyblock;
	pool->Blockalloc->firstBlock = 0;
	pool->Blockalloc->lastBlock = 0;
	blockalloc = pool->Blockalloc;
}

EmptyBlock* GetLastEmptyBlock(int size) {
	if (blockalloc->LastEmptyblock) {
		if (blockalloc->LastEmptyblock->size >= size) {
			return blockalloc->LastEmptyblock;
		}
		else
		{
			EmptyBlock* condutor = blockalloc->FirstEmptyblock;
			while (condutor->nextBlock != 0)
			{
				if (condutor->size >= size) {
					return condutor;
				}
				condutor = condutor->nextBlock;
			}
		}
	}
	return NULL;
}
/*Adds an Unlinked Block to the EmptyBlock List*/
void AddEmptyBlock(EmptyBlock* eblock) {
	EmptyBlock* eb = blockalloc->LastEmptyblock;
	if (eb) {
		if (eblock->size > eb->size) {
			//Block is smaller
			if (eb->PreviousBlock == NULL) {
				//the eb is the first block
				blockalloc->FirstEmptyblock = eblock;
				eb->PreviousBlock = eblock;
				eblock->nextBlock = eb;
				eblock->PreviousBlock = NULL;
			}
			//insert in the list before eb
			eb->PreviousBlock->nextBlock = eblock;
			eb->PreviousBlock = eblock;
			eblock->nextBlock = eb;
			eblock->PreviousBlock = eb->PreviousBlock;
		}
		else if (eblock->size < eb->size) {
			//add on to the end
			eb->nextBlock = eblock;
			eblock->PreviousBlock = eb;
			blockalloc->LastEmptyblock = eb;
		}
		else if (eblock->size == eb->size) {
			//add on to the end
			eb->nextBlock = eblock;
			eblock->PreviousBlock = eb;
			blockalloc->LastEmptyblock = eblock;
		}
	}
	else
	{
		blockalloc->FirstEmptyblock = eblock;
		blockalloc->LastEmptyblock = eblock;
	}
}
BlockMapNode* GetMapNode(Block* token) {
	BlockMapNode* condutor = blockalloc->FirstMapNode;
	if (condutor) {
		while (condutor->nextBlock != NULL) {
			if (condutor->Blocktoken == token->id) {
				if (token->size != condutor->size) {
					printf("MAJOR ERROR: Size Validation on Mapped Block FAILED\n");
				}
				return condutor;//return the block we are looking for!
			}
			condutor = condutor->nextBlock;
		}
		if (condutor->Blocktoken == token->id) {
			if (token->size != condutor->size) {
				printf("MAJOR ERROR: Size Validation on Mapped Block FAILED\n");
			}
			return condutor;//return the block we are looking for!
		}
	}
	return NULL;
}
Block* GetBlock(Block* token) {
	BlockMapNode* block = GetMapNode(token);
	if (block) {
		if (block->realblock == NULL) {
			GetBlockFromDisk(block->Blocktoken, blockalloc);
		}
		return block->realblock;
	}
	return NULL;
}
void RemoveMapNode(Block * block)
{
	//Map Nodes Exist Outside of the Primary Allocation Space Defined By the Pool Struture
	BlockMapNode* node = GetMapNode(block);
	if (node->nextBlock != NULL && node->previousBlock != NULL) {
		//This node is in the middle of the  list
		node->nextBlock->previousBlock = node->previousBlock;
		node->previousBlock->nextBlock = node->nextBlock;
		//UnLink this node from the list
	}
	else if (node->nextBlock == NULL && node->previousBlock != NULL) {
		node->previousBlock->nextBlock = 0;
	}
	else if (node->nextBlock != NULL && node->previousBlock == NULL) {
		blockalloc->FirstMapNode = node->nextBlock;
		node->nextBlock->previousBlock = 0;
	}
	else
	{
		blockalloc->FirstMapNode = 0;
	}
	free(node);
}
Block* AddToMap(Block* block) {
	if (block != NULL && GetBlock(block) == NULL)
	{
		BlockMapNode* Mapnode = safe_malloc_set(sizeof(BlockMapNode));
		if (blockalloc->FirstMapNode) {
			BlockMapNode* conductor = blockalloc->FirstMapNode;
			while (conductor->nextBlock != NULL)
			{
				conductor = conductor->nextBlock;
			}
			conductor->nextBlock = Mapnode;
			Mapnode->previousBlock = conductor;
		}
		else
		{
			blockalloc->FirstMapNode = Mapnode;
		}
		Mapnode->realblock = block;
		Block* tokenblock = safe_malloc_set(sizeof(Block));
		//ensure the node, token and real block all have matching id's and sizes
		Mapnode->size = block->size;
		tokenblock->size = block->size;
		Mapnode->Blocktoken = AllocationTimer + block->offset + block->size;
		tokenblock->id = AllocationTimer + block->offset + block->size;
		block->id = AllocationTimer + block->offset + block->size;
		Mapnode->allocationtime = AllocationTimer;
		return tokenblock;
	}
	return NULL;
}
/*Create a new block and link into the allocation list*/
Block* CreateNewBlock(int size, Pool* pool) {
	EmptyBlock* currentblock = GetLastEmptyBlock(size);
	blockalloc->poolptr = pool;
	if (currentblock == NULL) {
		printf("ERROR: No free space\n");
		return NULL;
	}
	else if (currentblock->size < size) {
		printf("ERROR: No space big enough left\n");
		printf("%d != %d\n", currentblock->size, size);
		return NULL;
	}
	Block* Newblock = safe_malloc_set(sizeof(Block));
	Newblock->memory = safe_malloc(sizeof(char)*size); // initilise the memory before it is stored in the pool
	Newblock->offset = 0;
	Newblock->size = size;
	//We Store the memory in the pool but not the block struct as we want to be able to use 100% of the pool for data
	store(pool, currentblock->offset, size, Newblock->memory);
	Newblock->memory = retrieve(pool, currentblock->offset, size);//store then link
	if (Newblock == NULL) {
		printf("ERROR: Failed to store in pool\n");
		return NULL;
	}
	Newblock->size = size;
	Newblock->offset = currentblock->offset;
	if (currentblock->size - size < 1) {
		//The block is too small
		RemoveEmptyBlock(currentblock);
	}
	else
	{
		//TODO: OffsetBoundsChecking?
		currentblock->offset += size;//shift the empty block along
		currentblock->size -= size;//Reduce the size
	}
	Newblock->nextBlock = 0;
	if (blockalloc->firstBlock == NULL) {
		Newblock->nextBlock = 0;
		blockalloc->firstBlock = Newblock;
		blockalloc->lastBlock = Newblock;
	}
	else
	{
		//link to the list of allocated blocks
		blockalloc->lastBlock->nextBlock = Newblock;
		Newblock->PreviousBlock = blockalloc->lastBlock;
		blockalloc->lastBlock = Newblock;
	}
	return Newblock;
}

void print_allocation_table() {
	Block* Conductor = blockalloc->firstBlock;
	printf("-Table Of Allocations-\n");
	printf("|  Block |Size|Offset|\n");
	if (Conductor) {
		while (Conductor->nextBlock != NULL)
		{
			printf("|%p| %3d|  %3d |\n", Conductor, Conductor->size, Conductor->offset);
			Conductor = Conductor->nextBlock;
		}
		printf("|%p| %3d|  %3d |\n", Conductor, Conductor->size, Conductor->offset);
	}
}
void print_empty_allocation_table() {
	EmptyBlock* Conductor = blockalloc->FirstEmptyblock;
	printf("-Table empty Allocations-\n");
	printf("|  Block |Size|Offset|\n");
	if (Conductor) {
		while (Conductor->nextBlock != NULL)
		{
			printf("|%p| %3d|  %3d |\n", Conductor, Conductor->size, Conductor->offset);
			Conductor = Conductor->nextBlock;
		}
		printf("|%p| %3d|  %3d |\n", Conductor, Conductor->size, Conductor->offset);
	}
}
/* Reserve a block of memory in a pool */
Block* allocate(Pool* pool, int size) {
	if (size < 0 || pool == NULL) {
		return NULL;
	}
	Block* newblock = 0;
	if (SizeOfPool < pool->size && blockalloc) {
		AddEmptyBlock(CreateEmptyBlock(pool->size - SizeOfPool, SizeOfPool));//increase the pool size
	}
	SizeOfPool = pool->size;
	if (pool->Blockalloc == NULL) {//|| blockalloc == NULL
		//Run the First time allocate is called to set everything up!
		Initilise(pool);
		newblock = CreateNewBlock(size, pool);
		blockalloc->firstBlock = newblock;
	}
	else
	{
		newblock = CreateNewBlock(size, pool);
	}
	if (newblock == NULL && Usevms == TRUE) {
		SwapBlocks(size, blockalloc);
	}
	AllocationTimer++;
	return AddToMap(newblock);
}
/* Write to a block of memory obtained via allocate() */
void write(Block* inblock, void *object, int objectSize, int offsetInBlock) {
	Block* block = GetBlock(inblock);
	if (block == NULL) {
		fprintf(stderr, "ERROR: Block is null\n");
		return;
	}
	if (offsetInBlock + objectSize > block->size) {
		fprintf(stderr, "ERROR: Attempted to store a value out of bounds\n");
		return;
	}
	if (objectSize <= 0) {
		fprintf(stderr, "ERROR: Attempted to store a value of less than 1 length\n");
		return;
	}
	if (offsetInBlock < 0) {
		fprintf(stderr, "ERROR : offset cannot be negative\n");
		return;
	}
	memcpy((char *)block->memory + offsetInBlock, object, objectSize);
}
// Read from a block of memory obtained via allocate()
void* read(Block* inblock, int objectSize, int offsetInBlock) {
	Block* block = GetBlock(inblock);
	if (block == NULL) {
		fprintf(stderr, "ERROR: Block is null\n");
		return NULL;
	}
	if (offsetInBlock < 0) {
		fprintf(stderr, "ERROR : offset cannot be negative\n");
		return NULL;
	}
	if (objectSize <= 0) {
		fprintf(stderr, "ERROR: Attempted to retrive a value of less than 1 length\n");
		return NULL;
	}
	if (offsetInBlock + objectSize > block->size) {
		fprintf(stderr, "ERROR: Attempted to retrive a value out of bounds\n");
		return NULL;
	}
	return (char*)block->memory + offsetInBlock;
}
// Free up a block of memory obtained via allocate()
void deallocate(Block* inblock) {
	if (blockalloc && inblock) {
		if (blockalloc->firstBlock == NULL) {
			return;
		}
		Block* block = GetBlock(inblock);
		if (block) {
			EmptyBlock* BlockGhost = 0;
			if (blockalloc->firstBlock == block && block->nextBlock == NULL) {
				//if first block the pool is empty so get all our space in the pool back
				BlockGhost = CreateEmptyBlock(SizeOfPool, 0);
				blockalloc->FirstEmptyblock = BlockGhost;
				blockalloc->LastEmptyblock = BlockGhost;
			}
			else
			{
				BlockGhost = CreateEmptyBlock(block->size, block->offset);
				AddEmptyBlock(BlockGhost);
			}
			RemoveBlock(block);
			RemoveMapNode(inblock);
			DefragmentEmpty();
		}
	}
}
void DefragmentEmpty() {
	EmptyBlock* Conductor = blockalloc->FirstEmptyblock;
	while (Conductor->nextBlock != NULL)
	{
		if (Conductor->nextBlock != NULL) {
			if (Conductor->offset + Conductor->size == Conductor->nextBlock->offset) {
				//if the blocks are contiguous
				EmptyBlock* mergedblock = CreateEmptyBlock(Conductor->size + Conductor->nextBlock->size, Conductor->offset);
				//create a new block that spans the space
				if (mergedblock) {
					if (Conductor->PreviousBlock != NULL) {
						Conductor->PreviousBlock->nextBlock = mergedblock;
						Conductor->nextBlock->PreviousBlock = mergedblock;
						//Bridged the two sub blocks from the list					
					}
					else
					{
						if (Conductor->nextBlock->nextBlock) {
							mergedblock->nextBlock = Conductor->nextBlock->nextBlock;
						}
						Conductor->nextBlock->PreviousBlock = mergedblock;
					}
					if (Conductor->nextBlock->nextBlock == NULL) {
						//Two block in the list
						blockalloc->LastEmptyblock = mergedblock;
					}
					if (Conductor == blockalloc->FirstEmptyblock) {
						blockalloc->FirstEmptyblock = mergedblock;
					}
				}

			}
		}
		Conductor = Conductor->nextBlock;
	}
}
EmptyBlock* GetEmptyBlock(int size, int offset) {
	EmptyBlock* Conductor = blockalloc->FirstEmptyblock;
	if (Conductor) {
		if (Conductor->offset == offset)
		{
			//found target space
			return Conductor;
		}
		if ((Conductor->offset + Conductor->size) > offset && Conductor->offset < offset) {
			return Conductor;
		}
		while (Conductor->nextBlock != NULL)
		{
			if (Conductor->offset == offset)
			{
				//found target space
				return Conductor;
			}
			if ((Conductor->offset + Conductor->size) > offset && Conductor->offset < offset) {
				return Conductor;
			}
			Conductor = Conductor->nextBlock;
		}
	}
	return NULL;
}
EmptyBlock* GetEmptyBlockByOffset(int offset) {
	EmptyBlock* Conductor = blockalloc->FirstEmptyblock;
	if (Conductor) {
		if (Conductor->offset == offset)
		{
			//found target space
			return Conductor;
		}
		while (Conductor->nextBlock != NULL)
		{
			if (Conductor->offset == offset)
			{
				//found target space
				return Conductor;
			}
			Conductor = Conductor->nextBlock;
		}
	}
	return NULL;
}
EmptyBlock* CheckSpaceBetween(Block* block, Block* otherblock) {
	EmptyBlock* emptyblock = GetEmptyBlockByOffset(block->offset + block->size);
	if (emptyblock) {
		if ((emptyblock->size + emptyblock->offset) >= otherblock->offset) {
			//The empty block spans the gap inbetween the two blocks or is bigger than the gap
			return emptyblock;
		}
	}
	return 0;
}
void DefragmentDataBlocks() {
	Block* Conductor = blockalloc->firstBlock;	
	if (Conductor) {
		int counter = 0;
		while (counter < SizeOfPool)
		{
			EmptyBlock* eblock = GetEmptyBlockByOffset(counter);
			if (eblock) {
				if (Conductor->offset == (eblock->offset + eblock->size)) {
					char* tempmemory = safe_malloc_set(Conductor->size);
					memcpy(tempmemory, Conductor->memory, Conductor->size);
					Conductor->offset = counter;
					eblock->offset = counter + Conductor->size;
					counter++;
					memcpy(Conductor->memory, tempmemory, Conductor->size);
					if (Conductor->nextBlock != NULL) {
						Conductor = Conductor->nextBlock;
					}
					DefragmentEmpty();
				}
				else
				{
					counter++;
				}
			}
			else
			{
				counter++;
			}
		}
	}
}
