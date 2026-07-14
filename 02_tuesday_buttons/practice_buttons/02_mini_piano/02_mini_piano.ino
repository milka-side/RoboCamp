const int BUZZER_PIN = 8;
const int BUTTON1_PIN = 2;
const int BUTTON2_PIN = 3;
const int BUTTON3_PIN = 4;

const int NOTE_C4 = 262;
const int NOTE_D4 = 294;
const int NOTE_E4 = 330;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
}

void loop() {
  bool btn1Pressed = digitalRead(BUTTON1_PIN) == LOW;
  bool btn2Pressed = digitalRead(BUTTON2_PIN) == LOW;
  bool btn3Pressed = digitalRead(BUTTON3_PIN) == LOW;

  if (btn1Pressed) {
    tone(BUZZER_PIN, NOTE_C4);
  } 
  else if (btn2Pressed) {
    tone(BUZZER_PIN, NOTE_D4);
  } 
  else if (btn3Pressed) {
    tone(BUZZER_PIN, NOTE_E4);
  } 
  else {
    noTone(BUZZER_PIN);
  }
}

