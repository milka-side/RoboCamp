const int BUTTON_PIN = 13;
const int LED_PIN = 12;
const int BUZZER_PIN = 11;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  // Якщо кнопка натиснута
  if (buttonState == LOW) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 500);
    
  } else {
    // Якщо кнопку відпустили
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
  }
}
