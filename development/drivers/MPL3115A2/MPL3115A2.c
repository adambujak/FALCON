/****************************************************************************
* Copyright (C) 2015 Sensorian
*                                                                          *
* This file is part of Sensorian.                                          *
*                                                                          *
*   Sensorian is free software: you can redistribute it and/or modify it   *
*   under the terms of the GNU Lesser General Public License as published  *
*   by the Free Software Foundation, either version 3 of the License, or   *
*   (at your option) any later version.                                    *
*                                                                          *
*   Sensorian is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU Lesser General Public License for more details.                    *
*                                                                          *
*   You should have received a copy of the GNU Lesser General Public       *
*   License along with Sensorian.                                          *
*   If not, see <http://www.gnu.org/licenses/>.                            *
****************************************************************************/

#include "MPL3115A2.h"

void MPL3115A2_Initialize(void)
{
  MPL3115A2_StandbyMode();
  MPL3115A2_WriteByte(CTRL_REG1, OS_128);
  MPL3115A2_WriteByte(PT_DATA_CFG, DREM | PDEFE | TDEFE);     //Enable data ready flags for pressure and temperature )
  MPL3115A2_WriteByte(CTRL_REG1, OS_128 | SBYB);              //Set sensor to active state with oversampling ratio 128 (512 ms between samples)
}

unsigned char MPL3115A2_ID(void)
{
  unsigned char id = MPL3115A2_ReadByte(WHO_AM_I);

  return id;
}

unsigned char MPL3115A2_GetMode(void)
{
  unsigned char status = MPL3115A2_ReadByte(SYSMOD);

  return (status & 0x01) ? 1 : 0;
}

void MPL3115A2_StandbyMode(void)
{
  unsigned char ctrl_reg = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  ctrl_reg &= ~SBYB;                                      //Clear SBYB bit for Standby mode
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);               //Put device in Standby mode
}

void  MPL3115A2_ActiveMode(void)
{
  unsigned char tempSetting = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  tempSetting |= SBYB;                                       //Set SBYB bit for Active mode
  MPL3115A2_WriteByte(CTRL_REG1, tempSetting);
}

void MPL3115A2_AltimeterMode(void)
{
  unsigned char ctrl_reg = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  ctrl_reg &= ~(SBYB);                                    //Go to Standby mode
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);

  ctrl_reg = ALT | OS_128 | SBYB;                    //Set ALT bit to one and enable back Active mode
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);
}

float MPL3115A2_ReadAltitude(void)
{
  unsigned char status = MPL3115A2_ReadByte(STATUS);
  uint8_t altpbyte[3] = { 0x00 };

  //Check PDR bit, if it's not set then toggle OST
  if ((status & PDR) == 0) {
    MPL3115A2_ToggleOneShot();   //Toggle the OST bit causing the sensor to immediately take another reading
  }

  MPL3115A2_ReadByteArray(OUT_P_MSB, altpbyte, 3);       //Read altitude data
  int m_altitude = altpbyte[0];
  int c_altitude = altpbyte[1];
  int l_altitude = altpbyte[2];
  uint32_t al = (m_altitude << 24) | (c_altitude << 16) | (l_altitude << 8);

  return (float) (al / 65536.f);
}

void MPL3115A2_SetAltimeterOffset(unsigned char H_Offset)
{
  if ((H_Offset > -128) || (H_Offset < 128)) {
    MPL3115A2_WriteByte(OFF_H, H_Offset);
  }
}

void MPL3115A2_BarometerMode(void)
{
  unsigned char ctrl_reg = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  ctrl_reg &= ~(SBYB);                                    //Set SBYB to 0 and go to Standby mode
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);

  ctrl_reg = OS_128;                      //Set ALT bit to zero and enable back Active mode
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);
}

void MPL3115A2_SetPressureOffset(unsigned char P_Offset)
{
  if ((P_Offset > -128) || (P_Offset < 128)) {
    MPL3115A2_WriteByte(OFF_P, P_Offset);
  }
}

float MPL3115A2_GetMinimumPressure(void)
{
  uint8_t minPressure[3] = { 0x00 };

  MPL3115A2_ReadByteArray(P_MIN_MSB, minPressure, 3);

  unsigned char m_altitude = minPressure[0];
  unsigned char c_altitude = minPressure[1];
  float l_altitude = (float) (minPressure[2] >> 4) / 4;             //dividing by 4, since two lowest bits are fractional value

  return (float) (m_altitude << 10 | c_altitude << 2) + l_altitude; //shifting 2 to the left to make room for LSB
}

float MPL3115A2_GetMaximumPressure(void)
{
  uint8_t maxPressure[3] = { 0x00 };

  MPL3115A2_ReadByteArray(P_MAX_MSB, maxPressure, 3);

  unsigned char m_altitude = maxPressure[0];
  unsigned char c_altitude = maxPressure[1];
  float l_altitude = (float) (maxPressure[2] >> 4) / 4;             //dividing by 4, since two lowest bits are fractional value

  return (float) (m_altitude << 10 | c_altitude << 2) + l_altitude; //shifting 2 to the left to make room for LSB
}

unsigned int MPL3115A2_ReadBarometicPressureInput(void)
{
  unsigned char barMSB = MPL3115A2_ReadByte(BAR_IN_MSB);
  unsigned char barLSB = MPL3115A2_ReadByte(BAR_IN_LSB);

  return (barMSB << 8) | barLSB;
}

float MPL3115A2_ReadBarometricPressure(void)
{
  unsigned char status = MPL3115A2_ReadByte(STATUS);
  uint8_t pbyte[3] = { 0x00 };

  //Check PDR bit, if it's not set then toggle OST
  if ((status & PDR) == 0) {
    MPL3115A2_ToggleOneShot();   //Toggle the OST bit causing the sensor to immediately take another reading
  }

  MPL3115A2_ReadByteArray(OUT_P_MSB, pbyte, 3);

  unsigned long m_altitude = pbyte[0];
  unsigned long c_altitude = pbyte[1];
  unsigned long l_altitude = pbyte[2];

  uint32_t pressure = (m_altitude << 16) | (c_altitude << 8) | (l_altitude);

  return (float) (pressure / 64.f);
}

float MPL3115A2_ReadPressure(unitsType units)
{
  float pressure = MPL3115A2_ReadBarometricPressure();

  switch (units) {
  case PSI:
    return pressure * 0.000145037738;

  case INHG:
    return pressure * 0.00029529983071;

  case MMHG:
    return pressure * 0.007500615613;
  }
  return pressure;
}

void MPL3115A2_SetPressureAlarmThreshold(unsigned int thresh)
{
  unsigned char threshMSB = (unsigned char) (thresh << 8);
  unsigned char threshLSB = (unsigned char) thresh;

  MPL3115A2_WriteByte(P_TGT_MSB, threshMSB);
  MPL3115A2_WriteByte(P_TGT_LSB, threshLSB);
}

void MPL3115A2_SetPressureTargetWindow(unsigned int target, unsigned int window)
{
  unsigned char tmpMSB = (unsigned char) (window << 8);
  unsigned char tmpLSB = (unsigned char) (window);

  MPL3115A2_WriteByte(P_WND_MSB, tmpMSB);
  MPL3115A2_WriteByte(P_WND_LSB, tmpLSB);

  tmpMSB = (unsigned char) (target << 8);
  tmpLSB = (unsigned char) (target);
  MPL3115A2_WriteByte(P_WND_MSB, tmpMSB);
  MPL3115A2_WriteByte(P_WND_LSB, tmpLSB);
}

float MPL3115A2_ReadTemperature(void)
{
  uint8_t temperature[2] = { 0x00 };

  MPL3115A2_ReadByteArray(OUT_T_MSB, temperature, 2);

  float templsb = (temperature[1] >> 4) / 16.0;             //temp, fraction of a degree
  float unpacked = (float) (temperature[0] + templsb);

  return unpacked;
}

float MPL3115A2_GetMinimumTemperature(void)
{
  uint8_t temperature[2] = { 0x00 };

  MPL3115A2_ReadByteArray(T_MIN_MSB, temperature, 2);

  float templsb = (temperature[1] >> 4) / 16.0;             //temp, fraction of a degree
  float minTemp = (float) (temperature[0] + templsb);

  return minTemp;
}

float MPL3115A2_GetMaximumTemperature(void)
{
  uint8_t temperature[2] = { 0x00 };

  MPL3115A2_ReadByteArray(T_MAX_MSB, temperature, 2);

  float templsb = (temperature[1] >> 4) / 16.0;             //temp, fraction of a degree
  float maxTemp = (float) (temperature[0] + templsb);

  return maxTemp;
}

void MPL3115A2_SetTempTargetWindow(unsigned int target, unsigned int window)
{
  MPL3115A2_WriteByte(T_TGT, target);
  MPL3115A2_WriteByte(T_WND, window);
}

void MPL3115A2_SetTemperatureThreshold(unsigned char thresh)
{
  MPL3115A2_WriteByte(CTRL_REG4, INT_EN_TTH);      //enable temperature interrupt
  MPL3115A2_WriteByte(CTRL_REG5, INT_EN_TTH);      //map to interrupt
}

void MPL3115A2_SetTempOffset(char T_Offset)
{
  MPL3115A2_WriteByte(OFF_T, T_Offset);
}

void MPL3115A2_OutputSampleRate(unsigned char sampleRate)
{
  unsigned char ctrl_reg = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  ctrl_reg &= ~(SBYB);
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);               //Put sensor in Standby mode

  if (sampleRate > 7) sampleRate = 7;                     //OS cannot be larger than 0b.0111
  sampleRate <<= 3;                                       //Align it for the CTRL_REG1 register

  ctrl_reg = MPL3115A2_ReadByte(CTRL_REG1);               //Read current settings
  ctrl_reg &= ~(0x38);                                    //Clear current sample rate
  ctrl_reg |= sampleRate;                                 //Mask in new Output Sample bits
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);               //Update sample rate settings
  MPL3115A2_ActiveMode();
}

void MPL3115A2_SetAcquisitionTimeStep(unsigned char ST_Value)
{
  unsigned char ctrl_reg = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  ctrl_reg &= ~(1 << SBYB);
  MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg);               //Put sensor in Standby mode

  if (ST_Value <= 0xF) {
    ctrl_reg = MPL3115A2_ReadByte(CTRL_REG2);          //Read current settings
    MPL3115A2_WriteByte(CTRL_REG1, ctrl_reg | ST_Value);
  }
}

void MPL3115A2_EnableEventFlags(void)
{
  MPL3115A2_WriteByte(PT_DATA_CFG, 0x07);   // Enable all three pressure and temp event flags
}

void MPL3115A2_ToggleOneShot(void)
{
  unsigned char tempSetting = MPL3115A2_ReadByte(CTRL_REG1); //Read current settings

  tempSetting &= ~(1 << 1);                                  //Clear OST bit
  MPL3115A2_WriteByte(CTRL_REG1, tempSetting);
  tempSetting = MPL3115A2_ReadByte(CTRL_REG1);               //Read current settings to be safe
  tempSetting |= (1 << 1);                                   //Set OST bit
  MPL3115A2_WriteByte(CTRL_REG1, tempSetting);
}

void MPL3115A2_ConfigureInterruptPin(unsigned char intrrpt, unsigned char pin)
{
  MPL3115A2_WriteByte(CTRL_REG5, (pin << intrrpt));
}

void MPL3115A2_ConfigurePressureInterrupt(void)
{
  MPL3115A2_WriteByte(CTRL_REG3, PP_OD1 | PP_OD2);          //Configure Interrupt pins for open drain active low
}

void MPL3115A2_ConfigureAltitudeInterrupt(void)
{
  MPL3115A2_WriteByte(CTRL_REG3, PP_OD1 | PP_OD2);    //Configure Interrupt pins for open drain active low
}

void MPL3115A2_ClearInterrupts(void)
{
  MPL3115A2_ReadByte(OUT_P_MSB);
  MPL3115A2_ReadByte(OUT_P_CSB);          //Configure Interrupt pins for open drain active low
  MPL3115A2_ReadByte(OUT_P_LSB);
  MPL3115A2_ReadByte(OUT_T_MSB);
  MPL3115A2_ReadByte(OUT_T_LSB);
  MPL3115A2_ReadByte(F_STATUS);
}
