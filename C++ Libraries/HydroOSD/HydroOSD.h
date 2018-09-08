/*
	HydroOSD.h
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#ifndef HYDROOSD_h
#define HYDROOSD_h

#include "Arduino.h"

#define SCREEN_BUF_SZ 480

class HydroOSD {

	public:

		HydroOSD(void);

		void begin(void);
		void writeScreen(void);

		void putVoltage(float);
		void putCurrent(float);
		void putPower(float);
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
		void swSetRudder(bool);
		void swSetHeight(bool);

	private:

		void spiTransfer(byte, byte);
		byte convert_dec(int);
		void chrono(bool);

		byte 		screen_buffer[SCREEN_BUF_SZ];
		uint8_t sec;
		uint8_t min;

		bool 		chrono_running;

};

#endif
