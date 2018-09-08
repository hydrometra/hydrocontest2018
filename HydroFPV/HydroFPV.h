/*
	HydroFPV.h
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#ifndef HYDROFPV_h
#define HYDROFPV_h

#include "Arduino.h"

#define PAYLOAD_SIZE 14
#define PRINT_MS 200

class HydroFPV_Tx {

	public:

		HydroFPV_Tx(HardwareSerial&);

		void writeScreen(void);
		void begin(uint32_t);
		void putVoltage(float);
		void putCurrent(float);
		void putVelocity(float);
		void putHeight(float);
		void putRPM(int);
		void putTemperature(int);
		void putRSSI(int);
		void putPWM(int);
		void putRudderAngle(int);
		void swChrono(bool);
		void swReverse(bool);
		void swPump(bool);
		void swESC(bool);
		void swStabilityController(bool);
		void swTurnController(bool);
		void swSetHeight(bool);
		void swSetRudder(bool);

	private:

		HardwareSerial* _serial_tx;

		uint16_t	_voltage;
		uint16_t	_current;
		uint16_t	_velocity;
		uint16_t	_rpm;
		uint8_t		_height;
		uint8_t		_temperature;
		uint8_t		_rssi;
		uint8_t		_pwm;
		uint8_t   _switches;
		int8_t		_rudder_angle;

		char 	payload[PAYLOAD_SIZE];

};


#define READ_MS 200

class HydroFPV_Rx {

	public:

		HydroFPV_Rx(HardwareSerial&);

		void begin(uint32_t);
		void readPayload(void);
		float getVoltage(void);
		float getCurrent(void);
		float getVelocity(void);
		float getHeight(void);
		int getRPM(void);
		int getTemperature(void);
		int getRSSI(void);
		int getPWM(void);
		int getRudderAngle(void);
		bool get_swChrono(void);
		bool get_swReverse(void);
		bool get_swPump(void);
		bool get_swESC(void);
		bool get_swStabilityController(void);
		bool get_swTurnController(void);
		bool get_swSetHeight(void);
		bool get_swSetRudder(void);

	private:

		HardwareSerial* _serial;

		byte payload[PAYLOAD_SIZE];
		bool new_recv_data;

		float	_voltage;
		float	_current;
		float	_velocity;
		float	_rpm;
		uint8_t		_height;
		uint8_t		_temperature;
		uint8_t		_rssi;
		uint8_t		_pwm;
		int8_t		_rudder_angle;
		bool			_sw_crono;
		bool			_sw_reverse;
		bool			_sw_pump;
		bool			_sw_esc;
		bool 			_sw_stability_controller;
		bool 			_sw_turn_controller;
		bool 			_sw_set_height;
		bool 			_sw_set_rudder;

};


#endif
