/*
	HydroOSD.cpp
	Created by Nicolás Villegas E., 2018.
	(nvillegase AT unal.edu.co)
	Hydrómetra Team - National University of Colombia
	Released into the public domain: spread the Hydrocontest spirit.
*/

#include "HydroOSD.h"

enum letter {
  LT_A = 65,  LT_B,     LT_C,
  LT_D,       LT_E,     LT_F,
  LT_G,       LT_H,     LT_I,
  LT_J,       LT_K,     LT_L,
  LT_M,       LT_N,     LT_O,
  LT_P,       LT_Q,     LT_R,
  LT_S,       LT_T,     LT_U,
  LT_V,       LT_W,     LT_X,
  LT_Y,       LT_Z
};

HydroOSD::HydroOSD(void) {

  chrono_running = false;

  /* Logo Hydrómetra */
  screen_buffer[0] = 0x70;    screen_buffer[1] = 0x71;    screen_buffer[2] = 0x72;    screen_buffer[3] = 0x73;
  screen_buffer[4] = 0x74;    screen_buffer[5] = 0x75;    screen_buffer[31] = 0x76;   screen_buffer[32] = 0x77;
  screen_buffer[33] = 0x78;   screen_buffer[34] = 0x79;   screen_buffer[35] = 0x7A;   screen_buffer[36] = 0x7B;
  screen_buffer[60] = 0x7C;   screen_buffer[61] = 0x7D;   screen_buffer[62] = 0x7E;   screen_buffer[63] = 0x7F;
  screen_buffer[64] = 0x80;   screen_buffer[65] = 0x81;   screen_buffer[66] = 0x82;   screen_buffer[67] = 0x83;

  /* Crosshair */
  screen_buffer[224] = 0xB2;
  screen_buffer[226] = 0xB2;

}


void HydroOSD::begin(void) {

  pinMode(6, OUTPUT);                // To SC of MAX7456
  pinMode(10, OUTPUT);               // Not SS of the ATmega328. Needs to be declared as output to get the SPI to work
  pinMode(11, OUTPUT);               // To DIN of MAX7456
  pinMode(13, OUTPUT);               // To SCK of MAX7456

  digitalWrite(6,HIGH);              // Disable device
  SPCR = (1<<SPE) | (1<<MSTR);       // Set SPI enable and SPI Master bit
  delay(250);

  /*Reset the MAX7456*/
  digitalWrite(6,LOW);               // Enable device
  spiTransfer(0x00, 0x42);           // Write 0b01000010 (PAL & software reset bits) to register h00 (Video Mode 0)
  digitalWrite(6, HIGH);             // disable device
  delay(250);                        // Wait for the MAX7456 to complete reset

  digitalWrite(6, LOW);              // Enable device

  spiTransfer(0x02, 0x30);           // Horizontal position offset: 0x20 = no correction, min = 0x00 (-32 pixels), max = 0x3F (+31 pixels)
  spiTransfer(0x03, 0x16);           // Vertical position offset: 0x10 = no correction, min = 0x00 (+16 pixels), max = 0x1F (-15 pixels)

  /*Enable MAX7456*/
  spiTransfer(0x00, 0x48);           // Write 0b01001000 (PAL & enable OSD) to register h00 (Video Mode 0)
  digitalWrite(6, HIGH);             // disable device

}


void HydroOSD::spiTransfer(byte address, byte data) {

  SPDR = address;                   // Load SPI data register
  while (!(SPSR & (1<<SPIF)));      // Wait until the byte is send
  SPDR = data;                      // Load SPI data register
  while (!(SPSR & (1<<SPIF)));      // Wait until the byte is send

}


void HydroOSD::chrono(bool chr) {

  static unsigned long t_start = 0;

  if (chr) {

    if (!chrono_running) {
      min = 0;
      sec = 0;
      chrono_running = true;
      t_start = millis();
    }
    else {
      sec = (millis() - t_start) / 1000;
      min = sec / 60;
      sec = sec % 60;
    }

  }

  else {

    chrono_running = false;

  }

}


void HydroOSD::writeScreen(void) {

  /*
  digitalWrite(6,LOW);      // Enable the MAX7456
  spiTransfer(0x04, 0x04);  // Clear the display
  digitalWrite(6,HIGH);     // Disable the MAX7456
  */

  digitalWrite(6, LOW);     // Enable the MAX7456
  spiTransfer(0x04, 0x01);  // Auto increment mode for writing the display characters
  spiTransfer(0x05, 0x00);  // Set start address high to the position 0
  spiTransfer(0x06, 0x00);  // Set start address low to the position 0

  for (int i = 0; i < SCREEN_BUF_SZ; ++i) {
    spiTransfer(0x07, screen_buffer[i]);
  }

  spiTransfer(0x07, 0xFF);   // Disable the auto increment mode
  spiTransfer(0x00, 0x4C);   // Turn on the on screen display
  digitalWrite(6, HIGH);     // Disable the MAX7456

}


byte HydroOSD::convert_dec(int dec) {

  switch(dec) {
    case 0:
      return 0x30;
    case 1:
      return 0x31;
    case 2:
      return 0x32;
    case 3:
      return 0x33;
    case 4:
      return 0x34;
    case 5:
      return 0x35;
    case 6:
      return 0x36;
    case 7:
      return 0x37;
    case 8:
      return 0x38;
    case 9:
      return 0x39;
  }

}


void HydroOSD::putVoltage(float volt) {

  int _volt = volt * 10;

  screen_buffer[23] = (_volt / 100) == 0 ? 0x00 : convert_dec(_volt / 100);
  _volt -= (_volt / 100) * 100;
  screen_buffer[24] = convert_dec(_volt / 10);
  screen_buffer[25] = 0x2E; // dot (.)
  screen_buffer[26] = convert_dec(_volt % 10);
  screen_buffer[27] = LT_V;

}


void HydroOSD::putCurrent(float curr) {

  int _curr = curr * 10;

  screen_buffer[53] = (_curr / 100) > 0 ? convert_dec(_curr / 100) : 0x00;
  _curr -= (_curr / 100) * 100;
  screen_buffer[54] = convert_dec(_curr / 10);
  screen_buffer[55] = 0x2E; // dot (.)
  screen_buffer[56] = convert_dec(_curr % 10);
  screen_buffer[57] = LT_A;

}


void HydroOSD::putPower(float pow) {

  int _pow = pow;

  screen_buffer[83] = (_pow / 1000) > 0 ? convert_dec(_pow / 1000) : 0x00;
  _pow -= (_pow / 1000) * 1000;
  screen_buffer[84] = (_pow / 100) > 0 ? convert_dec(_pow / 100) : 0x00;
  _pow -= (_pow / 100) * 100;
  screen_buffer[85] = convert_dec(_pow / 10);
  screen_buffer[86] = convert_dec(_pow % 10);
  screen_buffer[87] = LT_W;

}


void HydroOSD::putVelocity(float vel) {

  int _vel = vel * 100;

  screen_buffer[390] = convert_dec(_vel / 100);
  screen_buffer[391] = 0x2E; // dot (.)
  _vel -= (_vel / 100) * 100;
  screen_buffer[392] = convert_dec(_vel / 10);
  screen_buffer[393] = convert_dec(_vel % 10);
  screen_buffer[394] = 0x9F;

}


void HydroOSD::putHeight(float h) {

  int _h = h;

  screen_buffer[403] = (_h / 100) > 0 ? convert_dec(_h / 100) : 0x00;
  _h -= (_h / 100) * 100;
  screen_buffer[404] = convert_dec(_h / 10);
  screen_buffer[405] = convert_dec(_h % 10);
  screen_buffer[406] = 0xCC;

}


void HydroOSD::putRPM(int rpm) {

  int _rpm = rpm;

  screen_buffer[420] = convert_dec(_rpm / 1000);
  _rpm -= (_rpm / 1000) * 1000;
  screen_buffer[421] = convert_dec(_rpm / 100);
  _rpm -= (_rpm / 100) * 100;
  screen_buffer[422] = convert_dec(_rpm / 10);
  screen_buffer[423] = convert_dec(_rpm % 10);
  screen_buffer[424] = 0xE1;

}


void HydroOSD::putTemperature(int T) {

  int _T = T;

  screen_buffer[415] = convert_dec(_T / 10);
  screen_buffer[416] = convert_dec(_T % 10);
  screen_buffer[417] = 0x0E;

}


void HydroOSD::putRSSI(int rssi) {

  int _rssi = rssi;

  screen_buffer[13] = (_rssi / 100) > 0 ? convert_dec(_rssi / 100) : 0x00;
  _rssi -= (_rssi / 100) * 100;
  screen_buffer[14] = (_rssi / 10) > 0 ? convert_dec(_rssi / 10) : 0x00;
  screen_buffer[15] = convert_dec(_rssi % 10);
  screen_buffer[16] = 0xBA;

}


void HydroOSD::putPWM(int pwm) {

  int _pwm = pwm;

  screen_buffer[43] = pwm / 100 > 0 ? convert_dec(pwm / 100) : 0x00;
  pwm -= (pwm / 100) * 100;
  screen_buffer[44] = convert_dec(pwm / 10);
  screen_buffer[45] = convert_dec(pwm % 10);
  screen_buffer[46] = 0xE3;

}


void HydroOSD::putRudderAngle(int angle) {

  int _angle = angle;

  if (_angle < 0) {
    screen_buffer[433] = 0x2D;
    _angle *= -1;
  }
  else {
    screen_buffer[433] = 0x00;
  }

  screen_buffer[434] = convert_dec(_angle / 10);
  screen_buffer[435] = convert_dec(_angle % 10);
  screen_buffer[436] = 0xBD;

}


void HydroOSD::swChrono(bool sw) {

  screen_buffer[236] = 0xEA;
  screen_buffer[237] = 0xEB;

  if (sw) {

    screen_buffer[235] = 0x0A;

    chrono(true);

    if (min / 100) {
      screen_buffer[440] = convert_dec(min / 100);
    }

    else {
      screen_buffer[440] = 0x00;
    }

    min -= (min / 100) * 100;

    screen_buffer[442] = convert_dec(min / 10);
    min -= (min / 10) * 10;
    screen_buffer[443] = convert_dec(min);

    if (sec % 2 == 0) {
      screen_buffer[444] = 0x63; // Dos puntos
    }
    else {
      screen_buffer[444] = 0x00;
    }

    screen_buffer[445] = convert_dec(sec / 10);
    screen_buffer[446] = convert_dec(sec % 10);
    screen_buffer[447] = 0x60;

  }

  else {
    chrono(false);
    screen_buffer[235] = 0x00;
  }

}


void HydroOSD::swReverse(bool sw) {

  screen_buffer[180] = 0xF2;
  screen_buffer[181] = 0xF3;

  if (sw) {
    screen_buffer[182] = 0x09;
  }
  else {
    screen_buffer[182] = 0x00;
  }

  if (sw && (millis() / 1000) % 2 == 0) {

    screen_buffer[162] = LT_R;
    screen_buffer[163] = LT_E;
    screen_buffer[164] = LT_V;
    screen_buffer[165] = LT_E;
    screen_buffer[166] = LT_R;
    screen_buffer[167] = LT_S;
    screen_buffer[168] = LT_A;

  }

  else {

    screen_buffer[162] = 0x00;
    screen_buffer[163] = 0x00;
    screen_buffer[164] = 0x00;
    screen_buffer[165] = 0x00;
    screen_buffer[166] = 0x00;
    screen_buffer[167] = 0x00;
    screen_buffer[168] = 0x00;

  }

}


void HydroOSD::swPump(bool sw) {

  screen_buffer[206] = 0xF0;
  screen_buffer[207] = 0xF1;

  if (sw) {
    screen_buffer[205] = 0x0A;
  }
  else {
    screen_buffer[205] = 0x00;
  }

}


void HydroOSD::swESC(bool sw) {

  screen_buffer[210] = 0xEC;
  screen_buffer[211] = 0xED;

  if (sw) {
    screen_buffer[212] = 0x09;
  }
  else {
    screen_buffer[212] = 0x00;
  }

}


void HydroOSD::swStabilityController(bool sw) {

  screen_buffer[240] = 0xF6;
  screen_buffer[241] = 0xF7;

  if (sw) {
    screen_buffer[242] = 0x09;
  }
  else {
    screen_buffer[242] = 0x00;
  }

}


void HydroOSD::swTurnController(bool sw) {

  screen_buffer[266] = 0xF8;
  screen_buffer[267] = 0xF9;

  if (sw) {
    screen_buffer[265] = 0x0A;
  }
  else {
    screen_buffer[265] = 0x00;
  }

}


void HydroOSD::swSetRudder(bool sw) {

  screen_buffer[78] = 0xF4;
  screen_buffer[79] = 0xF5;

  if (sw) {
    screen_buffer[80] = 0x09;
  }

  else {
    screen_buffer[80] = 0x00;
  }

}


void HydroOSD::swSetHeight(bool sw) {

  screen_buffer[70] = 0xEE;
  screen_buffer[71] = 0xEF;

  if (sw) {
    screen_buffer[72] = 0x09;
  }

  else {
    screen_buffer[72] = 0x00;
  }

}
