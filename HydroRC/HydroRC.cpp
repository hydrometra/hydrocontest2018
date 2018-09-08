/*
	HydroRC.cpp
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#include "HydroRC.h"


void HydroRC::pwmEnableCh4(bool enable) {

  _ch4pwm = true;

}


void HydroRC::begin(void) {

  _ch4pwm = false;

  for (byte i = 0; i < NUM_CHS; ++i) {
		_channels[i]      = 0;
	}

	_goodFrames         = 0;
	_lostFrames         = 0;
	_decoderErrorFrames = 0;
	_failsafe           = SBUS_FAILSAFE_INACTIVE;
  _pump_state         = false;

	_serial.begin(100000, SERIAL_8E2);

}


void HydroRC::debug(void) {

  for (int i = 0; i < NUM_CHS; ++i) {
    Serial.print(getChannel(i +1));
    Serial.print('\t');
  } Serial.println();

}


void HydroRC::process(void) {

  static byte buffer[25];
	static byte buffer_index = 0;

	while (_serial.available()) {

		byte rx = _serial.read();

		if (buffer_index == 0 && rx != SBUS_STARTBYTE) { // Incorrect start byte, out of sync

			_decoderErrorFrames++;
			continue;

		}

		buffer[buffer_index++] = rx;

		if (buffer_index == 25) {
			buffer_index = 0;

			if (buffer[24] != SBUS_ENDBYTE) { // Incorrect end byte, out of sync

				_decoderErrorFrames++;
				continue;

			}

			_goodFrames++;

			_channels[0]  = ((buffer[1]     | buffer[2]<<8)                    & 0x07FF);
			_channels[1]  = ((buffer[2]>>3  | buffer[3]<<5)                    & 0x07FF);
			_channels[2]  = ((buffer[3]>>6  | buffer[4]<<2 | buffer[5]<<10)    & 0x07FF);
			_channels[3]  = ((buffer[5]>>1  | buffer[6]<<7)                    & 0x07FF);
			_channels[4]  = ((buffer[6]>>4  | buffer[7]<<4)                    & 0x07FF);
			_channels[5]  = ((buffer[7]>>7  | buffer[8]<<1 | buffer[9]<<9)     & 0x07FF);
			_channels[6]  = ((buffer[9]>>2  | buffer[10]<<6)                   & 0x07FF);
			_channels[7]  = ((buffer[10]>>5 | buffer[11]<<3)                   & 0x07FF);
			_channels[8]  = ((buffer[12]    | buffer[13]<<8)                   & 0x07FF);
			_channels[9]  = ((buffer[13]>>3 | buffer[14]<<5)                   & 0x07FF);
			_channels[10] = ((buffer[14]>>6 | buffer[15]<<2 | buffer[16]<<10)  & 0x07FF);
			_channels[11] = ((buffer[16]>>1 | buffer[17]<<7)                   & 0x07FF);
			_channels[12] = ((buffer[17]>>4 | buffer[18]<<4)                   & 0x07FF);
			_channels[13] = ((buffer[18]>>7 | buffer[19]<<1 | buffer[20]<<9)   & 0x07FF);
			_channels[14] = ((buffer[20]>>2 | buffer[21]<<6)                   & 0x07FF);
			_channels[15] = ((buffer[21]>>5 | buffer[22]<<3)                   & 0x07FF);

			((buffer[23])      & 0x0001) ? _channels[16] = 2047: _channels[16] = 0;
			((buffer[23] >> 1) & 0x0001) ? _channels[17] = 2047: _channels[17] = 0;

			if ((buffer[23] >> 3) & 0x0001) {
				_failsafe = SBUS_FAILSAFE_ACTIVE;
			}
      else {
				_failsafe = SBUS_FAILSAFE_INACTIVE;
			}

			if ((buffer[23] >> 2) & 0x0001) {
				_lostFrames++;
			}

			_lastGoodFrame = millis();

		}
	}
}


int HydroRC::getChannel(int channel) { // Range: 1000~2000

  static float val;
  static unsigned long last_pump_ms = 0;

	if (channel < 1 or channel > NUM_CHS) {
		return 0;
	}
  else {
    val = (_channels[channel - 1] - 990) * (500.0 / 819.0) + 1500;
    val = val > 2000 ? 2000 : val;
    val = val < 1000 ? 1000 : val;
    val = abs(val - 1500) < 2 ? 1500 : val;

    if (channel == CH_PUMP) {
      if (millis() - last_pump_ms >= 1000) {
        last_pump_ms = millis();
        if (!_pump_state && val > 1750) {
          _pump_state = true;
        }
        else if (_pump_state && val > 1750) {
          _pump_state = false;
        }
      }

      if (_pump_state) return 2000;
      else return 1500;

    }

		return val;
	}

}


int HydroRC::getFailsafeStatus() {
	return _failsafe;
}


int HydroRC::getFrameLoss() {
	return (int) ((_lostFrames + _decoderErrorFrames) * 100 / (_goodFrames + _lostFrames + _decoderErrorFrames));
}


long HydroRC::getGoodFrames() {
	return _goodFrames;
}


long HydroRC::getLostFrames() {
	return _lostFrames;
}


long HydroRC::getDecoderErrorFrames() {
	return _decoderErrorFrames;
}


long long HydroRC::getLastTime() {
	return _lastGoodFrame;
}
