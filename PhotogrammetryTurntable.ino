#include <AccelStepper.h>
#include "DFRemote.h"

#define IDLE 0
#define CAPTURING 1
#define IMPORTING 2
#define MOVING 3

int state = IDLE;

const int buttonPin = 3;
const int ledPin = 13;

// stepper pins
const int dirPin = 4;
const int stepPin = 5;
const int sleepPin = 6;

const int totalFrames = 30;
const int stepsPerRevolution = 1600;

int buttonState = 0;
int frame = 0;

DFRemote df = DFRemote();
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

void setup()
{
  Serial.begin(57600);
  
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  stepper.setEnablePin(sleepPin);
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);
  stepper.disableOutputs();
}

void loop()
{
  switch (state)
  {
    case IDLE:
    {
      digitalWrite(ledPin, LOW);
      // Enable stepper motor sleep to reduce heat and power consumption while idle
      stepper.disableOutputs();
      buttonState = digitalRead(buttonPin);
      if (buttonState == HIGH)
      {
        stepper.enableOutputs(); // disable stepper sleep
        delay(500);
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
      if (cmd != DF_CC_MSG)
      {
        return;
      }
      state = MOVING;
      break;
    }
    case MOVING:
    {
      if (frame < totalFrames)
      {
        frame++;
        stepper.runToNewPosition(frame * (stepsPerRevolution/totalFrames));
        state = CAPTURING;
      }
      else
      {
        frame = 0;
        stepper.runToNewPosition(0);
        state = IDLE;
      }
      break;
    }
  }
}
