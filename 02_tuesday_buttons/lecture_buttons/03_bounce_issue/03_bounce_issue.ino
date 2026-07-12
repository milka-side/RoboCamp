const int BUTTON_PIN = 13;
const int LED_PIN = 12;

int buttonState = HIGH;
int ledState = LOW;
int counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != buttonState) {
    buttonState = reading;

    if (buttonState == LOW) {
      counter++;
      Serial.print("Кнопка натиснута, лічильник: ");
      Serial.println(counter);

      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }
}


