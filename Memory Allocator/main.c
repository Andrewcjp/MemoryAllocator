#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "pool.h"
#include "block.h"


int main()
{
	printf("****************************************\n");
	printf("*            Phase 1 Tests             *\n");
	printf("****************************************\n");
	const int poolSize = 500;
	Pool* pool;
	int x = 5;
	char c = 'c';
	char str[] = "Hello World";


	/* Test 1 - Allocate and deallocate a pool */
	printf("Test 1: Allocate and Deallocate a Pool\n");
	pool = allocatePool(50);
	freePool(pool);
	pool = NULL;

	/* Tests 2-4 - Simple data storage*/
	pool = allocatePool(poolSize);

	store(pool, 0, 1, &c);
	printf("Test 2: Store a single character\n");
	printf("\tStored: %c\n", c);
	printf("\tRetrieves: %c\n", *((char*)retrieve(pool, 0, 1)));


	store(pool, 3, 4, &x);
	printf("Test 3: Store a single multi-byte value\n");
	printf("\tStored: %d\n", x);
	printf("\tRetrieves: %d\n", *((int*)retrieve(pool, 3, 4)));

	store(pool, 8, sizeof(str), str);
	printf("Test 4: Store an arbitrary multi-byte value\n");
	printf("\tStored: %s\n", str);
	printf("\tRetrieves: %s\n", (char*)retrieve(pool, 8, sizeof(str)));

	/* Test 5 - Missing Null terminator */
	store(pool, 50, sizeof(str) - 1, str);
	printf("Test 5: Store an arbitrary multi-byte value with no null terminator\n");
	printf("\tStored: %s\n", str);
	char* retstr = (char*)retrieve(pool, 50, sizeof(str) - 1);
	printf("\tRetrieves: %s\n", retstr);
	if (strlen(retstr) > 12)
	{
		printf("\tPass: Correctly not null terminated %d\n", strlen(retstr));
	}
	else
	{
		printf("\tFail: string is of incorrect length %d\n", strlen(retstr));
	}

	/* Test 6 - Allocate past end of memory */
	store(pool, poolSize - 1, sizeof(str), str);
	printf("Test 6: Store / Retrieve past the end of memory\n");
	printf("\tStored: %s\n", str);
	char* temp = retrieve(pool, poolSize - 1, sizeof(str) - 1);
	if (temp != NULL)
	{
		printf("\tFail: Retrieved a value\n");
	}
	else
	{
		printf("\tPass: Retrieved NULL\n");
	}
	freePool(pool);

	/* Test 7 - Double Deallocate Pool */
	printf("Test 7: Double Deallocate Pool\n");
	pool = NULL;
	freePool(pool);
	printf("\tPass\n");
	/* Test 8 - Allocate 0 Pool */
	printf("Test 9: Allocate Empty Pool\n");
	pool = allocatePool(0);
	if (pool == NULL || pool->size == 0)
	{
		printf("\tPass: Pool is NULL, or Pool->size is 0\n");
	}
	else
	{
		printf("\tFail: Pool exists\n");
	}

	/* Test 9 - Allocate negative Pool */
	printf("Test 9: Allocate negative Pool\n");
	pool = allocatePool(-poolSize);
	if (pool == NULL)
	{
		printf("\tPass: Pool is NULL\n");
	}
	else
	{
		printf("\tFail: Pool is not NULL\n");
	}
	freePool(pool);

	printf("****************************************\n");
	printf("*            Phase 2 Tests             *\n");
	printf("*                                      *\n");
	printf("****************************************\n");
	const int blockSize = 100;
	Block* block;

	/* Test 1 - Allocate and deallocate a pool */
	printf("Test 1: Allocate and Deallocate a Block within a Pool\n");
	pool = allocatePool(50);
	block = allocate(pool, 50);
	deallocate(block);
	freePool(pool);
	pool = NULL;
	printf("\tPass:");

	/* Tests 2-4 - Simple data storage*/
	pool = allocatePool(poolSize);
	block = allocate(pool, blockSize);

	printf("Test 2: Store a single character\n");
	write(block, &c, 1, 0);
	printf("\tStored: %c\n", c);
	printf("\tRetrieves: %c\n", *((char*)read(block, 1, 0)));

	printf("Test 3: Store a single multi-byte value\n");
	write(block, &x, 4, 3);
	printf("\tStored: %d\n", x);
	printf("\tRetrieves: %d\n", *((int*)read(block, 4, 3)));

	printf("Test 4: Store an arbitrary multi-byte value\n");
	write(block, str, strlen(str), 8);
	printf("\tStored: %s\n", str);
	printf("\tRetrieves: %s\n", (char*)read(block, strlen(str), 8));

	/* Test 5 - Missing Null terminator */
	printf("Test 5: Store an arbitrary multi-byte value with no null terminator\n");
	write(block, str, strlen(str) - 1, 8);
	printf("\tStored: %s\n", str);
	char* retstrb = (char*)read(block, strlen(str) - 1, 8);
	if (strlen(retstrb) > 12)
	{
		printf("\tPass: Correctly not null terminated %d\n", strlen(retstrb));
	}
	else
	{
		printf("\tFail: string is of incorrect length %d\n", strlen(retstrb));
	}

	/* Test 6 - Allocate past end of memory */
	printf("Test 6: Store / Retrieve past the end of memory\n");
	write(block, str, strlen(str), blockSize - 1);
	printf("\tStored: %s\n", str);
	char* tempb = (char*)read(block, strlen(str), blockSize - 1);
	if (tempb != NULL)
	{
		printf("\tFail: Retrieved a value\n");
	}
	else
	{
		printf("\tPass: Retrieved NULL\n");
	}
	int* bob = malloc(2048);

	deallocate(block);
	free(bob);
	/* Test 7 - Double Deallocate Pool */
	printf("Test 7: Double Deallocate Block\n");
	deallocate(block);
	printf("\tPass\n");

	printf("Test 8: Deallocate NULL Block\n");
	block = NULL;
	deallocate(block);
	printf("\tPass\n");

	/* Test 9 - Allocate 0 Block */
	printf("Test 9: Allocate Empty Pool\n");
	block = allocate(pool, 0);
	if (block == NULL || block->size == 0)
	{
		printf("\tPass: Block is NULL, or Block->size is 0\n");
	}
	else
	{
		printf("\tFail: Block exists\n");
	}
	deallocate(block);
	block = NULL;

	/* Test 10 - Allocate negative Block */
	printf("Test 10: Allocate negative Block\n");
	block = allocate(pool, -blockSize);
	if (block == NULL)
	{
		printf("\tPass: Block is NULL\n");
	}
	else
	{
		printf("\tFail: Black is not NULL\n");
	}
	deallocate(block);

	/* Test 11 - Allocate Multiple Blocks */
	printf("Test 11: Allocate Multiple Block\n");
	block = allocate(pool, blockSize);
	Block* microBlock1 = allocate(pool, 25);
	Block* microBlock2 = allocate(pool, 25);
	Block* microBlock3 = allocate(pool, 25);
	Block* microBlock4 = allocate(pool, 25);
	Block* macroBlock1 = allocate(pool, blockSize);
	Block* macroBlock2 = allocate(pool, blockSize);
	Block* macroBlock3 = allocate(pool, blockSize);
	deallocate(microBlock1);
	deallocate(microBlock2);
	Block* failBlock = allocate(pool, blockSize);
	if (failBlock == NULL)
	{
		printf("\tPass: Block is NULL\n");
	}
	else
	{
		printf("\tFail: Black is not NULL\n");
	}
	deallocate(microBlock3);
	deallocate(microBlock4);

	/* Test 12 - Contiguous Space Test */
	printf("Test 12: Contiguous Space Test\n");
	Block* passBlock = allocate(pool, blockSize);
	if (passBlock)
	{
		printf("\tPass: Block is not NULL, contiguous space found\n");
	}
	else
	{
		printf("\tFail: Black is NULL, contiguous space not reclaimed\n");
	}
	deallocate(block);
	deallocate(passBlock);
	deallocate(macroBlock1);
	deallocate(macroBlock2);
	deallocate(macroBlock3);
	microBlock1 = allocate(pool, 25);
	block = allocate(pool, blockSize);
	microBlock2 = allocate(pool, 25);
	macroBlock1 = allocate(pool, blockSize);
	macroBlock2 = allocate(pool, blockSize);
	microBlock3 = allocate(pool, 25);
	macroBlock3 = allocate(pool, blockSize);
	microBlock4 = allocate(pool, 25);
	write(block, &x, 4, 21);
	deallocate(microBlock1);
	deallocate(microBlock2);
	deallocate(microBlock3);
	deallocate(microBlock4);
	printf("****************************************\n");
	printf("*         Phase 2 Extra Tests          *\n");
	printf("*                                      *\n");
	printf("****************************************\n");
	/* Test 13 - Fragmentation Space Test */
	printf("Test 13: Fragmentation Space Test\n");
	DefragmentDataBlocks();
	passBlock = allocate(pool, blockSize);
	if (passBlock)
	{
		printf("\tPass: Block is not NULL, Defragmented space found\n");
	}
	else
	{
		printf("\tFail: Black is NULL, Fragmented space not reclaimed\n");
	}

	/* Test 14 -  Defragmented Data Test */
	printf("Test 14: Defragmented Data Test\n");
	printf("\tStored: %d\n", x);
	printf("\tRetrieves: %d\n", *((int*)read(block, 4, 21)));

	/* Test 15 - Writing Structs  Test */
	printf("Test 15: Writing Structs Test\n");
	struct TestStruct
	{
		int number1;
		int number2;
	};
	struct TestStruct* testStruct = safe_malloc_set(sizeof(struct TestStruct));
	testStruct->number1 = 28375;
	testStruct->number2 = 2345;
	write(block, testStruct, sizeof(struct TestStruct), 0);
	struct TestStruct* returnedstruct = (struct TestStruct*)read(block, sizeof(struct TestStruct), 0);
	if (returnedstruct->number1 == testStruct->number1 && returnedstruct->number2 == testStruct->number2) {
		printf("\tPass\n");
	}
	else
	{
		printf("\tFailed\n");
	
	}

	/* Test 16 - allocate to Null pool */
	printf("Test 16: Allocate to Null Pool\n");
	passBlock = allocate(NULL, 0);
	if (passBlock)
	{
		printf("\tFail: Block is not NULL\n");
	}
	else
	{
		printf("\tPass: Black is NULL \n");
	}

	/* Test 17 - WAllocation Table */
	printf("Test 17: Allocation Table\n");
	print_allocation_table();
	deallocate(block);
	deallocate(macroBlock1);
	deallocate(macroBlock2);
	deallocate(macroBlock3);
	deallocate(passBlock);
	freePool(pool);
	getchar();
	return 0;
}
