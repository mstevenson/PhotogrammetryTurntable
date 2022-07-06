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

//const int stepperPostCircum = 18.06;
//const int turntableCircum = 1225;

const int shotsPerRevolution = 45;

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
}

void loop()
{
  switch (state)
  {
    case IDLE:
    {
      digitalWrite(ledPin, LOW);
      // Enable stepper motor sleep to reduce heat and power consumption while idle
      digitalWrite(sleepPin, LOW);
      buttonState = digitalRead(buttonPin);
      if (buttonState == HIGH)
      {
        state = CAPTURING;
      }
      break;
    }
    case CAPTURING:
    {
      df.shootFrame(1);
      state = IMPORTING;
      break;
    }
    case IMPORTING:
    {
      // Wait for DF to finish capturing the frame
      int cmd = df.processSerial();
      if (cmd != DF_CC_MSG) {
        return;
      }
      // Once capture is complete, move the stepper motor
      state = MOVING;
      break;
    }
    case MOVING:
    {
      //rotateDeg((360/shotsPerRevolution) * (turntableCircum / stepperPostCircum), 0.5);
      rotateDeg(360/shotsPerRevolution, 0.02); // direct drive
      if (frame < shotsPerRevolution) {
        frame++;
        state = CAPTURING;
      }
      else
      {
        frame = 1;
        state = IDLE;
      }
      break;
    }
  }

  if (state != IDLE)
  {
    // Turn on LED while capturing a sequence
    digitalWrite(ledPin, HIGH);
    // Disable stepper motor sleep
    digitalWrite(sleepPin, HIGH);
  }
}

void rotateDeg(float deg, float speed)
{
  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(usDelay); 
    digitalWrite(stepPin, LOW);
    delayMicroseconds(usDelay);
  }
}
