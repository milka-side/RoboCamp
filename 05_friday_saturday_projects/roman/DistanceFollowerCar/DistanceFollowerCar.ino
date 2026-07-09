/**********************************************************************
  Filename    : DistanceFollowerCarStudent.ino
  Purpose     : машинка їде до об'єкта попереду і тримає безпечну
                відстань, за даними ультразвукового датчика.
**********************************************************************/
#include "Freenove_4WD_Car_For_ESP32.h"
#include "Freenove_4WD_Car_Emotion.h"

#define DRIVE_SPEED         700    // швидкість вперед/назад (0-4095)
#define TURN_SPEED          1200   // швидкість пошуку (0-4095)
#define FOUND_DISTANCE_CM   25     // об'єкт "видно", якщо ближче за це (см)
#define TOO_CLOSE_ENTER_CM  5      // відступати, якщо ближче за це
#define TOO_CLOSE_EXIT_CM   15     // вперед - тільки якщо далі за це
#define DEBUG_PRINT         true

#define HEAD_CENTER_ANGLE 90       // кут голови: прямо вперед
#define HEAD_LEFT_ANGLE   150
#define HEAD_RIGHT_ANGLE  30

enum Mood {
  MOOD_OFF        = 0,
  MOOD_SEARCHING  = 1,
  MOOD_SCARED     = 2,
  MOOD_HAPPY      = 3,
  MOOD_SAD        = 4,
  MOOD_WHEEL_A    = 5,
  MOOD_WHEEL_B    = 6,
};

bool tooCloseState = false;        // знадобиться в TODO 1

// Ці дві змінні - підказка для Search() нижче, необов'язково їх
// використовувати саме так, якщо придумаєш свій спосіб.
int searchDirection = 1;
unsigned long searchChangeAt = 0;

void setup() {
  Serial.begin(115200);
  PCA9685_Setup();
  Emotion_Setup();
  Ultrasonic_Setup();
  Look_Center();
}

void loop() {
  float distance = Get_Sonar();   // см; 300 якщо нічого не знайдено

  if (DEBUG_PRINT) {
    Serial.print("distance_cm=");
    Serial.println(distance);
  }

  // Доступно:
  //   distance                            -> см до найближчого об'єкта
  //   Drive_Forward(speed) / Drive_Backward(speed)
  //   Turn_Left(speed) / Turn_Right(speed) / Stop()   (голова сама повертається слідом)
  //   Search()                            -> твоя функція пошуку, дивись нижче
  //   Set_Mood(MOOD_HAPPY / MOOD_SAD / MOOD_SEARCHING / MOOD_SCARED / MOOD_WHEEL_A / MOOD_WHEEL_B)
  //
  // TODO 1: якщо distance < FOUND_DISTANCE_CM - об'єкт видно.
  //   Гістерезис (щоб не смикалось на межі):
  //     tooCloseState==false і distance < TOO_CLOSE_ENTER_CM -> tooCloseState = true
  //     tooCloseState==true  і distance > TOO_CLOSE_EXIT_CM  -> tooCloseState = false
  //   Далі: tooCloseState ? Set_Mood(MOOD_SCARED)+Drive_Backward
  //                       : Set_Mood(MOOD_HAPPY)+Drive_Forward
  //
  // TODO 2: інакше - tooCloseState = false, Set_Mood(MOOD_SEARCHING), Search();

}

// TODO 3 (головне): зроби пошук більш "живим", а не завжди в один бік.
// Ідеї:
//   - обирай випадковий напрямок: random(0, 2) == 0 ? ліворуч : праворуч
//   - обирай випадкову тривалість повороту, наприклад random(600, 2200) мс
//   - millis() підкаже, коли час обрати новий напрямок (порівняй з
//     searchChangeAt - змінна вже оголошена вище, можеш нею скористатись)
// Поки що ця функція просто крутиться ліворуч - працює, але не дуже
// "жива". Заміни/додай свою логіку.
void Search(void) {
  Turn_Left(TURN_SPEED);
}

void Drive_Forward(int speed)  { Motor_Move(speed, speed, speed, speed); Look_Center(); }
void Drive_Backward(int speed) { Motor_Move(-speed, -speed, -speed, -speed); Look_Center(); }
void Turn_Left(int speed)      { Motor_Move(-speed, -speed, speed, speed); Look_Left(); }
void Turn_Right(int speed)     { Motor_Move(speed, speed, -speed, -speed); Look_Right(); }
void Stop(void)                { Motor_Move(0, 0, 0, 0); }

void Set_Mood(Mood mood) {
  Emotion_Show((int)mood);
}

void Show_Random_Face(void) {
  staticEmtions(millis() % 22);
}

void Look_Center(void) { Servo_1_Angle(HEAD_CENTER_ANGLE); }
void Look_Left(void)   { Servo_1_Angle(HEAD_LEFT_ANGLE); }
void Look_Right(void)  { Servo_1_Angle(HEAD_RIGHT_ANGLE); }