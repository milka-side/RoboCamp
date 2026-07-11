void setup() {
  Serial.begin(9600);

    for (int i = 0; i < 10; i++) {
    if (i == 8) {
      break;
    }

    if (i == 6) {
      continue;
    }

    Serial.println(i);
  }
}

void loop() {}