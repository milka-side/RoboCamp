#include "pgmspace.h"
#include "Car_Core.h"
#include <Preferences.h>
#include <Wire.h>
#include "Freenove_4WD_Car_For_ESP32.h"

#define MOTOR_UPDATE_MS   20  // мінімальний інтервал між командами моторам
#define PIN_BOOT_BTN       0  // кнопка BOOT — скидання прив'язки
#define RESET_HOLD_MS   100  // скільки тримати BOOT для скидання

// Налаштування бузера
#define BUZZER_PIN         2
#define BUZZER_FREQ     2000

Preferences prefs;
uint8_t pairedMac[6];
bool    isBound = false;
ControllerPtr myGamepad = nullptr;

static unsigned long bootPressedSince = 0;
static int currentHWAngle = 90;

// --- Системні функції ---

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
void Buzzer_Setup() { ledcAttach(BUZZER_PIN, BUZZER_FREQ, 10); ledcWriteTone(BUZZER_PIN, 0); }
void HornRaw(bool on){ ledcWriteTone(BUZZER_PIN, on ? BUZZER_FREQ : 0); }
#else
#define BUZZER_CHN 0
void Buzzer_Setup() { ledcSetup(BUZZER_CHN, BUZZER_FREQ, 10); ledcAttachPin(BUZZER_PIN, BUZZER_CHN); ledcWriteTone(BUZZER_CHN, 0); }
void HornRaw(bool on){ ledcWriteTone(BUZZER_CHN, on ? BUZZER_FREQ : 0); }
#endif

void Beep(int ms) { HornRaw(true); delay(ms); HornRaw(false); delay(80); }

void printMac(const uint8_t* m) {
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", m[0],m[1],m[2],m[3],m[4],m[5]);
}

void resetBinding() {
  Serial.println(">>> Скидаю прив'язку геймпада... <<<");
  Motor_Move(0, 0, 0, 0);
  prefs.remove("padmac");
  BP32.forgetBluetoothKeys();
  Beep(600);
  delay(200);
  ESP.restart();
}

void checkResetRequest() {
  if (digitalRead(PIN_BOOT_BTN) == LOW) {
    if (bootPressedSince == 0) bootPressedSince = millis();
    else if (millis() - bootPressedSince >= RESET_HOLD_MS) resetBinding();
  } else {
    bootPressedSince = 0;
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') resetBinding();
  }
}

// --- Колбеки Bluetooth ---

void onConnectedController(ControllerPtr ctl) {
  ControllerProperties p = ctl->getProperties();

  if (!isBound) {
    memcpy(pairedMac, p.btaddr, 6);
    prefs.putBytes("padmac", pairedMac, 6);
    isBound = true;
    myGamepad = ctl;
    Serial.print("ПРИВ'ЯЗАНО геймпад: "); printMac(pairedMac); Serial.println();
    Beep(100); Beep(100);
    return;
  }

  if (memcmp(p.btaddr, pairedMac, 6) == 0 && myGamepad == nullptr) {
    myGamepad = ctl;
    Serial.print("Геймпад підключено: "); printMac(p.btaddr); Serial.println();
    Beep(100);
  } else {
    Serial.print("ЧУЖИЙ геймпад відхилено: "); printMac(p.btaddr); Serial.println();
    ctl->disconnect();
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  if (myGamepad == ctl) {
    myGamepad = nullptr;
    Motor_Move(0, 0, 0, 0);
    Serial.println("Геймпад від'єднано — машинка зупинена.");
  }
}

// --- Публічне API для скетчу ---

void Car_System_Setup() {
  Serial.begin(115200);
  pinMode(PIN_BOOT_BTN, INPUT_PULLUP);
  delay(300);

  PCA9685_Setup();
  Wire.setClock(400000);   // I2C 400 кГц
  Buzzer_Setup();
  Motor_Move(0, 0, 0, 0);
  Servo_1_Angle(currentHWAngle);

  prefs.begin("car", false);
  isBound = (prefs.getBytes("padmac", pairedMac, 6) == 6);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);
  BP32.enableNewBluetoothConnections(true);

  if (isBound) {
    Serial.print("Машинка прив'язана до геймпада: ");
    printMac(pairedMac); Serial.println();
  } else {
    BP32.forgetBluetoothKeys();
    Serial.println("Машинка ВІЛЬНА: перший геймпад стане її (натисніть PS).");
  }
}

void Car_System_Update() {
  BP32.update();
  checkResetRequest();
}

bool Car_IsGamepadConnected() {
  return (myGamepad && myGamepad->isConnected());
}

ControllerPtr Car_GetGamepad() {
  return myGamepad;
}

void Car_SetMotors(int left, int right) {
  static int lastLeft = 9999, lastRight = 9999;
  static unsigned long lastSend = 0;

  // Оновлюємо мотори лише якщо значення змінилось і пройшов таймаут
  if (left == lastLeft && right == lastRight) return;
  if (millis() - lastSend < MOTOR_UPDATE_MS) return;

  Motor_Move(left, left, right, right);
  lastLeft = left; lastRight = right;
  lastSend = millis();
}

void Car_SetHorn(bool on) {
  static bool hornState = false;
  if (on != hornState) { 
    HornRaw(on); 
    hornState = on; 
  }
}

void Car_SetServo(int angle) {
  if (angle != currentHWAngle) {
    currentHWAngle = angle;
    Servo_1_Angle(currentHWAngle);
  }
}

void printRawControllerDebug(ControllerPtr ctl) {
  Serial.println("---- Gamepad button debug ----");

  // Face buttons (on most gamepads: a=X/Cross, b=Circle, x=Square, y=Triangle)
  Serial.printf("a=%d b=%d x=%d y=%d\n", ctl->a(), ctl->b(), ctl->x(), ctl->y());

  // Shoulder / triggers
  Serial.printf("l1=%d r1=%d l2=%d r2=%d\n", ctl->l1(), ctl->r1(), ctl->l2(), ctl->r2());

  // Stick clicks
  Serial.printf("thumbL=%d thumbR=%d\n", ctl->thumbL(), ctl->thumbR());

  // System / misc buttons
  Serial.printf("miscButtons: system=%d start=%d select=%d capture=%d\n",
                ctl->miscSystem(), ctl->miscStart(), ctl->miscSelect(), ctl->miscCapture());

  // D-pad (bitmask)
  uint8_t d = ctl->dpad();
  Serial.printf("dpad=0x%02X (up=%d down=%d left=%d right=%d)\n",
                d, (d & DPAD_UP) != 0, (d & DPAD_DOWN) != 0,
                (d & DPAD_LEFT) != 0, (d & DPAD_RIGHT) != 0);

  // Raw button bitmask (all buttons at once, useful to spot mis-mapped bits)
  Serial.printf("buttons()=0x%04X\n", ctl->buttons());

  // Sticks and analog triggers
  Serial.printf("axisX=%4d axisY=%4d axisRX=%4d axisRY=%4d brake=%4d throttle=%4d\n",
                ctl->axisX(), ctl->axisY(), ctl->axisRX(), ctl->axisRY(),
                ctl->brake(), ctl->throttle());

  Serial.println("-------------------------------");
}

void printControllerDebug(ControllerPtr ctl) {
  Serial.printf("axisX=%4d axisY=%4d axisRX=%4d axisRY=%4d\n",
              ctl->axisX(), ctl->axisY(), ctl->axisRX(), ctl->axisRY());
}

// ==================== ОБГОРТКИ ДЛЯ КНОПОК (за типом геймпада) ====================
#if CONTROLLER_TYPE == CONTROLLER_TYPE_BLUE

  bool Btn_Cross(ControllerPtr ctl)      { return ctl->a(); }
  bool Btn_Circle(ControllerPtr ctl)     { return ctl->b(); }
  bool Btn_Square(ControllerPtr ctl)     { return ctl->x(); }
  bool Btn_Triangle(ControllerPtr ctl)   { return ctl->y(); }

  bool Btn_L1(ControllerPtr ctl)         { return ctl->l1(); }
  bool Btn_R1(ControllerPtr ctl)         { return ctl->r1(); }
  bool Btn_L2(ControllerPtr ctl)         { return ctl->l2(); }
  bool Btn_R2(ControllerPtr ctl)         { return ctl->r2(); }

  bool Btn_L3(ControllerPtr ctl)         { return ctl->thumbL(); }
  bool Btn_R3(ControllerPtr ctl)         { return ctl->thumbR(); }

  bool Btn_Start(ControllerPtr ctl)      { return ctl->miscStart(); }
  bool Btn_Select(ControllerPtr ctl)     { return ctl->miscSelect(); }
  bool Btn_System(ControllerPtr ctl)     { return ctl->miscSystem(); }

  bool Btn_DpadUp(ControllerPtr ctl)     { return (ctl->dpad() & DPAD_UP)    != 0; }
  bool Btn_DpadDown(ControllerPtr ctl)   { return (ctl->dpad() & DPAD_DOWN)  != 0; }
  bool Btn_DpadLeft(ControllerPtr ctl)   { return (ctl->dpad() & DPAD_LEFT)  != 0; }
  bool Btn_DpadRight(ControllerPtr ctl)  { return (ctl->dpad() & DPAD_RIGHT) != 0; }

#elif CONTROLLER_TYPE == CONTROLLER_TYPE_GREY

  bool Btn_Cross(ControllerPtr ctl)      { return ctl->thumbR(); }
  bool Btn_Circle(ControllerPtr ctl)     { return ctl->thumbL(); }
  bool Btn_Square(ControllerPtr ctl)     { return false; } // Button isn't working
  bool Btn_Triangle(ControllerPtr ctl)   { return ctl->miscSystem(); }

  bool Btn_L1(ControllerPtr ctl)         { return ctl->miscSelect(); }
  bool Btn_R1(ControllerPtr ctl)         { return ctl->miscStart(); }
  bool Btn_L2(ControllerPtr ctl)         { return false; } // Button isn't working
  bool Btn_R2(ControllerPtr ctl)         { return false; } // Button isn't working

  bool Btn_L3(ControllerPtr ctl)         { return ctl->b(); }
  bool Btn_R3(ControllerPtr ctl)         { return false; } // Button isn't working

  bool Btn_Start(ControllerPtr ctl)      { return ctl->x(); }
  bool Btn_Select(ControllerPtr ctl)     { return ctl->a(); }
  bool Btn_System(ControllerPtr ctl)     { return false; } // Button isn't working

  bool Btn_DpadUp(ControllerPtr ctl)     { return ctl->y(); }
  bool Btn_DpadDown(ControllerPtr ctl)   { return ctl->l1(); }
  bool Btn_DpadLeft(ControllerPtr ctl)   { return ctl->r1(); }
  bool Btn_DpadRight(ControllerPtr ctl)  { return false; } // Button isn't working

#endif
// ===================================================================================