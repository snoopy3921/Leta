#include "os_prio.h"
static uint32_t prio_curr;
static uint8_t prio_tbl[OS_PRIO_TBL_SIZE];

void os_prio_init()
{
    uint8_t i;
    for (i = 0u; i < OS_PRIO_TBL_SIZE; i++)
    {
        prio_tbl[i] = 0u;
    }
    /* OS_CFG_PRIO_MAX-1 is the lowest priority level and that is idle task's prio     */
    os_prio_insert(OS_CFG_PRIO_MAX - 1);
}

void os_prio_insert(uint32_t prio)
{
    uint8_t bit;
    uint8_t row;

    row = (uint32_t)(prio / (8u));
    bit = (uint8_t)prio & ((8u) - 1u);
    prio_tbl[row] |= (uint8_t)1u << (((8u) - 1u) - bit);
}

void os_prio_remove(uint32_t prio)
{
    uint8_t bit;
    uint8_t row;

    row = (uint32_t)(prio / (8u));
    bit = (uint8_t)prio & ((8u) - 1u);
    prio_tbl[row] &= ~((uint8_t)1u << (((8u) - 1u) - bit));
}

uint32_t os_prio_get_highest(void)
{
    uint8_t *p_tbl;
    uint32_t prio;

    prio = 0u;
    p_tbl = &prio_tbl[0];
    while (*p_tbl == 0u)
    {                 /* Search the bitmap table for the highest priority     */
        prio += (8u); /* Compute the step of each CPU_DATA entry              */
        p_tbl++;
    }

    uint8_t bit = (uint8_t)prio & ((8u) - 1u);
    while (!(*p_tbl & ((uint8_t)1u << (((8u) - 1u) - bit))))
    {
        prio++;
        bit = (uint8_t)prio & ((8u) - 1u);
    }
    return (prio);
}

uint32_t os_prio_get_curr(void)
{
    return prio_curr;
}