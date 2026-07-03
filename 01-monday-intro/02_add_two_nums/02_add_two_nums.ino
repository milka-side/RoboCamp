void setup() {
  Serial.begin(9600);
  
  Serial.println("Введи два числа через пробіл і натисни Enter");
}

void loop() {
  // Перевіряємо, чи є для нас якесь повідомлення у Serial
  if (Serial.available() > 0) {

    int a = Serial.parseInt();
    int b = Serial.parseInt(); 

    int sum = a + b;

    Serial.println(sum);

    Serial.println("Введи наступні два числа:");
    Serial.readString();
  }
}
