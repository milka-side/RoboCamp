const int FLOATING_PIN = 13;

void setup() {
  Serial.begin(9600);
  pinMode(FLOATING_PIN, INPUT);
}

void loop() {
  bool result = digitalRead(FLOATING_PIN);
  Serial.println(result);
  delay(75);
}
