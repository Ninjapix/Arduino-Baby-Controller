#include "Nintendo.h"
/*
 * Code was heavily refrenced from 
 * https://github.com/SimpleControllers/SimpleControllersBuild-a-Box
 * Please check his work out!!! Especially if you want a non-joke controller
*/  
//EDIT THIS NUMBER IF YOU WANT A SMALLER/LARGER DEADZONE
//const int DEADZONE = 10;
//deadzone not implemented yet
//don't mess with the stuff below unless you know what you're doing

CGamecubeConsole GamecubeConsole(12); //makes controller bidirection data line to be wired to digitalpin 12
Gamecube_Data_t d = defaultGamecubeData; //sets up the gamecube data object
CGamecubeController GamecubeController(13);//this is apparently needed, don't solder anything to digialpin 19 or analog pin A5 or SCL pin without changing this
//Defining Pin Numbers: All are digital EXCEPT FOR THE X and Y AXIS ONES (so they're on the analog only pins)
//Facebuttons (Letter corrisponds to Gamecube controller, see comments for actual buttons on the toy)
const int pinA = 4; //point T13 on toy pcb, A button on toy
const int pinB = 3; //point T16 on toy pcb, D button on toy
const int pinX = 5; //point T14 on toy pcb, B button on toy
const int pinY = 2; //point T15 on toy pcb, C buton on toy
//Originally A = 2, B = 3, X = 4, Y = 5
//Shoulder button(Z)
const int pinZ = 10; //Purple button on toy, Shoulder Button (z)
//Right Trigger
const int pinL = 11; //Orange button on toy, Right Trigger

//DPad inputs
const int pinDpadU = 6; //point T10 on toy pcb
const int pinDpadD = 7; //point T12 on toy pcb
const int pinDpadL = 8; //point T9 on toy pcb
const int pinDpadR = 9; //point T11 on toy pcb

//Joystick Axis
const int pinAxisX = A6;
const int pinAxisY = A7;

//Pins for toggles:
const int DIP_SWITCH_ONE = 15;
const int DIP_SWITCH_TWO = 16;
const int DIP_SWITCH_THREE = 17;
const int DIP_SWITCH_FOUR = 18;

//Rumble:
const int RUMBLE_PIN = 14;
const int RUMBLE_TOGGLE = 19;

//Setting up Variables
int rawXPosition = 0;
int rawYPosition = 0;
int xAxisValue = 0;
int yAxisValue = 0;
bool rumbleToggle = false;
bool rightAnalogToggle = false;
bool leftAnalogToggle = false;
bool zToRightTriggerToggle = false;
bool leftTriggerToZToggle = false;
int rumbleTracker = 0;

void setup() {
  //establishing pin assignments so I can just refrence them and be ok
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinZ, INPUT_PULLUP);
  pinMode(pinL, INPUT_PULLUP);
  pinMode(pinDpadU, INPUT);
  pinMode(pinDpadD, INPUT);
  pinMode(pinDpadL, INPUT);
  pinMode(pinDpadR, INPUT);
  //Joystick Axises - pinMode is *NOT NEEDED*.
  //R button is wired only to the arduino so it can become a pullup button
  //toggles:
  pinMode(DIP_SWITCH_ONE, INPUT_PULLUP);
  pinMode(DIP_SWITCH_TWO, INPUT_PULLUP);
  pinMode(DIP_SWITCH_THREE, INPUT_PULLUP);
  pinMode(DIP_SWITCH_FOUR, INPUT_PULLUP);

  //rumble:
  pinMode(RUMBLE_PIN, OUTPUT);
  pinMode(RUMBLE_TOGGLE, INPUT_PULLUP);
  digitalWrite(RUMBLE_PIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Check if dipswitches have changed:
  checkDipSwitches();

  //Check if Rumble is activated, and if so activate
  if(rumbleToggle == true){
    digitalWrite(RUMBLE_PIN, LOW);
    if(d.status.rumble){
      digitalWrite(RUMBLE_PIN, HIGH);
    }
  }
  
  //init the values of the controller's buttons, make sure they get refreshed back to 0
  int aValue = 0;
  int bValue = 0;
  int xValue = 0;
  int yValue = 0;
  int zValue = 0;
  int lValue = 0;
  int dpadUpValue = 0;
  int dpadDownValue = 0;
  int dpadLeftValue = 0;
  int dpadRightValue = 0;
  int startValue = 0; //Start Button - Activated when all four face buttons are pressed at the same time.
  //toggle related variables:
  int rValue = 0;
  int lAnalogValue = 0;
  int rAnalogValue = 0;
  
  //Checking if the buttons are pressed or not
  if(digitalRead(pinA) == HIGH) aValue = 1;
  if(digitalRead(pinB) == HIGH) bValue = 1;
  if(digitalRead(pinX) == HIGH) xValue = 1;
  if(digitalRead(pinY) == HIGH) yValue = 1;
  if(digitalRead(pinZ) == LOW){
    if(zToRightTriggerToggle == true){
      if(rightAnalogToggle == true){
        rAnalogValue = 130;
      } else {
        rValue = 1;
      }
    } else {
      zValue = 1;
    }
  }
  if(digitalRead(pinL) == LOW){
    if(leftAnalogToggle == true){
      lAnalogValue = 130;
    } else if(leftTriggerToZToggle == true){
      zValue = 1;
    } else{
      lValue = 1;
    }
  }
  if(digitalRead(pinDpadU) == HIGH) dpadUpValue = 1;
  if(digitalRead(pinDpadD) == HIGH) dpadDownValue = 1;
  if(digitalRead(pinDpadL) == HIGH) dpadLeftValue = 1;
  if(digitalRead(pinDpadR) == HIGH) dpadRightValue = 1;
  //Checks if all four face buttons are pressed and if so the controller outputs a Start Press with none of the other face buttons being reported
  if(aValue == 1 && bValue == 1 && xValue == 1 && yValue == 1){
    if(dpadUpValue == 1){
      aValue = 0;
      bValue = 0;
      xValue = 0;
      yValue = 0;
      dpadUpValue = 0;
      quitMatch(); //meelee exit seuquence
    }
    else{
      startValue = 1;
      aValue = 0;
      bValue = 0;
      xValue = 0;
      yValue = 0;
    }
  }
  
  /* Axis Values:
   * Basically, raw position is divided by 4.
   * Afterwards, it checks if it's between +/-5 of the default resting position
   * Then, it inverts the value by taking the absolute value of the value minus the max value.
   */
  rawXPosition = analogRead(pinAxisX);
  xAxisValue = (int) (rawXPosition)/4;
  if(486 <= rawXPosition && rawXPosition <= 496){
    xAxisValue = 128;
  }
  xAxisValue = abs(xAxisValue - 255);
  
  rawYPosition = analogRead(pinAxisY);
  yAxisValue = (int) (rawYPosition)/4;
  if(535 <= rawYPosition && rawYPosition <= 531){
    yAxisValue = 128;
  }
  yAxisValue = abs(yAxisValue - 255);

  //Reporting the button inputs:
  d.report.a = aValue;
  d.report.b = bValue; 
  d.report.x = xValue;
  d.report.y = yValue;
  d.report.z = zValue;
  d.report.l = lValue;
  d.report.dup = dpadUpValue;
  d.report.ddown = dpadDownValue;
  d.report.dleft = dpadLeftValue;
  d.report.dright = dpadRightValue;
  d.report.xAxis = xAxisValue;
  d.report.yAxis = yAxisValue;
  d.report.start = startValue;
  //toggles:
  d.report.r = rValue;
  d.report.right = rAnalogValue;
  d.report.left = lAnalogValue;
  GamecubeConsole.write(d);  //Compiling and outputting the data to the gamecube
}

void quitMatch(){
      d.report.start = 1;
      GamecubeConsole.write(d);
      delay(5);
      d.report.start = 0;
      GamecubeConsole.write(d);
      delay(5);
      d.report.a = 1;
      d.report.l = 1;
      d.report.r = 1;
      d.report.start = 1;
      GamecubeConsole.write(d);
      delay(5);
      //reset values
      d.report.a = 0;
      d.report.l = 0;
      d.report.r = 0;
      d.report.start = 0;
      GamecubeConsole.write(d);
}

void checkDipSwitches(){
  if (rumbleToggle == false){
    if(digitalRead(RUMBLE_TOGGLE) == LOW){
      rumbleToggle = true;
    }
  } else if (rumbleToggle == true){
    if(digitalRead(RUMBLE_TOGGLE) == HIGH){
      rumbleToggle = false;
    }
  }
//Toggle for the Right Trigger to be 50% analog press instead of digital press, Dip Switch 4
//Needs dip switch 2 to be on in order to work.
  if (rightAnalogToggle == false){
    if(digitalRead(DIP_SWITCH_FOUR) == LOW){
      rightAnalogToggle = true;
    }
  } else if (rightAnalogToggle == true){
    if(digitalRead(DIP_SWITCH_FOUR) == HIGH){
      rightAnalogToggle = false;
    }
  }
//Toggle for the Left Trigger to be 50% analog press instead of digital press, Dip Switch 3
  if (leftAnalogToggle == false){
    if(digitalRead(DIP_SWITCH_THREE)== LOW){
      leftAnalogToggle = true;
    }
  } else if (leftAnalogToggle == true){
    if(digitalRead(DIP_SWITCH_THREE) == HIGH){
      leftAnalogToggle = false;
    }
  }
//Toggle for Z to become Right Trigger, Dip Switch 2
  if (zToRightTriggerToggle == false){
    if(digitalRead(DIP_SWITCH_TWO) == LOW){
      zToRightTriggerToggle = true;
    }
  } else if (zToRightTriggerToggle == true){
    if(digitalRead(DIP_SWITCH_TWO) == HIGH){
      zToRightTriggerToggle = false;
    }
  }
//Toggle for Left Trigger to become the Z button, Dip Switch 1
  if (leftTriggerToZToggle == false){
    if(digitalRead(DIP_SWITCH_ONE) == LOW){
      leftTriggerToZToggle = true;
    }
  } else if (leftTriggerToZToggle == true){
    if(digitalRead(DIP_SWITCH_ONE) == HIGH){
      leftTriggerToZToggle = false;
    }
  }
  return;
}
