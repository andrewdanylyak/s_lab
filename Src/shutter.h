#ifndef _shutter_h
#define _shutter_h

typedef enum{
  Init,
  Adjusting,
  Work,
}tMode;

typedef enum{
  Ok,
  Busy,
  Error
}tWorkStatus;

typedef struct {
  tMode Mode;
  tWorkStatus WorkStatus;
  uint8_t position;
  bool allow_memory_pwm1;
  uint32_t pwm1;
  bool allow_memory_pwm2;
  uint32_t pwm2;
  bool EndStop1;
  bool EndStop2;
  bool QuvetePresent;
  uint32_t timeout;
  uint32_t const_timeout;
  bool enable_position_timeout;
  uint32_t position_timeout;
  uint8_t sn[8];
  uint8_t error_count;
}tShutter;

extern tShutter Shutter;

extern void CheckLimits();
extern void SetPosition(uint8_t pos);
extern void InitShutter();

#endif