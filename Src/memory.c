bool GetMemoryId(){
  uint16_t tx_buffer[4];
  memset(tx_buffer, 0, 8);
  tx_buffer[0] = 0x9F;
  if(MasterSpiMessage(LPC_SPI1 ,tx_buffer, 4, m_rx_buffer)){
    if((m_rx_buffer[1] == 0x1f)&&(m_rx_buffer[2] == 0x25))
      return true;
  }
  return false;
}

bool StatusRegisterRead(){
  uint16_t tx_buffer[4];
  memset(tx_buffer, 0, 8);
  tx_buffer[0] = 0xD7;
  if(MasterSpiMessage(LPC_SPI1, tx_buffer, 2, m_rx_buffer)){
    if((m_rx_buffer[1]&0x01) == 0)
      SetMemorySettingsPowerof2();
    if((m_rx_buffer[1]&0x80) == 0x80) 
      return true;
  }
  return false;
}

void SetMemorySettingsPowerof2(){
  uint16_t tx_buffer[4];
  memset(tx_buffer, 0, 8);
  tx_buffer[0] = 0x3d;
  tx_buffer[1] = 0x2a;
  tx_buffer[2] = 0x80;
  tx_buffer[3] = 0xa6;
  MasterSpiMessage(LPC_SPI1, tx_buffer, 4, m_rx_buffer);
}

void WriteData(uint32_t address,uint16_t length,uint8_t *data){
  uint16_t tx_buffer[256+12];
  tx_buffer[0] = 0x82;
  tx_buffer[1] = (uint8_t)(address >> 16);
  tx_buffer[2] = (uint8_t)(address >> 8);
  tx_buffer[3] = (uint8_t)(address);
  if(length > 256)
    length = 256;
  for(uint16_t i = 0; i < length; i++)
    tx_buffer[8 + i] = data[i];
  MasterSpiMessage(LPC_SPI1, tx_buffer, length + 8, m_rx_buffer);
  while(!StatusRegisterRead());
}

void ReadData(uint32_t address,uint16_t length,uint8_t *data){
  uint16_t tx_buffer[256+12];
  tx_buffer[0] = 0xD2;
  tx_buffer[1] = (uint8_t)(address >> 16);
  tx_buffer[2] = (uint8_t)(address >> 8);
  tx_buffer[3] = (uint8_t)(address);
  if(length > 256)
    length = 256;
  MasterSpiMessage(LPC_SPI1, tx_buffer, length + 8, m_rx_buffer);
  while(!StatusRegisterRead());
  for(uint16_t i = 0; i < length; i++)
    data[i] = m_rx_buffer[12 + i];
}

void WritePwm1Position(){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&buffer[0], &Shutter.pwm1, sizeof(uint32_t));
  WriteData(0, 256, buffer);
}

void WritePwm2Position(){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&buffer[4], &Shutter.pwm2, sizeof(uint32_t));
  WriteData(0, 256, buffer);
}

void WriteTimeout(){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&buffer[8], &Shutter.const_timeout, sizeof(uint32_t));
  WriteData(0, 256, buffer);
}

void WriteSn(){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&buffer[12], Shutter.sn, 8);
  WriteData(0, 256, buffer);
}

void ReadPwm1Position(uint32_t *pos){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&Shutter.pwm1, &buffer[0], sizeof(uint32_t));
}

void ReadPwm2Position(uint32_t *pos){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&Shutter.pwm2, &buffer[4], sizeof(uint32_t));
}

void ReadTimeout(uint32_t *val){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(&Shutter.const_timeout, &buffer[8], sizeof(uint32_t));
}

void ReadSn(uint8_t *data){
  uint8_t buffer[256];
  ReadData(0, 256, buffer);
  memcpy(Shutter.sn, &buffer[12], 8);
}
