#ifndef motors_h
#define motors_h 

#include <Arduino.h>

#define M1_PWM 32
#define M1_DIR 33

#define M2_PWM 25
#define M2_DIR 26

void initMotor();

void driveMotor(int dirpin, int pwmpin, int dir, int pwm);

#endif