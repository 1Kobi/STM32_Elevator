#ifndef WLASNY_PID_H_
#define WLASNY_PID_H_

typedef struct {
    float Kp;
    float Ki;
    float Kd;

    float prevError;
    float integrator;

    float outMin;
    float outMax;

    float integratorMin;
    float integratorMax;

    float out_p;
    float out_i;
    float out_d;

    float minPWM;

    int out_pwm_up;
    int out_pwm_down;

} PID_Controller;

void Lift_PID_Init(PID_Controller *pid);
void Lift_PID_Update_Transparent(PID_Controller *pid, float target, float measurement);

#endif /* WLASNY_PID_H_ */
