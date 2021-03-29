#include "system_time.h"

#include "board.h"

#define TIMER_FREQ (1000000)
#define PRESCALER  (((SYSCLK_FREQ)/TIMER_FREQ) - 1)
#define AUTORELOAD 0xFFFFFFFF

#define TICKS_TO_US(ticks) (ticks)
#define TICKS_TO_MS(ticks) ((ticks)/1000)

static void timer_init(void)
{
  LL_TIM_InitTypeDef timer_config = {0};

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  timer_config.Prescaler = PRESCALER;
  timer_config.CounterMode = LL_TIM_COUNTERMODE_UP;
  timer_config.Autoreload = AUTORELOAD;
  timer_config.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(SYSTEM_TIME_TIMER, &timer_config);
  LL_TIM_EnableARRPreload(SYSTEM_TIME_TIMER);
  LL_TIM_SetClockSource(SYSTEM_TIME_TIMER, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(SYSTEM_TIME_TIMER, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(SYSTEM_TIME_TIMER);
  LL_TIM_EnableCounter(SYSTEM_TIME_TIMER);
}

/** Get difference between two system times in ticks
 * @param  time1 First system time - (in ticks)
 * @param  time2 Second system time - (in ticks)
 * @retval Difference between times in ticks
*/
inline uint32_t system_time_cmp_ticks(uint32_t time1, uint32_t time2)
{
  if (time2 < time1) {
    return (AUTORELOAD - time1) + time2;
  }
  return time2 - time1;
}

/** Get difference between two system times in us
 * @param  time1 First system time - (in ticks)
 * @param  time2 Second system time - (in ticks)
 * @retval Difference between times in us
*/
inline uint32_t system_time_cmp_us(uint32_t ticks1, uint32_t ticks2)
{
  uint32_t tick_diff = system_time_cmp_us(ticks1, ticks2);
  return TICKS_TO_US(tick_diff);
}

/** Get difference between two system times in ms
 * @param  time1 First system time - (in ticks)
 * @param  time2 Second system time - (in ticks)
 * @retval Difference between times in ms
*/
inline uint32_t system_time_cmp_ms(uint32_t ticks1, uint32_t ticks2)
{
  uint32_t tick_diff = system_time_cmp_us(ticks1, ticks2);
  return TICKS_TO_MS(tick_diff);
}

/** Get timer's cnt
 * @retval Tick count of system time timer
*/
uint32_t system_time_get(void)
{
  return SYSTEM_TIME_TIMER->CNT;
}

/* Initialize system_time module */
void system_time_init(void)
{
  timer_init();
}