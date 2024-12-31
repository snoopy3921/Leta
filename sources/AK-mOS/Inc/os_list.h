/*
 * os_list.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 */

#ifndef OS_LIST_H
#define OS_LIST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdio.h>
#include "os_kernel.h"

	typedef struct list list_t;
	typedef struct list_item list_item_t;

	struct list_item
	{
		struct list_item *next_ptr; /* Pointer to NextItem*/
		struct list_item *prev_ptr; /* Pointer to PrevItem*/
		uint32_t value;		    /* Value of this item*/
		void *owner_ptr;		    /*Owner of this item (TCB usually)*/
		struct list *list_ptr;	    /* Pointer to the list in which this list item is placed (if any). */
	};

	struct list
	{
		struct list_item *curr_item_ptr; /* Used to walk through the list in round-robin case */
		struct list_item end_item;	   /* Used to mark the end of list */
		uint16_t num_of_items;
	};

#define list_item_set_owner(p_list_item, p_owner) ((p_list_item)->owner_ptr = (void *)(p_owner))
#define list_item_get_owner(p_list_item) ((p_list_item)->owner_ptr)
#define list_item_set_value(p_list_item, val) ((p_list_item)->value = (uint32_t)(val))
#define list_item_get_value(p_list_item) ((p_list_item)->value)
#define list_item_get_list_contain(p_list_item) ((p_list_item)->list_ptr)

#define list_get_head_item_value(p_list) ((((p_list)->end_item).next_ptr)->value)
#define list_get_head_item(p_list) (((p_list)->end_item).next_ptr)
#define list_get_owner_of_head_item(p_list) (list_item_get_owner(((p_list)->end_item).next_ptr))
#define list_get_end_item(p_list) ((p_list)->end_item)
#define list_get_num_item(p_list) (((p_list)->num_of_items))
#define list_is_empty(p_list) (((p_list)->num_of_items == (uint8_t)0) ? OS_TRUE : OS_FALSE)

	void *list_get_owner_of_next_item(list_t *const p_list);

	void os_list_init(list_t *const p_list);
	void os_list_item_init(list_item_t *const p_list_item);
	list_item_t *list_item_get_next(list_item_t *p_list_item);
	list_item_t *list_item_get_prev(list_item_t *p_list_item);
	void os_list_insert_end(list_t *const p_list, list_item_t *const p_list_item);
	void os_list_insert(list_t *const p_list, list_item_t *const p_list_item);
	uint16_t os_list_remove(list_item_t *const p_list_item);

#ifdef __cplusplus
}
#endif
#endif /* OS_LIST_H */
