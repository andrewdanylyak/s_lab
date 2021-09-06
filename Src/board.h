#ifndef __BOARD_H
#define __BOARD_H

/* SWM pin assignment definitions for pin assignment/muxing */
typedef struct {
	uint16_t assignedpin : 9;		/* Function and mode */
	uint16_t port : 2;				/* Pin port */
	uint16_t pin : 5;				/* Pin number */
} SWM_GRP_T;

/*Fixed func*/
typedef struct{
	uint8_t fixed_function;
	bool enable;
}M_FIXED_FUNC_T;

/*Outputs & states*/
typedef struct{
	unsigned char port;
	unsigned char pin;
	bool 	 state;
}DEFAULT_PIN_T;

extern void SetupBoard();

#endif