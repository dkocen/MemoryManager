#include <stdlib.h>
#include <stdio.h>
#include "gc.h"

int *getBlock(int *p); //get header of block that p points to
void mark(void *p); //mark algorithm
void sweep(); //sweep algorithm

//Function that returns bottom memory address of stack
void *stackBottom() {
	unsigned long bottom;
    FILE *statfp = fopen("/proc/self/stat", "r");
    fscanf(statfp,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u "
           "%*u %*u %*u %*u %*u %*u %*d %*d "
           "%*d %*d %*d %*d %*u %*u %*d "
           "%*u %*u %*u %lu", &bottom);
    fclose(statfp);
	return (void *) bottom;
}

//Function that goes through heap and frees garbage
void gc()
{
	int **max = (int **) stackBottom();
	int *dummy;
	int **p = &dummy;

	while(p <= max)
	{
		mark(*p);
		p = p+1;
	}
	
	sweep();
}

//Function that marks non-garbage blocks on heap
void mark(void *p)
{
	int *first_block = (int*)firstBlock();
	int *last_block = (int*)lastBlock();
	
	int *ptr = (int*) p;
	if(ptr < first_block || ptr > last_block)
	{
		return;
	}
	
	int *b = getBlock(ptr);
	if(isAllocated(b))
	{
		*b = *b + 2;
		int b_size = *b/4;
		int **p2 = (int **)b+1;
	
		while(p2 <= (int**)(b+b_size))
		{
			mark(*p2);
			p2 = p2+1;
		}
	}
}

//Function that frees all non-marked blocks
void sweep()
{
	int *p = (int*)firstBlock();
	int *end = (int*)lastBlock();
	while(p != end)
	{
		if(*p%4 != 3)
		{
			myfree(p+1);
		}
		else
		{
			*p = *p-2;
		}
		p = (int*) nextBlock(p);
	}
	coalesce();
}

//Function that returns header for block that p points to
int *getBlock(int *p)
{
	int *block = (int*)firstBlock();
	while((int*)nextBlock(block) < p)
	{
		block = (int*)nextBlock(block);
	}
	return block;
}
