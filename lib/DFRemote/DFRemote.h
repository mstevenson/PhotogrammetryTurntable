#include <Arduino.h>

/*
 * DFRemote implements the Simple Serial Interface
 * for communicating with Dragonframe.
 */

#define DF_PIN_COUNT 16

#define DF_SHOOT_CMD  1
#define DF_DELETE_CMD 2
#define DF_PLAY_CMD   3
#define DF_LIVE_CMD   4

#define DF_SHOOT_MSG    1
#define DF_DELETE_MSG   2
#define DF_POSITION_MSG 3
#define DF_CC_MSG 4

#define DF_EXP_NAME_LENGTH  16

class DFRemote
{

public:
  DFRemote();
  void activatePin(int pin, int activeValue, int command);
  void activatePin(int pin, int activeValue, int command, int arg);
  void deactivatePin(int pin);

  void processPins();
  
  int processSerial();
  
  void shootFrame(int frames);
  
  void deleteFrame();

  void togglePlay();

  void goToLive();
  
  int commandFrame;
  int commandExposure;
  char commandExposureName[DF_EXP_NAME_LENGTH + 1];
  int commandStereoPosition;
  
private:
  int command[DF_PIN_COUNT];
  int logicActive[DF_PIN_COUNT];
  int arg[DF_PIN_COUNT];
  int value[DF_PIN_COUNT];
  
  int inCommand;
  
  int serialState;
  int serialNumber;
  int serialLastValue;
  
  int exposureNameIndex;
  
  unsigned long lastActivation[DF_PIN_COUNT];

};


#define STATE_START     0
#define STATE_CH1       1
#define STATE_PREFRAME  2
#define STATE_FRAME     3
#define STATE_EXP       4
#define STATE_EXP_NAME  5
#define STATE_EXP_STER  6
