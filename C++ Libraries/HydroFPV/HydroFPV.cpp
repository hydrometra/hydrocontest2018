/*
	HydroFPV.cpp
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#include "HydroFPV.h"


/*
	-------------------------- HydroFPV_Tx ------------------------------
*/


HydroFPV_Tx::HydroFPV_Tx(HardwareSerial &serial) {

	_serial_tx		= &serial;
	_voltage			= 0;
	_current			= 0;
	_velocity			= 0;
	_rpm					= 0;
	_height 			= 0;
	_temperature	= 0;
	_rssi					= 0;
	_pwm					= 0;
	_rudder_angle	= 0;

	for (int i = 0; i < PAYLOAD_SIZE; ++i) {
		payload[i] = 0;
	}

}


void HydroFPV_Tx::begin(uint32_t baudrate) {
	static_cast<HardwareSerial*>(_serial_tx)->begin(baudrate);
}


void HydroFPV_Tx::putVoltage(float voltage) {
	_voltage = voltage * 100;
	payload[0] = _voltage & 0xFF;
	payload[1] = _voltage >> 8;
}


void HydroFPV_Tx::putCurrent(float current) {
	_current = current * 100;
	payload[2] = _current & 0xFF;
	payload[3] = _current >> 8;
}


void HydroFPV_Tx::putVelocity(float velocity) {
	_velocity = uint16_t(velocity * 100.0);
	payload[4] = _velocity & 0xFF;
	payload[5] = _velocity >> 8;
}


void HydroFPV_Tx::putRPM(int rpm) {
	_rpm = rpm;
	payload[6] = _rpm & 0xFF;
	payload[7] = _rpm >> 8;
}


void HydroFPV_Tx::putHeight(float height) { // [m] [0.00 ~ 1.20]
	_height = uint8_t(height * 100);
	payload[8] = _height;
}


void HydroFPV_Tx::putTemperature(int temperature) {
	_temperature = uint8_t(temperature);
	payload[9] = _temperature;
}


void HydroFPV_Tx::putRSSI(int rssi) {
	_rssi = uint8_t(rssi);
	payload[10] = _rssi;
}


void HydroFPV_Tx::putPWM(int pwm) {
	_pwm = uint8_t(pwm);
	payload[11] = _pwm;
}


void HydroFPV_Tx::putRudderAngle(int rudder_angle) {
	_rudder_angle = int8_t(rudder_angle);
	payload[12] = _rudder_angle;
}


void HydroFPV_Tx::swChrono(bool chrono) {

	if (chrono) {
		payload[13] = payload[13] | 0x08;
	}
	else {
		payload[13] = payload[13] & 0xF7;
	}

}


void HydroFPV_Tx::swReverse(bool reverse) {

	if (reverse) {
		payload[13] = payload[13] | 0x04;
	}
	else {
		payload[13] = payload[13] & 0xFB;
	}

}


void HydroFPV_Tx::swPump(bool pump) {

	if (pump) {
		payload[13] = payload[13] | 0x02;
	}
	else {
		payload[13] = payload[13] & 0xFD;
	}

}


void HydroFPV_Tx::swESC(bool esc) {

	if (esc) {
		payload[13] = payload[13] | 0x01;
	}
	else {
		payload[13] = payload[13] & 0xFE;
	}

}


void HydroFPV_Tx::swStabilityController(bool sw) {

	if (sw) {
		payload[13] = payload[13] | 0x10;
	}
	else {
		payload[13] = payload[13] & 0xEF;
	}

}


void HydroFPV_Tx::swTurnController(bool sw) {

	if (sw) {
		payload[13] = payload[13] | 0x20;
	}
	else {
		payload[13] = payload[13] & 0xDF;
	}

}


void HydroFPV_Tx::swSetHeight(bool sw) {

	if (sw) {
		payload[13] = payload[13] | 0x40;
	}
	else {
		payload[13] = payload[13] & 0xBF;
	}

}


void HydroFPV_Tx::swSetRudder(bool sw) {

	if (sw) {
		payload[13] = payload[13] | 0x80;
	}
	else {
		payload[13] = payload[13] & 0x7F;
	}

}

void HydroFPV_Tx::writeScreen(void) {

	static uint64_t _last_ms = 0;

	if (millis() - _last_ms >= PRINT_MS) {

		_last_ms = millis();
		_serial_tx->write(0xAA);

		for (int i = 0; i < PAYLOAD_SIZE; ++i) {
			_serial_tx->write(payload[i]);
		}

		_serial_tx->write(0xBB);

	}

}


/*
	-------------------------- HydroFPV_Rx ------------------------------
*/


HydroFPV_Rx::HydroFPV_Rx(HardwareSerial &serial) {

	_serial 			= &serial;
	_voltage			= 0;
	_current			= 0;
	_velocity			= 0;
	_rpm					= 0;
	_height 			= 0;
	_temperature	= 0;
	_rssi					= 0;
	_pwm					= 0;
	_rudder_angle	= 0;
	_sw_crono			= false;
	_sw_reverse		= false;
	_sw_pump			= false;
	_sw_esc				= false;

	_sw_stability_controller = false;
	_sw_turn_controller 		 = false;
	_sw_set_height					 = false;
	_sw_set_rudder 					 = false;

	new_recv_data = false;

	for (int i = 0; i < PAYLOAD_SIZE; ++i) {
		payload[i] = 0;
	}

}


void HydroFPV_Rx::begin(uint32_t baudrate) {
	static_cast<HardwareSerial*>(_serial)->begin(baudrate);
}


void HydroFPV_Rx::readPayload(void) {

	static boolean recvInProgress = false;
	static byte ndx = 0;

	const byte startMarker = 0xAA;
	const byte endMarker = 0xBB;

	static byte rb;

	while (_serial->available() > 0 && new_recv_data == false) {

    rb = _serial->read();

    if (recvInProgress == true) {
      if (rb != endMarker) {
        payload[ndx] = rb;
        ndx++;
        if (ndx >= PAYLOAD_SIZE) {
            ndx = PAYLOAD_SIZE - 1;
        }
      }
      else {
        recvInProgress = false;
        ndx = 0;
        new_recv_data = true;
      }
    }

    else if (rb == startMarker) {
      recvInProgress = true;
    }

  }

	if (new_recv_data) {

		_voltage			= (payload[0] + (payload[1] << 8)) / 100.0;
		_current			= (payload[2] + (payload[3] << 8)) / 100.0;
		_velocity			= (payload[4] + (payload[5] << 8)) / 100.0;
		_rpm					= payload[6] + (payload[7] << 8);
		_height 			= payload[8];
		_temperature	= payload[9];
		_rssi					= payload[10];
		_pwm					= payload[11];
		_rudder_angle	= payload[12];
		_sw_crono			= payload[13] & 0x08;
		_sw_reverse		= payload[13] & 0x04;
		_sw_pump			= payload[13] & 0x02;
		_sw_esc				= payload[13] & 0x01;

		_sw_stability_controller = payload[13] & 0x10;
		_sw_turn_controller      = payload[13] & 0x20;
		_sw_set_height	 				 = payload[13] & 0x40;
		_sw_set_rudder 					 = payload[13] & 0x80;


		new_recv_data = false;

	}

}


float HydroFPV_Rx::getVoltage(void) {
	return _voltage;
}


float HydroFPV_Rx::getCurrent(void) {
	return _current;
}


float HydroFPV_Rx::getVelocity(void) {
	return _velocity;
}


float HydroFPV_Rx::getHeight(void) {
	return _height;
}


int HydroFPV_Rx::getRPM(void) {
	return _rpm;
}


int HydroFPV_Rx::getTemperature(void) {
	return _temperature;
}


int HydroFPV_Rx::getRSSI(void) {
	return _rssi;
}


int HydroFPV_Rx::getPWM(void) {
	return _pwm;
}


int HydroFPV_Rx::getRudderAngle(void) {
	return _rudder_angle;
}


bool HydroFPV_Rx::get_swChrono(void) {
	return _sw_crono;
}


bool HydroFPV_Rx::get_swReverse(void) {
	return _sw_reverse;
}


bool HydroFPV_Rx::get_swPump(void) {
	return _sw_pump;
}


bool HydroFPV_Rx::get_swESC(void) {
	return _sw_esc;
}


bool HydroFPV_Rx::get_swStabilityController(void) {
	return _sw_stability_controller;
}


bool HydroFPV_Rx::get_swTurnController(void) {
	return _sw_turn_controller;
}


bool HydroFPV_Rx::get_swSetHeight(void) {
	return _sw_set_height;
}


bool HydroFPV_Rx::get_swSetRudder(void) {
	return _sw_set_rudder;
}
