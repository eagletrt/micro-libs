#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>

// #define PID_ERRORS_VECTOR

typedef struct pidController_t{
    float kp;
    float ki;
    float kd;
    float integrator;
    float error;
    float sample_time;
    float set_point;
    float anti_windUp;
    #ifdef PID_ERRORS_VECTOR
    uint8_t n_prev_errors;
    int prev_error_index;
    float *prev_errors;
    #else
    float prev_error;
    #endif
} PidController_t;

#ifdef PID_ERRORS_VECTOR
void pid_init(PidController_t *pid_controller,
              float kp,
              float ki,
              float kd,
              float sample_time,
              float anti_windUp,
              float *prev_errors,
              uint8_t n_prev_errors);
#else
void pid_init(PidController_t *pid_controller,
              float kp,
              float ki,
              float kd,
              float sample_time,
              float anti_windUp);
#endif

void pid_update(PidController_t *pid_controller, float status);
float pid_compute(PidController_t *pid_controller);
void pid_reset(PidController_t *pid_controller);

#endif
