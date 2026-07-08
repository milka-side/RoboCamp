const int ledPin = 13;
const int buttonPin = 12;

int lastButtonState = HIGH;
int buttonState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    delay(50);

    int reading = digitalRead(buttonPin);

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        Serial.println("Кнопка натиснута");
        digitalWrite(ledPin, HIGH);
      } else {
        Serial.println("Кнопка відпущена");
        digitalWrite(ledPin, LOW);
      }
    }
  }

  lastButtonState = reading;
}


