
// one ms handler
void SysTick_Handler(void){
  static uint16_t counter = 1000;
  if(counter)
    counter--;
  else{
    counter = 1000;
    Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 14);
  }
  if(temperature_timeuot)
    temperature_timeuot--;
  CheckLimits();
  if(Shutter.Mode == Work){
    if(Shutter.timeout)
      Shutter.timeout--;
    else
      SetPosition(1);
  }
  if(Shutter.enable_position_timeout){
    if(Shutter.position_timeout)
      Shutter.position_timeout--;
    else{
      if((Shutter.Mode == Work)&&!Shutter.EndStop1&&(Shutter.position == 1))
        Shutter.WorkStatus = Error;
      else if((Shutter.Mode == Work)&&!Shutter.EndStop2&&(Shutter.position == 2))
        Shutter.WorkStatus = Error;
      else{
        Shutter.WorkStatus = Ok;
        Shutter.error_count = 0;
      }
      EnableServo(false);
      SetServoControlLevel(0);
      Shutter.enable_position_timeout = false;
    }
  }
}

int main(){
  SetupBoard();
  __enable_interrupt();
  SysTick_Config(SystemCoreClock / TICKRATE_HZ);
  InitServoControl();
  InitTimer1();
  SetupSPI(LPC_SPI1);
  InitShutter();
  SetupI2C();
  while(true){
    CheckI2c();
    if(temperature_timeuot == 0){
      get_temperature(TUBE_CASE);
      get_temperature(DETECTOR_CASE);
      temperature_timeuot = 1000;
    }   
  }
}
