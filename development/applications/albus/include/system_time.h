#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

#include <stdint.h>

/** Get difference between two system times in ticks
 * @param  time1 First system time - (in ticks)
 * @param  time2 Second system time - (in ticks)
 * @retval Difference between times in ticks
*/
uint32_t system_time_cmp_ticks(uint32_t time1, uint32_t time2);

/** Get difference between two system times in us
 * @param  time1 First system time - (in ticks)
 * @param  time2 Second system time - (in ticks)
 * @retval Difference between times in us
*/
uint32_t system_time_cmp_us(uint32_t ticks1, uint32_t ticks2);

/** Get difference between two system times in ms
 * @param  time1 First system time - (in ticks)
 * @param  time2 Second system time - (in ticks)
 * @retval Difference between times in ms
*/
uint32_t system_time_cmp_ms(uint32_t ticks1, uint32_t ticks2);

/** Get timer's cnt
 * @retval Tick count of system time timer
*/
uint32_t system_time_get(void);

/* Initialize system_time module */
void system_time_init(void);

#endif // SYSTEM_TIME_H