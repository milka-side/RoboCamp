#ifndef CAR_CORE_H
#define CAR_CORE_H

#include <Arduino.h>
#include <Bluepad32.h>

void Car_System_Setup();
void Car_System_Update();

bool Car_IsGamepadConnected();
ControllerPtr Car_GetGamepad();

void Car_SetMotors(int left, int right);
void Car_SetHorn(bool on);
void Car_SetServo(int angle);

#endif
