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

void printControllerDebug(ControllerPtr ctl);
void printRawControllerDebug(ControllerPtr ctl);

// ==================== ТИП ГЕЙМПАДА (мапінг кнопок) ====================
#define CONTROLLER_TYPE_BLUE 1
#define CONTROLLER_TYPE_GREY 2
#define CONTROLLER_TYPE CONTROLLER_TYPE_GREY // Виберіть CONTROLLER_TYPE_GREY (сірий) чи CONTROLLER_TYPE_BLUE (синій) залежно від кольору вашого геймпаду

bool Btn_Cross(ControllerPtr ctl);      // X / Хрестик
bool Btn_Circle(ControllerPtr ctl);     // Circle / Кружечок
bool Btn_Square(ControllerPtr ctl);     // Square / Квадрат
bool Btn_Triangle(ControllerPtr ctl);   // Triangle / Трикутник

bool Btn_L1(ControllerPtr ctl);
bool Btn_R1(ControllerPtr ctl);
bool Btn_L2(ControllerPtr ctl);
bool Btn_R2(ControllerPtr ctl);

bool Btn_L3(ControllerPtr ctl);         // клік лівого стіка
bool Btn_R3(ControllerPtr ctl);         // клік правого стіка

bool Btn_Start(ControllerPtr ctl);
bool Btn_Select(ControllerPtr ctl);     // Select / Share
bool Btn_System(ControllerPtr ctl);     // PS / System

bool Btn_DpadUp(ControllerPtr ctl);
bool Btn_DpadDown(ControllerPtr ctl);
bool Btn_DpadLeft(ControllerPtr ctl);
bool Btn_DpadRight(ControllerPtr ctl);
// ========================================================================

#endif