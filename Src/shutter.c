
tShutter Shutter;

void InitShutter(){
  ReadPwm1Position();
  ReadPwm2Position();
  ReadTimeout();
  ReadSn();
  Shutter.error_count = 0;
  if((Shutter.pwm1 == 0xFFFFFFFF)||(Shutter.pwm2 == 0xFFFFFFFF)
     ||(Shutter.const_timeout == 0xFFFFFFFF)||(Shutter.sn[0] == 0xFF)){
    Shutter.Mode = Init;
    Shutter.pwm1 = Shutter.pwm2 = 1330000;
    SetServoControlLevel(Shutter.pwm1);//1.53 ms
    Shutter.position_timeout = POSITION_TIMEOUT;
    Shutter.enable_position_timeout = true;
    EnableServo(true);
  }
  else{
    Shutter.Mode = Work;
    SetPosition(1);
  }
}

void CheckLimits(){
  Shutter.EndStop1 = !Chip_GPIO_GetPinState(LPC_GPIO, 0, 16);
  Shutter.EndStop2 = !Chip_GPIO_GetPinState(LPC_GPIO, 0, 15);
}

void SetPosition(uint8_t pos){
  if(Shutter.Mode == Init)
    return;
  if(Shutter.error_count > 3)
    return;
  if(Shutter.WorkStatus == Error)
    Shutter.error_count++;
  Shutter.position = pos;
  switch(pos){
  case 1:
    if(Shutter.EndStop1)
      return;
    SetServoControlLevel(Shutter.pwm1);
    Shutter.position_timeout = POSITION_TIMEOUT;
    EnableServo(true);
    Shutter.timeout = Shutter.const_timeout;
    Shutter.enable_position_timeout = true;
    Shutter.WorkStatus = Busy;
    break;
  case 2:
    if(Shutter.EndStop2)
      return;
    SetServoControlLevel(Shutter.pwm2);
    Shutter.position_timeout = POSITION_TIMEOUT;
    EnableServo(true);
    Shutter.timeout = Shutter.const_timeout;
    Shutter.enable_position_timeout = true;
    Shutter.WorkStatus = Busy;
    break;
  }
}