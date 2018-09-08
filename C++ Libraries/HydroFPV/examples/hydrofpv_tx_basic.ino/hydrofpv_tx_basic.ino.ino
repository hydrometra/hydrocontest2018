#include <HydroFPV.h>

HydroFPV_Tx transmisor(Serial1);

void setup() {
  
  transmisor.begin(9600);
  Serial.begin(9600);

}

unsigned long ls = 0;
float vel = 6.84;
float volt = 38.50;
float curr = 15.5;
float height = 0.99;
int rpm = 2035;
int pwm = 78;
int rud_ang = -35;
bool chrono = true;
bool reverse = true;
bool pump = false;
bool esc = true;
bool stab = false;
bool turn = true;
bool dm = true;


void loop() {
  
  if (millis() - ls >= 1200) {

    ls = millis();

    transmisor.putVoltage(volt);
    transmisor.putCurrent(curr);
    transmisor.putHeight(height);
    transmisor.putVelocity(vel);
    
    transmisor.putPWM(pwm);
    transmisor.putRPM(rpm);
    transmisor.putRudderAngle(rud_ang);

    transmisor.swChrono(chrono);
    transmisor.swReverse(reverse);
    transmisor.swPump(pump);
    transmisor.swESC(esc);

    transmisor.swStabilityController(stab);
    transmisor.swTurnController(turn);
    transmisor.swDrivingMode(dm);
    
    transmisor.writeScreen();
    
  }
  
  
}
