#include <Arduino.h>
#include <ESP32Servo.h>

#include "motors.h"
int m1dir = 0;
int m2dir = 0;
int speed = 1;
const float ROTATE_FACTOR = 0.8;
int pwm1;
int pwm2;

#include "controller.h"

#define tool1 21
#define tool2 22

#define xservo 19
#define rservo 18

Servo stool1;
Servo stool2;
Servo xservo3;
Servo rservo4;

void attachServo()
{
  const int sg90minUs = 1000;
  const int sg90maxUs = 2000;
  const int sg995minUs = 625;
  const int sg995maxUs = 2850;
  stool1.attach(tool1, sg90minUs, sg90maxUs);
  stool2.attach(tool2, sg90minUs, sg90maxUs);
  xservo3.attach(xservo, sg995minUs, sg995maxUs);
  rservo4.attach(rservo, sg995minUs, sg995maxUs);
}

void initServo()
{
  stool1.setPeriodHertz(50);
  stool2.setPeriodHertz(50);
  xservo3.setPeriodHertz(50);
  rservo4.setPeriodHertz(50);
  attachServo();
  stool1.write(0);
  stool2.write(0);
  xservo3.write(0);
  rservo4.write(0);
}

void detachServo()
{
  stool1.detach();
  stool2.detach();
  xservo3.detach();
  rservo4.detach();
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  initMotor();
  initController();
  initServo();
}

long long int prev_servo_update = 0;
const int servo_update_interval = 120;

bool tool1_state = 0;
bool tool2_state = 0;
int xservo3_state = 45;
bool rservo4_state = 0;

void loop()
{
  if (millis() - prev_servo_update >= servo_update_interval)
  {
    prev_servo_update = millis();
    attachServo();
    if (controller.LS.A3 == 1)
      tool1_state = !tool1_state;
    if (controller.RS.C13 == 1)
      tool2_state = !tool2_state;
    if (controller.ENC.BTN == 0)
      rservo4_state = !rservo4_state;
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
    stool1.write((tool1_state) ? 0 : 110);
    stool2.write((tool2_state) ? 0 : 110);
    xservo3.write(xservo3_state);
    rservo4.write((rservo4_state) ? 0 : 180);
    // detachServo();
  }

  // doi toc
  if (controller.RS.A12 == 1) speed = 1;
  else if (controller.RS.B9 == 1) speed = 120;
  else if (controller.RS.B8 == 1) speed = 175;


  // quay tu dong
  if (controller.LS.A0 == 1)
  {
    // vi sai trai
    m1dir = 1;
    m2dir = 0;
    pwm1 = (int)((float)speed * ROTATE_FACTOR);
    pwm2 = (int)((float)speed * ROTATE_FACTOR);
    driveMotor(M1_DIR, M1_PWM, m1dir, pwm1);
    driveMotor(M2_DIR, M2_PWM, m2dir, pwm2);
  }
  else if (controller.RS.A1 == 1)
  {
    // vi sai phai
    m1dir = 0;
    m2dir = 1;
    pwm1 = (int)((float)speed * ROTATE_FACTOR);
    pwm2 = (int)((float)speed * ROTATE_FACTOR);
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
      pwm1 = (int)((float)speed * ROTATE_FACTOR);
      pwm2 = (int)((float)speed * ROTATE_FACTOR);
    }
    else if (controller.RD.B4 == 1)
    {
      // vi sai phai
      m1dir = 0;
      m2dir = 1;
      pwm1 = (int)((float)speed * ROTATE_FACTOR);
      pwm2 = (int)((float)speed * ROTATE_FACTOR);
    }
    else
    {
      m1dir = 0;
      m2dir = 0;
      pwm1 = speed;
      pwm2 = speed;
    }
    if (controller.LD.A6 == 1)
    {
      driveMotor(M1_DIR, M1_PWM, m1dir, pwm1);
      driveMotor(M2_DIR, M2_PWM, m2dir, pwm2);
    }
    else if (controller.LD.B2 == 1)
    {
      driveMotor(M1_DIR, M1_PWM, (m1dir == 1) ? 0 : 1, pwm1);
      driveMotor(M2_DIR, M2_PWM, (m2dir == 1) ? 0 : 1, pwm2);
    }
    else
    {
      driveMotor(M1_DIR, M1_PWM, m1dir, 255);
      driveMotor(M2_DIR, M2_PWM, m2dir, 255);
    }
  }
  delay(50);
  checkControllerTimeout();
}