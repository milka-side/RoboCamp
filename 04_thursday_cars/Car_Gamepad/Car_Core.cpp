#include "Car_Core.h"
#include <Preferences.h>
#include <Wire.h>
#include "Freenove_4WD_Car_For_ESP32.h"

#define MOTOR_UPDATE_MS   20  // мінімальний інтервал між командами моторам
#define PIN_BOOT_BTN       0  // кнопка BOOT — скидання прив'язки
#define RESET_HOLD_MS   2000  // скільки тримати BOOT для скидання

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
