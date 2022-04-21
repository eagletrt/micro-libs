#include <inttypes.h>

/**
 * @brief     Check whether the timer is placed in either APB1 or APB2 bus
 * 
 * @param     __HANDLE__ TIM Handle
 * @return    True if the timer is on APB1 bus false if it is on APB2 
 */
#define _M_GET_TIM_APB_PLACEMENT(__HANDLE__) (((__HANDLE__)->Instance < (TIM_TypeDef *)APB2PERIPH_BASE) ? 1U : 0U)

#define TIM_GET_FREQ(TIM)             (uint32_t)(TIM_GetInternalClkFreq((TIM)) / ((TIM)->Instance->PSC + 1))

#define TIM_MS_TO_TICKS(TIM, MS)      (uint32_t)(((float)TIM_GET_FREQ((TIM)) * (MS)) / 1000)

#define TIM_TICKS_TO_MS(TIM, TICKS)   (((float)(TICKS) * 1000) / TIM_GET_FREQ((TIM)))

uint32_t TIM_GetInternalClkFreq(TIM_HandleTypeDef *htim);