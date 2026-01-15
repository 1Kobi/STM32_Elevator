#ifndef REGULATOR_H_
#define REGULATOR_H_

#include "arm_math.h"

typedef struct {
    arm_pid_instance_f32 PID;   // Instancja PID z biblioteki CMSIS

    float Kp_UP, Ki_UP, Kd_UP;
    float Kp_DOWN, Ki_DOWN, Kd_DOWN;

    int32_t deadzone_pwm;     // Minimalny PWM startowy (np. 250)
    int32_t max_pwm;          // Maksymalny PWM (np. 1000 lub 4500)
    int32_t out_pwm_up;       // Gotowa wartość do TIM_CH1
    int32_t out_pwm_down;

} LiftController_t;

void Lift_PID_Init(LiftController_t *lift,
        float kp_up, float ki_up, float kd_up,
        float kp_down, float ki_down, float kd_down,
        int32_t deadzone, int32_t max_pwm);
void Lift_PID_Update(LiftController_t *lift, float32_t target, float32_t current);

#endif /* REGULATOR_H_ */
