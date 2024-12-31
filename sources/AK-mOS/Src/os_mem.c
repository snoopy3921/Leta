/*
*********************************************************************************************************
*                                            MEMORY MANAGEMENT
*
* File    : os_mem.c
* Version : none
* Author  : JiaHui
*********************************************************************************************************
*/

#include "os_cfg.h"
#include "os_mem.h"
#include "os_kernel.h"
#include <stdio.h>
#include <stdlib.h>


#if 0
#define ALIGNMENT 		((size_t)4u) // must be a power of 2

#define mem_align(size) 	(size_t)(((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define MIN_SIZE_TO_SPLIT 	((size_t)8u) // In byte

#define SIZE_OF_BLOCK_HEADER 	((size_t)mem_align(sizeof(mem_blk_header_t)))

static uint8_t mem_heap[OS_CFG_HEAP_SIZE];
static mem_blk_header_t mem_blk_start;
static mem_blk_header_t *mem_blk_end_ptr = NULL;

static uint32_t byte_available = 0;


static void os_mem_heap_init(void)
{
	uint32_t total_heap_size = OS_CFG_HEAP_SIZE;
	uint32_t heap_addr = (uint32_t)mem_heap;
	if ((heap_addr & ALIGNMENT) != 0) /* If address of heap memory is not alligned	*/
	{
		heap_addr += (ALIGNMENT - 1);
		heap_addr &= ALIGNMENT;
		total_heap_size -= heap_addr - (uint32_t)mem_heap; /* Recalculate size of heap memory	*/
	}

	mem_blk_end_ptr = (mem_blk_header_t *)(heap_addr); /* Address of first block	*/
	mem_blk_end_ptr->size = total_heap_size - SIZE_OF_BLOCK_HEADER;
	mem_blk_end_ptr->state = MEM_STATE_FREE;
	mem_blk_end_ptr->next_ptr = NULL;

	mem_blk_start.size = 0;
	mem_blk_start.next_ptr = mem_blk_end_ptr;

	byte_available = total_heap_size - SIZE_OF_BLOCK_HEADER;
}

void *os_mem_malloc(size_t size)
{
	uint8_t *p_return = NULL;
	if (mem_blk_end_ptr == NULL)
	{
		os_mem_heap_init();
	}
	size = mem_align(size);
	if (size == 0 || size > byte_available)
	{
		os_assert(0);
		return p_return; // Invalid size
	}

	else
	{
		mem_blk_header_t *p_block = &mem_blk_start;
		p_block = p_block->next_ptr;
		while (p_block != mem_blk_end_ptr)
		{
			if (p_block->size < size || p_block->state == MEM_STATE_BUSY)
			{
				p_block = p_block->next_ptr;
			}
			else
				break;
		}
		if (p_block != NULL)
		{
			if (p_block == mem_blk_end_ptr && p_block->size < size)
			{
				os_assert(0);
				return (void *)p_return; // No block available
			}

			if ((p_block->size - size) > MIN_SIZE_TO_SPLIT)
			{
				p_return = ((uint8_t *)p_block + SIZE_OF_BLOCK_HEADER);

				mem_blk_header_t *p_new_block = (mem_blk_header_t *)(((uint8_t *)p_block) + SIZE_OF_BLOCK_HEADER + size);
				p_new_block->size = p_block->size - size - SIZE_OF_BLOCK_HEADER;
				p_new_block->state = MEM_STATE_FREE;
				p_new_block->next_ptr = p_block->next_ptr;
				if (p_new_block->next_ptr == NULL)
				{
					mem_blk_end_ptr = p_new_block;
				}

				p_block->size = size;
				p_block->state = MEM_STATE_BUSY;
				p_block->next_ptr = p_new_block;
				// p_block = p_block->next_ptr;

				byte_available -= (size + SIZE_OF_BLOCK_HEADER);
			}

			else
			{
				p_return = ((uint8_t *)p_block + SIZE_OF_BLOCK_HEADER);
				p_block->state = MEM_STATE_BUSY;
				byte_available -= p_block->size;
			}
			return (void *)p_return;
		}
		else
		{
			os_assert(0);
			/* MEM_FAULT (p_block null) */
		}
	}
	return (void *) p_return;
}

void os_mem_free(void *p_addr)
{
	if (mem_blk_end_ptr == NULL)
	{
		os_mem_heap_init();
	}
	if (((uint8_t *)p_addr) > ((uint8_t *)mem_blk_end_ptr + SIZE_OF_BLOCK_HEADER + mem_blk_end_ptr->size))
	{
		os_assert(0);
		return; // Invalid address
	}

	if ((size_t)(((uint8_t *)p_addr) - SIZE_OF_BLOCK_HEADER) < (size_t)mem_blk_start.next_ptr)
	{
		os_assert(0);
		return; // Invalid address
	}
	else
	{
		mem_blk_header_t *p_block = (mem_blk_header_t *)((uint8_t *)p_addr - SIZE_OF_BLOCK_HEADER);
		if (p_block->state == MEM_STATE_FREE)
		{
			return;
		}
		mem_blk_header_t *p_block_temp = &mem_blk_start;
		mem_blk_header_t *p_prev_block = p_block_temp;

		while (p_block_temp != mem_blk_end_ptr && (uint8_t *)p_block_temp != (uint8_t *)p_block)
		{
			p_prev_block = p_block_temp;
			p_block_temp = p_block_temp->next_ptr;
		}
		if (p_block_temp != NULL)
		{
			byte_available += p_block_temp->size;

			p_block_temp->state = MEM_STATE_FREE;

			// Merge the next block
			if (p_block_temp->next_ptr != NULL && p_block_temp->next_ptr->state == MEM_STATE_FREE)
			{
				byte_available += SIZE_OF_BLOCK_HEADER;

				p_block_temp->size += p_block_temp->next_ptr->size + SIZE_OF_BLOCK_HEADER;
				p_block_temp->next_ptr = p_block_temp->next_ptr->next_ptr;
				if (p_block_temp->next_ptr == NULL)
				{
					mem_blk_end_ptr = p_block_temp;
				}
			}

			// Merge the prev block
			if (p_prev_block->state == MEM_STATE_FREE && (uint8_t *)p_prev_block != (uint8_t*)&mem_blk_start)
			{
				byte_available += SIZE_OF_BLOCK_HEADER;

				p_prev_block->size += p_block_temp->size + SIZE_OF_BLOCK_HEADER;
				p_prev_block->next_ptr = p_block_temp->next_ptr;
			}
		}
		else
		{
			os_assert(0);
			return; // Invalid address
		}
	}
	return;
}

#else
void *os_mem_malloc(size_t size)
{
	uint32_t * p_return = malloc(size);
	return p_return;
}

void os_mem_free(void * p_addr)
{
	free(p_addr);
	return;
}
#endif
