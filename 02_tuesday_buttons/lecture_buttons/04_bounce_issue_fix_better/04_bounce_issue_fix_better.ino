const int BUTTON_PIN = 13;
const int LED_PIN = 12;

int lastButtonState = HIGH;
int buttonState = HIGH;
int ledState = LOW;
int counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    delay(50);

    reading = digitalRead(BUTTON_PIN);

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

  lastButtonState = reading;
}


