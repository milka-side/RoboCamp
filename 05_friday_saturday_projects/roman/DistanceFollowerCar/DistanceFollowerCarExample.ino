/**********************************************************************
  Filename    : DistanceFollowerCar.ino
  Product     : Freenove 4WD Car for ESP32 (FNK0053)
  Purpose     : машинка їде до об'єкта попереду, тримає безпечну
                відстань і показує настрій на LED-очах. Відстань
                міряє ультразвуковий датчик HC-SR04 (TRIG=GPIO12,
                ECHO=GPIO15) - без камери. Голова (сервопривід 1)
                повертається туди, куди повертає машинка.
**********************************************************************/
#include "Freenove_4WD_Car_For_ESP32.h"
#include "Freenove_4WD_Car_Emotion.h"

#define DRIVE_SPEED 700   // швидкість вперед/назад (0-4095)
#define TURN_SPEED  1200    // швидкість обертання під час пошуку

// Об'єкт вважається "видно", якщо ультразвук показує менше цієї відстані (см).
#define FOUND_DISTANCE_CM 25

// Гістерезис для безпечної відстані: відступаємо тільки коли ближче за
// ENTER, повертаємось вперед - тільки коли далі за EXIT. EXIT > ENTER,
// інакше машинка смикатиметься на межі (шум датчика).
#define TOO_CLOSE_ENTER_CM 5
#define TOO_CLOSE_EXIT_CM  15

#define DEBUG_PRINT true   // числа distance_cm у Serial Monitor

// Кут повороту голови (сервопривід 1, канал PCA9685). 90 - прямо вперед.
#define HEAD_CENTER_ANGLE 90
#define HEAD_LEFT_ANGLE   150
#define HEAD_RIGHT_ANGLE  30

// Випадковий пошук: кожен раз обирається випадковий напрямок повороту і
// випадкова тривалість (мс) в цьому діапазоні - щоб машинка не крутилась
// завжди в один бік з однаковим ритмом, а виглядала більш "живою".
#define SEARCH_MIN_MS 600
#define SEARCH_MAX_MS 2200

enum Mood {
  MOOD_OFF        = 0,
  MOOD_SEARCHING  = 1,
  MOOD_SCARED     = 2,
  MOOD_HAPPY      = 3,
  MOOD_SAD        = 4,
  MOOD_WHEEL_A    = 5,
  MOOD_WHEEL_B    = 6,
};

bool tooCloseState = false;        // стан гістерезису "занадто близько"

int searchDirection = 1;           // 1 = ліворуч, -1 = праворуч
unsigned long searchChangeAt = 0;  // millis(), коли обрати новий напрямок/тривалість

void setup() {
  Serial.begin(115200);
  PCA9685_Setup();       // мотори + сервоприводи
  Emotion_Setup();        // LED-матриця (очі)
  Ultrasonic_Setup();     // датчик відстані
  Look_Center();          // голова прямо вперед на старті
}

void loop() {
  float distance = Get_Sonar();   // см; 300 якщо нічого не знайдено

  if (DEBUG_PRINT) {
    Serial.print("distance_cm=");
    Serial.println(distance);
  }

  if (distance < FOUND_DISTANCE_CM) {
    // Гістерезис: заходимо в "занадто близько" тільки при перетині ENTER,
    // виходимо - тільки при перетині EXIT.
    if (!tooCloseState && distance < TOO_CLOSE_ENTER_CM) {
      tooCloseState = true;
    } else if (tooCloseState && distance > TOO_CLOSE_EXIT_CM) {
      tooCloseState = false;
    }

    if (tooCloseState) {
      Set_Mood(MOOD_SCARED);
      Drive_Backward(DRIVE_SPEED);
    } else {
      Set_Mood(MOOD_HAPPY);
      Drive_Forward(DRIVE_SPEED);
    }
  } else {
    tooCloseState = false;   // почати оцінку заново, коли об'єкт з'явиться
    Set_Mood(MOOD_SEARCHING);
    Search();
  }
}

// Пошук: випадково обирає напрямок повороту і випадкову тривалість, щоб
// не крутитись завжди в один бік з однаковим ритмом - виглядає живіше.
// Голова автоматично дивиться в той самий бік (через Turn_Left/Turn_Right).
void Search(void) {
  unsigned long now = millis();
  if (now >= searchChangeAt) {
    searchDirection = (random(0, 2) == 0) ? 1 : -1;
    searchChangeAt = now + random(SEARCH_MIN_MS, SEARCH_MAX_MS);
  }

  if (searchDirection > 0) {
    Turn_Left(TURN_SPEED);
  } else {
    Turn_Right(TURN_SPEED);
  }
}

// --------------------------------------------------------------------------
// Допоміжні функції руху. Кожна також повертає голову в бік руху -
// Look_Center() при русі прямо, Look_Left()/Look_Right() при поворотах.
// --------------------------------------------------------------------------
void Drive_Forward(int speed)  { Motor_Move(speed, speed, speed, speed); Look_Center(); }
void Drive_Backward(int speed) { Motor_Move(-speed, -speed, -speed, -speed); Look_Center(); }
void Turn_Left(int speed)      { Motor_Move(-speed, -speed, speed, speed); Look_Left(); }
void Turn_Right(int speed)     { Motor_Move(speed, speed, -speed, -speed); Look_Right(); }
void Stop(void)                { Motor_Move(0, 0, 0, 0); }

void Set_Mood(Mood mood) {
  Emotion_Show((int)mood);
}

// Бонус: одна з 22 готових статичних мордочок кита.
void Show_Random_Face(void) {
  staticEmtions(millis() % 22);
}

// --------------------------------------------------------------------------
// Голова (сервопривід 1, канал 0 PCA9685).
// --------------------------------------------------------------------------
void Look_Center(void) { Servo_1_Angle(HEAD_CENTER_ANGLE); }
void Look_Left(void)   { Servo_1_Angle(HEAD_LEFT_ANGLE); }
void Look_Right(void)  { Servo_1_Angle(HEAD_RIGHT_ANGLE); }