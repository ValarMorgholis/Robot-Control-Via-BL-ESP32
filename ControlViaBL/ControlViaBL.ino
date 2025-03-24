#include <Wire.h>
#include "BluetoothSerial.h"
#include "ESP32Servo.h"

#define echoPin 2
#define trigPin 4

// Declaration of R motor controller pins
#define eR 26
#define in2 13
#define in1 12
// Declaration of L motor controller pins
#define eL 25
#define in4 14
#define in3 27

BluetoothSerial SerialBT;
Servo srv;

const int servoPin = 33;

int speed;

String message = "";
String incomingChar;
String ch = "";

void setup() {

  speed = 255;
  Serial.begin(115200);
  SerialBT.begin("esp32Bot");  //Bluetooth device name

  Serial.println("The device started, now you can pair it with bluetooth!");

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(eL, OUTPUT);
  pinMode(eR, OUTPUT);
  analogWrite(eR, speed);
  analogWrite(eL, speed);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  srv.attach(servoPin);
}

void loop() {
  readFromBL();

  /*  Turn on the servo motor to look around and print the distance. 
      "d" stand for distance and "e" means end of the mission.         */

  if (readFromBL() == "d") {
    Serial.println("Distance");
    while (readFromBL() != "e") {
      masure();
      readFromBL();
    }
  }

  /* Let you set the speed. (Turn it of when you set the speed)
      "s" stand for speed and "e" means end of the mission.             */

  if (readFromBL() == "s") {
    while (readFromBL() != "e") {
      readFromBL();
      speed = ch.toInt();
      analogWrite(eR, speed);
      analogWrite(eL, speed);
    }
  }

  /*  All While loop here are for control the dc motor to drive "Forward", "Backward", "Left" and "Right".
      When the key got released "#" is sent and the dc motor get turn off.  */

  while (readFromBL() == "F") {
    Serial.println("Forward");

    if (readFromBL() != "#") {
      digitalWrite(in1, 1);
      digitalWrite(in3, 1);
    } else {
      off();
    }
    readFromBL();
  }

  while (readFromBL() == "R") {
    Serial.println("Right");

    if (readFromBL() != "#") {
      digitalWrite(in1, 1);
      digitalWrite(in4, 1);
    } else {
      off();
    }
    readFromBL();
  }

  while (readFromBL() == "B") {
    Serial.println("Backward");

    if (readFromBL() != "#") {
      digitalWrite(in2, 1);
      digitalWrite(in4, 1);
    } else {
      off();
    }
    readFromBL();
  }

  while (readFromBL() == "L") {
    Serial.println("Left");

    if (readFromBL() != "#") {
      digitalWrite(in2, 1);
      digitalWrite(in3, 1);
    } else {
      off();
    }
    readFromBL();
  }
}

/*Get the input and if its the speed number(Most cases 3 digits) then got it until the full number was entered. */
String readFromBL() {
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar == '*') {
      ch = "";
      incomingChar = SerialBT.read();
      while (incomingChar != '#') {
        ch += incomingChar;
        incomingChar = SerialBT.read();
      }
    } else {
      message = String(incomingChar);
      Serial.println(message);
    }
  }
  return message;
}

int getDis() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  return distance;
}
void masure() {
  int max = 0, side = 0, sides[3] = { 0, 0, 0 };
  String rlf[3] = { "Right", "Front", "Left" };

  // for (int i = 0; i <= 18; i++) {
  //   srv.write(i * 10);
  //   delay(100);
  // }
  // for (int i = 180; i >= 0; i--) {
  //   srv.write(i * 10);
  //   delay(100);
  // }

  srv.write(0);
  Serial.print("Right side: ");
  sides[0] = getDis();
  Serial.println(sides[0]);
  delay(1000);

  srv.write(90);
  delay(100);
  Serial.print("Front side: ");
  sides[1] = getDis();
  Serial.println(sides[1]);
  delay(1000);

  srv.write(180);
  Serial.print("Left side: ");
  sides[2] = getDis();
  Serial.println(sides[2]);
  delay(1000);

  /* Find the maximum Distance and highlight wich direction was it.  */
  for (int i = 0; i <= 2; i++) {
    if (sides[i] > max) {
      max = sides[i];
      side = i;
    }
  }
  SerialBT.print("You have more space in the ");
  SerialBT.println(String(rlf[side]));

  SerialBT.print("Distance: ");
  SerialBT.println(String(max));
  SerialBT.println("-------------------------");
}

void off() {
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
}