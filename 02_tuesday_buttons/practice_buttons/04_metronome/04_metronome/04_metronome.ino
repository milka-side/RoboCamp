const int BUTTON_PIN = 13;
const int LED_PIN = 12;
const int BUZZER_PIN = 11;

int buttonState = HIGH;
int metronomeState = LOW;
int currentDelay = 1000;
const int BUZZER_TONE = 100;

unsigned long currentMillis = 0;
unsigned long previousMetronomeMillis = 0;

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, metronomeState);
  digitalWrite(BUZZER_PIN, metronomeState);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != buttonState) {
    delay(50);
    buttonState = reading;

    if (reading == LOW) {
      currentDelay += 1000;

      if (currentDelay > 4000) {
        currentDelay = 1000;
      }

      Serial.print("Затримку змінено на: ");
      Serial.print(currentDelay);
      Serial.println(" мс");
    }
  }

  if (millis() - previousMetronomeMillis > currentDelay) {
    metronomeState = !metronomeState;
    
    digitalWrite(LED_PIN, metronomeState);
  
    if (metronomeState) {
      tone(BUZZER_PIN, BUZZER_TONE);
    } else {
      noTone(BUZZER_PIN);
    }
    
    previousMetronomeMillis = millis();
  }
}
