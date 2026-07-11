const int RED_LED_PIN = 11;
const int YELLOW_LED_PIN = 12;
const int GREEN_LED_PIN = 13;

unsigned long redLightTime;
unsigned long yellowLightTime;
unsigned long greenLightTime;

void setup() {
  Serial.begin(9600);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  Serial.print("Введіть скільки секунд має горіти червоний, жовтий та зелений через пробіли: ");
  while (true) {
    if (Serial.available() > 0) {
      redLightTime = Serial.parseInt() * 1000;
      yellowLightTime = Serial.parseInt() * 1000;
      greenLightTime = Serial.parseInt() * 1000;

      break;
    }
  }
}

void loop() {
  // 1. Червоний
  digitalWrite(RED_LED_PIN, HIGH);
  delay(redLightTime);

  // 2. Червоний + Жовтий
  digitalWrite(YELLOW_LED_PIN, HIGH);
  delay(yellowLightTime);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);

  // 3. Зелений (світить 2/3 від введеного часу)
  digitalWrite(GREEN_LED_PIN, HIGH);
  delay(greenLightTime * 2 / 3); 
  digitalWrite(GREEN_LED_PIN, LOW);

  // 4. Зелений блимає (займає решту 1/3 часу)
  for (int i = 0; i < 3; i++) {
    delay(greenLightTime / 18);
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(greenLightTime / 18);
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  // 5. Жовтий
  digitalWrite(YELLOW_LED_PIN, HIGH);
  delay(yellowLightTime);
  digitalWrite(YELLOW_LED_PIN, LOW);
}