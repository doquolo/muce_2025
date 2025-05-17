#include <motors.h>

void initMotor()
{
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  digitalWrite(M1_DIR, LOW);
  digitalWrite(M2_DIR, LOW);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  analogWrite(M1_PWM, 1);
  analogWrite(M2_PWM, 1);
}

void driveMotor(int dirpin, int pwmpin, int dir, int pwm)
{
  analogWrite(pwmpin, (dir == -1) ? 255 : pwm);
  digitalWrite(dirpin, (dir == -1) ? abs(dir) : dir);
}
