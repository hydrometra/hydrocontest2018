/*
	HydroCAN.h - Library for communicating with the Hydrocontest ESC.
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#ifndef HYDROCAN_h
#define HYDROCAN_h

#include "Arduino.h"
#include <mcp_can.h>
#include <SPI.h>

class HydroCAN {

	public:

		HydroCAN(void);

		float 		getVoltage(void);
		float 		getCurrent(void);
		uint32_t 	getRPM(void);
		uint32_t 	getOdometer(void);
		uint8_t 	getControllerTemperature(void);
		uint8_t 	getMotorTemperature(void);
		uint8_t 	getBatteryTemperature(void);
		uint8_t 	getInputPWM(void);
		uint8_t 	getOutputPWM(void);
		uint16_t 	readWarnings(void);
		uint16_t 	readErrors(void);

		bool initialize(void);
		void update(void);


	private:

		uint8_t _int_pin;
		unsigned char len;
		unsigned char rxBuf[8];
		uint32_t rxId;

		float voltage;
		float current;
		uint32_t rpm;
		uint32_t odometer;
		uint8_t controller_temperature;
		uint8_t motor_temperature;
		uint8_t battery_temperature;
		uint8_t input_pwm;
		uint8_t output_pwm;
		uint16_t warnings;
		uint16_t errors;

};

#endif
