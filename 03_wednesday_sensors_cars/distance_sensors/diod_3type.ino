const int trigPin= 9;
const int echoPin =10;
const int ledPin1 =11;
const int ledPin2 =12;
const int ledPin3 =13;
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin1,OUTPUT);
  pinMode(ledPin2,OUTPUT);
  pinMode(ledPin3,OUTPUT);
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
    digitalWrite(ledPin1,HIGH);
    digitalWrite(ledPin2,LOW);
    digitalWrite(ledPin3,LOW);
  }
  else if (distance>10 &&  distance<20){
    digitalWrite(ledPin2,HIGH);
    digitalWrite(ledPin1,LOW);
    digitalWrite(ledPin3,LOW);
  }
  else if (distance>20 && distance<30){
    digitalWrite(ledPin3,HIGH);
    digitalWrite(ledPin1,LOW);
    digitalWrite(ledPin2,LOW);

  }
  else{
    digitalWrite(ledPin1,LOW);
    digitalWrite(ledPin2,LOW);
    digitalWrite(ledPin3,LOW);

  }
  delay(100);

}
