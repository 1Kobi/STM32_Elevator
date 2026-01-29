#include "wlasny_PID.h"
#include <math.h>

void Lift_PID_Init(PID_Controller *pid) {
    pid->Kp = 5.0f;
    pid->Ki = 0.1f;
    pid->Kd = 2.0f;

    pid->prevError = 0.0f;
    pid->integrator = 0.0f;

    pid->outMin = -1000.0f;
    pid->outMax = 1000.0f;

    pid->minPWM = 340.0f;

    pid->integratorMin = -50.0f;
    pid->integratorMax = 50.0f;
}

void Lift_PID_Update_Transparent(PID_Controller *pid, float target, float measurement) {

	float error = target - measurement;

	if (fabsf(error) < 0.5f) {
		pid->out_pwm_up = 0;
		pid->out_pwm_down = 0;

		pid->prevError = 0.0f;
		pid->integrator = 0.0f;

		return;
	}

    pid->out_p = pid->Kp * error;

    pid->integrator += (pid->Ki * error);
    // Anti-Windup
    if (pid->integrator > pid->integratorMax) {
        pid->integrator = pid->integratorMax;
    } else if (pid->integrator < pid->integratorMin) {
        pid->integrator = pid->integratorMin;
    }
    pid->out_i = pid->integrator;

    pid->out_d = pid->Kd * (error - pid->prevError);
    pid->prevError = error;

    // Suma PID
    float output = pid->out_p + pid->out_i + pid->out_d;

    if (output > 2.0f) { // Używamy małego progu, żeby uniknąć szumu
    	output += pid->minPWM;
	}
	else if (output < -2.0f) {
		output -= pid->minPWM;
	}
	else {
		output = 0.0f;
	}

    if (output > pid->outMax) output = pid->outMax;
    if (output < pid->outMin) output = pid->outMin;

    // Rozdzielenie na kanały
        if (output > 0.0f) {
            pid->out_pwm_up = (int)output;
            pid->out_pwm_down = 0;
        }
        else if (output < 0.0f) {
            pid->out_pwm_up = 0;
            pid->out_pwm_down = (int)(-output);
        }
        else {
            pid->out_pwm_up = 0;
            pid->out_pwm_down = 0;
        }
}
