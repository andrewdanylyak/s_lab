
#define SCT_PWM_RATE   50 //Hz
#define SCT_PWM_PIN_OUT    0        /* COUT1 Generate square wave */
#define SCT_PWM_OUT        1        /* Index of OUT PWM */

void InitServoControl(){
    Chip_SCTPWM_Init(LPC_SCT0);
    Chip_SCTPWM_SetRate(LPC_SCT0, SCT_PWM_RATE);
    Chip_SCTPWM_SetOutPin(LPC_SCT0, SCT_PWM_OUT, SCT_PWM_PIN_OUT);
    Chip_SCTPWM_Start(LPC_SCT0);
}

void EnableServo(bool En){
  Chip_GPIO_SetPinState(LPC_GPIO, 1, 2, En);
}

bool GetEnableServo(){
  return Chip_GPIO_GetPinState(LPC_GPIO, 1, 2);
}

void SetServoControlLevel(uint32_t pwm){
  if(pwm == 0)
    Chip_SCTPWM_SetDutyCycle(LPC_SCT0, SCT_PWM_OUT, 0);
  else{
    if(pwm < 1200000) //2.5 ms
      pwm = 1200000;
    if(pwm > 1500000) //0.56 ms
      pwm = 1500000;
    Chip_SCTPWM_SetDutyCycle(LPC_SCT0, SCT_PWM_OUT, pwm);
  }
}