#include "regulator.h"
#include <math.h>

void Lift_PID_Init(LiftController_t *lift,
        float kp_up, float ki_up, float kd_up,
        float kp_down, float ki_down, float kd_down,
        int32_t deadzone, int32_t max_pwm) {
    // 1. Ustawienie wzmocnień
	lift->Kp_UP = kp_up;     lift->Ki_UP = ki_up;     lift->Kd_UP = kd_up;
	lift->Kp_DOWN = kp_down; lift->Ki_DOWN = ki_down; lift->Kd_DOWN = kd_down;

	lift->deadzone_pwm = deadzone;
	lift->max_pwm = max_pwm;

	lift->PID.Kp = kp_up;
	lift->PID.Ki = ki_up;
	lift->PID.Kd = kd_up;
	arm_pid_init_f32(&lift->PID, 1);
}

void Lift_PID_Update(LiftController_t *lift, float32_t target, float32_t current) {
    // 1. Obliczenie uchybu (Błąd = Cel - Gdzie jestem)
    float32_t error = target - current;

    // 2. Wyliczenie sterowania
    float32_t pid_out = arm_pid_f32(&lift->PID, error);

    lift->out_pwm_up = 0;
    lift->out_pwm_down = 0;

    // 3. Rozdzielacz + Deadzone + Limity
    if (pid_out > 0) {
            // --- GÓRA ---
            int32_t pwm = (int32_t)pid_out;

            // Deadzone
            if (pwm < lift->deadzone_pwm) pwm = lift->deadzone_pwm;
            // Max Limit
            if (pwm > lift->max_pwm) pwm = lift->max_pwm;

            lift->out_pwm_up = pwm;

	} else if (pid_out < 0) {
            // --- DÓŁ ---
            int32_t pwm = (int32_t)fabs(pid_out); // Wartość bezwzględna

            // Deadzone
            if (pwm < lift->deadzone_pwm) pwm = lift->deadzone_pwm;
            // Max Limit
            if (pwm > lift->max_pwm) pwm = lift->max_pwm;

            lift->out_pwm_down = pwm;
	}

    if (fabs(error) < 0.5f) {
            lift->out_pwm_up = 0;
            lift->out_pwm_down = 0;
            arm_pid_init_f32(&lift->PID, 0); //reset calki
        }
}
