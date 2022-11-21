#define echoPin 11 // The number of the digital pin connected to the echo input
#define trigPin 12 // The number of the digital pin connected to the trigger output
#define LEDPin 13 // This enables the LED feedback for testing

int maximumRange = 30; // The max distance observed from the sensor 
int minimumRange = 0; 
long duration, distance; //duration used to calculate distance

void setup() {
  Serial.begin(9600); //starts the serial communication via USB
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT); //board LED for testing

}

void loop() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // pulse off

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // pulse for 10 microseconds

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // echo pin listens / receives

  // from that data calculate distance in CentiMeters based on Speed of Sound
  distance = duration / 58.2;

// error handling 
  if (distance >= maximumRange || distance <= minimumRange) {
    Serial.println("-1"); //if nothing observed ouput -1
    digitalWrite(LEDPin, HIGH); // LED will be on
  } else {
    Serial.println(distance); // prints the distance in CM
    digitalWrite(LEDPin, LOW); // LED will be off if object detected
  }

  delay(50); // good practice not to overload the serial port
 
}
