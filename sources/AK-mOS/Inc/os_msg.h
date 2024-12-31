/*
 * os_msg.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 */

#ifndef OS_MSG_H
#define OS_MSG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "os_cfg.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

    typedef struct msg msg_t;
    typedef struct msg_queue msg_queue_t;
    typedef struct msg_pool msg_pool_t;

    typedef enum
    {
        MSG_TYPE_PURE = 0,
        MSG_TYPE_DYNAMIC
    } msg_type_t;

    struct msg
    {
        msg_t *next;

        uint8_t size;
        int32_t sig;
        uint8_t *content_ptr;

        msg_type_t type;

        /* task header */
        uint8_t src_task_id;
        uint8_t des_task_id;
    };

    struct msg_queue
    {
        msg_t *head_ptr;
        msg_t *tail_ptr;
        uint8_t size_max;
        uint8_t size_curr;
    };

    void os_msg_init(void);

    void os_msg_free(msg_t *p_msg);

    void os_msg_queue_init(msg_queue_t *p_msg_q, uint8_t size);

    void os_msg_queue_put_dynamic(msg_queue_t *p_msg_q, int32_t sig, void *p_content, uint8_t size);

    void os_msg_queue_put_pure(msg_queue_t *p_msg_q, int32_t sig);

    msg_t *os_msg_queue_get(msg_queue_t *p_msg_q);

    void *os_msg_get_dynamic_data(msg_t *p_msg, uint8_t *p_msg_size);

    msg_t *os_msg_queue_get_pure(msg_queue_t *p_msg_q);

    int32_t os_msg_get_pure_data(msg_t *p_msg);


#ifdef __cplusplus
}
#endif
#endif /* OS_MSG_H */
