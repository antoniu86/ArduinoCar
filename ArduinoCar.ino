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

int RIGHT = 13;
int LEFT = 12;
int LIGHT = 11;

boolean enable_safety = true;
boolean honk_check = false;
boolean lights = false;
boolean stanga = false;
boolean dreapta = false;

long duration;
int distance;
int max_distance = 20;
int honk_time = 15000;
int honk_count = 0;
int honk_freq = 1500;
int stanga_count = 0;
int dreapta_count = 0;
int avarii_count = 0;
int semnalizare_timp = 10000;
int semnalizare_on = false;
int avarii_timp = 10000;
int avarii_on = false;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  pinMode(LIGHT, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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
  faruri();
  semnalizare_stanga();
  semnalizare_dreapta();
  avarii();
  
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
    } else if (cmd == 'b') {
      if (stanga == true && dreapta == true) {
        stanga = false;
        dreapta = false;
      } else {
        stanga = true;
        dreapta = true;
      }
    } else if (cmd == 'c') {
      dreapta = false;
      
      if (stanga == true) {
        stanga = false;
      } else {
        stanga = true;
      }
    } else if (cmd == 'z') {
      stanga = false;
      
      if (dreapta == true) {
        dreapta = false;
      } else {
        dreapta = true;
      }
    } else if (cmd == 'a') {
      if (lights == true) {
        lights = false;
      } else {
        lights = true;
      }
    }
    
  }
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

void faruri() {
  if (lights == true) {
    digitalWrite(LIGHT, HIGH);
  } else {
    digitalWrite(LIGHT, LOW);
  }
}

void semnalizare_stanga() {
  if (stanga == true && dreapta == false) {
    stanga_count++;
    
    if (stanga_count > semnalizare_timp) {
      stanga_count = 0;

      if (semnalizare_on == true) {
        semnalizare_on = false;
      } else {
        semnalizare_on = true;
      }
    }

    if (semnalizare_on == true) {
      digitalWrite(LEFT, HIGH);
    } else {
      digitalWrite(LEFT, LOW);
    }
  } else {
    digitalWrite(LEFT, LOW);
  }
}

void semnalizare_dreapta() {
  if (dreapta == true && stanga == false) {
    dreapta_count++;
    
    if (dreapta_count > semnalizare_timp) {
      dreapta_count = 0;

      if (semnalizare_on == true) {
        semnalizare_on = false;
      } else {
        semnalizare_on = true;
      }
    }

    if (semnalizare_on == true) {
      digitalWrite(RIGHT, HIGH);
    } else {
      digitalWrite(RIGHT, LOW);
    }
  } else {
    digitalWrite(RIGHT, LOW);
  }
}

void avarii() {
  if (stanga == true && dreapta == true) {
    avarii_count++;
    
    if (avarii_count > semnalizare_timp) {
      avarii_count = 0;

      if (avarii_on == true) {
        avarii_on = false;
      } else {
        avarii_on = true;
      }
    }

    if (avarii_on == true) {
      digitalWrite(LEFT, HIGH);
      digitalWrite(RIGHT, HIGH);
    } else {
      digitalWrite(LEFT, LOW);
      digitalWrite(RIGHT, LOW);
    }
  } else {
    digitalWrite(LEFT, LOW);
    digitalWrite(RIGHT, LOW);
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
