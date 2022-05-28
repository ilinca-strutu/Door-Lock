#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
#define servoPin 9 // attach pin D9 to pin Signal of Servo Motor
#define RECV_PIN 7 // attach pin D7 to pin Out of IR sensor

#include<Servo.h>
#include <IRremote.hpp>

// defines variables  

// IR sensor
IRrecv irrecv(RECV_PIN);     
decode_results results;
int readResults;

// Servo
Servo Myservo;

// HC-SR04
long duration; // variable for the duration of sound wave travel
double distance; // variable for the distance measurement

// distance
int distCount = 0;
double distAvg = 0.0;
double distToWall = 3.0;

// serv poz
int openPoz = 130;
int closePoz = 50;

// reads the input form the IR sensor
void readIR(){
  if (irrecv.decode(&results)) 
  {   
    if(results.value)
    readResults = results.value;
    irrecv.resume();
  }
}

// checks if the pressed button is OK
bool validOpen(){
  readIR();    
  if(readResults == 14535)
    return true;
  return false;
}

// moves lock to open(1) or close(-1) poz
// it waits a bit after open to have time open the door
// it waits a bit before closing the lock to have time to close the door all the way 
void moveLock(int dir){
  if(dir == 1){
    Myservo.write(openPoz);
    delay(2000);
} else{
    delay(1000);
    Myservo.write(closePoz);
  }
}

// does the distance average between 
// the previous distance and the new distance 
void doAvg(){
  if(distCount != 0){
    distAvg += distance;
    distAvg = distAvg/2;
  } else {
    distAvg = distance;
    distCount++;
  }
}

// get the distance from the ultrasonic sensor
void getDistance(){
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
 
}
     
void setup()     
{     
  Myservo.attach(servoPin);
  Myservo.write(closePoz); // we start with the door loked
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600);     
  irrecv.enableIRIn();     
}  

// we get the new distance
// we get the new average
// if the average is smaller than a set distance to the wall
// we close the door
// else we do nothing
// we check if the ir senor reads the OK button
// if we get OK we open the door
// else we do nothing
void loop()   {
  getDistance();
  doAvg();
  if(distAvg < distToWall){
    moveLock(-1);
  }
  if(validOpen())
    moveLock(1);
}
