const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

STATIC const PINMUX_GRP_T ioconSetup[] = {
    {0, 30,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_30 MOSI1*/
    {0, 0,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_00 MISO1*/
    {0, 31,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_31 SCK1*/
    {1, 0,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO1_00 CS1*/
    {0, 1,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_1 MOSI0*/
    {0, 2,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_2 MISO0*/
    {0, 3,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_3 SCK0*/
    {0, 4,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_4 CS0*/
    {0, 5,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_5 Lift motor step*/
    {0, 6,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_6 Lift motor dir*/
    {1, 1,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO1_1 Lift motor error*/
    {0, 9,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO0_9 Presence sensor (reed switch)*/
    {0, 12,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_12 Servo controll*/
    {1, 2,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO1_2 Servo enable*/
    {0, 13,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_13 Spin*/
    {0, 14,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_14 Led*/
    {0, 15,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_15 Down endswitch*/
    {0, 16,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_16 Up endswitch*/
    {1, 4,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO1_4 USART(RxTx)*/
    {1, 5,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO1_5 USART(RxTx)*/
    {1, 6,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},     /* PIO1_6 Lift stop*/
    {0, 22,  (IOCON_DIGMODE_EN | IOCON_STDI2C_EN)},                        /* PIO0_22 SCL*/
    {0, 23,  (IOCON_DIGMODE_EN | IOCON_STDI2C_EN)},                        /* PIO0_23 SDA*/
    {1, 7,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN | IOCON_MODE_PULLDOWN)},     /* PIO1_7 Lift On*/
    {1, 10,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO1_10 Alert*/
    {0, 25,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_25 Changer motor step*/
    {0, 26,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_26 Changer motor dir*/
    {0, 27,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN | IOCON_MODE_PULLDOWN)},    /* PIO0_27 Changer motor en*/
    {0, 28,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)},    /* PIO0_28 Changer motor error*/
    {0, 29,  (IOCON_MODE_INACT | IOCON_DIGMODE_EN)}     /* PIO0_29 Set zero*/
};

/* Pin muxing table, only items that need changing from their default pin
   state are in this table. */
STATIC const SWM_GRP_T swmSetup[] = {
	/* UART */
	{(uint16_t) SWM_SPI1_MOSI_IO, 0, 30},       /* PIO0_30-MOSI1 */
	{(uint16_t) SWM_SPI1_MISO_IO, 0, 0},		/* PIO0_0-MISO1 */
	{(uint16_t) SWM_SPI1_SCK_IO, 0, 31},		/* PIO0_31-SCK1*/
	{(uint16_t) SWM_SPI1_SSELSN_0_IO, 1, 0},		/* PIO1_0-SSEl1*/
	//	{(uint16_t) SWM_SPI0_MOSI_IO, 0, 1},		/* PIO0_1-MOSI0 */
	//    {(uint16_t) SWM_SPI0_MISO_IO, 0, 2},		/* PIO0_2-MISO0 */
	//    {(uint16_t) SWM_SPI0_SCK_IO, 0, 3},         /* PIO0_3-SCK0 */
	//    {(uint16_t) SWM_SPI0_SSELSN_0_IO, 0, 4},		/* PIO0_4-SSEl0*/
	{(uint16_t) SWM_SCT0_OUT0_O, 0, 12},         /*PIO0_12-PWM0_0 Servo control */
};

/*Fixed group*/
STATIC const M_FIXED_FUNC_T fixedFunc[] = {
	{(uint8_t)SWM_FIXED_I2C0_SCL, true},
	{(uint8_t)SWM_FIXED_I2C0_SDA, true},
};

void SetupPinMux(){
	int i;
	/* Enable SWM and IOCON clocks */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
	Chip_SYSCTL_PeriphReset(RESET_IOCON);
	/* IOCON setup */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, ioconSetup, sizeof(ioconSetup) / sizeof(PINMUX_GRP_T));
		
	/* SWM assignable pin setup */
	for (i = 0; i < (sizeof(swmSetup) / sizeof(SWM_GRP_T)); i++)
		Chip_SWM_MovablePortPinAssign((CHIP_SWM_PIN_MOVABLE_T) swmSetup[i].assignedpin,swmSetup[i].port, swmSetup[i].pin);
		
	/* SWM fixed pin setup */
	for (i = 0; i < (sizeof(fixedFunc) / sizeof(M_FIXED_FUNC_T)); i++)
		Chip_SWM_FixedPinEnable((CHIP_SWM_PIN_FIXED_T) fixedFunc[i].fixed_function,fixedFunc[i].enable);
}

STATIC const DEFAULT_PIN_T default_PIN[] = {
	{0, 14, false},      /*Led pin*/
	{1, 2, false},      /*Enable servo pin*/
	//{1, 4, true},      /*Tube case pin*/
//	{1, 7, false},      /*Lift on pin*/
	{1, 10, false},     /*Alert pin*/
//	{0, 26, false},     /*Changer dir pin*/
//	{0, 27, false},     /*Changer en pin*/
//    {0, 25, false},     /*Changer step pin*/
//    {1, 1, false},     /*test pin*/
};

void SetupGPIO(){
	int i;
	Chip_GPIO_Init(LPC_GPIO);
	for(i = 0; i < (sizeof(default_PIN) / sizeof(DEFAULT_PIN_T)); i++){
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, default_PIN[i].port, default_PIN[i].pin);
		Chip_GPIO_SetPinState(LPC_GPIO, default_PIN[i].port, default_PIN[i].pin, default_PIN[i].state);
	}
}

void SetupClock(){
	Chip_SetupXtalClocking();
	Chip_Clock_SetSysTickClockDiv(1);
	LPC_SYSCTL->SYSTICKCLKDIV = 1;
}

void SetupBoard(){
	SetupPinMux();
	SetupClock();
	SetupGPIO();
	SystemCoreClockUpdate();
}