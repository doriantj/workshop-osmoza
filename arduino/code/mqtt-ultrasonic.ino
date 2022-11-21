// libraries
#include <WiFiNINA.h>
#include <MQTT.h>
#include "Arduino_LSM6DS3.h"

char ssid[] = "OSMO/ZA";
char pass[] = "osmo8floor";

// set up pin numbers for echo pin and trigger pins:
const int trigPin = 9;
const int echoPin = 10;
const int LEDPin = 13;

int maximumRange = 30;
int minimumRange = 0;
long duration, distance;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect()
{
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    Serial.print(WiFi.status());
    delay(1000);
  }

  //change name of the device "arduino"
  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "oasisrazor294", "604NiryUDL1xQd9j"))
  {
    Serial.print("/");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("hello");
}

void messageReceived(String &topic, String &payload)
{
  Serial.println(topic + ": " + payload);
}

void setup()
{
  // set the modes for the trigger pin and echo pin:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // initialize serial communication:
  Serial.begin(9600);

  // start the IMU:
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU");
    // stop here if you can't access the IMU:
    while (true)
      ;
  }

  // start wifi and mqtt
  WiFi.begin(ssid, pass);
  client.begin("oasisrazor294.cloud.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop()
{

  float xGyro, yGyro, zGyro, x, y, z;

  if (IMU.accelerationAvailable() &&
      IMU.gyroscopeAvailable())
  {
    // read accelerometer and gyrometer:

    // print the results:
    IMU.readGyroscope(xGyro, yGyro, zGyro);
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
  }

  // take the trigger pin low to start a pulse:
  digitalWrite(trigPin, LOW);
  // delay 2 microseconds:
  delayMicroseconds(2);
  // take the trigger pin high:
  digitalWrite(trigPin, HIGH);
  // delay 10 microseconds:
  delayMicroseconds(10);
  // take the trigger pin low again to complete the pulse:
  digitalWrite(trigPin, LOW);

  // listen for a pulse on the echo pin:
  duration = pulseIn(echoPin, HIGH);
  // calculate the distance in cm.
  // Sound travels approx.0.0343 microseconds per cm.,
  // and it's going to the target and back (hence the /2):
  distance = duration / 58.2;

  if (distance >= maximumRange || distance <= minimumRange)
  {
    Serial.println("-1");
    digitalWrite(LEDPin, HIGH);
  }
  else
  {
    Serial.println(distance);
    digitalWrite(LEDPin, LOW);
  }

  Serial.println(zGyro);

  delay(50);

  client.loop();
  delay(200);

  // check if connected
  if (!client.connected())
  {
    connect();
  }

  String message = String(distance);
  String messageGyro = String(zGyro);
  client.publish("/distance", message);
  client.publish("/zgyro", messageGyro);
}
