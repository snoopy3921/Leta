/*
 * os_task.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 */

#ifndef OS_TASK_H
#define OS_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdio.h>
#include "os_cfg.h"

#include "os_msg.h"


  /* Task states */
  typedef enum
  {
    TASK_STATE_RUNNING = 0,
    TASK_STATE_READY,
    TASK_STATE_DELAYED,
    TASK_STATE_SUSPENDED,
    TASK_STATE_SUSPENDED_ON_MSG,
    TASK_STATE_DELAYED_ON_MSG
  } task_state_t;

  typedef struct task_tcb *task_handle_t; /*Reference (pointer) of TCB*/
  typedef void (*task_func_t)(void *p_arg);
  typedef uint8_t task_id_t;

  typedef struct
  {
    // task_handle_t * const   p_tsk_handle;
    task_id_t id;
    task_func_t pf_task;
    void *p_arg;
    uint8_t prio;
    size_t queue_size;
    size_t stack_size;
  } task_t;

	uint32_t os_task_get_tick(void);
	
  void os_task_create_list(task_t *task_tbl, uint8_t size);

  uint8_t os_task_increment_tick(void);

  void os_task_delay(const uint32_t tick_to_delay);

  void os_task_start(void);

  void os_task_post_msg_dynamic(uint8_t des_task_id, int32_t sig, void *p_content, uint8_t msg_size);

  void os_task_post_msg_pure(uint8_t des_task_id, int32_t sig);

  msg_t *os_task_wait_for_msg(uint32_t time_out);

#ifdef __cplusplus
}
#endif
#endif /* OS_TASK_H */