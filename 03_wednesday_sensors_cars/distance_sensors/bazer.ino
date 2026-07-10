const int bazer = 8;
const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 13;

int distance = 0;
long duration = 0;


void setup() {
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(bazer,OUTPUT);
  Serial.begin(9600);


}

void loop() {
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;

  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance>0 && distance<15){
    digitalWrite(ledPin,HIGH);
    tone(bazer,1000,1000);
    delay(100);

  }
  else if(distance>15 && distance<40){
    digitalWrite(ledPin,HIGH);
    tone(bazer,1500,1500);
    delay(100);
  }
  else{
    digitalWrite(ledPin,LOW);
    noTone(bazer);
    delay(100);
  }

}
