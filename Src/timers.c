#define TIMER1_RATE_HZ 1000000

uint16_t temperature_timeuot;

static volatile uint32_t t_value;

void InitTimer1(){
  Chip_SCT_Init(LPC_SCT1);
  Chip_SCT_Config(LPC_SCT1, SCT_CONFIG_32BIT_COUNTER|SCT_CONFIG_CLKMODE_BUSCLK|SCT_CONFIG_AUTOLIMIT_L);
  Chip_SCT_SetMatchCount(LPC_SCT1, SCT_MATCH_0, Chip_Clock_GetSystemClockRate()/TIMER1_RATE_HZ);
  Chip_SCT_SetMatchReload(LPC_SCT1, SCT_MATCH_0, Chip_Clock_GetSystemClockRate()/TIMER1_RATE_HZ);
  Chip_SCT_SetEventStateRegister(LPC_SCT1, 0, 0xFFFFFFFF);
  Chip_SCT_SetEventControlRegister(LPC_SCT1, 0, EV_CTRL_COMBMODE_MATCH);
  Chip_SCT_ClearControl(LPC_SCT1, SCT_CTRL_HALT_L);
  Chip_SCT_EnableEventInt(LPC_SCT1, SCT_EVT_0);	
  NVIC_SetPriority(SCT1_IRQn, 1);
  NVIC_EnableIRQ(SCT1_IRQn);
}

//
//one us hsndler
//
void SCT1_IRQHandler(void){
  if(t_value)
    t_value--;
  Chip_SCT_ClearEventFlag(LPC_SCT1, SCT_EVT_0);
}

void delay_us(uint32_t t){
  t_value = t;
  while(t_value){}
}

void delay_ms(uint32_t t){
  t_value = t*1000;
  while(t_value){}
}