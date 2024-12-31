/*
 * os_mem.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 */

#ifndef OS_MEM_H
#define OS_MEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

   typedef enum
   {
      MEM_STATE_FREE = 0,
      MEM_STATE_BUSY
   } mem_state_t;

   typedef struct mem_blk_header mem_blk_header_t;

   struct mem_blk_header
   {
      size_t size;
      mem_state_t state;
      struct mem_blk_header *next_ptr;
   };

   void *os_mem_malloc(size_t size);
   void os_mem_free(void *p_addr);

#ifdef __cplusplus
}
#endif
#endif /* OS_MEM_H */
