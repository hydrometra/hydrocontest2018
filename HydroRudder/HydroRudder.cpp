/*
	HydroRudder.cpp
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
  A big part of the code is based on 'PID_v1' library.
*/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <HydroRudder.h>

HydroRudder::HydroRudder(double* input, double* output, double* setpoint,
                         bool angleSet, double Kp, double Ki, double Kd) {

  myOutput = output;
  myInput = input;
  mySetpoint = setpoint;
	setOutputLimits(-255, 255);
  sampleTime = 50; // [ms]

  setTunings(Kp, Ki, Kd);
  lastTime = millis() - sampleTime;

}


bool HydroRudder::control(void) {

  unsigned long now = millis();
  unsigned long timeChange = (now - lastTime);

  if (angleSet) {

    if (*mySetPoint < -2) {
      analogWrite(pin_r, 50);
      analogWrite(pin_l, 0);
    }

    else if (*mySetPoint > 2) {
      analogWrite(pin_r, 0);
      analogWrite(pin_l, 50);
    }

    else {
      analogWrite(pin_r, 0);
      analogWrite(pin_l, 0);
    }

    delayMicroseconds(100);

  }


  else {

    if (timeChange >= sampleTime) {

      timeChange = timeChange > 2*sampleTime ? 2*sampleTime : timeChange;

      double input = *myInput;
      double error = *mySetpoint - input;

      ITerm += (ki * error);

      if (ITerm > outMax)
        ITerm = outMax;
      else if (ITerm < outMin)
        ITerm = outMin;

      double dInput = (input - lastInput);
      double output = kp * error + ITerm - kd * dInput;

      if (output > outMax)
        output = outMax;
      else if (output < outMin)
        output = outMin;

      *myOutput = output;
      lastInput = input;
      lastTime = now;

      return true;

    }

  }

  else return false;

}


void HydroRudder::setTunings(double Kp, double Ki, double Kd) {

  if (Kp < 0 || Ki < 0 || Kd < 0) return;

  dispKp = Kp;
  dispKi = Ki;
  dispKd = Kd;

  double sampleTimeInSec = ((double)sampleTime)/1000;

  kp = Kp;
  ki = Ki * sampleTimeInSec;
  kd = Kd / sampleTimeInSec;

 }


void HydroRudder::setSampleTime(int NewsampleTime) {

  if (NewsampleTime > 0) {
    double ratio  = (double)NewsampleTime / (double)sampleTime;
    ki *= ratio;
    kd /= ratio;
    sampleTime = (unsigned long)NewsampleTime;
  }

}


void HydroRudder::setOutputLimits(double Min, double Max) {

  if (Min >= Max) return;

  outMin = Min;
  outMax = Max;

  if (*myOutput > outMax)
    *myOutput = outMax;
  else if (*myOutput < outMin)
    *myOutput = outMin;

  if (ITerm > outMax)
    ITerm= outMax;
  else if (ITerm < outMin)
    ITerm= outMin;

}


void HydroRudder::initialize(int PINR, int PINL) {

  ITerm = *myOutput;
  lastInput = *myInput;

  if (ITerm > outMax)
    ITerm = outMax;
  else if (ITerm < outMin)
    ITerm = outMin;

  pin_r = PINR;
  pin_l = PINL;

}


double HydroRudder::getKp() {
  return  dispKp;
}


double HydroRudder::getKi() {
  return  dispKi;
}


double HydroRudder::getKd() {
  return  dispKd;
}
