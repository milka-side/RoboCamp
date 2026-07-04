const int BUTTON_PIN = 13;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Магія випадкових чисел
  randomSeed(analogRead(0)); 
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  // Якщо кнопку натиснули
  if (buttonState == LOW) {
    
    // Генеруємо число від 1 до 6
    int diceRoll = random(1, 7); 
    
    Serial.print("Dice rolled! Result: ");
    Serial.println(diceRoll);
    
    // Затримка, щоб за одне натискання
    // не випало 100 чисел одразу
    delay(1000); 
  }
}
