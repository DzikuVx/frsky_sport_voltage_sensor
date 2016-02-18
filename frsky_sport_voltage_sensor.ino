/*
  Alternative FrSky S-Port voltage sensor
  (c) Pawelsky 20160123
  Not for commercial use
*/

#include "FrSkySportSensor.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportTelemetry.h"
#include "SoftwareSerial.h"

#define LPF_FACTOR 0.75
#define LOOP_TIME 1000

FrSkySportSensorFlvss flvss;
FrSkySportTelemetry telemetry;

float cell1DividerMultiplier = (10000.0 + 3300.0) / 3300.0 * 1.1 / 1023.0;
float cell2DividerMultiplier = (10000.0 + 1500.0) / 1500.0 * 1.1 / 1023.0;
float cell3DividerMultiplier = (10000.0 +  910.0) /  910.0 * 1.1 / 1023.0;
float cell4DividerMultiplier = (10000.0 +  680.0) /  680.0 * 1.1 / 1023.0;
float cell5DividerMultiplier = (10000.0 +  510.0) /  510.0 * 1.1 / 1023.0;
float cell6DividerMultiplier = (10000.0 +  390.0) /  390.0 * 1.1 / 1023.0;

float cell1Voltage = 0.0;
float cell2Voltage = 0.0;
float cell3Voltage = 0.0;
float cell4Voltage = 0.0;
float cell5Voltage = 0.0;
float cell6Voltage = 0.0;

int rawCell1Voltage = 0;
int rawCell2Voltage = 0;
int rawCell3Voltage = 0;
int rawCell4Voltage = 0;
int rawCell5Voltage = 0;
int rawCell6Voltage = 0;

int smoothCell1Voltage = 0;
int smoothCell2Voltage = 0;
int smoothCell3Voltage = 0;
int smoothCell4Voltage = 0;
int smoothCell5Voltage = 0;
int smoothCell6Voltage = 0;

int smooth(int data, float filterVal, float smoothedVal) {

  if (filterVal > 1){      // check to make sure params are within range
    filterVal = .99;
  }
  else if (filterVal <= 0){
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  analogReference(INTERNAL);
  telemetry.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_4, &flvss);
}

void loop()
{

  rawCell1Voltage = analogRead(A5);
  rawCell2Voltage = analogRead(A4);
  rawCell3Voltage = analogRead(A3);
  rawCell4Voltage = analogRead(A2);
  rawCell5Voltage = analogRead(A0);
  rawCell6Voltage = analogRead(A1);

  if (smoothCell1Voltage == 0) {
    smoothCell1Voltage = rawCell1Voltage;
  } else {
    smoothCell1Voltage = smooth(rawCell1Voltage, LPF_FACTOR, smoothCell1Voltage);
  }

  if (smoothCell2Voltage == 0) {
    smoothCell2Voltage = rawCell2Voltage;
  } else {
    smoothCell2Voltage = smooth(rawCell2Voltage, LPF_FACTOR, smoothCell2Voltage);
  }

  if (smoothCell3Voltage == 0) {
    smoothCell3Voltage = rawCell3Voltage;
  } else {
    smoothCell3Voltage = smooth(rawCell3Voltage, LPF_FACTOR, smoothCell3Voltage);
  }
  
  if (smoothCell4Voltage == 0) {
    smoothCell4Voltage = rawCell4Voltage;
  } else {
    smoothCell4Voltage = smooth(rawCell4Voltage, LPF_FACTOR, smoothCell4Voltage);
  }

  if (smoothCell5Voltage == 0) {
    smoothCell5Voltage = rawCell5Voltage;
  } else {
    smoothCell5Voltage = smooth(rawCell5Voltage, LPF_FACTOR, smoothCell5Voltage);
  }

  if (smoothCell6Voltage == 0) {
    smoothCell6Voltage = rawCell6Voltage;
  } else {
    smoothCell6Voltage = smooth(rawCell6Voltage, LPF_FACTOR, smoothCell6Voltage);
  }
  
  cell1Voltage = (float)smoothCell1Voltage * cell1DividerMultiplier;
  cell2Voltage = (float)smoothCell2Voltage * cell2DividerMultiplier;
  cell3Voltage = (float)smoothCell3Voltage * cell3DividerMultiplier;
  cell4Voltage = (float)smoothCell4Voltage * cell4DividerMultiplier;
  cell5Voltage = (float)smoothCell5Voltage * cell5DividerMultiplier;
  cell6Voltage = (float)smoothCell6Voltage * cell6DividerMultiplier;
  flvss.setData(cell1Voltage, cell2Voltage - cell1Voltage, cell3Voltage - cell2Voltage, cell4Voltage - cell3Voltage, cell5Voltage - cell4Voltage, cell6Voltage - cell5Voltage);
  telemetry.send();

  delay(LOOP_TIME);
}
