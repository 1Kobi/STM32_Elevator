#include "regulator.h"

void Lift_PID_Init(LiftController_t *lift, float kp, float ki, float kd) {
    // 1. Ustawienie wzmocnień
    lift->PID.Kp = kp;
    lift->PID.Ki = ki;
    lift->PID.Kd = kd;

    // 2. Inicjalizacja instancji CMSIS (1 oznacza reset stanu)
    arm_pid_init_f32(&lift->PID, 1);

    // 3. Limity PWM (zależą od Twojego Timera, np. 0 - 9999)
    lift->output_min = 0.0f;
    lift->output_max = 1000.0f; // Jeśli ARR = 999
}

float32_t Lift_PID_Compute(LiftController_t *lift, float32_t setpoint, float32_t input) {
    // 1. Obliczenie uchybu (Błąd = Cel - Gdzie jestem)
    float32_t error = setpoint - input;

    if (fabs(error) < 1.0f) {
            error = 0.0f;
            // Opcjonalnie: Zeruj też człon całkujący, żeby nie narastał
            arm_pid_reset_f32(&lift->PID);
        }

    // 2. Magia CMSIS PID - wyliczenie sterowania
    float32_t output = arm_pid_f32(&lift->PID, error);

    // 3. Ograniczenie wyjścia (Saturacja) - żeby nie przekroczyć zakresu PWM
    if (output > lift->output_max) {
        output = lift->output_max;
    } else if (output < lift->output_min) {
        output = lift->output_min;
    }

    return output;
}
