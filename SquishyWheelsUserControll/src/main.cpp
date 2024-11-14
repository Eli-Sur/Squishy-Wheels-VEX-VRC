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
motor leftTopMotor = motor(PORT1, ratio18_1, false);

motor rightTopMotor = motor(PORT10, ratio18_1, true);

motor leftBackMotor = motor(PORT11, ratio18_1, false);

motor rightBackMotor = motor(PORT20, ratio18_1, true);

motor intakeRamp = motor(PORT17, ratio18_1, false);

motor flipArm = motor(PORT15, ratio36_1, false);

motor goalHitArm = motor(PORT18, ratio18_1, false);

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

void setFlipArm() {
  flipArm.setPosition(-10, degrees);
  flipArm.spinTo(0, degrees, true);
}


void handleHitArm() {
  goalHitArm.spinTo(90, degrees, true);
  goalHitArm.spinTo(0, degrees, false);
}

void hitRing() {
  waitUntil(!switchSensor.pressing());
  //waitUntil(switchSensor.pressing());
  //waitUntil(!switchSensor.pressing());

  wait(0.2, seconds);

  handleHitArm();
  waitUntil(!goalHitArm.isSpinning());
  handleHitArm();
}

void pre_auton(void) {
  leftTopMotor.setVelocity(100.0, percent);
  leftBackMotor.setVelocity(100.0, percent);
  rightBackMotor.setVelocity(100.0, percent);
  rightTopMotor.setVelocity(100.0, percent);
  intakeRamp.setVelocity(100.0, percent);
  goalHitArm.setVelocity(100.0, percent);
  goalHitArm.setPosition(170.0, degrees);
  setFlipArm();
  switchSensor.pressed(hitRing);

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
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


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

bool spinRamp = false;
vex::directionType rampDirection = forward;
bool lockedRamp = false;

void driveForward(float numSquares) {
  float squareInTurns = 24.0 / ((1.5) * (1.5) * (3.14159));

  rightBackMotor.spinTo(squareInTurns * numSquares, turns, false);
  leftBackMotor.spinTo(squareInTurns * numSquares, turns, false);
  rightTopMotor.spinTo(squareInTurns * numSquares, turns, false);
  leftTopMotor.spinTo(squareInTurns * numSquares, turns, true);
}

void trunRight(float degree) {
  float degreeInTurns = (6.5 * 6.5) / ((1.5) * (1.5));

  rightBackMotor.spinTo(degreeInTurns * degree * -1, turns, false);
  leftBackMotor.spinTo(degreeInTurns * degree, turns, false);
  rightTopMotor.spinTo(degreeInTurns * degree * -1, turns, false);
  leftTopMotor.spinTo(degreeInTurns * degree , turns, true);
}

void turnLeft(float degree) {
  trunRight(degree * -1);
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
  //waitUntil(!Controller1.ButtonR2.pressing());
}

void handleLockStake() {
  //waitUntil(!Controller1.ButtonL1.pressing());

  if(!lockedRamp) {
    flipArm.spin(forward, 10.0, volt);
  } else {
    flipArm.stop();
    flipArm.spinTo(100, degrees, false);
  }

  lockedRamp = !lockedRamp;
}


void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................

  driveForward(1);
  turnLeft(180);
  driveForward(-1);

  handleLockStake();

  turnLeft(90);
  handleRampForward();
  driveForward(1);
  driveForward(-1);
  turnLeft(120);

  handleLockStake();

  driveForward(1);
}

void usercontrol(void) {
  goalHitArm.spinTo(0, degrees);
  flipArm.spinTo(100, degrees);

  Controller1.ButtonR1.pressed(handleRampForward);
  Controller1.ButtonR2.pressed(handleRampReverse);
  Controller1.ButtonL2.pressed(handleLockStake);
  Controller1.ButtonX.pressed(handleHitArm);

  // User control code here, inside the loop
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.
    leftTopMotor.spin(forward, (Controller1.Axis3.position() + Controller1.Axis1.position() / 2) / 90.0 * 12, volt);
    leftBackMotor.spin(forward, (Controller1.Axis3.position() + Controller1.Axis1.position() / 2) / 90.0 * 12, volt);
    rightTopMotor.spin(forward, (Controller1.Axis3.position() - Controller1.Axis1.position() / 2) / 90.0 * 12, volt);
    rightBackMotor.spin(forward, (Controller1.Axis3.position() - Controller1.Axis1.position() / 2) / 90.0 * 12, volt);
    if(spinRamp) {
      intakeRamp.spin(rampDirection, 12, volt);
    } else {
      intakeRamp.stop();
    }
    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
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
