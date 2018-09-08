#include <HydroCAN.h>

HydroCAN can;

void setup() {
  Serial.begin(9600);
  can.initialize();
}

void loop() {
  can.update();
  print_each(200);
}

void print_each(int ms) {
  static unsigned long last_ms = 0;
  if (millis() - last_ms >= ms) {
    last_ms = millis();
    Serial.print(can.getVoltage()); Serial.print('\t');
    Serial.print(can.getCurrent()); Serial.print('\t');
    Serial.print(can.getRPM()); Serial.print('\t');
    Serial.print(can.getOdometer()); Serial.print('\t');
    Serial.print(can.getMotorTemperature()); Serial.print('\t');
    Serial.print(can.getBatteryTemperature()); Serial.print('\t');
    Serial.print(can.getInputPWM()); Serial.print('\t');
    Serial.print(can.getOutputPWM()); Serial.print('\t');
    Serial.print(can.readWarnings()); Serial.print('\t');
    Serial.print(can.readErrors()); Serial.println('\t');
  }
}

