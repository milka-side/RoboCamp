const int LED_PIN = 13;
const int CORRECT_PASSWORD = 1234;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Введіть пароль: ");
}

void loop() {
  if (Serial.available() > 0) {
    int password = Serial.parseInt();
    
    if (password == CORRECT_PASSWORD) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Пароль правильний!");
    } else {
      Serial.println("Помилка, пароль неправильний!");
    }

    // Очищення символів переходу на новий рядок, що залишились
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}



