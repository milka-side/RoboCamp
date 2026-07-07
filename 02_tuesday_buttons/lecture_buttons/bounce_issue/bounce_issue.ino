const int ledPin = 13;
const int buttonPin = 12;

int buttonState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != buttonState) {
    buttonState = reading;

    Serial.println("Стан кнопки змінився");

    if (buttonState == LOW) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
}


