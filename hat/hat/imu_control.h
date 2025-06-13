#ifndef IMU_CONTROL_H
#define IMU_CONTROL_H

#include <stdint.h>

#define IDLE_DUTY_LEFT 50
#define IDLE_DUTY_RIGHT 50
#define MAX_DUTY 100
#define MIN_DUTY 0
#define MAX_ABS_IMU_VAL 260
#define DUTY_TURNING_OFFSET 50

/**
 * @brief Converts accelerometer data to motor duty cycles and directions.
 * 
 * @param accel Accelerometer data (X, Y, Z).
 * @param duty_left Pointer to store the left motor duty cycle.
 * @param duty_right Pointer to store the right motor duty cycle.
 * @param direction_left Pointer to store the left motor direction ('f' or 'r').
 * @param direction_right Pointer to store the right motor direction ('f' or 'r').
 */
void convert_accel_to_duty(int16_t* accel, int* duty_left, int* duty_right,
                           char* direction_left, char* direction_right);

#endif // IMU_CONTROL_H