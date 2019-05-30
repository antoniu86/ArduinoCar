#include "SoftwareSerial.h"

const int trigPin = 8;
const int echoPin = 7;

int MOTOR2_PIN1 = 3;
int MOTOR2_PIN2 = 5;
int MOTOR1_PIN1 = 6;
int MOTOR1_PIN2 = 9;

int bluetoothTx = 4;
int bluetoothRx = 2;

int BUZZER_PIN = 10;

long duration;
int distance;
int max_distance = 20;
boolean enable_safety = true;
boolean honk_check = false;
int honk_time = 15000;
int honk_count = 0;
int honk_freq = 1500;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  
  Serial.begin(9600);

  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("U,9600,N");
  bluetooth.begin(9600);
}

void loop() {
  claxon();
  
  if (bluetooth.available()) {
    char cmd = (char)bluetooth.read();
    Serial.println(cmd);

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance= duration*0.034/2;
    //Serial.println(distance);

    if (cmd == 'u') {
      if (enable_safety == true) {
        if (distance > max_distance) {
          deplasare(255,-255);
        } else {
          deplasare(0,0);
        }
      } else {
        deplasare(255,-255);
      }
    } else if (cmd == 'd') {
      deplasare(-255,255);
    } else if (cmd == 'l') {
      deplasare(-190,-255);
    } else if (cmd == 'r') {
      deplasare(255,190);
    } else if (cmd == 's') {
      deplasare(0,0);
    } else if (cmd == 'x') {
      if (enable_safety == true) {
        enable_safety = false;
      } else {
        enable_safety = true;
      }
    } else if (cmd == 'y') {
      honk_check = true;
      honk_count = 0;
    }
  }

  //if(Serial.available()) {
  //  char toSend = (char)Serial.read();
  //  bluetooth.print(toSend);
  //}
}

void claxon() {
  if (honk_check == true && honk_count < honk_time) {
    honk_count++;
    tone(BUZZER_PIN, honk_freq);
  } else if (honk_check == true) {
    honk_check = false;
    honk_count = 0;
    noTone(BUZZER_PIN);
  }
}

void deplasare(int stanga, int dreapta) {
  if (stanga > 0) {
    analogWrite(MOTOR1_PIN1, stanga);
    analogWrite(MOTOR1_PIN2, 0);
  } else {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -stanga);
  }
 
  if (dreapta > 0) {
    analogWrite(MOTOR2_PIN1, dreapta);
    analogWrite(MOTOR2_PIN2, 0);
  } else {
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, -dreapta);
  }

  if ((stanga == 0) && (dreapta == 0)) {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, 0);
    
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, 0);
  }
}
