#include "os_kernel.h"
#include "os_timer.h"
#include "os_task.h"
#include "os_msg.h"
#include "task_list.h"
#include "os_list.h"

#define REF_TASK_TIMER_ID               ((task_id_t)OS_CFG_NUM_OF_TASKS_MAX - 2u)

static os_timer_t timer_pool[OS_CFG_TIMER_POOL_SIZE];
static os_timer_t *free_list_timer_pool;
static uint8_t timer_pool_used;

static uint32_t timer_counter;

static list_t timer_list_1;
static list_t timer_list_2;

static list_t *volatile timer_list_ptr; /* Current */
static list_t *volatile overflow_timer_list_ptr;

static volatile uint32_t next_tick_to_unblock_timer = (uint32_t)OS_CFG_DELAY_MAX;

static void timer_pool_init()
{
    uint8_t index;

    free_list_timer_pool = (os_timer_t *)timer_pool;

    for (index = 0; index < OS_CFG_TIMER_POOL_SIZE; index++)
    {
        if (index == (OS_CFG_TIMER_POOL_SIZE - 1))
        {
            timer_pool[index].next = NULL;
        }
        else
        {
            timer_pool[index].next = (os_timer_t *)&timer_pool[index + 1];
        }
    }

    timer_pool_used = 0;
}

static void init_timer_lists(void)
{
    /* Initialize lists */
    os_list_init(&timer_list_1);
    os_list_init(&timer_list_2);
    timer_list_ptr = &timer_list_1;
    overflow_timer_list_ptr = &timer_list_2;
    /********************/
}

static void timer_switch_lists()
{
    list_t *p_list_temp;
    p_list_temp = timer_list_ptr;
    timer_list_ptr = overflow_timer_list_ptr;
    overflow_timer_list_ptr = p_list_temp;

    if (list_is_empty(timer_list_ptr) == OS_TRUE)
    {
        next_tick_to_unblock_timer = OS_CFG_DELAY_MAX;
    }
    else
    {
        next_tick_to_unblock_timer = 0u;
    }
}
static void add_timer_to_list(os_timer_t *p_timer)
{
    ENTER_CRITICAL();
    {
        const uint32_t const_tick = os_task_get_tick();
        uint32_t tick_to_trigger = list_item_get_value(&(p_timer->timer_list_item));
        if (tick_to_trigger < const_tick)
        {
            /* Wake time has overflowed.  Place this item in the overflow
             * list. */
            os_list_insert(overflow_timer_list_ptr, &(p_timer->timer_list_item));
        }
        else
        {
            /* The wake time has not overflowed, so the current block list
             * is used. */
            os_list_insert(timer_list_ptr, &(p_timer->timer_list_item));
        }
    }
    EXIT_CRITICAL();
}
static void update_next_tick_to_unblock()
{
    os_timer_t *p_timer;
    uint32_t item_value;
    if (list_is_empty(timer_list_ptr) == OS_TRUE)
    {
        next_tick_to_unblock_timer = OS_CFG_DELAY_MAX;
    }
    else
    {
        p_timer = list_get_owner_of_head_item(timer_list_ptr);
        item_value = list_item_get_value(&(p_timer->timer_list_item));
        if (item_value < next_tick_to_unblock_timer)
        {
            next_tick_to_unblock_timer = item_value;
        }
    }
}

os_timer_t *os_timer_create(timer_id_t id, int32_t sig, timer_cb func_cb, uint8_t des_task_id, uint32_t period, timer_type_t type)
{
    /*It is important to set value for timer_list_item, because it holds time stamp*/
    if (des_task_id > (TASK_EOT_ID - 1U))
    {
        // OSUniversalError = OS_ERR_DES_TASK_ID_INVALID;
        os_assert(0);
        return NULL;
    }
    if (des_task_id == REF_TASK_TIMER_ID)
    {
        // OSUniversalError = OS_ERR_CAN_NOT_SET_DES_TO_ITSELF;
        os_assert(0);
        return NULL;
    }
    if (timer_pool_used >= OS_CFG_TIMER_POOL_SIZE)
    {
        // OSUniversalError = OS_ERR_TIMER_POOL_IS_FULL;
        os_assert(0);
        return NULL;
    }
    if (period == 0u)
    {
        // OSUniversalError = OS_ERR_TIMER_NOT_ACECPT_ZERO_PERIOD;
        os_assert(0);
        return NULL;
    }
    os_timer_t *p_timer;
    p_timer = free_list_timer_pool;
    free_list_timer_pool = p_timer->next;
    timer_pool_used++;

    p_timer->id = id;
    p_timer->sig = sig;
    p_timer->func_cb = func_cb;
    p_timer->des_task_id = des_task_id;

    p_timer->type = type;

    p_timer->period = period;

    /* Make connection */
    os_list_item_init(&(p_timer->timer_list_item));
    list_item_set_owner(&(p_timer->timer_list_item), (void *)p_timer);

    // list_item_set_value(&(p_timer->timer_list_item), period + os_task_get_tick());
    // add_timer_to_list(p_timer);
    // update_next_tick_to_unblock();
    // os_task_post_msg_pure(OS_CFG_TIMER_TASK_ID, TIMER_CMD_UPDATE);
    return p_timer;
}
void os_timer_remove(os_timer_t *p_timer)
{
    ENTER_CRITICAL();

    p_timer->next = free_list_timer_pool;
    free_list_timer_pool = p_timer;
    timer_pool_used--;

    if (list_item_get_list_contain(&(p_timer->timer_list_item)) != NULL)
        os_list_remove(&(p_timer->timer_list_item));

    EXIT_CRITICAL();
}

void os_timer_init(void)
{
    timer_pool_init();
    init_timer_lists();
    next_tick_to_unblock_timer = OS_CFG_DELAY_MAX;
    /*TODO: Create a task for timer here, or maybe in os_task.c and call os_timer_processing*/
}
void os_timer_processing()
{
    msg_t *p_msg;
    os_timer_t *p_timer;
    uint32_t item_value;
    static uint32_t last_time = (uint32_t)0U;
    uint32_t time_now = os_task_get_tick();
    if (time_now < last_time)
    {
        /* Overflown */
        timer_switch_lists();
    }
    if (time_now >= next_tick_to_unblock_timer)
    {
        for (;;)
        {
            if (list_is_empty(timer_list_ptr) == OS_TRUE)
            {
                next_tick_to_unblock_timer = OS_CFG_DELAY_MAX;
                break;
            }
            else
            {
                p_timer = list_get_owner_of_head_item(timer_list_ptr);
                item_value = list_item_get_value(&(p_timer->timer_list_item));
                if (item_value > time_now)
                {
                    /* Stop condition */
                    next_tick_to_unblock_timer = item_value;
                    break;
                }
                os_list_remove(&(p_timer->timer_list_item));

                if (p_timer->func_cb != NULL)
                    p_timer->func_cb();
                else
                    os_task_post_msg_pure(p_timer->des_task_id, p_timer->sig);
                if (p_timer->type == TIMER_PERIODIC)
                {
                    ENTER_CRITICAL();
                    list_item_set_value(&(p_timer->timer_list_item), p_timer->period + time_now);
                    EXIT_CRITICAL();
                    add_timer_to_list(p_timer);
                }
                else
                    os_timer_remove(p_timer); /* One shot */
                update_next_tick_to_unblock();
            }
        }
    }
    last_time = time_now;
    p_msg = os_task_wait_for_msg(next_tick_to_unblock_timer - time_now);
    if (p_msg != NULL)
        os_msg_free(p_msg);
}

void os_timer_start(os_timer_t *p_timer, uint32_t tick_to_wait)
{
    uint32_t time_now = os_task_get_tick();

    switch (p_timer->type)
    {
    case TIMER_ONE_SHOT:
        ENTER_CRITICAL();
        list_item_set_value(&(p_timer->timer_list_item), tick_to_wait + time_now + p_timer->period);
        EXIT_CRITICAL();
        break;
    case TIMER_PERIODIC:
        list_item_set_value(&(p_timer->timer_list_item), tick_to_wait + time_now);
        break;
    }

    add_timer_to_list(p_timer);
    update_next_tick_to_unblock();
    os_task_post_msg_pure(REF_TASK_TIMER_ID, 0); // Dummy signal
}

void os_timer_reset(os_timer_t *p_timer)
{
    if (list_item_get_list_contain(&(p_timer->timer_list_item)) == NULL)
    {
        // OSUniversalError = OS_ERR_TIMER_IS_NOT_RUNNING;
        os_assert(0);
        return;
    }
    uint32_t time_now = os_task_get_tick();
    os_list_remove(&(p_timer->timer_list_item));

    ENTER_CRITICAL();
    list_item_set_value(&(p_timer->timer_list_item), time_now + p_timer->period);
    EXIT_CRITICAL();
    add_timer_to_list(p_timer);

    update_next_tick_to_unblock();
    os_task_post_msg_pure(REF_TASK_TIMER_ID, 0); // Dummy signal
}

void os_timer_set_period(os_timer_t *p_timer, uint32_t period)
{   
    ENTER_CRITICAL();
    p_timer->period = period;
    EXIT_CRITICAL();
}