#include "os_msg.h"
#include "os_mem.h"
#include "os_kernel.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static msg_t msg_pool[OS_CFG_MSG_POOL_SIZE];
static msg_t *free_list_msg_pool;
static uint8_t msg_pool_used;

static void msg_pool_init(void)
{
    ENTER_CRITICAL();
    uint8_t index;

    free_list_msg_pool = (msg_t *)msg_pool;

    for (index = 0; index < OS_CFG_MSG_POOL_SIZE; index++)
    {
        if (index == (OS_CFG_MSG_POOL_SIZE - 1))
        {
            msg_pool[index].next = NULL;
        }
        else
        {
            msg_pool[index].next = (msg_t *)&msg_pool[index + 1];
        }
    }

    msg_pool_used = 0;

    EXIT_CRITICAL();
}
void os_msg_init(void)
{
    msg_pool_init();
}

void os_msg_free(msg_t *p_msg)
{
    ENTER_CRITICAL();

    p_msg->next = free_list_msg_pool;
    free_list_msg_pool = p_msg;
    if (p_msg->type == MSG_TYPE_DYNAMIC)
    {
        os_mem_free(p_msg->content_ptr);
    }
    msg_pool_used--;

    EXIT_CRITICAL();
}

void os_msg_queue_init(msg_queue_t *p_msg_q,
                       uint8_t size)
{
    p_msg_q->head_ptr = NULL;
    p_msg_q->tail_ptr = NULL;
    p_msg_q->size_max = size;
    p_msg_q->size_curr = 0u;
}

void os_msg_queue_put_dynamic(msg_queue_t *p_msg_q,
                              int32_t sig,
                              void *p_content,
                              uint8_t size)
{
    ENTER_CRITICAL();
    msg_t *p_msg;
    msg_t *p_msg_tail;
    if (p_msg_q->size_curr >= p_msg_q->size_max)
    {
        // OSUniversalError = OS_ERR_MSG_QUEUE_IS_FULL;
        os_assert(0);
        EXIT_CRITICAL();
        return;
    }
    if (msg_pool_used >= OS_CFG_MSG_POOL_SIZE)
    {
        // OSUniversalError = OS_ERR_MSG_POOL_IS_FULL;
        os_assert(0);
        EXIT_CRITICAL();
        return;
    }

    p_msg = free_list_msg_pool;
    free_list_msg_pool = p_msg->next;
    msg_pool_used++;

    if (p_msg_q->size_curr == 0u) /* Is this first message placed in the queue? */
    {
        p_msg_q->head_ptr = p_msg; /* Yes */
        p_msg_q->tail_ptr = p_msg;
        p_msg_q->size_curr = 1u;
        p_msg->next = NULL;
    }
    else
    {
        p_msg_tail = p_msg_q->tail_ptr;
        p_msg_tail->next = p_msg;
        p_msg_q->tail_ptr = p_msg;
        p_msg->next = NULL;

        p_msg_q->size_curr++;
    }

    p_msg->type = MSG_TYPE_DYNAMIC;
    p_msg->sig = sig;
    p_msg->size = size;
    p_msg->content_ptr = (uint8_t *)os_mem_malloc(size);
    memcpy(p_msg->content_ptr, p_content, size);

    EXIT_CRITICAL();
}

void os_msg_queue_put_pure(msg_queue_t *p_msg_q, int32_t sig)
{
    ENTER_CRITICAL();
    msg_t *p_msg;
    msg_t *p_msg_tail;
    if (p_msg_q->size_curr >= p_msg_q->size_max)
    {
        // OSUniversalError = OS_ERR_MSG_QUEUE_IS_FULL;
        os_assert(0);
        EXIT_CRITICAL();
        return;
    }
    if (msg_pool_used >= OS_CFG_MSG_POOL_SIZE)
    {
        /* This states that u forget to free msg somewhere.*/
        // OSUniversalError = OS_ERR_MSG_POOL_IS_FULL;
        os_assert(0);
        EXIT_CRITICAL();
        return;
    }

    p_msg = free_list_msg_pool;
    free_list_msg_pool = p_msg->next;
    msg_pool_used++;

    if (p_msg_q->size_curr == 0u) /* Is this first message placed in the queue? */
    {
        p_msg_q->head_ptr = p_msg; /* Yes */
        p_msg_q->tail_ptr = p_msg;
        p_msg_q->size_curr = 1u;
        p_msg->next = NULL;
    }
    else
    {
        p_msg_tail = p_msg_q->tail_ptr;
        p_msg_tail->next = p_msg;
        p_msg_q->tail_ptr = p_msg;
        p_msg->next = NULL;

        p_msg_q->size_curr++;
    }

    p_msg->type = MSG_TYPE_PURE;
    p_msg->sig = sig;

    EXIT_CRITICAL();
}

msg_t *os_msg_queue_get(msg_queue_t *p_msg_q)
{
    msg_t *p_msg;

    if (p_msg_q->size_curr == 0u)
    {
        // OSUniversalError = OS_ERR_MSG_QUEUE_IS_EMPTY;
        // os_assert(0);
        return NULL;
    }

    p_msg = p_msg_q->head_ptr;

    p_msg_q->head_ptr = p_msg->next;

    if (p_msg_q->size_curr == 1u) /* Are there any more messages in the queue? */
    {
        p_msg_q->head_ptr = NULL;
        p_msg_q->tail_ptr = NULL;
        p_msg_q->size_curr = 0u;
    }
    else
    {
        p_msg_q->size_curr--; /* Yes, One less message in the queue */
    }

    return (p_msg);
}

void *os_msg_get_dynamic_data(msg_t *p_msg,
                              uint8_t *p_msg_size)
{
    *p_msg_size = p_msg->size;
    return p_msg->content_ptr;
}

msg_t *os_msg_queue_get_pure(msg_queue_t *p_msg_q)
{
    msg_t *p_msg;

    if (p_msg_q->size_curr == 0u)
    {
        // OSUniversalError = OS_ERR_MSG_QUEUE_IS_EMPTY;
        // os_assert(0);
        return NULL;
    }

    p_msg = p_msg_q->head_ptr;

    p_msg_q->head_ptr = p_msg->next;

    if (p_msg_q->size_curr == 1u) /* Are there any more messages in the queue? */
    {
        p_msg_q->head_ptr = NULL;
        p_msg_q->tail_ptr = NULL;
        p_msg_q->size_curr = 0u;
    }
    else
    {
        p_msg_q->size_curr--; /* Yes, One less message in the queue */
    }

    return (p_msg);
}

int32_t os_msg_get_pure_data(msg_t *p_msg)
{
    return p_msg->sig;
}