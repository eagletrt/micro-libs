/**
 * @file      pid.c
 * @author    Daniele Facinelli [daniele.facinelli@studenti.unitn.it]
 * @author    Filippo Faccini [filippo.faccini@studenti.unitn.it]
 * @author    Simone Ruffini [simone.ruffini@tutanota.com]
 * @date      2018-04-27
 * @updated   2021-05-11 Simone Ruffini: refactoring
 * @ingroup
 * @prefix    PID
 * 
 * @brief     PID library 
 * 
 */

/* Includes ------------------------------------------------------------------*/

#include "pid.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

#define CONSTRAIN(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void PIDInit(
    PIDControl *pid,
    float kp,
    float ki,
    float kd,
    float sampleTimeSeconds,
    float minOutput,
    float maxOutput,
    PID_ModeTypeDef mode,
    PID_ControlActionTypeDef controlAction) {
    pid->controlAction = controlAction;
    pid->mode          = mode;
    pid->iTerm         = 0.0f;
    pid->input         = 0.0f;
    pid->lastInput     = 0.0f;
    pid->output        = 0.0f;
    pid->setpoint      = 0.0f;

    if (sampleTimeSeconds > 0.0f) {
        pid->sampleTime = sampleTimeSeconds;
    } else {
        // If the passed parameter was incorrect, set to 1 second
        pid->sampleTime = 1.0f;
    }

    PIDOutputLimitsSet(pid, minOutput, maxOutput);
    PIDTuningsSet(pid, kp, ki, kd);
}

bool PIDCompute(PIDControl *pid) {
    float error, dInput;

    if (pid->mode == PID_MODE_MANUAL) {
        return false;
    }

    // The classic PID error term
    error = (pid->setpoint) - (pid->input);

    // Compute the integral term separately ahead of time
    pid->iTerm += (pid->alteredKi) * error;

    // Constrain the integrator to make sure it does not exceed output bounds
    pid->iTerm = CONSTRAIN((pid->iTerm), (pid->outMin), (pid->outMax));

    // Take the "derivative on measurement" instead of "derivative on error"
    dInput = (pid->input) - (pid->lastInput);

    // Run all the terms together to get the overall output
    pid->output = (pid->alteredKp) * error + (pid->iTerm) - (pid->alteredKd) * dInput;

    // Bound the output
    pid->output = CONSTRAIN((pid->output), (pid->outMin), (pid->outMax));

    // Make the current input the former input
    pid->lastInput = pid->input;

    return true;
}

void PIDModeSet(PIDControl *pid, PID_ModeTypeDef mode) {
    // If the mode changed from PID_MODE_MANUAL to PID_MODE_AUTOMATIC
    if (pid->mode != mode && mode == PID_MODE_AUTOMATIC) {
        // Initialize a few PID parameters to new values
        pid->iTerm     = pid->output;
        pid->lastInput = pid->input;

        // Constrain the integrator to make sure it does not exceed output bounds
        pid->iTerm = CONSTRAIN((pid->iTerm), (pid->outMin), (pid->outMax));
    }

    pid->mode = mode;
}

void PIDOutputLimitsSet(PIDControl *pid, float min, float max) {
    // Check if the parameters are valid
    if (min >= max) {
        return;
    }

    // Save the parameters
    pid->outMin = min;
    pid->outMax = max;

    // If in automatic, apply the new constraints
    if (pid->mode == PID_MODE_AUTOMATIC) {
        pid->output = CONSTRAIN(pid->output, min, max);
        pid->iTerm  = CONSTRAIN(pid->iTerm, min, max);
    }
}

void PIDTuningsSet(PIDControl *pid, float kp, float ki, float kd) {
    // Check if the parameters are valid
    if (kp < 0.0f || ki < 0.0f || kd < 0.0f) {
        return;
    }

    // Save the parameters for displaying purposes
    pid->dispKp = kp;
    pid->dispKi = ki;
    pid->dispKd = kd;

    // Alter the parameters for PID
    pid->alteredKp = kp;
    pid->alteredKi = ki * pid->sampleTime;
    pid->alteredKd = kd / pid->sampleTime;

    // Apply reverse direction to the altered values if necessary
    if (pid->controlAction == PID_CONTROL_ACTION_REVERSE) {
        pid->alteredKp = -(pid->alteredKp);
        pid->alteredKi = -(pid->alteredKi);
        pid->alteredKd = -(pid->alteredKd);
    }
}

void PIDTuningKpSet(PIDControl *pid, float kp) {
    PIDTuningsSet(pid, kp, pid->dispKi, pid->dispKd);
}

void PIDTuningKiSet(PIDControl *pid, float ki) {
    PIDTuningsSet(pid, pid->dispKp, ki, pid->dispKd);
}

void PIDTuningKdSet(PIDControl *pid, float kd) {
    PIDTuningsSet(pid, pid->dispKp, pid->dispKi, kd);
}

void PIDcontrolActionSet(PIDControl *pid, PID_ControlActionTypeDef controlAction) {
    // If in automatic mode and the controller's sense of direction is reversed
    if (pid->mode == PID_MODE_AUTOMATIC && controlAction == PID_CONTROL_ACTION_REVERSE) {
        // Reverse sense of direction of PID gain constants
        pid->alteredKp = -(pid->alteredKp);
        pid->alteredKi = -(pid->alteredKi);
        pid->alteredKd = -(pid->alteredKd);
    }

    pid->controlAction = controlAction;
}

void PIDSampleTimeSet(PIDControl *pid, float sampleTimeSeconds) {
    float ratio;

    if (sampleTimeSeconds > 0.0f) {
        // Find the ratio of change and apply to the altered values
        ratio = sampleTimeSeconds / pid->sampleTime;
        pid->alteredKi *= ratio;
        pid->alteredKd /= ratio;

        // Save the new sampling time
        pid->sampleTime = sampleTimeSeconds;
    }
}
