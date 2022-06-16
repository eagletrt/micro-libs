#ifndef PWM_H
#define PWM_H

#include "main.h"
#include <inttypes.h>

#define PWM_MAX_FREQUENCIES         10
#define PWM_MAX_SOUNDS              4

typedef enum {PWM_SINE_WAVE, PWM_SQUARED_WAVE, PWM_SAW_TOOTH_WAVE } PWM_WAVE_TYPE;


typedef struct {
    uint32_t channel;
    uint8_t *duties;
    uint8_t duties_index;
    uint32_t duties_len;
    uint8_t enabled;
} sound_player;

/**
 * @brief     Set the period for the specified timer. This will affect every channel
 * 
 * @param     htim TIM Handle
 * @param     period_ms period of the timer in milliseconds
 */
void pwm_set_period(TIM_HandleTypeDef *htim, float period_ms);

/**
 * @brief     Set the duty cicle for the specified channel. Use this function only to generate a plain PWM (needs to call pwm_start_channel)
 * 
 * @param     htim TIM Handle
 * @param     channel timer channel
 * @param     duty_cicle duty cicle of the pwm. have to be between 0 and 1
 */
void pwm_set_duty_cicle(TIM_HandleTypeDef *htim, uint32_t channel, float duty_cicle);

/**
 * @brief     Generate a wave of the specified type on the specified channel
 * 
 * @param     htim TIM Handle
 * @param     channel timer channel
 * @param     wave_type wave type to generate
 * @param     freq_Hz array of the frequencies to be generated (sum of them)
 * @param     freq_size size of freq_Hz array
 */
void pwm_generate_wave(TIM_HandleTypeDef *htim, uint32_t channel, PWM_WAVE_TYPE wave_type, uint16_t *freq_Hz, uint8_t freq_size);

/**
 * @brief     Starts the specified channel
 * 
 * @param     htim TIM Handle
 * @param     channel timer channel
 */
void pwm_start_channel(TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief     Stops the specified channel
 * 
 * @param     htim TIM Handle
 * @param     channel timer channel
 */
void pwm_stop_channel(TIM_HandleTypeDef *htim, uint32_t channel);

void pwm_play_sound(TIM_HandleTypeDef *htim, sound_player *sound);

/**
 * @brief     HAL_TIM_PWMFinishedCallback
 * 
 * @param     htim TIM Handle
 */
void _pwm_tim_pulse_finished_handler(TIM_HandleTypeDef *htim);

#endif