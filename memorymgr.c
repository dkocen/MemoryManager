#include <stdlib.h>
#include <stdio.h>
#include "memorymgr.h"

//Global Variables
void *heap;
int *first_block;
int *last_block;
int *first_header;

int blockSize(int size);

void initmemory(int size)
{
	//add initial padding, sentinel block, and header
	size = size + 12;
	int num = size%8;
	if(num != 0)
	{
		size = size + (8-num);
	}
	
	//Get the actual memory from the system's heap
	heap = malloc(size);
	//Set first_block to start of heap (initial padding)
	first_block = heap;
	//Set sentinel block to last 4 bytes of heap
	last_block = first_block + (size/4) - 1;
	//Set first_header to first block after initial padding
	first_header = first_block + 1;
	
	//Set header for initial unallocated block
	//The header will be in the 2nd word (since the first cell is reserved) and will
	//have size-8 because exclude first and last cell
	*first_header = size - 8; 
	
	//Set header for sentinel block
	*last_block = 1;
}

//Allocate memory from heap
void *myalloc(int length)
{
	int previous_value = 0;
	int *header = firstBlock();
	int *ptr;
	int size = blockSize(length);
	
	//Go through implicit list looking for a valid, unallocated block
	while(*header < size || *header%2 != 0)
	{
		//If reach the sentinel block there is not valid block so return NULL
		if(*header == 1)
		{
			return NULL;
		}
		
		header = nextBlock(header);
	}

	previous_value = *header;
	
	//Size+1 because allocated now
	*header = size+1;
	
	//Actual value to be returned is header+1 because don't want to return the header
	ptr = header+1;
	
	//Adjust rest of heap so all headers are up to date
	header = nextBlock(header);
	if(*header != 1)
	{
		*header = previous_value - size;
	}

	return ptr;
}

//Free block
void myfree(void *ptr)
{
	//Do ptr-4 in order to get to the header from the pointer sent in
	int *header = ptr-4;
	if(isAllocated(header))
	{
		*header = *header - 1;
	}
}

//Coalesce all free blocks
void coalesce()
{
	int *header = firstBlock();
	int *nextHeader = nextBlock(header);
	while(*nextHeader != 1)
	{
		if(*header%2 == 0 && *nextHeader%2 == 0)
		{
			*header = *header + *nextHeader;
		}
		else
		{
			header = nextHeader;
		}
		
		nextHeader = nextBlock(header);
	}
}

//Print out heap information
void printallocation()
{
	int *header = firstBlock();
	int block_num = 0;
	while(*header != 1)
	{
		if(isAllocated(header))
		{
			printf("Block %d: size %d \t allocated\n", block_num, *header-1);
		}
		else
		{
			printf("Block %d: size %d \t unallocated\n", block_num, *header);
		}
		
		//Figure out location of next header
		header = nextBlock(header);

		block_num++;
	}
	printf("\n\n");
}

//Helper function to see if something is allocated
int isAllocated(int *p)
{
	if(*p % 8 == 0)
	{
		return 0;
	}
	
	return 1;
}

//Helper function to get what size should be allocated given a specific requestion (add header and go to nearest multiple of 8)
int blockSize(int size)
{
	size = size + 4;
	int num = size%8;
	if(num != 0)
	{
		size = size + (8-num);
	}
	
	return size;
}

//Helper function to go to next block in implicit list
int *nextBlock(int *p)
{
	int *block = p;
	
	if(*block % 2 != 0)
	{
		block = block + (*block - 1)/4;
	}
	else
	{
		block = block + *block/4;
	}
	
	return block;
}

//Returns the first header in the heap
int *firstBlock()
{
	return first_header;
}

//Returns sentinel block
int *lastBlock()
{
	return last_block;
}

