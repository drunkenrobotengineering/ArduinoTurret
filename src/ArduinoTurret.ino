/**
This sketch is to control the turret.
Motor 1 (controlled by joystick) is left/right (negative/positive)
Motor 2 (controlled by joystick) is up/down (negative/positive)
The C button cuts motor speed in half to enable more precise aiming
Firing is controlled by the Z button
**/

#include "DualMC33926MotorShield.h"
#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 19200
// the max speed can go up to 400, but that's too fast for aiming
#define MAX_SPEED 200
#define PAUSE 2

DualMC33926MotorShield md;
ArduinoNunchuk nunchuk = ArduinoNunchuk();

// digital pin controlling gun relay
// using a different pin would be good,
// as the digital pin lights up for a second when
// the arduino turns on, but the motor shield seems
// to make them not work properly
int gun = 13;

void print();
int absolute(int a);
void aim(int x, int y);
void horizontal(double speed);
void vertical(double speed);
void fire(int fire);

void stopIfFault()
{
  if (md.getFault())
  {
    Serial.println("fault");
    while(1);
  }
}

void setup()
{
  Serial.begin(BAUDRATE);
  md.init();
  nunchuk.init();
}

void loop()
{
  nunchuk.update();
  print();
  // we subtract half the magnitude of each joystick value
  // to make it easier to detect deviation from the default
  int x = nunchuk.analogX - 128;
  int y = nunchuk.analogY - 128;
  // the accelerometer isn't used in this program
  // it could be used to aim, if desired
  int aX = nunchuk.accelX - 512;
  int aY = nunchuk.accelY - 512;
  int aZ = nunchuk.accelZ - 512;
  // the buttons are used, but as they're binary,
  // no adjustment is necessary
  int bZ = nunchuk.zButton;
  int bC = nunchuk.cButton;
  
  aim(x,y,bC);
  // we always call fire, it checks if it actually should fire inside
  fire(bZ);
}

// print the input to serial for debugging
void print()
{
  Serial.print(nunchuk.analogX - 128, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.analogY - 128, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.accelX - 512, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.accelY - 512, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.accelZ - 512, DEC);
  Serial.print(' ');
  Serial.print(nunchuk.zButton, DEC);
  Serial.print(' ');
  Serial.println(nunchuk.cButton, DEC);
}

// return the absolute value of the number
int absolute(int a)
{
  if (a>=0) return a;
  else return -1*a;
}

void aim(int x, int y, int c)
{
  double divisor = 128;
  if (c==1) divisor*=2;
  // we check if it's more than 10 in order to prevent
  // slight wiggle or drift when the joystick isn't being
  // moved but still registers one or two off in either dimension
  if (absolute(x) > 10) {
    horizontal((1.0*x)/divisor);
  } else {
    horizontal(0);
  }
  if (absolute(y) > 10) {
    vertical((1.0*y)/divisor);
  } else {
    vertical(0);
  }
}

// this controls left/right aiming
void horizontal(double speed)
{
  if (md.getFault())
  {
    Serial.println("fault");
    while(1);
  }
  md.setM1Speed(speed * (1.0*MAX_SPEED));
}

// this controls up/down aiming
void vertical(double speed)
{
  if (md.getFault())
  {
    Serial.println("fault");
    while(1);
  }
  md.setM2Speed(speed * (1.0*MAX_SPEED));
}

void fire(int fire)
{
  if (fire == 1) {
    // fire!
    digitalWrite(gun, HIGH);
  } else {
    // don't fire!
    digitalWrite(gun, LOW);
  }
}


