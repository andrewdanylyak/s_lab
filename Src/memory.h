#ifndef _memory_h
#define _memory_h

extern bool GetMemoryId();
extern bool StatusRegisterRead();
extern void SetMemorySettingsPowerof2();
extern void WriteData(uint32_t address,uint16_t length,uint8_t *data);
extern void ReadData(uint32_t address,uint16_t length,uint8_t *data);

extern void WritePwm1Position();
extern void WritePwm2Position();
extern void WriteTimeout();
extern void WriteSn();

extern void ReadPwm1Position();
extern void ReadPwm2Position();
extern void ReadTimeout();
extern void ReadSn();

#endif