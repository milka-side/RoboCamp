const int trigPin= 9;
const int echoPin =10;
const int ledPin =13;
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);

}

void loop() {
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  duration = pulseIn(echoPin,HIGH);
  distance = duration *0.034/2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance >0 && distance <10){
    digitalWrite(ledPin,HIGH);
  }
  else{
    digitalWrite(ledPin,LOW);

  }
  delay(100);

}
//  code with filter 
// const int trigPin = 9;
// const int echoPin = 10;
// const int ledPin = 13;

// void setup() {
//   pinMode(trigPin, OUTPUT);
//   pinMode(echoPin, INPUT);
//   pinMode(ledPin, OUTPUT);
//   Serial.begin(9600);
// }

// void loop() {

//   int cleanDistance = getFilteredDistance();
  
//   Serial.print("Filtered Distance: ");
//   Serial.print(cleanDistance);
//   Serial.println(" cm");
  
//   // Логіка для світлодіода
//   if (cleanDistance > 0 && cleanDistance < 10) { 
//     digitalWrite(ledPin, HIGH); 
//   } else {
//     digitalWrite(ledPin, LOW);  
//   }
  
//   delay(200); // Робимо паузу між основними циклами
// }


// int getFilteredDistance() {
//   long sum = 0;
//   int validReadingsCount = 0;
  
//   for (int i = 0; i < 10; i++) {

//     digitalWrite(trigPin, LOW);
//     delayMicroseconds(2);
//     digitalWrite(trigPin, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(trigPin, LOW);
    
 
//     long duration = pulseIn(echoPin, HIGH, 30000); 
//     int rawDistance = duration * 0.034 / 2;

//     if (rawDistance > 0 && rawDistance <= 400) {
//       sum += rawDistance;       // Додаємо тільки правильні значення
//       validReadingsCount++;     // Рахуємо кількість успішних замірів
//     }
    
//     delay(10); 
//   }
  

//   if (validReadingsCount > 0) {
//     return sum / validReadingsCount;
//   } else {
//     return 0; 
//   }
// }
