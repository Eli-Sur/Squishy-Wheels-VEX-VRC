/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       elisur                                                    */
/*    Created:      9/23/2024, 5:49:17 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// Brain should be defined by default
brain Brain;


// START V5 MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END V5 MACROS


// Robot configuration code.
vision::signature visionSensor__STAKE = vision::signature (1, -1903, -1289, -1596,-5049, -4107, -4578,2.5, 0);
vision::signature visionSensor__REDRING = vision::signature (2, 10019, 12261, 11140,127, 799, 463,2.5, 0);
vision::signature visionSensor__BLUERING = vision::signature (3, -4097, -3147, -3622,7899, 9557, 8728,2.5, 0);
vision visionSensor = vision (PORT9, 50, visionSensor__STAKE, visionSensor__REDRING, visionSensor__BLUERING);

motor leftTopMotor = motor(PORT1, ratio18_1, false);

motor rightTopMotor = motor(PORT20, ratio18_1, true);

motor leftBackMotor = motor(PORT10, ratio18_1, false);

motor rightBackMotor = motor(PORT11, ratio18_1, true);

digital_out clamp = digital_out(Brain.ThreeWirePort.A);

motor scoringChain = motor(PORT17, ratio18_1, false);

motor intakeWheels = motor(PORT8, ratio18_1, false);

limit switchSensor = limit(Brain.ThreeWirePort.H);

controller Controller1 = controller(primary);


// generating and setting random seed
void initializeRandomSeed(){
  int systemTime = Brain.Timer.systemHighResolution();
  double batteryCurrent = Brain.Battery.current();
  double batteryVoltage = Brain.Battery.voltage(voltageUnits::mV);

  // Combine these values into a single integer
  int seed = int(batteryVoltage + batteryCurrent * 100) + systemTime;

  // Set the seed
  srand(seed);
}



void vexcodeInit() {

  //Initializing random seed.
  initializeRandomSeed(); 
}


// Helper to make playing sounds from the V5 in VEXcode easier and
// keeps the code cleaner by making it clear what is happening.
void playVexcodeSound(const char *soundName) {
  printf("VEXPlaySound:%s\n", soundName);
  wait(5, msec);
}



// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

#pragma endregion VEXcode Generated Robot Configuration

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

bool spinRamp = false;
vex::directionType rampDirection = forward;
bool lockedRamp = false;

bool isRedAlliance = true;
bool isRightSide = true;
bool isSkills = true;

float powerToRamp = 9.0f;

void setVelocity(float amount) {
  leftTopMotor.setVelocity(amount, percent);
  leftBackMotor.setVelocity(amount, percent);
  rightBackMotor.setVelocity(amount, percent);
  rightTopMotor.setVelocity(amount, percent);
}

void askUserForSideAndAlliance() {
  if(isSkills) {
    for(int i = 0 ; i < 4; i++) {
      Brain.Screen.setFillColor(red);
      Brain.Screen.drawRectangle(0, 0, 239 * 2, 239 * 2);
      wait(0.5, seconds);
      Brain.Screen.setFillColor(yellow);
      Brain.Screen.drawRectangle(0, 0, 239 * 2, 239 * 2);
      wait(0.5, seconds);
    }
    Brain.Screen.clearScreen();
    Brain.Screen.print("Cuation: Skills Atounomus");
    return;
  }

  Brain.Screen.setFillColor(red);
  Brain.Screen.drawRectangle(0, 0, 239, 239);
  Brain.Screen.setFillColor(blue);
  Brain.Screen.drawRectangle(239, 0, 239, 239);
  Brain.Screen.setFillColor(green);
  Brain.Screen.setPenColor(black);
  waitUntil(Brain.Screen.pressing());
  isRedAlliance = Brain.Screen.xPosition() < 239;
  Brain.Screen.clearScreen();

  waitUntil(!Brain.Screen.pressing());

  Brain.Screen.setFillColor(green);
  Brain.Screen.drawRectangle(0, 0, 239, 239);
  Brain.Screen.setFillColor(red);
  Brain.Screen.drawRectangle(239, 0, 239, 239);
  Brain.Screen.setFillColor(green);
  Brain.Screen.setPenColor(black);
  waitUntil(Brain.Screen.pressing());
  isRightSide = Brain.Screen.xPosition() > 239;

  Brain.Screen.clearScreen();
  if(isRedAlliance) {
    Brain.Screen.print("Red Alliance on ");
  } else {
    Brain.Screen.print("Blue Alliance on ");
  }

  if(isRightSide) {
    Brain.Screen.print("right side.");
  } else {
    Brain.Screen.print("left side.");
  }
}
 
void driveForward(float numSquares) {
  float squareInTurns = 24.0 / (4.0 * (3.14159));

  rightBackMotor.spinFor(squareInTurns * numSquares, turns, false);
  leftBackMotor.spinFor(squareInTurns * numSquares, turns, false);
  rightTopMotor.spinFor(squareInTurns * numSquares, turns, false);
  leftTopMotor.spinFor(squareInTurns * numSquares, turns, true);
}

void turnRight(float degree, bool tryToStop = true) {
  float degreeInTurns = 0.00884037036;

  rightBackMotor.spinFor(degreeInTurns * degree * -1, turns, false);
  leftBackMotor.spinFor(degreeInTurns * degree, turns, false);
  rightTopMotor.spinFor(degreeInTurns * degree * -1, turns, false);
  leftTopMotor.spinFor(degreeInTurns * degree, turns, true);

  if(tryToStop) {
    rightBackMotor.spinFor(degreeInTurns * (degree < 0 ? -1 : 1), turns, false);
    leftBackMotor.spinFor(degreeInTurns * (degree < 0 ? -1 : 1) * -1, turns, false);
    rightTopMotor.spinFor(degreeInTurns * (degree < 0 ? -1 : 1), turns, false);
    leftTopMotor.spinFor(degreeInTurns * (degree < 0 ? -1 : 1) * -1, turns, true);
  }
}

void turnLeft(float degree, bool tryToStop = true) {
  turnRight(degree * -1, tryToStop);
}

void slipRight(float degree) {
  float degreeInTurnsForLeftMotors = ((13.0 * 2) / (4.0)) / 360.0 - 0.001;

  leftBackMotor.spinFor(degreeInTurnsForLeftMotors * degree, turns, false);
  leftTopMotor.spinFor(degreeInTurnsForLeftMotors * degree, turns, true);
}

void slipLeft(float degree) {
  float degreeInTurnsForRightMotors = ((13.0 * 2) / (4.0)) / 360.0 - 0.001;

  rightBackMotor.spinFor(degreeInTurnsForRightMotors * degree, turns, false);
  rightTopMotor.spinFor(degreeInTurnsForRightMotors * degree, turns, true);
}


void handleRampForward() {
  if(rampDirection == forward) {
    spinRamp = !spinRamp;
  } else if(!spinRamp) {
    spinRamp = true;
    rampDirection = forward;
  } else {
    rampDirection = forward;
  }

  if(spinRamp) {
    scoringChain.spin(rampDirection, powerToRamp, volt);
    intakeWheels.spin(rampDirection, 12, volt);
  } else {
    scoringChain.stop();
    intakeWheels.stop();
  }
  //waitUntil(!Controller1.ButtonR1.pressing());
}

void handleRampReverse() {
  if(rampDirection == reverse) {
    spinRamp = !spinRamp;
  } else if(!spinRamp) {
    spinRamp = true;
    rampDirection = reverse;
  } else {
    rampDirection = reverse;
  }

  if(spinRamp) {
    scoringChain.spin(rampDirection, powerToRamp, volt);
    intakeWheels.spin(rampDirection, 9, volt);
  } else {
    scoringChain.stop();
    intakeWheels.stop();
  }
  //waitUntil(!Controller1.ButtonR2.pressing());
}

bool state = false;
void handlePneumaticClamp() {
  clamp.set(!state);
  state = !state;
}

void incrementPowerToRamp() {
  powerToRamp += 0.5;

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("%.2f", powerToRamp);
}

void decrementPowerToRamp() {
  powerToRamp -= 0.5;

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("%.2f", powerToRamp);
}

void turnToStake() {
  visionSensor.takeSnapshot(visionSensor__STAKE);
  while(visionSensor.largestObject.centerX < 200 - 20 || visionSensor.largestObject.centerX > 200 + 20) {
    turnRight((visionSensor.largestObject.centerX - 200) / 200.0 * 10, false);
    
    wait(0.1, seconds);
    visionSensor.takeSnapshot(visionSensor__STAKE);
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void redRightAtonomous() {
  driveForward(0.8); // drive forward 0.8 squares
  turnRight(180); // turn around
  driveForward(-0.7); // drive backward 0.7 squares
  setVelocity(10.0); // slow down to grab mobile goal
  turnToStake();
  driveForward(-0.1); // back up to modile goal
  handlePneumaticClamp(); // clamp mobile goal
  setVelocity(60.0); // go back to normal speed
  driveForward(0.2);
  handleRampForward(); // score starter ring
  wait(1, seconds);
  slipLeft(115); // turn to the left. Heavier so turning needs to be greater.
  driveForward(0.5); // drive forward one square into a ring
  wait(2, seconds); // pick up ring and score
  driveForward(-0.2); // back up to not intake blue ring
  // slipRight(90); // turn to another ring
  // driveForward(1); // grab ring
  // wait(2, seconds); // score ring
  handleRampReverse(); // Reverse ramp to expell ring.
  wait(2, seconds);
  handleRampReverse();
}

void blueLeftAtonomous() {
  driveForward(0.8); // drive forward 0.8 squares
  turnLeft(190); // turn around
  driveForward(-0.7); // drive backward 0.7 squares
  setVelocity(10.0); // slow down to grab mobile goal
  turnToStake();
  driveForward(-0.1); // back up to modile goal
  handlePneumaticClamp(); // clamp mobile goal
  setVelocity(60.0); // go back to normal speed
  handleRampForward(); // score starter ring
  wait(1, seconds);
  slipRight(125); // turn to the left. Heavier so turning needs to be greater.
  driveForward(0.5); // drive forward one square into a ring
  wait(1, seconds); // pick up ring and score
  driveForward(-0.2); // back up to not intake blue ring
  // slipLeft(90); // turn to another ring
  // driveForward(1); // grab ring
  // wait(2, seconds); // score ring
  handleRampReverse(); // Stop ramp.
  wait(1, seconds);
  handleRampReverse();
}

void redLeftAtonomous() {
  driveForward(0.8); // drive forward 0.8 squares
  turnLeft(190); // turn around
  driveForward(-0.7); // drive backward 0.7 squares
  setVelocity(10.0); // slow down to grab mobile goal
  turnToStake();
  driveForward(-0.1); // back up to modile goal
  handlePneumaticClamp(); // clamp mobile goal
  setVelocity(60.0); // go back to normal speed
  handleRampForward(); // score starter ring
  wait(0.2, seconds);
  driveForward(-0.2);
  slipRight(140); // turn to the right. Heavier so turning needs to be greater.
  driveForward(0.2); // drive forward one square into a ring
  wait(1.5, seconds); // pick up ring and score
  driveForward(-0.2); // back up to not intake blue ring
  handleRampReverse(); // Expell blue ring.
  wait(1, seconds);
  slipRight(80.0);
  handleRampForward();
  driveForward(0.3);
}

void blueRightAtonomous() {
  driveForward(0.8); // drive forward 0.8 squares
  turnRight(190); // turn around
  driveForward(-0.7); // drive backward 0.7 squares
  setVelocity(10.0); // slow down to grab mobile goal
  turnToStake();
  driveForward(-0.1); // back up to modile goal
  handlePneumaticClamp(); // clamp mobile goal
  setVelocity(60.0); // go back to normal speed
  handleRampForward(); // score starter ring
  wait(0.2, seconds);
  slipLeft(120); // turn to the right. Heavier so turning needs to be greater.
  driveForward(0.3); // drive forward one square into a ring
  wait(1, seconds); // pick up ring and score
  wait(1, seconds);
  driveForward(-0.2); // back up to not intake blue ring
  wait(1, seconds);
  slipLeft(60.0); 
  driveForward(0.3);
}

void clampStake() {
  setVelocity(10.0);
  driveForward(-0.2);
  handlePneumaticClamp();
  wait(0.2, seconds);
  setVelocity(100.0);
}

void circleTurn(float degree) {
  float degreeInTurns = 0.00884037036;

  rightBackMotor.spinFor(degreeInTurns * degree / 4.0, turns, false);
  leftBackMotor.spinFor(degreeInTurns * degree, turns, false);
  rightTopMotor.spinFor(degreeInTurns * degree / 4.0, turns, false);
  leftTopMotor.spinFor(degreeInTurns * degree, turns, true);
}

void skillsAtonomous() {
  driveForward(0.7);
  turnLeft(95);
  driveForward(-0.5);
  turnToStake();
  clampStake();

  // handleRampForward();
  // slipRight(120);
  // driveForward(0.5);
  // slipRight(180);
  // driveForward(0.5);
  // slipRight(100);
  // driveForward(1);
  // wait(2, seconds);
  // slipRight(150);
  // handlePneumaticClamp();
  // driveForward(-1);
  // wait(0.1, seconds);
  // driveForward(1);
  // handleRampForward();

  driveForward(1);
  handleRampForward();
  wait(1, seconds);
  slipRight(45);
  handleRampForward();
  handlePneumaticClamp();
  driveForward(-3);
  slipLeft(30);
  driveForward(2);
  slipRight(-90);
  driveForward(-0.5);
  wait(0.3, seconds);
  turnRight(-90);
  driveForward(-1);
  turnToStake();
  driveForward(-0.4);
  clampStake();
  turnLeft(45);
  handlePneumaticClamp();
  driveForward(-2);
  driveForward(1);
}

void autonomous(void) {
  setVelocity(60.0);

  if(isSkills) {
    skillsAtonomous();
    return;
  }
  
  if(isRedAlliance) {
    if(isRightSide) {
      redRightAtonomous();
    } else {
      redLeftAtonomous();
    }
  } else {
    if(isRightSide) {
      blueRightAtonomous();
    } else {
      blueLeftAtonomous();
    }
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

bool aboveLine(double x, double y, double px1, double py1, double px2, double py2) {
  return (py2 - py1) * (x - px1) / (px2 - px1) + py1 <= y;
}

void drawStar() {
  double pi = 3.1415926535;
  double xVals[5] = {cos(pi / 10.0), 0, cos(pi - pi / 10.0), cos(pi + 3 * pi / 10), cos(2 * pi - 3 * pi / 10)};
  double yVals[5] = {sin(pi / 10.0), 1, sin(pi - pi / 10.0), sin(pi + 3 * pi / 10), sin(2 * pi - 3 * pi / 10)};

  for(int x = 0; x < 239; x++) {
    double mappedX = (x * 2.0 / 239.0 - 1) * -1;
    for(int y = 10; y < 249; y++) {
      double mappedY = ((y-10) * 2.0 / 239.0 - 1) * -1;

      if(mappedY > yVals[0]) {
        if(!aboveLine(mappedX, mappedY, xVals[1], yVals[1], xVals[3], yVals[3]) && !aboveLine(mappedX, mappedY, xVals[1], yVals[1], xVals[4], yVals[4])) {
          Brain.Screen.drawPixel(x, y);
        }
      } else if(aboveLine(mappedX, mappedY, xVals[2], yVals[2], xVals[4], yVals[4])) {
        if(aboveLine(mappedX, mappedY, xVals[0], yVals[0], xVals[3], yVals[3])) {
          Brain.Screen.drawPixel(x, y);
        } else if(!aboveLine(mappedX, mappedY, xVals[1], yVals[1], xVals[4], yVals[4])) {
          Brain.Screen.drawPixel(x, y);
        }
      } else if(aboveLine(mappedX, mappedY, xVals[0], yVals[0], xVals[3], yVals[3]) && !aboveLine(mappedX, mappedY, xVals[1], yVals[1], xVals[3], yVals[3])) {
        Brain.Screen.drawPixel(x, y);
      }
    }
  }
}

void drawTexanFlag() {
  Brain.Screen.clearScreen();
  Brain.Screen.setFillColor(blue);
  Brain.Screen.drawRectangle(0, 0, 239, 239);
  Brain.Screen.setFillColor(white);
  Brain.Screen.drawRectangle(239, 0, 239, 239 / 2);
  Brain.Screen.setFillColor(red);
  Brain.Screen.drawRectangle(239, 239 / 2, 239, 239);
  
  Brain.Screen.setFillColor(white);
  drawStar();
  // Brain.Screen.drawCircle(239 / 2, 239 / 2, 239 / 2 - 30);
  // Brain.Screen.setFillColor(blue);
  // Brain.Screen.drawCircle(239 / 2, 239 / 2, 239 / 3 - 30);
}

bool istesting = false;
void turnRightTester() {
  istesting = true;
  setVelocity(60.0f);
  slipRight(90);
  setVelocity(100.0f);
  istesting = false;
}

void turnLeftTester() {
  istesting = true;
  setVelocity(60.0f);
  slipLeft(90);
  setVelocity(100.0f);
  istesting = false;
}

void grabStakeTester() {
  istesting = true;
  setVelocity(60);
  turnToStake();
  driveForward(-0.5);
  clampStake();
  istesting = false;
}

void usercontrol(void) {
  setVelocity(100.0);
  // istesting = true;

  

  drawTexanFlag();

  // User control code here, inside the loop
  while (1) {
    if(istesting) {
      wait(20, msec);
      continue;
    }
    visionSensor.takeSnapshot(visionSensor__STAKE);
    if(visionSensor.largestObject.exists) {
      Controller1.Screen.clearScreen();
      Controller1.Screen.setCursor(1, 1);
      Controller1.Screen.print("Angle: %d", visionSensor.largestObject.centerX);
    }

    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.
    leftTopMotor.spin(forward, (Controller1.Axis3.position() + Controller1.Axis1.position()) / 90.0 * 12, volt);
    leftBackMotor.spin(forward, (Controller1.Axis3.position() + Controller1.Axis1.position()) / 90.0 * 12, volt);
    rightTopMotor.spin(forward, (Controller1.Axis3.position() - Controller1.Axis1.position()) / 90.0 * 12, volt);
    rightBackMotor.spin(forward, (Controller1.Axis3.position() - Controller1.Axis1.position()) / 90.0 * 12, volt);
    
    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

void pre_auton(void) {
  scoringChain.setVelocity(100.0, percent);

  Controller1.ButtonX.pressed(handlePneumaticClamp);
  Controller1.ButtonB.pressed(grabStakeTester);
  Controller1.ButtonR1.pressed(handleRampForward);
  Controller1.ButtonR2.pressed(handleRampReverse);
  Controller1.ButtonRight.pressed(turnRightTester);
  Controller1.ButtonLeft.pressed(turnLeftTester);
  Controller1.ButtonDown.pressed(decrementPowerToRamp);
  Controller1.ButtonUp.pressed(incrementPowerToRamp);

  askUserForSideAndAlliance();
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
