#ifndef __POOL_H
#define __POOL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "block.h"

Pool* allocatePool(int n);
void freePool(Pool* pool);
void store(Pool* pool, int offset, int size, void *object);
void *retrieve(Pool* pool, int offset, int size);


#endif