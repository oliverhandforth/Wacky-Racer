#include "imu_control.h"
#include <stdlib.h>

#define DEAD_ZONE_HALF_WIDTH 50

// #define DUTY_DIVISOR 1
#define DUTY_DIVISOR 1.0f

void convert_accel_to_duty(int16_t* accel, int* duty_left, int* duty_right,
                           char* direction_left, char* direction_right)
{
    int16_t X = accel[0];
    int16_t Y = accel[1];

    // Clamp values
    if (X > MAX_ABS_IMU_VAL) X = MAX_ABS_IMU_VAL;
    if (X < -MAX_ABS_IMU_VAL) X = -MAX_ABS_IMU_VAL;
    if (Y > MAX_ABS_IMU_VAL) Y = MAX_ABS_IMU_VAL;
    if (Y < -MAX_ABS_IMU_VAL) Y = -MAX_ABS_IMU_VAL;

    if (X < DEAD_ZONE_HALF_WIDTH && X > -DEAD_ZONE_HALF_WIDTH) X = 0;
    if (Y < DEAD_ZONE_HALF_WIDTH && Y > -DEAD_ZONE_HALF_WIDTH) Y = 0;

    // *duty_left = (((IDLE_DUTY_LEFT + (MAX_DUTY - IDLE_DUTY_LEFT) * Y / MAX_ABS_IMU_VAL)
    //              + DUTY_TURNING_OFFSET * X / MAX_ABS_IMU_VAL) * 2 - 100)/DUTY_DIVISOR;

    // *duty_right = (((IDLE_DUTY_RIGHT + (MAX_DUTY - IDLE_DUTY_RIGHT) * Y / MAX_ABS_IMU_VAL)
    //               - DUTY_TURNING_OFFSET * X / MAX_ABS_IMU_VAL) * 2 - 100)/DUTY_DIVISOR;

    float duty_left_float = (((IDLE_DUTY_LEFT + (MAX_DUTY - IDLE_DUTY_LEFT) * Y / MAX_ABS_IMU_VAL) + DUTY_TURNING_OFFSET * X / MAX_ABS_IMU_VAL) * 2 - 100) / DUTY_DIVISOR;

    float duty_right_float = (((IDLE_DUTY_RIGHT + (MAX_DUTY - IDLE_DUTY_RIGHT) * Y / MAX_ABS_IMU_VAL) - DUTY_TURNING_OFFSET * X / MAX_ABS_IMU_VAL) * 2 - 100) / DUTY_DIVISOR;

    *duty_left = (int)duty_left_float; // Convert back to integer
    *duty_right = (int)duty_right_float; // Convert back to integer

    // Determine directions and make duty cycles positive
    if (*duty_left < 0) {
        *direction_left = 'r';
        *duty_left = abs(*duty_left);
    } else {
        *direction_left = 'f';
    }

    if (*duty_right < 0) {
        *direction_right = 'r';
        *duty_right = abs(*duty_right);
    } else {
        *direction_right = 'f';
    }

    // Clamp to 0–MAX_DUTY
    if (*duty_left > MAX_DUTY) *duty_left = MAX_DUTY;
    if (*duty_right > MAX_DUTY) *duty_right = MAX_DUTY;

    // int duty_l = *duty_left / DUTY_DIVISOR;
    // int duty_r = *duty_right / DUTY_DIVISOR;

    // *duty_left = duty_l;
    // *duty_right = duty_r;

    // printf("duty l: %d, duty r: %d\n",
    //        *duty_left, *duty_right);
}