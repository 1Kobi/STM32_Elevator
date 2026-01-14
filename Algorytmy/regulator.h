#ifndef REGULATOR_H_
#define REGULATOR_H_

#include "arm_math.h"

typedef struct {
    arm_pid_instance_f32 PID;   // Instancja PID z biblioteki CMSIS
    float32_t output_min;       // Minimalne wysterowanie PWM (np. 0)
    float32_t output_max;       // Maksymalne wysterowanie PWM (np. ARR)
} LiftController_t;

void Lift_PID_Init(LiftController_t *lift, float kp, float ki, float kd);
float32_t Lift_PID_Compute(LiftController_t *lift, float32_t setpoint, float32_t input);

#endif /* REGULATOR_H_ */
