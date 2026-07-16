/**********************************************************************
  Freenove 4WD Car for ESP32 (FNK0053) + Bluetooth-геймпад (P53 та інші)
  
  Додано інтеграцію з Car_Peripherals:
    * Матриця та світлодіоди реагують на перешкоди, повороти та авто-режим.
**********************************************************************/

#include "Car_Core.h"
#include "Car_Peripherals.h" // Підключаємо наш новий файл для екрану та діодів

// ------------------------- НАЛАШТУВАННЯ -----------------------------
#define STICK_MAX        512  // діапазон стіків
#define DEADZONE          25  // "мертва зона" стіків (~5%)
#define SPEED_NORMAL    2600  // звичайна швидкість (макс. 4095)
#define SPEED_TURBO     4095  // швидкість при затиснутому R1
#define TURN_FACTOR     0.9f  // різкість повороту (0..1)
#define DEBUG_STICKS       1  // 1 = друк значень стіків, 0 = вимкнути

// ------------------------- УЛЬТРАЗВУКОВИЙ ДАТЧИК --------------------
#define PIN_SONIC_TRIG    12  // Пін Trig
#define PIN_SONIC_ECHO    15  // Пін Echo
#define OBSTACLE_DIST     20  // Дистанція реакції на перешкоду (см)

int currentServoAngle = 90;
unsigned long lastServoMove = 0;
unsigned long lastDebugPrint = 0;

// Змінні для датчика та режимів
unsigned long lastSonicCheck = 0;
int currentDistance = 999;
bool autoMode = false;
bool lastTriangleBtnState = false;

// Змінні для візуальних ефектів
int currentVisualState = -1;
unsigned long lastBlinkTime = 0;
bool blinkState = false;

// Функція для отримання дистанції
int getDistance() {
  digitalWrite(PIN_SONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_SONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);
  
  long duration = pulseIn(PIN_SONIC_ECHO, HIGH, 30000); 
  if (duration == 0) return 999; 
  
  return duration / 58; 
}

void setup() {
  pinMode(PIN_SONIC_TRIG, OUTPUT);
  pinMode(PIN_SONIC_ECHO, INPUT);

  Car_System_Setup();
  Car_Peripherals_Setup(); // Налаштовуємо екран та світлодіоди
}

void loop() {
  Car_System_Update();
  Matrix_Update(); // ОБОВ'ЯЗКОВО: дозволяє анімаціям (очам) рухатися

  if (Car_IsGamepadConnected()) {
    ControllerPtr pad = Car_GetGamepad();

    // 1. Читаємо дистанцію та стіки
    if (millis() - lastSonicCheck > 50) {
      currentDistance = getDistance();
      lastSonicCheck = millis();
    }
    bool obstacleInFront = (currentDistance > 0 && currentDistance < OBSTACLE_DIST);
    
    int stickY = pad->axisY();  
    int stickX = pad->axisRX(); 
    if (abs(stickY) < DEADZONE) stickY = 0;
    if (abs(stickX) < DEADZONE) stickX = 0;

    // 2. Перемикання режимів
    bool currentTriangleState = Btn_Triangle(pad);
    if (currentTriangleState && !lastTriangleBtnState) {
      autoMode = !autoMode;
    }
    lastTriangleBtnState = currentTriangleState;

    // 3. Клаксон
    if (obstacleInFront) Car_SetHorn(true);
    else Car_SetHorn(Btn_Cross(pad));

    // 4. ================= ВІЗУАЛЬНІ ЕФЕКТИ =================
    // Визначаємо, який стан зараз у машинки
    int newVisualState = 0;
    if (obstacleInFront) newVisualState = 1;      // Стан 1: Перешкода
    else if (autoMode)   newVisualState = 2;      // Стан 2: Авто-патруль
    else if (stickX < -100) newVisualState = 3;   // Стан 3: Поворот ліворуч (в ручному)
    else if (stickX > 100) newVisualState = 4;    // Стан 4: Поворот праворуч (в ручному)
    else newVisualState = 0;                      // Стан 0: Стоїмо / Їдемо прямо

    // Оновлюємо діоди та екран ТІЛЬКИ якщо стан змінився (або якщо потрібне блимання в авто-режимі)
    if (newVisualState != currentVisualState || newVisualState == 2) {
      
      if (newVisualState == 1 && currentVisualState != 1) {
        Matrix_AnimCry();
        LED_SetAllColors(255, 0, 0); // Червона тривога
      } 
      else if (newVisualState == 2) {
        if (currentVisualState != 2) Matrix_AnimSmile();
        
        // Блимання діодів кожні 300мс
        if (millis() - lastBlinkTime > 300) {
          blinkState = !blinkState;
          lastBlinkTime = millis();
          if (blinkState) LED_SetAllColors(0, 0, 255); // Синій колір
          else LED_TurnOffAll();
        }
      }
      else if (newVisualState == 3 && currentVisualState != 3) {
        Matrix_ShowArrowLeft();
        LED_TurnOffAll();
        // Вмикаємо 3 лівих діоди
        LED_SetColor(0, 255, 100, 0); 
        LED_SetColor(1, 255, 100, 0);
        LED_SetColor(2, 255, 100, 0);
      }
      else if (newVisualState == 4 && currentVisualState != 4) {
        Matrix_ShowArrowRight();
        LED_TurnOffAll();
        // Вмикаємо 3 правих діоди (наприклад, 0, 1, 2)
        LED_SetColor(3, 255, 100, 0);
        LED_SetColor(4, 255, 100, 0);
        LED_SetColor(5, 255, 100, 0);
      }
      else if (newVisualState == 0 && currentVisualState != 0) {
        Matrix_Clear();
        LED_TurnOffAll();
      }

      currentVisualState = newVisualState; // Запам'ятовуємо новий стан
    }

    // 5. ================= ЛОГІКА РУХУ =================
    if (autoMode) {
      if (obstacleInFront) Car_SetMotors(0, 0); 
      else Car_SetMotors(SPEED_NORMAL, SPEED_NORMAL / 5);
    } else {
      stickY = constrain(stickY, -STICK_MAX, STICK_MAX);
      stickX = constrain(stickX, -STICK_MAX, STICK_MAX);
      int maxSpeed = Btn_R1(pad) ? SPEED_TURBO : SPEED_NORMAL;
      int drive = -stickY * maxSpeed / STICK_MAX;
      int turn  =  stickX * maxSpeed / STICK_MAX * TURN_FACTOR;

      if (obstacleInFront && drive > 0) drive = 0; 

      int left  = drive + turn;
      int right = drive - turn;
      int biggest = max(abs(left), abs(right));
      if (biggest > 4095) {
        left  = left  * 4095 / biggest;
        right = right * 4095 / biggest;
      }
      Car_SetMotors(left, right);
    }

    // 6. Керування серво
    if (millis() - lastServoMove > 30) {   
      if (Btn_DpadLeft(pad))  currentServoAngle = constrain(currentServoAngle + 2, 0, 180);
      if (Btn_DpadRight(pad)) currentServoAngle = constrain(currentServoAngle - 2, 0, 180);
      if (Btn_DpadUp(pad))    currentServoAngle = 90;
      
      Car_SetServo(currentServoAngle);
      lastServoMove = millis();
    }

#if DEBUG_STICKS
    if (millis() - lastDebugPrint > 250) {
      Serial.printf("Mode: %s | Dist: %3d cm | ", autoMode ? "AUTO" : "MANUAL", currentDistance);
      printControllerDebug(pad);
      lastDebugPrint = millis();
    }
#endif

  } else {
    Car_SetMotors(0, 0);
    // Додатково: якщо геймпад відключився, можна вимкнути фари
    if (currentVisualState != -1) {
      Matrix_Clear();
      LED_TurnOffAll();
      currentVisualState = -1;
    }
  }

  delay(1);
}