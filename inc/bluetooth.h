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
/*
1. Test communication
Send: AT (please send it every second)
Back: OK
*/
#define CMD_AT					"AT\0"	// respond OK
#define RESP_AT					"OK"
#define RESP_AT_LEN				2

/*
2. Reset the Bluetooth serial baud rate
Send: AT+BAUD1
Back: OK1200
Send: AT+BAUD2
Back: OK2400

1---------1200
2---------2400
3---------4800
4---------9600 (Default)
5---------19200
6---------38400
7---------57600
8---------115200
9---------230400
A---------460800
B---------921600
C---------1382400
*/
// AT Command to BT Device


#define CMD_B1200_1				"AT+BAUD1\0"
#define CMD_B2400_2				"AT+BAUD2\0"
#define CMD_B4800_3				"AT+BAUD3\0"
#define CMD_B9600_4				"AT+BAUD4\0"
#define CMD_B19200_5			"AT+BAUD5\0"
#define CMD_B38400_6			"AT+BAUD6\0"
#define CMD_B57600_7			"AT+BAUD7\0"
#define CMD_B115200_8			"AT+BAUD8\0"
#define CMD_B230400_9			"AT+BAUD9\0"
#define CMD_B460800_A			"AT+BAUDA\0"
#define CMD_B921600_B			"AT+BAUDB\0"
#define CMD_B1382400_C			"AT+BAUDC\0"


#define RESP_B1200_1			"OK1200"
#define RESP_B1200_1_LEN		6

#define RESP_B2400_2			"OK2400"
#define RESP_B2400_2_LEN		6

#define RESP_B4800_3			"OK4800"
#define RESP_B4800_3_LEN		6

#define RESP_B9600_4			"OK9600"
#define RESP_B9600_4_LEN		6

#define RESP_B19200_5			"OK19200"
#define RESP_B19200_5_LEN		7

#define RESP_B38400_6			"OK38400"
#define RESP_B38400_6_LEN		7

#define RESP_B57600_7			"OK57600"
#define RESP_B57600_7_LEN		7

#define RESP_B115200_8			"OK115200"
#define RESP_B115200_8_LEN		8

#define RESP_B230400_9			"OK230400"
#define RESP_B230400_9_LEN		8

#define RESP_B460800_A			"OK460800"
#define RESP_B460800_A_LEN		8

#define RESP_B921600_B			"OK921600"
#define RESP_B921600_B_LEN		8

#define RESP_B1382400_C			"OK1382400"
#define RESP_B1382400_C_LEN		9

/*
3. Reset the Bluetooth name
Send: AT+NAMEname
Back: OKname

Parameter name: Name needed to be set (20 characters limited)
Example:
Send: AT+NAMEbill_gates
Back: OKname
 */
#define CMD_NAME				"AT+NAMELPC1788-WS\0"
#define RESP_NAME				"OKsetname"	// plus the name you set
#define RESP_NAME_LEN			9

/*
4. change the Bluetooth pair password
Send: AT+PINxxxx
Back:OKsetpin

Example:
Send: AT+PIN8888
Back: OKsetpin
Then the password is changed to be 8888, while the default is 1234.
This parameter can be kept even though the power is cut off.
*/
#define CMD_PIN					"AT+PIN1466\0"
#define RESP_PIN				"OKsetpin"
#define RESP_PIN_LEN			8

/*
 * 5. No parity check ( The version, higher than V1.5, can use this command )
Send: AT+PN (This is the default value)
Back: OK NONE
 */
#define CMD_PARITY_NONE			"ATPN\0"
#define RESP_PARITY_NONE		"OK NONE"
#define RESP_PARITY_NONE_LEN	7

/*
 * 6. Set odd parity check ( The version, higher than V1.5, can use this command )
Send: AT+PO
Back: OK ODD
 */
#define CMD_PARITY_ODD			"ATPO\0"
#define RESP_PARITY_ODD			"OK ODD"
#define RESP_PARITY_ODD_LEN		6

/*
 * 7. Set even parity check( The version, higher than V1.5, can use this command )
Send: AT+PE
Back: OK EVEN
 */
#define CMD_PARITY_EVEN			"ATPE\0"
#define RESP_PARITY_EVEN		"OK EVEN"
#define RESP_PARITY_EVEN_LEN	7

/*
 * 8. Get the AT version
Send: AT+VERSION
Back: LinvorV1.n
 */
#define CMD_GET_VERSION			"AT+VERSION\0"
#define RESP_GET_VERSION		"OKLinvorV1.8"
#define RESP_GET_VERSION_LEN	12

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
	BT_AT_MODE=3
}BT_STATE_t;

typedef struct{
	Bool cmd_result;		// The string returned was OK or not
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
//uint8_t*  bt_GetVersion(void);

/*
 * Send AT+NAME to module + Name you want for the module
 * to display on host systems, Return a pointer to the
 * response string
 */
//uint8_t*  bt_ChangeDisplayName();

/*
 * Send AT+PIN+wanted new PIN for the module to pair
 * return the pointer for the response string
 */
//uint8_t* bt_ChangePIN();

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
