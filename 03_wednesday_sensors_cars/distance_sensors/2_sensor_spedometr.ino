// Піни для першого датчика (СТАРТ)
const int trigPin1 = 9;
const int echoPin1 = 10;

// Піни для другого датчика (ФІНІШ)
const int trigPin2 = 11;
const int echoPin2 = 12;

// Контрольний світлодіод фіксації
const int ledPin = 13;

// НАЛАШТУВАННЯ: Відстань між датчиками в сантиметрах
const float distanceBetweenSensors = 15.0; 

// ПОПРАВКА 1: Тепер поріг спрацювання 40 см (машинка фіксується, якщо вона ближче ніж 40 см)
const int triggerThreshold = 40; 

unsigned long startTime = 0;
unsigned long finishTime = 0;
bool carIsDriving = false; 

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  pinMode(ledPin, OUTPUT); // Налаштовуємо світлодіод контролю
  digitalWrite(ledPin, LOW); // Спочатку він вимкнений
  
  Serial.begin(9600);
  Serial.println("--- Спідометр (40см + LED контролю) готовий ---");
}

void loop() {
  // 1. Очікуємо старт
  if (!carIsDriving) {
    int d1 = getDistance(trigPin1, echoPin1);
    
    if (d1 > 0 && d1 < triggerThreshold) {
      startTime = millis(); 
      carIsDriving = true;  
      
      // ПОПРАВКА 2: Вмикаємо світлодіод — фіксація старту успішна!
      digitalWrite(ledPin, HIGH); 
      
      Serial.println("СТАРТ! Машина зафіксована першим датчиком.");
      delay(200); // Захист від повторного зчитування
    }
  } 
  // 2. Очікуємо фініш
  else {
    int d2 = getDistance(trigPin2, echoPin2);
    
    if (d2 > 0 && d2 < triggerThreshold) {
      finishTime = millis(); 
      
      // ПОПРАВКА 2: Вимикаємо світлодіод — фіксація фінішу успішна!
      digitalWrite(ledPin, LOW); 
      
      unsigned long timeTaken = finishTime - startTime; 
      
      float timeInSeconds = timeTaken / 1000.0;
      float speedCmPerSec = distanceBetweenSensors / timeInSeconds;
      float speedKmPerHour = speedCmPerSec * 0.036; 
      
      Serial.println("ФІНІШ! Машина пройшла дистанцію.");
      Serial.print("Час: ");
      Serial.print(timeInSeconds, 3);
      Serial.println(" сек");
      
      Serial.print("Швидкість: ");
      Serial.print(speedCmPerSec, 1);
      Serial.print(" см/с  |  ");
      Serial.print(speedKmPerHour, 2);
      Serial.println(" км/год");
      Serial.println("------------------------------------");
      
      carIsDriving = false; 
      delay(2000); // Пауза, щоб машина встигла проїхати ворота
    }
  }
  
  delay(30); 
}

// Функція зчитування відстані
int getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Збільшили таймаут до 30мс, бо відстань тепер до 40 см і звук летить трохи довше
  long duration = pulseIn(echo, HIGH, 30000); 
  int distance = duration * 0.034 / 2;
  return distance;
}
