#include <AccelStepper.h>
#include "DFRemote.h"

#define IDLE 0
#define CAPTURE 1
#define IMPORT 2
#define MOVE 3

const int startButtonPin = 3;

const int dirPin = 4;
const int stepPin = 5;
const int sleepPin = 6;

const int totalFrames = 30;
const int stepsPerRevolution = 1600;

int state = IDLE;
int dfMessage;
int frame = 0;

DFRemote df = DFRemote();
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

void setup()
{
  Serial.begin(57600);
  
  pinMode(startButtonPin , INPUT);

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
      stepper.disableOutputs();
      if (digitalRead(startButtonPin) == HIGH)
      {
        stepper.enableOutputs();
        delay(200);
        state = CAPTURE;
      }
      break;
    }
    case CAPTURE:
    {
      df.shootFrame(1);
      frame++;
      state = IMPORT;
      break;
    }
    case IMPORT:
    {
      int msg = df.processSerial();
      if (msg != DF_CC_MSG)
      {
        return;
      }
      if (frame < totalFrames)
      {
        stepper.moveTo(frame * (stepsPerRevolution/totalFrames));
        state = MOVE;
      }
      else
      {
        stepper.runToNewPosition(0); // blocking
        frame = 0;
        state = IDLE;
      }
      break;
    }
    case MOVE:
    {
      if (stepper.distanceToGo() != 0)
      {
        stepper.run();
      }
      else
      {
        state = CAPTURE;
      }
      break;
    }
  }
}
