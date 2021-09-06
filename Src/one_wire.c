#define CRC8INIT            0x00
#define CRC8POLY            0x18 
#define OW_SKIP_ROM         0xCC
#define OW_TRANSFER_T       0x44
#define OW_READ_RAM         0xBE
#define	OW_WRITE_RAM        0x4E
#define	OW_SEARCH_ROM       0xF0
#define OW_READ_ROM         0x33
#define DS18S20_ID          0x10
#define DS18B20_ID          0x28

tSensors Sensors;

uint8_t crc8(uint8_t *buffer, uint32_t num_bytes){
  uint8_t crc;
  uint32_t loop_count;
  uint8_t bit_counter;
  uint8_t data;
  uint8_t feedback_bit;
  crc = CRC8INIT;
  for(loop_count = 0; loop_count != num_bytes; loop_count++){ 
    data = buffer[loop_count];
    bit_counter = 8;
    do { 			
      feedback_bit = (crc ^ data) & 0x01;
      if(feedback_bit == 0x01) 
        crc = crc ^ CRC8POLY;
      crc = (crc >> 1) & 0x7F;
      if(feedback_bit == 0x01) 
        crc = crc | 0x80;
      data = data >> 1;
      bit_counter--;
    } 
    while (bit_counter > 0);
  }
  return crc;
}

uint8_t one_wire_reset(uint8_t sensor){
  uint8_t result;
  uint8_t port;
  uint8_t pin;
  switch(sensor){
    case TUBE_CASE:
      port = 1;
      pin = 4;
      break;
    case DETECTOR_CASE:
      port = 0;
      pin = 9;
      break;
  }
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
  Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
  delay_us(480);
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
  delay_us(70);
  result = (uint8_t)Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
  delay_us(190);
  return result;
}

void one_wire_transmit_bit(uint8_t sensor, uint8_t bit){
  uint8_t port;
  uint8_t pin;
  switch(sensor){
    case TUBE_CASE:
      port = 1;
      pin = 4;
      break;
    case DETECTOR_CASE:
      port = 0;
      pin = 9;
      break;
  }
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
  Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
  delay_us(5);
  if(bit == 0)
    delay_us(65);
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
  delay_us(5);
  if(bit == 1)
    delay_us(60);  
}

uint8_t one_wire_recive_bit(uint8_t sensor){
  uint8_t port;
  uint8_t pin;
  uint8_t result;
  switch(sensor){
    case TUBE_CASE:
      port = 1;
      pin = 4;
      break;
    case DETECTOR_CASE:
      port = 0;
      pin = 9;
      break;
  }
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
  Chip_GPIO_SetPinOutLow(LPC_GPIO, port, pin);
  delay_us(5);
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
  delay_us(10);
  result = (uint8_t)Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
  delay_us(45);
  return result;
}

void one_wire_transmit_byte(uint8_t sensor, uint8_t data){
  uint8_t counter = 0;
  while(counter < 8){
    if(data&0x01)
      one_wire_transmit_bit(sensor, 1);
    else
      one_wire_transmit_bit(sensor, 0);
    data = data >> 1;
    counter++;
  }
}

uint8_t one_wire_recive_byte(uint8_t sensor){
  uint8_t data = 0, counter = 0, bit = 0;
  while(counter < 8){
    bit = one_wire_recive_bit(sensor);
    data = data >> 1;
    if(bit == 1)
      data |= 0x80;
    counter++;
  }
  return data;
}

void wait_for_conversion(uint8_t sensor){
  uint8_t port;
  uint8_t pin;
  switch(sensor){
    case TUBE_CASE:
      port = 1;
      pin = 4;
      break;
    case DETECTOR_CASE:
      port = 0;
      pin = 9;
      break;
  }
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
  while (!Chip_GPIO_GetPinState(LPC_GPIO, port, pin));
}

void get_temperature(uint8_t sensor){
  float temperature;
  uint8_t DS_ROM[9],i;
  uint8_t DS_ID,DS_CRC,result,s,d,c;
	uint16_t temp = 0;
  if(one_wire_reset(sensor) == 0){
    one_wire_transmit_byte(sensor, OW_READ_ROM);
    for(i = 0; i < 8; i++)
      DS_ROM[i] = one_wire_recive_byte(sensor);
  }
  DS_ID = DS_ROM[0];
  DS_CRC = crc8(DS_ROM, 7);
  if((DS_ID == DS18S20_ID)&&(DS_CRC == DS_ROM[7])){
    one_wire_transmit_byte(sensor, OW_TRANSFER_T);
    wait_for_conversion(sensor);
    one_wire_reset(sensor);
    one_wire_transmit_byte(sensor, OW_SKIP_ROM);
    one_wire_transmit_byte(sensor, OW_READ_RAM);
    for(i = 0; i < 9; i++)
      DS_ROM[i] = one_wire_recive_byte(sensor);
    DS_CRC = crc8(DS_ROM, 8);
    if(DS_CRC == DS_ROM[8]){
      s = DS_ROM[1]&0x01;
      c = DS_ROM[0] >> 1;
      if(s == 1)
        c = (c^0x7F)+1;
      d = (uint8_t)(c/10);
      c = (uint8_t)(c - (uint8_t)d*10);
      result = ((s << 7)|(d << 4)|c)&0xFF;
      switch(sensor){
        case DETECTOR_CASE:
        Sensors.DetectorCaseSensor.uchar_temperature = result;
        break;
      case TUBE_CASE:
        Sensors.TubeCaseSensor.uchar_temperature = result;
        break;
      }
    }
  }
  if((DS_ID == DS18B20_ID)&&(DS_CRC == DS_ROM[7])){
    one_wire_transmit_byte(sensor, OW_TRANSFER_T);
    wait_for_conversion(sensor);
    one_wire_reset(sensor);
    one_wire_transmit_byte(sensor, OW_SKIP_ROM);
    one_wire_transmit_byte(sensor, OW_READ_RAM);
    for(i = 0; i < 9; i++)
      DS_ROM[i] = one_wire_recive_byte(sensor);
    DS_CRC = crc8(DS_ROM, 8);
    if(DS_CRC == DS_ROM[8]){
      s = (DS_ROM[1] >> 3)&0x01;
      c = (DS_ROM[0] >> 4)|((DS_ROM[1] << 4)&0x70);
      if(s == 1)
        c = (c^0x7F)+1;
      d = (uint8_t)(c/10);
      c = (uint8_t)(c - (uint8_t)d*10);
      result = ((s << 7)|(d << 4)|c)&0xFF;
      temp = (DS_ROM[1] << 8)|DS_ROM[0];
      if((temp & 0x8000) != 0){
        temp = (temp^0xFFFF) + 1;
        temperature = -1.0 * temp * 0.0625;
      }
      else
        temperature = temp * 0.0625;
      switch(sensor){
        case DETECTOR_CASE:
        Sensors.DetectorCaseSensor.uchar_temperature = result;
        Sensors.DetectorCaseSensor.float_temperature = temperature;
        break;
      case TUBE_CASE:
        Sensors.TubeCaseSensor.uchar_temperature = result;
        Sensors.TubeCaseSensor.float_temperature = temperature;
        break;
      }
    }
  }
}

