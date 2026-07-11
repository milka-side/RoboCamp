const int DATA_PIN = 12;
const int LED_PIN = 13;

void setup() {
  pinMode(DATA_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Зчитати значення з піна
  int value = digitalRead(DATA_PIN);

  // Засвітити світлодіод, якщо зчитане значення HIGH, інакше - вимкнути
  if (value == HIGH) {
    digitalWrite(LED_PIN, HIGH); // Увімкнути
  }
  
  if (value == LOW) {
    digitalWrite(LED_PIN, LOW);  // Вимкнути
  }
}


