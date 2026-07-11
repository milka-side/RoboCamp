const int redLedPin = 13;
const int yellowLedPin = 12;
const int greenLedPin = 11;

float redLightTime;
float yellowLightTime;
float greenLightTime;

void setup() {
  Serial.begin(9600);

  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  digitalWrite(redLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
  digitalWrite(greenLedPin, LOW);

  while (true) {
    if (Serial.available() > 0) {
      Serial.print("Введіть скільки секунд має горіти червоний, жовтий та зелений через пробіли: ");
      redLightTime = Serial.parseFloat();
      yellowLightTime = Serial.parseFloat();
      greenLightTime = Serial.parseFloat();

      break;
    }
  }
}

void loop() {
  digitalWrite(redLedPin, HIGH);
  delay(redLightTime);
  digitalWrite(redLedPin, LOW);

  digitalWrite(yellowLedPin, HIGH);
  delay(yellowLightTime);
  digitalWrite(yellowLedPin, LOW);

  digitalWrite(greenLedPin, HIGH);
  delay(greenLightTime / 3);
  digitalWrite(greenLedPin, LOW);

  for (int i = 0; i < 3; i++) {
    digitalWrite(greenLedPin, HIGH);
    delay(greenLightTime / 9);
    digitalWrite(greenLedPin, LOW);
  }
}
