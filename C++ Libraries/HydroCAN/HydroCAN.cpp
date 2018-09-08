/*
	HydroCAN.cpp - Library for communicating with the Hydrocontest ESC.
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia, Medellín Campus.
	Released into the public domain: spread the Hydrocontest spirit.
*/

#include <HydroCAN.h>

#define P15 0 			// P15 parameter on controller settings (via MGM COMPRO)
#define int_pin 2
#define cs_pin 53

extern MCP_CAN canbus_esc(cs_pin);

HydroCAN::HydroCAN() {

	_int_pin = int_pin;

}

bool HydroCAN::initialize() {

	pinMode(_int_pin, INPUT);

	if (canbus_esc.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
		Serial.println(F("CAN-BUS failed to initiate."));
		return false;
	}
	else {
		Serial.println(F("CAN-BUS successfully initiated."));
		canbus_esc.setMode(MCP_LISTENONLY);
		return true;
	}

}


void HydroCAN::update() {

	if (!digitalRead(_int_pin)) {
		canbus_esc.readMsgBuf(&rxId, &len, rxBuf);			// Read CAN-BUS data
		uint8_t dir = rxId & 0x07;											// Extract the 3 LSB's of rxId

		switch (dir) {
			case 1:
				voltage = (rxBuf[0] + (rxBuf[1] << 8)) / 57.45; // Volts
				current = (rxBuf[2] + (rxBuf[3] << 8)) / 10.0;  // Amps
				rpm     = (uint32_t(rxBuf[4]) + (uint32_t(rxBuf[5]) << 8) + (uint32_t(rxBuf[6]) << 16)) * 10; // Motor speed (rpm)
				break;

			case 2:
				odometer                = (uint32_t(rxBuf[0]) + (uint32_t(rxBuf[1]) << 8) + (uint32_t(rxBuf[2]) << 16) + (uint32_t(rxBuf[3]) << 24)); // Number of motor turns since powerup
				controller_temperature  = rxBuf[4];	// Celsius degrees
				motor_temperature       = rxBuf[5]; // Celsius degrees
				battery_temperature     = rxBuf[6]; // Celsius degrees
				break;

			case 3:
				input_pwm   = (rxBuf[0] + (uint16_t(rxBuf[1] << 8))) / 10; // Requested PWM (set-point)
				output_pwm  = (rxBuf[2] + (uint16_t(rxBuf[3] << 8))) / 10; // Supplied PWM (controller output)
				warnings    = (rxBuf[4] + (uint16_t(rxBuf[5] << 8))); // See table below
				errors      = (rxBuf[6] + (uint16_t(rxBuf[7] << 8))); // See table below
				break;
		}
	}
}


float HydroCAN::getVoltage(void) {
	return voltage;
}


float HydroCAN::getCurrent(void) {
	return current;
}


uint32_t HydroCAN::getRPM(void) {
	return rpm;
}


uint32_t HydroCAN::getOdometer(void) {
	return odometer;
}


uint8_t HydroCAN::getControllerTemperature(void){
	return controller_temperature;
}


uint8_t HydroCAN::getMotorTemperature(void) {
	return motor_temperature;
}


uint8_t HydroCAN::getBatteryTemperature(void) {
	return battery_temperature;
}


uint8_t HydroCAN::getInputPWM(void) {
	return input_pwm;
}


uint8_t HydroCAN::getOutputPWM(void) {
	return output_pwm;
}


uint16_t HydroCAN::readWarnings(void) {
	return warnings;
}


uint16_t HydroCAN::readErrors(void) {
	return errors;
}
