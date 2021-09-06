tTcommandStructure I2cCommand;

char ID[] = "ElvaX S Lab Shutter v1.1";
#define I2C_ADDR_7BIT   0x36
#define I2C_BUFFER_LEN  150
static uint8_t i2c_input_ptr = 0;
static uint8_t i2c_output_ptr = 0;
static uint8_t i2c_state = 0;
static uint8_t i2c_buffer[I2C_BUFFER_LEN];
static uint8_t temp_buffer[I2C_BUFFER_LEN];

static uint8_t calculate_checksum(uint8_t length, uint8_t* data){
  uint8_t crc = 0;
  for(uint8_t i = 0; i < length; i++)
    crc += data[i];
  return crc;
}

static void prepare_output_buffer(uint8_t command, uint8_t status, uint8_t length, uint8_t* data){
  uint8_t crc = 0;
  memset(i2c_buffer, 0x00, I2C_BUFFER_LEN);
  i2c_buffer[0] = status;
  i2c_buffer[1] = command;
  memcpy(&i2c_buffer[2], data, length);
  for(uint8_t i = 0; i < length + 2; i++)
    crc += i2c_buffer[i];
  crc = (uint8_t)(0 - crc);
  i2c_buffer[length + 2] = crc;
}

void SetupI2C(){
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_I2C0);
  Chip_SYSCTL_PeriphReset(RESET_I2C0);
  Chip_I2C_Init(LPC_I2C0);
  Chip_I2CS_SetSlaveAddr(LPC_I2C0, 0, I2C_ADDR_7BIT);
  Chip_I2CS_SetSlaveQual0(LPC_I2C0, false, 0);//?
  Chip_I2CS_EnableSlaveAddr(LPC_I2C0, 0);
  Chip_I2CS_ClearStatus(LPC_I2C0, I2C_STAT_SLVDESEL);
  Chip_I2C_EnableInt(LPC_I2C0, I2C_STAT_SLVPENDING | I2C_STAT_SLVDESEL);
  Chip_I2CS_Enable(LPC_I2C0);
  NVIC_SetPriority(I2C0_IRQn, 2);
  NVIC_EnableIRQ(I2C0_IRQn);
}

static void check_buffer(){
  uint8_t status = STATUS_OK;
  uint8_t tx_length = 0;
  uint8_t command = i2c_buffer[0];
  uint8_t subcommand = i2c_buffer[1];
  if(calculate_checksum(i2c_input_ptr,i2c_buffer) != 0)
    status |= STATUS_CRC_ERR;
  if(status != STATUS_OK){
    prepare_output_buffer(command,status,tx_length,temp_buffer);
    return;
  }
  switch(command){
    case SHUTTER_SET_COMMAND:
      if(i2c_input_ptr < (2 + 10)){
        status |= STATUS_WRONG_LENGTH;
        break;
      }
      memcpy(&I2cCommand.buffer, i2c_buffer, 10);
      I2cCommand.hasNewCommand = true;
      break;
    case SHUTTER_GET_COMMAND:
      if(i2c_input_ptr < (2 + 1)){
        status |= STATUS_WRONG_LENGTH;
        break;
      }
      switch(subcommand){
      case SC_GET_ID:
        strcpy((char*)temp_buffer,ID);
        tx_length = 128;
        break;
      case SC_GET_STATUS:
        temp_buffer[0] = (uint8_t)Shutter.WorkStatus;
        temp_buffer[1] = (uint8_t)Shutter.EndStop1;
        temp_buffer[2] = (uint8_t)Shutter.EndStop2;
        temp_buffer[3] = (uint8_t)Shutter.QuvetePresent;
        tx_length = 4;
        break;
      case SC_READ_TEMPERATURE:
        temp_buffer[0] = Sensors.TubeCaseSensor.uchar_temperature;
        temp_buffer[1] = Sensors.DetectorCaseSensor.uchar_temperature;
        memcpy(&temp_buffer[2], &Sensors.TubeCaseSensor.float_temperature, sizeof(float));
        memcpy(&temp_buffer[6], &Sensors.DetectorCaseSensor.float_temperature, sizeof(float));
        tx_length = 10;
        break;
      case SC_READ_POS1_PWM_DATA:
        memcpy(temp_buffer, &Shutter.pwm1, sizeof(uint32_t));
        tx_length = sizeof(uint32_t);
        break;
      case SC_READ_POS2_PWM_DATA:
        memcpy(temp_buffer, &Shutter.pwm2, sizeof(uint32_t));
        tx_length = sizeof(uint32_t);
        break;
      case SC_READ_TIMEOUT_DATA:
        memcpy(temp_buffer, &Shutter.const_timeout, sizeof(uint32_t));
        tx_length = sizeof(uint32_t);
        break;
      case SC_READ_SN_DATA:
        memcpy(temp_buffer, &Shutter.sn, 8);
        tx_length = 8;
        break;
      }
      break;
  }
  prepare_output_buffer(command,status,tx_length,temp_buffer);
}

static void processSlaveTransferStart(uint8_t addr){
  //printf("start st %i\r\n", i2c_state);
  if(i2c_state == 0){
    i2c_input_ptr = 0;
    memset(i2c_buffer, 0 , I2C_BUFFER_LEN);
    i2c_state = 1;
    return;
  }
  if(i2c_state == 1){
    i2c_output_ptr = 0;
  }
}

/* Handler
 for slave send callback */
static uint8_t processSlaveTransferSend(uint8_t *data){
  if((i2c_output_ptr == 0)){//&&(i2c_state == 1))
    __disable_interrupt();
    check_buffer();
    __enable_interrupt();
  }
  //printf("Tx out %i 0x%X \r\n", i2c_output_ptr, *(i2c_buffer + i2c_output_ptr));
  *data = i2c_buffer[i2c_output_ptr++];
  if(i2c_output_ptr > I2C_BUFFER_LEN)
    i2c_output_ptr = 0;
  return 0;
}

/* Handler for slave receive callback */
static uint8_t processSlaveTransferRecv(uint8_t data){
  //printf("Rx in %i 0x%X \r\n", i2c_input_ptr, data);
  i2c_buffer[i2c_input_ptr++] = data;
  if(i2c_input_ptr > I2C_BUFFER_LEN)
    i2c_input_ptr = 0;
  return 0;
}

/* Handler for slave transfer complete callback */
static void processSlaveTransferDone(void){
  //printf("done \r\n");
  i2c_state = 0;
  i2c_input_ptr = 0;
  i2c_output_ptr = 0;
  memset(i2c_buffer, 0, I2C_BUFFER_LEN);
  Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 14);
}

/* I2C slavecallback function list */
const static I2CS_XFER_T i2csCallBacks = {
  &processSlaveTransferStart,
  &processSlaveTransferSend,
  &processSlaveTransferRecv,
  &processSlaveTransferDone
};

/*I2C irq handler*/
void I2C0_IRQHandler(void){
  Chip_I2CS_XferHandler(LPC_I2C0, &i2csCallBacks);
  Chip_I2CS_ClearStatus(LPC_I2C0, I2C_INTENSET_SLVPENDING);
}

void CheckI2c(){
  uint8_t subcommand;
  uint32_t data = 0;
  if(I2cCommand.hasNewCommand){
    subcommand = I2cCommand.buffer[1];
      switch(subcommand){
      case SC_SET_POSITION:
        if(I2cCommand.buffer[2] != 0)
          SetPosition(I2cCommand.buffer[2]);
        break;
      case SC_POSITION_BY_PWM:
        memcpy(&data, &I2cCommand.buffer[2], sizeof(uint32_t));
        SetServoControlLevel(data);
        Shutter.position_timeout = POSITION_TIMEOUT;
        EnableServo(true);
        Shutter.timeout = Shutter.const_timeout;
        Shutter.enable_position_timeout = true;
        Shutter.WorkStatus = Busy;
        break;
      case SC_SET_WORK_MODE:
        Shutter.Mode = (tMode)I2cCommand.buffer[2];
        break;
      case SC_STATE_UPDATE:
        //printf("state update \r\n");
        Shutter.timeout = Shutter.const_timeout;
        break;
      case SC_WRITE_POS1_PWM_DATA:
        memcpy(&Shutter.pwm1, &I2cCommand.buffer[2], sizeof(uint32_t));
        WritePwm1Position();
        break;
      case SC_WRITE_POS2_PWM_DATA:
        memcpy(&Shutter.pwm2, &I2cCommand.buffer[2], sizeof(uint32_t));
        WritePwm2Position();
        break;
      case SC_WRITE_TIMEOUT_DATA:
        memcpy(&Shutter.const_timeout, &I2cCommand.buffer[2], sizeof(uint32_t));
        WriteTimeout();
        break;
      case SC_WRITE_SN_DATA:
        memcpy(&Shutter.sn, &I2cCommand.buffer[2], 8);
        WriteSn();
        break;
      }
    I2cCommand.hasNewCommand = false;
  }
}