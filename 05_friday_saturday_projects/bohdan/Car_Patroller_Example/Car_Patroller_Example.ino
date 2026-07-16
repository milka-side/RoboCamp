#include "Car_Core.h"
#include "Car_Peripherals.h"

// ------------------------- НАЛАШТУВАННЯ -----------------------------
#define STICK_MAX        512  // діапазон стіків
#define DEADZONE          25  // "мертва зона" стіків (~5%)
#define MAX_SPEED        4095 // Максимальна швидкість моторів

// ------------------------- УЛЬТРАЗВУКОВИЙ ДАТЧИК --------------------
#define TRIG_SONIC_PIN    12  // Пін Trig
#define ECHO_SONIC_PIN    15  // Пін Echo

void setup() {
  Car_System_Setup();
  Car_Peripherals_Setup();
}

void loop() {
  Car_System_Update();
  Matrix_Update();

  if (Car_IsGamepadConnected()) {
    ControllerPtr pad = Car_GetGamepad();

    int stickY = pad->axisY();  
    int stickX = pad->axisRX();

    if (abs(stickX) < DEADZONE) {
      stickX = 0;
    }
    if (abs(stickY) < DEADZONE) {
      stickY = 0;
    }

    stickY = constrain(stickY, -STICK_MAX, STICK_MAX);
    stickX = constrain(stickX, -STICK_MAX, STICK_MAX);

    int drive = -stickY * MAX_SPEED / STICK_MAX;
    int turn  =  stickX * MAX_SPEED / STICK_MAX;

    int left  = drive + turn;
    int right = drive - turn;
  
    int biggest = max(abs(left), abs(right));
    if (biggest > 4095) {
      left  = left  * 4095 / biggest;
      right = right * 4095 / biggest;
    }

    Car_SetMotors(left, right);
  } else {
    Car_SetMotors(0, 0);
  }
}