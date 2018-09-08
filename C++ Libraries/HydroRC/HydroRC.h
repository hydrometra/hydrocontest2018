/*
	HydroRC.h
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#ifndef HYDRORC_h
#define HYDRORC_h

#include "Arduino.h"

#define SBUS_FAILSAFE_INACTIVE 0
#define SBUS_FAILSAFE_ACTIVE   1
#define SBUS_STARTBYTE         0x0f
#define SBUS_ENDBYTE           0x00

#define NUM_CHS 12

#define CH_PROPULSION			1
#define CH_DIRECTION			2
#define CH_REVERSE				3
#define CH_ESC						4
#define CH_STABILITY_CNT	5
#define CH_TURN_CNT				6
#define CH_CHRONO					7
#define CH_PUMP 					8
#define CH_FPV						9
#define CH_SET_HEIGHT			10
#define CH_SET_RUDDER 		11
#define CH_DRIVE_MODE 		12

class HydroRC {

	public:

		HydroRC(HardwareSerial &serial) : _serial (serial) {};
		void pwmEnableCh4(bool);

		void 	begin();
		void 	process();
		void  debug();
		int 	getChannel(int channel);
		int 	getFailsafeStatus();
		int 	getFrameLoss();
		long 	getGoodFrames();
		long 	getLostFrames();
		long 	getDecoderErrorFrames();
		long long getLastTime();

	private:

		HardwareSerial & _serial;
		int _channels[NUM_CHS];
		int _failsafe;
		long _goodFrames;
		long _lostFrames;
		long _decoderErrorFrames;
		long long _lastGoodFrame;
		bool _pump_state;
		bool _ch4pwm;

};

#endif
