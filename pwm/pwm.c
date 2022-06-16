#include "pwm.h"
#include "timer_utils.h"
#include "math.h"
#include <string.h>

struct {
    uint32_t rep_counter;
    uint32_t channel;
    uint8_t freq_size;
    uint16_t freq_Hz[PWM_MAX_FREQUENCIES];
    PWM_WAVE_TYPE wave_type;
    uint8_t enabled;
} wave_generation;

sound_player *sounds[PWM_MAX_SOUNDS];
uint8_t sound_count = 0;

void pwm_set_period(TIM_HandleTypeDef *htim, float period_ms) {
    __HAL_TIM_SetAutoreload(htim, TIM_MS_TO_TICKS(htim, period_ms)); //set the period
}

void pwm_set_duty_cicle(TIM_HandleTypeDef *htim, uint32_t channel, float duty_cicle) {
    if(duty_cicle < 0 || duty_cicle > 1) return;
    
   __HAL_TIM_SetCompare(htim, channel, __HAL_TIM_GetAutoreload(htim) * duty_cicle);
}

void pwm_generate_wave(TIM_HandleTypeDef *htim, uint32_t channel, PWM_WAVE_TYPE wave_type, uint16_t *freq_Hz, uint8_t freq_size) {
    memcpy(wave_generation.freq_Hz, freq_Hz, sizeof(*freq_Hz)*freq_size);
    wave_generation.freq_size = freq_size;
    wave_generation.rep_counter = 0;
    wave_generation.channel = channel;
    wave_generation.wave_type = wave_type;
    wave_generation.enabled = 1;
    HAL_TIM_PWM_Start_IT(htim, channel);
}

void pwm_start_channel(TIM_HandleTypeDef *htim, uint32_t channel) {
    HAL_TIM_PWM_Start_IT(htim, channel);
}

void pwm_stop_channel(TIM_HandleTypeDef *htim, uint32_t channel) {
    if(wave_generation.enabled) wave_generation.enabled = 0;
    HAL_TIM_PWM_Stop_IT(htim, channel);
}

void pwm_play_sound(TIM_HandleTypeDef *htim, sound_player *sound) {
    sound->enabled = 1;
    sound->duties_index = 0;
    if(sound_count == PWM_MAX_SOUNDS-1) return;
    sounds[sound_count++] = sound;
    HAL_TIM_PWM_Start_IT(htim, sound->channel);
}

void _pwm_tim_pulse_finished_handler(TIM_HandleTypeDef *htim) {
    if(wave_generation.enabled) {
        float Tpwm = TIM_TICKS_TO_MS(htim, __HAL_TIM_GetAutoreload(htim)) / 1000;
        float duty_cicle = 0;
        switch (wave_generation.wave_type)
        {
        case PWM_SINE_WAVE:
            for(uint8_t i=0; i<wave_generation.freq_size; ++i) {
                duty_cicle += sinf(2*M_PI*wave_generation.freq_Hz[i]*Tpwm*(wave_generation.rep_counter % (uint32_t)(1/(wave_generation.freq_Hz[i] * Tpwm))))+1;
            }
            duty_cicle /= wave_generation.freq_size*2;
            break;

        case PWM_SQUARED_WAVE:
            for(uint8_t i=0; i<wave_generation.freq_size; ++i) {
                if(wave_generation.rep_counter % (uint32_t)(1/(wave_generation.freq_Hz[0] * Tpwm)) > (uint32_t)(1/(wave_generation.freq_Hz[0] * Tpwm))/2) {
                    duty_cicle += 1;
                }
            }
            duty_cicle /= wave_generation.freq_size;
            break;

        case PWM_SAW_TOOTH_WAVE:
            for(uint8_t i=0; i<wave_generation.freq_size; ++i) {
                float t = (wave_generation.rep_counter % (uint32_t)(1/(wave_generation.freq_Hz[0] * Tpwm)));
                duty_cicle = t/((uint32_t)(1/(wave_generation.freq_Hz[0] * Tpwm)));
            }
            duty_cicle /= wave_generation.freq_size;
            break;

        default:
            break;
        }

        __HAL_TIM_SetCompare(htim, wave_generation.channel, __HAL_TIM_GetAutoreload(htim) * duty_cicle);

        ++wave_generation.rep_counter;
    }
    for(uint8_t i=0; i<sound_count; ++i) {
        if(sounds[i]->enabled) {
            if(sounds[i]->duties_index < sounds[i]->duties_len)
                __HAL_TIM_SetCompare(htim, sounds[i]->channel, __HAL_TIM_GetAutoreload(htim) * ((float)sounds[i]->duties[(sounds[i]->duties_index)++] / UINT8_MAX));
            else
                sounds[i]->enabled = 0;
        }
    }
}
