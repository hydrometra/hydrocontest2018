/*

	HydroTube - v 1.0
	Library for measuring the velocity of the boat with respect to
	the water using a pitot tube and a differential	pressure sensor.
	For best results, a moving average filter was implemented.

	Created by Nicolás Villegas E. and David Escudero - 2018.
	Hydrómetra Team - National University of Colombia
	(nvillegase AT unal.edu.co)

	Released into the public domain: spread the Hydrocontest spirit.

	Sensor: MPX5100DP

*/

#ifndef HydroTube_h
#define HydroTube_h

#include "Arduino.h"

class HydroTube {

	public:

		HydroTube(uint8_t, int16_t);

		int16_t 	getRawMeasurement(void);
		float 		getPressure(void);
		float 		getVelocity(void);
		void  		update(void);
		void  		initialize(void);

	private:

		uint8_t		_analogPin;
		int32_t 	_sum;
		double 		_average;
		int16_t		_position;
		int16_t		*_array;
		int8_t 		_length;
		int16_t 	_latest_measurement;
		int16_t		_offset;

		int16_t 	_getOffset(uint16_t);
		float 		mapf(float, int16_t, int16_t, int16_t, int16_t);

};

#endif
