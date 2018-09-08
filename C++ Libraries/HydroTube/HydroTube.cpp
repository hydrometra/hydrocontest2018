/*

	HydroTube - v 1.0
	Library for measuring the velocity of the boat with respect to
	the water using a pitot tube and a differential	pressure sensor.
	For best results, a moving average filter was implemented.

	Created by Nicolás Villegas E. and David Escudero - 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia

	Released into the public domain: spread the Hydrocontest spirit.

	Sensor: MPX5100DP

*/

#include <HydroTube.h>

#define AREF								5000 	// Analog reference Arduino
#define MILLIVOLTS_PER_KPA 	45 		// from MPX5100DP datasheet
#define WATER_DENSITY 			1027 	// kg/m^3, 1027 in salt water, 1000 in fresh water.
#define DEFAULT_OFFSET 			41
#define SAMPLES_FOR_OFFSET 	200
#define MAX_OFFSET 					80


HydroTube::HydroTube(uint8_t analogPin, int16_t filterOrder) {

	_length			= filterOrder;
	_array			= new int16_t[_length];
	_analogPin 	= analogPin;

	pinMode(_analogPin, INPUT);

	_sum 			= 0;
	_average 	= 0;
	_position = 0;

}


void HydroTube::initialize(void) {

	_offset 	= _getOffset(SAMPLES_FOR_OFFSET);
	_offset 	= _offset > MAX_OFFSET ? DEFAULT_OFFSET : _offset;
	_sum 			= 0; // Just to make sure.
	_average 	= 0;
	_position	= 0;

}


void HydroTube::update(void) { // Moving average filter implementation.

	_latest_measurement	= analogRead(_analogPin) - _offset;
	_latest_measurement = _latest_measurement > 0 ? _latest_measurement : 0;
	_latest_measurement = _latest_measurement <= 1 ? 0 : _latest_measurement;
	_sum 								= _sum + _latest_measurement - _array[_position];
	_array[_position++] = _latest_measurement;
	_average 						= _sum / float(_length);

	if (_position >= _length) _position = 0;
	if (_sum < 0) _sum = 0;

}


int16_t HydroTube::getRawMeasurement(void) {
	return _latest_measurement;
}


float HydroTube::getPressure(void) { // kPA
	float millivolts = _average > 0 ? mapf(_average, 0, 1024, 0, AREF) : 0;
	return millivolts / float(MILLIVOLTS_PER_KPA);
}


float HydroTube::getVelocity(void) {
	float pressure = getPressure();
	float velocity_sq = 2000 * pressure / WATER_DENSITY;
	return sqrt(velocity_sq);
}


int16_t HydroTube::_getOffset(uint16_t nSamples) { // Use this function when the boat isn't moving to calculate sensor's offset.
	_sum = 0;
	for (uint16_t i = 0; i < nSamples; ++i) {
		_sum += analogRead(_analogPin);
		delay(1);
	}
	_offset = uint16_t(float(_sum) / float(nSamples));
	_sum    = 0;
	return _offset;
}


float HydroTube::mapf(float x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) {

	return (x - float(in_min)) * float(out_max - out_min) / float(in_max - in_min) + float(out_min);

}
