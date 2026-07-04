const int BUTTON_PIN = 13;
const int LED_PIN = 12;

int mode = 0;
int lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int currentButtonState = digitalRead(BUTTON_PIN);

  // ВІДЛОВЛЮЄМО КЛІК: перевіряємо
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    
    mode = mode + 1;
    
    // Якщо режими закінчилися (їх у нас 3: 0, 1, 2),
    // скидаємо на нуль
    if (mode > 2) {
      mode = 0; 
    }
    
    Serial.print("Flashlight mode: ");
    Serial.println(mode);
    
    // Маленька пауза (антибрязкіт)
    delay(200); 
  }
  
  // Запам'ятовуємо поточний стан кнопки для наступного кола
  lastButtonState = currentButtonState;

  // 2. ВМИКАЄМО РЕЖИМИ
  if (mode == 0) {
    // Режим 0: Вимкнено
    digitalWrite(LED_PIN, LOW);
  } 
  else if (mode == 1) {
    // Режим 1: Світить постійно
    digitalWrite(LED_PIN, HIGH);
  } 
  else if (mode == 2) {
    // Режим 2: Швидко блимає
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}
