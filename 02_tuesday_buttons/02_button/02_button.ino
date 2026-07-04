const int BUTTON_PIN = 13;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  bool result = digitalRead(BUTTON_PIN);
  Serial.println(result);
  delay(75);
}
