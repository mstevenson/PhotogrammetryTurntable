#include "DFRemote.h"

#define IDLE 0
#define CAPTURING 1
#define IMPORTING 2
#define MOVING 3

const int buttonPin = 3;
const int dirPin = 4;
const int stepPin = 5;
const int sleepPin = 6;
const int ledPin = 13;

const int stepperPostCircum = 18.06;
const int turntableCircum = 1225;

const int shotsPerRevolution = 45;


// Create global object to communicate with DragonFrame
DFRemote df = DFRemote();

int buttonState = 0;
int frame = 1;

int state = IDLE;


void setup() {
  // set up serial port to 57600 kbps
  Serial.begin(57600);
  
  pinMode(buttonPin, INPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // Need to prime it for some reason. It always fails on the first shot.
  df.shootFrame(1);
} 


void loop() {

  switch (state) {
    case IDLE:
      idle();
      break;
    case CAPTURING:
      capture();
      break;
    case IMPORTING:
      import();
      break;
    case MOVING:
      move();
      break;
  }

  if (state != IDLE) {
    // Turn on LED while capturing a sequence
    digitalWrite(ledPin, HIGH);
    // Disable stepper motor sleep
    digitalWrite(sleepPin, HIGH);
  }
  
}


void idle()
{
  digitalWrite(ledPin, LOW);
  // Enable stepper motor sleep to reduce heat and power consumption while idle
  digitalWrite(sleepPin, LOW);
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    state = CAPTURING;
  }
}


void capture()
{
  df.shootFrame(1);
  state = IMPORTING;
}


void import()
{
  // Wait for DF to finish capturing the frame
  int cmd = df.processSerial();
  if (cmd != DF_CC_MSG) {
    return;
  }
  // Once capture is complete, move the stepper motor
  state = MOVING;
}


void move()
{
  rotateDeg((360/shotsPerRevolution) * (turntableCircum / stepperPostCircum), 0.5);
  if (frame < shotsPerRevolution) {
    frame++;
    state = CAPTURING;
  }
  else {
    frame = 1;
    state = IDLE;
  }
}



void rotateDeg(float deg, float speed) {
  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(usDelay); 

    digitalWrite(stepPin, LOW);
    delayMicroseconds(usDelay);
  }
}
