const int buzzerPin = 8;
const int button1 = 2;
const int button2 = 3;
const int button3 = 4;

const int noteC4 = 262;
const int noteD4 = 294;
const int noteE4 = 330;

void setup() {
  pinMode(buzzerPin, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
}

void loop() {
  bool btn1Pressed = digitalRead(button1) == LOW;
  bool btn2Pressed = digitalRead(button2) == LOW;
  bool btn3Pressed = digitalRead(button3) == LOW;

  if (btn1Pressed) {
    tone(buzzerPin, noteC4);
  } 
  else if (btn2Pressed) {
    tone(buzzerPin, noteD4);
  } 
  else if (btn3Pressed) {
    tone(buzzerPin, noteE4);
  } 
  else {
    noTone(buzzerPin);
  }
}