#ifndef _onewire_h
#define _onewire_h

#define TUBE_CASE           0x01
#define DETECTOR_CASE       0x02

typedef struct{
  uint8_t uchar_temperature;
  float float_temperature;
}tSensor;

typedef struct{
  tSensor TubeCaseSensor;
  tSensor DetectorCaseSensor;
}tSensors;

extern tSensors Sensors;
extern void get_temperature(uint8_t sensor);

#endif