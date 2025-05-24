#include <Arduino.h>
#include <ESP32Servo.h>

#include "motors.h"
int m1dir = 0;
int m2dir = 0;
int currentSpeed = 1;
const float ROTATE_FACTOR = 0.8;
int pwm1;
int pwm2;
const int speed[3] = {1, 120, 175};
const int brakeValue = 254;
int currentBrake = 254;

#include "controller.h"

#define tool1 5
#define tool2 22

#define xservo 19

const int st_timeout = 250;
Servo stool1;
uint64_t prev_st1 = 0;
Servo stool2;
uint64_t prev_st2 = 0;

Servo xservo3;

void attachServo()
{
  const int sg995minUs = 625;
  const int sg995maxUs = 2850;
  stool1.attach(tool1, sg995minUs, sg995maxUs);
  stool2.attach(tool2, sg995minUs, sg995maxUs);
  xservo3.attach(xservo, sg995minUs, sg995maxUs);
}

void initServo()
{
  stool1.setPeriodHertz(50);
  stool2.setPeriodHertz(50);
  xservo3.setPeriodHertz(50);
  attachServo();
  stool1.write(0);
  stool2.write(0);
  xservo3.write(0);
}

void detachServo()
{
  stool1.detach();
  stool2.detach();
  xservo3.detach();
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  initMotor();
  initController();
  initServo();
  attachServo();
}

bool tool1_state = 0;
bool tool2_state = 0;
int xservo3_state = 45;

void loop()
{
  if (controller.LS.A3 == 1 && millis() - prev_st1 >= st_timeout)
  {
    prev_st1 = millis();
    tool1_state = !tool1_state;
  }
  if (controller.RS.C13 == 1 && millis() - prev_st2 >= st_timeout)
  {
    prev_st2 = millis();
    tool2_state = !tool2_state;
  }
  if (controller.RD.B5 == 1)
  {
    xservo3_state = 0;
  }
  else if (controller.RD.A15 == 1)
  {
    xservo3_state = 180;
  }
  else
  {
    xservo3_state = 45;
  }
  stool1.write((tool1_state) ? 0 : 90);
  stool2.write((tool2_state) ? 0 : 50);
  xservo3.write(xservo3_state);
  // detachServo();

  // doi toc
  if (controller.RS.A12 == 1)
    currentSpeed = speed[0];
  else if (controller.RS.B9 == 1)
    currentSpeed = speed[1];
  else if (controller.RS.B8 == 1)
    currentSpeed = speed[2];

  // quay tu dong
  if (controller.LS.A0 == 1)
  {
    // vi sai trai
    m1dir = 1;
    m2dir = 0;
    pwm1 = (int)((float)currentSpeed * ROTATE_FACTOR);
    pwm2 = (int)((float)currentSpeed * ROTATE_FACTOR);
    driveMotor(M1_DIR, M1_PWM, m1dir, pwm1);
    driveMotor(M2_DIR, M2_PWM, m2dir, pwm2);
  }
  else if (controller.RS.A1 == 1)
  {
    // vi sai phai
    m1dir = 0;
    m2dir = 1;
    pwm1 = (int)((float)currentSpeed * ROTATE_FACTOR);
    pwm2 = (int)((float)currentSpeed * ROTATE_FACTOR);
    driveMotor(M1_DIR, M1_PWM, m1dir, pwm1);
    driveMotor(M2_DIR, M2_PWM, m2dir, pwm2);
  }
  else
  {
    if (controller.RD.B3 == 1)
    {
      // vi sai trai
      m1dir = 1;
      m2dir = 0;
      pwm1 = (int)((float)currentSpeed * ROTATE_FACTOR);
      pwm2 = (int)((float)currentSpeed * ROTATE_FACTOR);
    }
    else if (controller.RD.B4 == 1)
    {
      // vi sai phai
      m1dir = 0;
      m2dir = 1;
      pwm1 = (int)((float)currentSpeed * ROTATE_FACTOR);
      pwm2 = (int)((float)currentSpeed * ROTATE_FACTOR);
    }
    else
    {
      m1dir = 0;
      m2dir = 0;
      pwm1 = currentSpeed;
      pwm2 = currentSpeed;
    }
    if (controller.LD.A6 == 1)
    {
      driveMotor(M1_DIR, M1_PWM, m1dir, pwm1);
      driveMotor(M2_DIR, M2_PWM, m2dir, pwm2);
      currentBrake = brakeValue;
    }
    else if (controller.LD.B2 == 1)
    {
      driveMotor(M1_DIR, M1_PWM, (m1dir == 1) ? 0 : 1, pwm1);
      driveMotor(M2_DIR, M2_PWM, (m2dir == 1) ? 0 : 1, pwm2);
      currentBrake = 255;
    }
    else
    {
      driveMotor(M1_DIR, M1_PWM, m1dir, currentBrake);
      driveMotor(M2_DIR, M2_PWM, m2dir, currentBrake);
    }
  }
  delay(50);
  checkControllerTimeout();
}