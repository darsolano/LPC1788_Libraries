/*
 *bluetooth.h
 *
 *  Created on: 5/1/2015
 *      Author: dsolano
 */

#ifndef INCLUDE_BLUETOOTH_H_
#define INCLUDE_BLUETOOTH_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>


#define CMD_INTERVAL		1000	//1sec
#define ALIVE_TIME			1000	//1sec
#define BT_RINGBUFFER_SIZE	32
#define BT_DATA_BUFF		24

#define CMD_LEN(cmd)			strlen(cmd)

typedef enum{
	B1200 = 1,
	B2400 = 2,
	B4800 = 3,
	B9600 = 4, 				//(Default)
	B19200 = 5,
	B38400 = 6,
	B57600 = 7,
	B115200 = 8,
	B230400 = 9,
	B460800 = 10,
	B921600 = 11,
	B1382400 = 12,
	AT,
	ATPIN,
	ATNAME,
	ATPN,
	ATPO,
	ATPE,
	ATVER
}CMD_TYPE_t;

typedef enum{
	BT_NOT_CONN=1,
	BT_CONN=2,
	BT_AT_MODE=3,
	BT_ALREADY_CONN
}BT_STATE_t;

typedef struct{
	Bool cmd_result;		// The string returned was OK or not
	uint8_t uart_irq_id;	// IRQ system ID for UART IRQ system
	BT_STATE_t bt_state;	// Weather the BT device is ready or not
	CMD_TYPE_t cmd_type;	// Command AT type
	char* cmd;				// Command Pattern
	char* response;			// Command response pattern
	char* data_buffer;		// Receive data buffer from bluetooth
	int cmd_len;			// Command data length
	int response_len;		// Command response length
	int baudrate;			// Baudrate speed of USART
}BT_CMD_t;

void bt_init(BT_CMD_t* bt, LPC_USART_T* USARTx, int baudrate);

/*
 * Send AT command to BT module
 * if response is OK then is alive.
 * return the pointer to response buffer
 */
char* bt_Is_Alive(BT_CMD_t* bt);

/*
 * Send AT+Version to the module
 * response OKLinvorV1.8
 * return pointer to string responded
 */
char* bt_Get_Version(BT_CMD_t* bt);

/*
 * Send AT+NAME to module + Name you want for the module
 * to display on host systems, Return a pointer to the
 * response string
 */
char*  bt_ChangeDisplayName(BT_CMD_t* bt, const char* name);

/*
 * Send AT+PIN+wanted new PIN for the module to pair
 * return the pointer for the response string
 */
char* bt_ChangePIN(BT_CMD_t* bt, const char* pin);

/*
 * Set specific baud rate by
 * Sending the appropriate AT command thru UART
 * remember that once the BT is connected all data
 * sent to modulo will pass thru to tty
 */
//uint8_t* bt_SetDataBaudRate(uint8_t baud);

/*
 * Use to monitor the state machine for the module
 * if it is connected all data will be sent thru it
 * If not you can program and interact with module.
 * This function must be called from endless loop in main.
 */
BT_STATE_t bt_ModuleMonitor(BT_CMD_t* bt);

#endif /* INCLUDE_BLUETOOTH_H_ */
