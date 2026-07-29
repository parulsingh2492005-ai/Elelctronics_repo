/*
  LINE FOLLOWING ROBOT - 2 IR Sensor Version
  Board: ESP32 WROOM
  Driver: L298N
  Motors: 2x DC Motor

  Logic (2 sensor, both sensors OVER white line = go straight):
  LEFT   RIGHT   ACTION
   0      0      Forward   (both on white line)
   1      0      Turn Left  (left sensor sees black -> off line on left)
   0      1      Turn Right
   1      1      Stop / Sharp turn (line lost or intersection)

  NOTE: IR module output logic depends on your module.
  Most common: HIGH (1) = BLACK surface detected, LOW (0) = WHITE surface detected.
  If your bot behaves backwards, just swap LINE_DETECTED value below.
*/

// ---------------- PIN CONFIG ----------------
#define IR_LEFT   34   // Left IR sensor OUT pin
#define IR_RIGHT  35   // Right IR sensor OUT pin

// L298N Motor Driver
#define ENA 25   // Left motor speed (PWM)
#define IN1 26   // Left motor dir 1
#define IN2 27   // Left motor dir 2

#define ENB 14   // Right motor speed (PWM)
#define IN3 12   // Right motor dir 1
#define IN4 13   // Right motor dir 2

// ---------------- SPEED CONFIG ----------------
const int BASE_SPEED = 180;   // 0-255
const int TURN_SPEED = 150;   // 0-255

#define LINE_DETECTED HIGH   // change to LOW if your sensor logic is inverted

// PWM setup (ESP32 LEDC channels)
const int PWM_FREQ = 1000;
const int PWM_RES  = 8;      // 8-bit -> 0-255
const int CH_A = 0;
const int CH_B = 1;

void setup() {
  Serial.begin(115200);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(CH_A, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, CH_A);

  ledcSetup(CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENB, CH_B);

  stopMotors();
}

void loop() {
  int leftVal  = digitalRead(IR_LEFT);
  int rightVal = digitalRead(IR_RIGHT);

  if (leftVal != LINE_DETECTED && rightVal != LINE_DETECTED) {
    // both on line -> go straight
    forward(BASE_SPEED, BASE_SPEED);
  }
  else if (leftVal == LINE_DETECTED && rightVal != LINE_DETECTED) {
    // left sensor off line -> turn left
    forward(0, TURN_SPEED);
  }
  else if (rightVal == LINE_DETECTED && leftVal != LINE_DETECTED) {
    // right sensor off line -> turn right
    forward(TURN_SPEED, 0);
  }
  else {
    // both sensors detect black -> intersection / line lost
    stopMotors();
  }
}

// ---------------- MOTOR FUNCTIONS ----------------
void forward(int leftSpeed, int rightSpeed) {
  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(CH_A, leftSpeed);

  // Right motor forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(CH_B, rightSpeed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(CH_A, 0);
  ledcWrite(CH_B, 0);
}
