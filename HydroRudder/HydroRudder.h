/*
	HydroRudder.h
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
  A good part of the code is based on 'PID_v1' library.
*/

#ifndef HydroRudder_h
#define HydroRudder_h

class HydroRudder {

  public:

    HydroRudder(double*, double*, double*, bool, double, double, double); // Constructor
    void initialize(int, int);
    void setMode(int);
    void setOutputLimits(double, double);
    void setTunings(double, double, double);
    void setSampleTime(int);
    void control(void);

    bool control(void);

    double getKp(void);
  	double getKi(void);
  	double getKd(void);

    int pin_r; // H-Bridge input pins.
    int pin_l;

  private:

  	double dispKp;
  	double dispKi;
  	double dispKd;

  	double kp;
    double ki;
    double kd;

  	int controllerDirection;

    double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
    double *myOutput;             //   This creates a hard link between the variables and the
    double *mySetpoint;           //   PID, freeing the user from having to constantly tell us
                                  //   what these values are.  with pointers we'll just know.

  	unsigned long lastTime;
  	double ITerm, lastInput;

  	unsigned long sampleTime;
  	double outMin, outMax;

};

#endif
