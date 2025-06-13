#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <stdio.h>

// Constants
#define PWM_FREQ_HZ 30000  

// Motor control variables
extern char motor_direction_a;
extern int motor_duty_a;
extern char motor_direction_b;
extern int motor_duty_b;

// Function declarations
void pwm_control_init(void);

void update_pwm_duty(char channel, char direction, int duty_cycle);

void prompt_command(void);

void pwm_set(char channel, char direction, int duty);

void parse_and_set_motor(const char* input);

#endif
