#include "os_task.h"
#include "os_kernel.h"
#include "os_list.h"
#include "os_mem.h"
#include "os_timer.h"
#include "os_prio.h"
#include "os_cpu.h"
#include <string.h>

/* For strict compliance with the Cortex-M spec the task start address should
have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define VALUE_START_ADDRESS_MASK    ((uint32_t)0xfffffffeUL)

/* Constants required to set up the initial stack. */
#define VALUE_INITIAL_XPSR          (0x01000000)

#define TASK_IDLE_ID                ((task_id_t)OS_CFG_NUM_OF_TASKS_MAX - 1u)

#define TASK_IDLE_PRI               (OS_CFG_PRIO_MAX - 1u)

#define TASK_TIMER_ID               ((task_id_t)OS_CFG_NUM_OF_TASKS_MAX - 2u)

#define TASK_TIMER_PRI              ((uint8_t)OS_CFG_TIMER_TASK_PRI) 

#define TASK_TIMER_STK_SIZE         (200u) 


typedef struct task_tcb task_tcb_t;

#define SIZE_OF_TCB                 (sizeof(task_tcb_t))

static task_tcb_t *task_tcb_list[OS_CFG_NUM_OF_TASKS_MAX]; /*< Holds the list of task tcb. */

task_tcb_t *volatile tcb_curr_ptr = NULL;
task_tcb_t *volatile tcb_high_rdy_ptr = NULL;

static list_t rdy_task_list[OS_CFG_PRIO_MAX];       /*< Prioritised ready tasks. */
static list_t dly_task_list_1;                      /*< Delayed tasks. */
static list_t dly_task_list_2;                      /*< Delayed tasks (two lists are used - one for delays that have overflowed the current tick count. */
static list_t *volatile dly_task_list_ptr;          /*< Points to the delayed task list currently being used. */
static list_t *volatile overflow_dly_task_list_ptr; /*< Points to the delayed task list currently being used to hold tasks that have overflowed the current tick count. */
static list_t suspended_task_list;                  /*< Tasks that are currently suspended. */

static volatile uint16_t num_of_tasks           = (uint16_t)0U;
static volatile uint32_t tick_count             = (uint32_t)0u;
static volatile uint32_t ticks_pended           = (uint32_t)0U;
static volatile uint32_t next_tick_to_unblock   = (uint32_t)OS_CFG_DELAY_MAX; /* Initialised to portMAX_DELAY before the scheduler starts. */

static volatile uint8_t sched_is_running        = (uint8_t)OS_FALSE;

uint32_t os_task_get_tick(void)
{
    return tick_count;
}

static void task_idle_func(void *p_arg)
{
    for (;;)
    {
    }
}

static void task_timer_func(void *p_arg)
{
    for (;;)
    {
        os_timer_processing();
    }
}

struct task_tcb
{
    volatile uint32_t *stk_ptr;  /* Stack pointer, has to be the first member of TCB        */
    list_item_t state_list_item; /*Item in StateList include Ready, Blocked, Suspended List */
    list_item_t event_list_item; /*Item in Event List */
    uint32_t *stk_limit_ptr;     /* Pointer used to set stack 'watermark' limit             */
    uint8_t prio;
    size_t stk_size;            /* Size of task stack (in number of stack elements)         */
    uint32_t *stk_base_ptr;     /* Pointer to base address of stack 					    */
    task_id_t id;
    msg_queue_t msg_queue;
    task_state_t state;         /* States */
};

static void init_task_lists(void)
{
    /* Initialize lists */
    uint8_t prio;
    for (prio = 0; prio < OS_CFG_PRIO_MAX; prio++)
    {
        os_list_init(&(rdy_task_list[prio]));
    }
    os_list_init(&dly_task_list_1);
    os_list_init(&dly_task_list_2);
    os_list_init(&suspended_task_list);
    dly_task_list_ptr = &dly_task_list_1;
    overflow_dly_task_list_ptr = &dly_task_list_2;
    /********************/
}

static void task_switch_delay_lists()
{
    list_t *p_list_temp;
    p_list_temp = dly_task_list_ptr;
    dly_task_list_ptr = overflow_dly_task_list_ptr;
    overflow_dly_task_list_ptr = p_list_temp;

    if (list_is_empty(dly_task_list_ptr) == OS_TRUE)
    {
        /* The new current delayed list is empty.  Set xNextTaskUnblockTime to
         * the maximum possible value so it is  extremely unlikely that the
         * if( xTickCount >= xNextTaskUnblockTime ) test will pass until
         * there is an item in the delayed list. */
        next_tick_to_unblock = OS_CFG_DELAY_MAX;
    }
    else
    {
        /* The new current delayed list is not empty, get the value of
         * the item at the head of the delayed list.  This is the time at
         * which the task at the head of the delayed list should be removed
         * from the Blocked state. */
        task_tcb_t *p_tcb = list_get_owner_of_head_item(dly_task_list_ptr);
        uint32_t item_value = list_item_get_value(&(p_tcb->state_list_item));
        next_tick_to_unblock = item_value;
    }
}

static void add_new_task_to_rdy_list(task_tcb_t *p_tcb)
{
    ENTER_CRITICAL();
    {
        num_of_tasks++;
        if (num_of_tasks == 1)
        {
            init_task_lists();
            tcb_curr_ptr = p_tcb;
        }
        else
        {
            if (tcb_curr_ptr->prio <= p_tcb->prio)
            {
                tcb_curr_ptr = p_tcb;
            }
        }
        os_list_insert_end(&(rdy_task_list[p_tcb->prio]), &((p_tcb)->state_list_item));
        if (list_get_num_item(&(rdy_task_list[p_tcb->prio])) == 1u)
        {
            os_prio_insert(p_tcb->prio);
        }
        /*Save state*/
        p_tcb->state = TASK_STATE_READY;
    }
    EXIT_CRITICAL();
}

static void add_task_to_rdy_list(task_tcb_t *p_tcb)
{
    os_list_insert_end(&(rdy_task_list[p_tcb->prio]), &((p_tcb)->state_list_item));
    if (list_get_num_item(&(rdy_task_list[p_tcb->prio])) == 1u)
    {
        os_prio_insert(p_tcb->prio);
    }
    /*Save state*/
    p_tcb->state = TASK_STATE_READY;
}

static void add_curr_task_to_delay_list(uint32_t tick_to_delay, uint8_t can_block_indefinitely)
{
    uint32_t time_to_wake;
    const uint32_t const_tick = tick_count;
    if (os_list_remove(&(tcb_curr_ptr->state_list_item)) == 0u)
    {
        os_prio_remove(tcb_curr_ptr->prio);
    }
    if ((tick_to_delay == OS_CFG_DELAY_MAX) && (can_block_indefinitely != OS_FALSE))
    {
        /* Add the task to the suspended task list instead of a delayed task
         * list to ensure it is not woken by a timing event.  It will block
         * indefinitely. */
        os_list_insert_end(&suspended_task_list, &(tcb_curr_ptr->state_list_item));
        /*Save state*/
        tcb_curr_ptr->state = TASK_STATE_SUSPENDED;
    }
    else
    {
        time_to_wake = const_tick + tick_to_delay;
        list_item_set_value(&(tcb_curr_ptr->state_list_item), time_to_wake);

        if (time_to_wake < const_tick)
        {
            /* Wake time has overflowed.  Place this item in the overflow
             * list. */
            os_list_insert(overflow_dly_task_list_ptr, &(tcb_curr_ptr->state_list_item));
        }
        else
        {
            /* The wake time has not overflowed, so the current block list
             * is used. */
            os_list_insert(dly_task_list_ptr, &(tcb_curr_ptr->state_list_item));

            /*Update next tick to block is important in order scheduler not to miss this stamp
            Just update next tick to block in this branch because overflow delay is just the background list
            */

            if (time_to_wake < next_tick_to_unblock)
            {
                next_tick_to_unblock = time_to_wake;
            }
        }
        /*Save state*/
        tcb_curr_ptr->state = TASK_STATE_DELAYED;
    }
    uint8_t highest_prio = os_prio_get_highest();
    tcb_high_rdy_ptr = list_get_owner_of_head_item(&(rdy_task_list[highest_prio]));

    /*Save state*/
    tcb_high_rdy_ptr->state = TASK_STATE_RUNNING;
}

static task_tcb_t *os_task_create(task_id_t id,
                                  task_func_t pf_task,
                                  void *p_arg,
                                  uint8_t prio,
                                  size_t queue_size,
                                  size_t stack_size)
{
    if (sched_is_running == OS_TRUE)
    {
        // OSUniversalError = OS_ERR_SCHED_IS_RUNNING;
        os_assert(0);
        return NULL;
    }
    if (prio > (OS_CFG_PRIO_MAX - 1U))
    {
        // OSUniversalError = OS_ERR_TCB_PRIO_INVALID;
        os_assert(0);
        return NULL;
    }
    if (pf_task == NULL)
    {
        // OSUniversalError = OS_ERR_TCB_FUNC_INVALID;
        os_assert(0);
        return NULL;
    }
    if (stack_size < OS_CFG_TASK_STK_SIZE_MIN)
    {
        // OSUniversalError = OS_ERR_TCB_STK_SIZE_INVALID;
        os_assert(0);
        return NULL;
    }
    if (num_of_tasks > OS_CFG_NUM_OF_TASKS_MAX - 1)
    {
        // OSUniversalError = OS_ERR_EXCEED_MAX_TASK_NUM;
        os_assert(0);
        return NULL;
    }
    

    task_tcb_t *p_new_tcb;
    uint32_t *p_stack;

    p_stack = os_mem_malloc(stack_size * sizeof(uint32_t));
    if (p_stack != NULL)
    {
        p_new_tcb = (task_tcb_t *)os_mem_malloc(SIZE_OF_TCB);
        if (p_new_tcb != NULL)
        {
            memset((void *)p_new_tcb, 0x00, SIZE_OF_TCB);

            /*Save stack limit pointer*/
            p_new_tcb->stk_limit_ptr = p_stack;
        }
        else
        {
            os_mem_free(p_stack);
            // OSUniversalError = OS_ERR_TCB_NOT_ENOUGH_MEM_ALLOC;
            os_assert(0);
            return NULL;
        }
    }
    else
    {
        // OSUniversalError = OS_ERR_TCB_NOT_ENOUGH_MEM_ALLOC;
        os_assert(0);
        return NULL;
    }

    /*Now TCB and stack are created*/
    uint32_t *p_stack_ptr;

    /* Fill the stack with a known value to assist debugging. */
    //( void ) memset( p_new_tcb->stk_limit_ptr, OS_CFG_TASK_STACK_FILL_BYTE, stack_size );

    /*Init stack frame*/
    p_stack_ptr = &p_stack[stack_size - (uint32_t)1];
    p_stack_ptr = (uint32_t *)(((uint32_t)p_stack_ptr) & (~((uint32_t)0x007))); /* Allign byte */
    // p_stack_ptr             =   ( p_stack + ( uint32_t )( ( uint32_t )( stack_size / 4 ) - (uint32_t) 1 ) );
    *(--p_stack_ptr) = VALUE_INITIAL_XPSR;                             /*Add offset and assign value for xPSR */
    *(--p_stack_ptr) = ((uint32_t)pf_task) & VALUE_START_ADDRESS_MASK; /* PC */
    *(--p_stack_ptr) = (uint32_t)0x000000EU;                           /* LR */
    p_stack_ptr -= 5;                                                  /* R12, R3, R2 and R1. */
    *p_stack_ptr = (uint32_t)p_arg;                                    /* R0 */
    p_stack_ptr -= 8;                                                  /* R11, R10, R9, R8, R7, R6, R5 and R4. */

    /*Save top of stack (Stack pointer)*/
    p_new_tcb->stk_ptr = p_stack_ptr;

    /*Save stack size*/
    p_new_tcb->stk_size = stack_size;

    /*Save ID*/
    p_new_tcb->id = id;

    /*Save prio*/
    p_new_tcb->prio = prio;
    // os_prio_insert(prio);

    os_msg_queue_init(&(p_new_tcb->msg_queue), queue_size);

    /* Init linked lists */
    os_list_item_init(&(p_new_tcb->state_list_item));
    os_list_item_init(&(p_new_tcb->event_list_item));

    list_item_set_owner(&(p_new_tcb->state_list_item), (void *)p_new_tcb);
    list_item_set_owner(&(p_new_tcb->event_list_item), (void *)p_new_tcb);

    list_item_set_value(&(p_new_tcb->state_list_item), prio);

    add_new_task_to_rdy_list(p_new_tcb);

    return p_new_tcb;
}

void os_task_create_list(task_t *task_tbl, uint8_t size)
{
    uint8_t idx = 0;
    task_tcb_t *p_tcb;
    while (idx < size)
    {
        p_tcb = os_task_create((task_id_t)task_tbl[idx].id,
                               (task_func_t)task_tbl[idx].pf_task,
                               (void *)task_tbl[idx].p_arg,
                               (uint8_t)task_tbl[idx].prio,
                               (size_t)task_tbl[idx].queue_size,
                               (size_t)task_tbl[idx].stack_size);
        task_tcb_list[task_tbl[idx].id] = p_tcb;
        idx++;
    }
    p_tcb = os_task_create((task_id_t)TASK_TIMER_ID,
                           (task_func_t)task_timer_func,
                           (void *)NULL,
                           (uint8_t)TASK_TIMER_PRI,
                           (size_t)(OS_CFG_TASK_MSG_Q_SIZE_NORMAL),
                           (size_t)TASK_TIMER_STK_SIZE);
    task_tcb_list[TASK_TIMER_ID] = p_tcb;

    p_tcb = os_task_create((task_id_t)TASK_IDLE_ID,
                           (task_func_t)task_idle_func,
                           (void *)NULL,
                           (uint8_t)TASK_IDLE_PRI,
                           (size_t)(OS_CFG_TASK_MSG_Q_SIZE_NORMAL),
                           (size_t)OS_CFG_TASK_STK_SIZE_MIN);
    task_tcb_list[TASK_IDLE_ID] = p_tcb;
}

uint8_t os_task_increment_tick(void)
{
    uint8_t is_switch_needed = OS_FALSE;
    task_tcb_t *p_tcb;
    uint32_t item_value;

    const uint32_t const_tick = tick_count + (uint32_t)1;

    /* Increment the RTOS tick, switching the delayed and overflowed
     * delayed lists if it wraps to 0. */
    tick_count = const_tick;

    if (const_tick == (uint32_t)0U) /* Overflowed, switch delaylist*/
    {
        task_switch_delay_lists();
    }

    if (const_tick >= next_tick_to_unblock)
    {
        for (;;)
        {
            if (list_is_empty(dly_task_list_ptr) == OS_TRUE)
            {
                next_tick_to_unblock = OS_CFG_DELAY_MAX;
                break;
            }
            else
            {
                p_tcb = list_get_owner_of_head_item(dly_task_list_ptr);
                item_value = list_item_get_value(&(p_tcb->state_list_item));
                if (item_value > const_tick)
                {
                    /* Stop condition */
                    next_tick_to_unblock = item_value;
                    break;
                }
                os_list_remove(&(p_tcb->state_list_item)); /*Remove from block state*/

                /* Is the task waiting on an event also?  If so remove
                 * it from the event list. */
                if (list_item_get_list_contain(&(p_tcb->event_list_item)) != NULL)
                {
                    os_list_remove(&(p_tcb->event_list_item));
                }
                add_task_to_rdy_list(p_tcb);
                if (p_tcb->prio < tcb_curr_ptr->prio)
                {
                    tcb_high_rdy_ptr = p_tcb;
                    tcb_curr_ptr->state = TASK_STATE_READY;
                    is_switch_needed = OS_TRUE;
                }
            }
        }
    }

    uint8_t highest_prio = os_prio_get_highest();
    if (list_get_num_item(&(rdy_task_list[highest_prio])) > 1u)
    {
        tcb_high_rdy_ptr = list_get_owner_of_next_item(&(rdy_task_list[highest_prio]));
        tcb_curr_ptr->state = TASK_STATE_READY;
        is_switch_needed = OS_TRUE;
    }
    /*Save state*/
    if(is_switch_needed == OS_TRUE) tcb_high_rdy_ptr->state = TASK_STATE_RUNNING;
    return is_switch_needed;
}

void os_task_delay(const uint32_t tick_to_delay)
{
    if (tick_to_delay > (uint32_t)0U)
    {
        ENTER_CRITICAL();
        add_curr_task_to_delay_list(tick_to_delay, OS_FALSE);
        os_cpu_trigger_PendSV();
        EXIT_CRITICAL();
    }
}

void os_task_start(void)
{
    tcb_high_rdy_ptr = list_get_owner_of_head_item(&(rdy_task_list[os_prio_get_highest()]));
    tcb_curr_ptr = tcb_high_rdy_ptr;
    tick_count = 0u;
    next_tick_to_unblock = OS_CFG_DELAY_MAX;
    sched_is_running = OS_TRUE;
}

void os_task_post_msg_dynamic(uint8_t des_task_id, int32_t sig, void *p_content, uint8_t msg_size)
{
    ENTER_CRITICAL();
    if (task_tcb_list[des_task_id] == tcb_curr_ptr)
    {
        // OSUniversalError = OS_ERR_TASK_POST_MSG_TO_ITSELF;
        os_assert(0);
        EXIT_CRITICAL();
        return;
    }

    switch (task_tcb_list[des_task_id]->state)
    {
    case TASK_STATE_SUSPENDED_ON_MSG:
        os_msg_queue_put_dynamic(&(task_tcb_list[des_task_id]->msg_queue),
                                 sig,
                                 p_content,
                                 msg_size);

        /* Is the task waiting on an event ?  If so remove
         * it from the event list. */
        if (list_item_get_list_contain(&(task_tcb_list[des_task_id]->event_list_item)) != NULL)
        {
            os_list_remove(&(task_tcb_list[des_task_id]->event_list_item));
        }
        add_task_to_rdy_list(task_tcb_list[des_task_id]);
        if (task_tcb_list[des_task_id]->prio < tcb_curr_ptr->prio)
        {
            tcb_high_rdy_ptr = task_tcb_list[des_task_id];

            /*Save state*/
            tcb_high_rdy_ptr->state = TASK_STATE_RUNNING;

            os_cpu_trigger_PendSV();
        }
        EXIT_CRITICAL();
        break;
    case TASK_STATE_DELAYED_ON_MSG:
        os_msg_queue_put_dynamic(&(task_tcb_list[des_task_id]->msg_queue),
                                 sig,
                                 p_content,
                                 msg_size);
        /* Is the task waiting on an event ?  If so remove
         * it from the event list. */
        if (list_item_get_list_contain(&(task_tcb_list[des_task_id]->state_list_item)) != NULL)
        {
            os_list_remove(&(task_tcb_list[des_task_id]->state_list_item));
        }

        add_task_to_rdy_list(task_tcb_list[des_task_id]);
        if (task_tcb_list[des_task_id]->prio < tcb_curr_ptr->prio)
        {
            tcb_high_rdy_ptr = task_tcb_list[des_task_id];

            /*Save state*/
            tcb_high_rdy_ptr->state = TASK_STATE_RUNNING;

            os_cpu_trigger_PendSV();
        }
        EXIT_CRITICAL();
        break;

    default:
        os_msg_queue_put_dynamic(&(task_tcb_list[des_task_id]->msg_queue),
                                 sig,
                                 p_content,
                                 msg_size);
        EXIT_CRITICAL();
        break;
    }
}

void os_task_post_msg_pure(uint8_t des_task_id, int32_t sig)
{
    ENTER_CRITICAL();
    if (task_tcb_list[des_task_id] == tcb_curr_ptr)
    {
        // OSUniversalError = OS_ERR_TASK_POST_MSG_TO_ITSELF;
        os_assert(0);
        EXIT_CRITICAL();
        return;
    }
    switch (task_tcb_list[des_task_id]->state)
    {
    case TASK_STATE_SUSPENDED_ON_MSG:
        os_msg_queue_put_pure(&(task_tcb_list[des_task_id]->msg_queue), sig);
        /* Is the task waiting on an event ?  If so remove
         * it from the event list. */
        if (list_item_get_list_contain(&(task_tcb_list[des_task_id]->event_list_item)) != NULL)
        {
            os_list_remove(&(task_tcb_list[des_task_id]->event_list_item));
        }
        add_task_to_rdy_list(task_tcb_list[des_task_id]);
        if (task_tcb_list[des_task_id]->prio < tcb_curr_ptr->prio)
        {
            tcb_high_rdy_ptr = task_tcb_list[des_task_id];

            /*Save state*/
            tcb_high_rdy_ptr->state = TASK_STATE_RUNNING;

            os_cpu_trigger_PendSV();
        }
        EXIT_CRITICAL();
        break;
    case TASK_STATE_DELAYED_ON_MSG:
        os_msg_queue_put_pure(&(task_tcb_list[des_task_id]->msg_queue), sig);
        /* Is the task waiting on an event ?  If so remove
         * it from the event list. */
        if (list_item_get_list_contain(&(task_tcb_list[des_task_id]->state_list_item)) != NULL)
        {
            os_list_remove(&(task_tcb_list[des_task_id]->state_list_item));
        }

        /* Is the task waiting on an event also?  If so remove
         * it from the event list. */
        if (list_item_get_list_contain(&(task_tcb_list[des_task_id]->event_list_item)) != NULL)
        {
            os_list_remove(&(task_tcb_list[des_task_id]->event_list_item));
        }

        add_task_to_rdy_list(task_tcb_list[des_task_id]);
        if (task_tcb_list[des_task_id]->prio < tcb_curr_ptr->prio)
        {
            tcb_high_rdy_ptr = task_tcb_list[des_task_id];

            /*Save state*/
            tcb_high_rdy_ptr->state = TASK_STATE_RUNNING;

            os_cpu_trigger_PendSV();
        }
        EXIT_CRITICAL();
        break;

    default: /*DELAYED, SUSPEND, RUNNING*/
        os_msg_queue_put_pure(&(task_tcb_list[des_task_id]->msg_queue), sig);
        EXIT_CRITICAL();
        break;
    }
}

msg_t *os_task_wait_for_msg(uint32_t time_out)
{
    msg_t *p_msg = os_msg_queue_get(&(task_tcb_list[tcb_curr_ptr->id]->msg_queue));
    if (time_out > (uint32_t)0U && p_msg == NULL)
    {
        ENTER_CRITICAL();

        add_curr_task_to_delay_list(time_out, OS_TRUE); // Can block indefinitely
        if (time_out == OS_CFG_DELAY_MAX)
        {
            tcb_curr_ptr->state = TASK_STATE_SUSPENDED_ON_MSG;
        }
        else
        {
            tcb_curr_ptr->state = TASK_STATE_DELAYED_ON_MSG;
        }
        os_cpu_trigger_PendSV();
        EXIT_CRITICAL();

        p_msg = os_msg_queue_get(&(task_tcb_list[tcb_curr_ptr->id]->msg_queue));
        return p_msg;
    }
    else
    {
        return p_msg;
    }
}