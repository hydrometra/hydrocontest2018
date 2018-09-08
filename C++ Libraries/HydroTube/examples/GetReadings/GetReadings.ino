#include "HydroTube.h"

const int analog_pin = 0;
const int filter_order = 10; // Library outputs the average of the last filter_order readings.

HydroTube velocity_sensor(analog_pin, filter_order);

void setup() {
  Serial.begin(9600);
  velocity_sensor.initialize(); // Calculates sensor offset and some other stuff.
}

#define print_time 200 // ms
uint64_t timer = 0;

void loop() {

  velocity_sensor.update(); // Update readings

  if (millis() - timer >= print_time) { // Library methods:
    Serial.print("Raw data: ");
    Serial.print(velocity_sensor.getRawMeasurement());
    Serial.print("\tPressure (kPa): ");
    Serial.print(velocity_sensor.getPressure());
    Serial.print("\tVelocity (m/s): ");
    Serial.print(velocity_sensor.getVelocity());
    Serial.println();
    timer = millis();
  }

}
