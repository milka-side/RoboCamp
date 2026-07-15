/**********************************************************************
  Freenove 4WD Car for ESP32 (FNK0053) + Bluetooth-геймпад (P53 та інші)
  Основа: 01.1_Car_Move_and_Turn (Freenove) + бібліотека Bluepad32
  Поруч мають лежати: Freenove_4WD_Car_For_ESP32.h / .cpp
  (версія з папки Sketches/01.1_Car_Move_and_Turn — без LEDC!)

  ПРИВ'ЯЗКА "КОЖНІЙ МАШИНЦІ — СВІЙ ГЕЙМПАД":
    * Перший геймпад, що підключиться до "вільної" машинки (кнопка PS),
      стає її геймпадом назавжди (2 короткі "піп"). Чужі відхиляються.
    * Скидання (машинка увімкнена і працює): тримати BOOT 2 секунди
      або надіслати R в Serial Monitor. Довгий "піііп" → перезапуск.

  КЕРУВАННЯ:
    Лівий стік вгору/вниз    — газ вперед / назад
    Правий стік вліво/вправо — поворот
    R1 (утримувати)          — турбо (повна швидкість)
    Хрестик (X)              — клаксон
    D-pad вліво/вправо       — серво-"голова", D-pad вгору — 90°

  ПРО ШВИДКІСТЬ РЕАКЦІЇ (важливо, якщо змінюватимете код):
    Мотори керуються через чип PCA9685 по шині I2C — це ПОВІЛЬНА
    операція. Тому команда моторам надсилається лише коли значення
    змінилося, і не частіше ніж раз на MOTOR_UPDATE_MS. Якщо надсилати
    її на кожен Bluetooth-звіт геймпада, цикл не встигає, події стають
    у чергу і машинка реагує із запізненням у секунди.
**********************************************************************/

#include "Car_Core.h"

// ------------------------- НАЛАШТУВАННЯ -----------------------------
#define STICK_MAX        512  // діапазон стіків
#define DEADZONE          25  // "мертва зона" стіків (~5%)
#define SPEED_NORMAL    2600  // звичайна швидкість (макс. 4095)
#define SPEED_TURBO     4095  // швидкість при затиснутому R1
#define TURN_FACTOR     0.9f  // різкість повороту (0..1)
#define DEBUG_STICKS       1  // 1 = друк значень стіків, 0 = вимкнути

int currentServoAngle = 90;
unsigned long lastServoMove = 0;
unsigned long lastDebugPrint = 0;

void setup() {
  // Вся магія підключення та налаштування "захована" тут
  Car_System_Setup();
}

void loop() {
  // Оновлюємо стан Bluetooth та перевіряємо кнопку скидання
  Car_System_Update();

  if (Car_IsGamepadConnected()) {
    ControllerPtr pad = Car_GetGamepad();

    // 1. Читаємо значення стіків
    int stickY = pad->axisY();     // лівий стік: вгору = мінус
    int stickX = pad->axisRX();    // правий стік: вліво/вправо

    // 2. Відкидаємо "мертву зону" (щоб машинка не повзла сама)
    if (abs(stickY) < DEADZONE) stickY = 0;
    if (abs(stickX) < DEADZONE) stickX = 0;
    
    // 3. Обмежуємо максимальні значення
    stickY = constrain(stickY, -STICK_MAX, STICK_MAX);
    stickX = constrain(stickX, -STICK_MAX, STICK_MAX);

    // 4. Визначаємо швидкість (чи натиснута кнопка Турбо)
    int maxSpeed = pad->r1() ? SPEED_TURBO : SPEED_NORMAL;

    // 5. Розраховуємо газ та поворот
    int drive = -stickY * maxSpeed / STICK_MAX;               
    int turn  =  stickX * maxSpeed / STICK_MAX * TURN_FACTOR; 

    // 6. Аркадне змішування для лівого та правого борту
    int left  = drive + turn;
    int right = drive - turn;

    // Якщо швидкість перевищує максимум — пропорційно зменшуємо, 
    // щоб поворот працював і на повному газу
    int biggest = max(abs(left), abs(right));
    if (biggest > 4095) {
      left  = left  * 4095 / biggest;
      right = right * 4095 / biggest;
    }

    // 7. Передаємо команду моторам
    Car_SetMotors(left, right);

    // 8. Клаксон (кнопка Х)
    Car_SetHorn(pad->a());

    // 9. Керування "головою" (сервопривід) через D-pad
    if (millis() - lastServoMove > 30) {   
      uint8_t d = pad->dpad();
      if (d & DPAD_LEFT)  currentServoAngle = constrain(currentServoAngle + 2, 0, 180);
      if (d & DPAD_RIGHT) currentServoAngle = constrain(currentServoAngle - 2, 0, 180);
      if (d & DPAD_UP)    currentServoAngle = 90;
      
      Car_SetServo(currentServoAngle);
      lastServoMove = millis();
    }

#if DEBUG_STICKS
    if (millis() - lastDebugPrint > 250) {
      Serial.printf("[%lu ms] Y=%4d  RX=%4d  ->  left=%5d  right=%5d\n",
                    millis(), pad->axisY(), pad->axisRX(), left, right);
      lastDebugPrint = millis();
    }
#endif

  } else {
    // Якщо геймпад не підключено — стоїмо
    Car_SetMotors(0, 0);
  }

  delay(1);
}