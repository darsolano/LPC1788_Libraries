/*
 * bluetooth.c
 *
 *  Created on: 5/1/2015
 *      Author: dsolano
 */

#include <bluetooth.h>
#include <hardware_delay.h>
#include <string.h>
#include <BSP_Waveshare/bsp_waveshare.h>
#include <Cli/cli.h>
#include <Cli/vt100.h>
#include <monitor.h>
#include <datetime.h>
#include <FatFs/rtc.h>

#define BTHC06_IntEventHandler	UART2_IRQHandler

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
#define CMD_NAME				"AT+NAME\0"
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
#define CMD_PIN					"AT+PIN\0"
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


#define CMD_INTERVAL			1000	//1sec
#define ALIVE_TIME				1000	//1sec
#define TIMEOUT_VALUE			2000	//
#define BT_RINGBUFFER_SIZE		32
#define BT_DATA_BUFF			24
#define CMD_LEN(cmd)			strlen(cmd)

//***************************************************
//		VARIABLES, STRUCTURES, BUFFERS
//***************************************************

RINGBUFF_T bt_rb;								// Ring Buffer definition for UART2
char 			bt_Buffer[BT_RINGBUFFER_SIZE]; 	// Ringbuffer handler data buffer definition
char 			bt_data_buff[BT_DATA_BUFF];
char			bt_cmd_buffer[BT_DATA_BUFF];
volatile char 	bt_Rx_Data;						// used for the interrupt
LPC_USART_T* 	Ux;								// floating usart pointer
volatile BT_CMD_t *Lclbtx;						// To enable Interrupts to use struct data
Bool			bt_data_rx;

int Baud_Table[] ={
		1200,
		2400,
		4800,
		9600,// (Default)
		19200,
		38400,
		57600,
		115200,
		230400,
		460800,
		921600,
		1382400
};

//***************************************************
//	PRIVATE FUNCTIONS PROTOTYPES
//***************************************************

static int 		UARTGetChar(void);
static void 	UARTPutChar(char ch);
static void 	UARTSendFrame(char* frame, int bytes);
static void 	init_uart(BT_CMD_t* bt); // UART
static void 	bt_GetDataStruct(BT_CMD_t* bt, CMD_TYPE_t br);
static char*  	parse_hc06_command(BT_CMD_t* bt);
static CMD_TYPE_t comm_scan_baudrate(BT_CMD_t* bt);


const char msg[] =
"********************************************************************************\n\r"
" Hello NXP Semiconductors \n\r"
" CLIENT COMMAND LINE PROCESSOR: \n\r"
"\t - MCU: LPC1788 LPC OPEN 1788 WAVESHARE \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Bluetooth HC-06 Communication: 9600 bps \n\r"
" EXAMPLE TO USE ALL PERIPHERAL AND MAKE THEM WORK THRU CLI \n\r"
" To start terminal press *, to end terminal press # \n\r"
"********************************************************************************\n\r";

/* Gets a character from the UART, returns EOF if no character is ready */
static int UARTGetChar(void)
{
	if (Ux->LSR & UART_LSR_RDR) {
		return (uint8_t) (Ux->RBR & UART_RBR_MASKBIT);
	}
	return 0;
}

/* Sends a character on the UART */
static void UARTPutChar(char ch)
{
	while ((Ux->LSR & UART_LSR_THRE) == 0) {}
	Ux->THR = (uint32_t) ch;
}

static void UARTSendFrame(char* frame, int bytes){
	char* ptr = frame;
	int len = 0;

	while (len < bytes){
		UARTPutChar(*ptr++);
		len++;
	}
}


void BTHC06_IntEventHandler(void)
{
	__disable_irq();	// Disable all interrupts to prevent the interrupt being stopped
	{
		bt_Rx_Data = UARTGetChar();
		if (bt_Rx_Data == 'K') {
			Lclbtx->cmd_result = TRUE;
			Lclbtx->bt_state = BT_AT_MODE;
		}
		if (bt_Rx_Data == '*') Lclbtx->bt_state = BT_CONN;
		if (bt_Rx_Data == '#') Lclbtx->bt_state = BT_AT_MODE;
		bt_data_rx	= TRUE;
		RingBuffer_Insert(&bt_rb, (const void*) &bt_Rx_Data);
		NVIC_ClearPendingIRQ(Lclbtx->uart_irq_id);
	}
	__enable_irq();	// enable all interrupt back again
}


static void init_uart(BT_CMD_t* bt) // UART
{
	if (Ux == LPC_UART1){
		/* Initialize UART1 pin connect */
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART1);
		Chip_IOCON_PinMux(LPC_IOCON, 0, 15, IOCON_MODE_INACT, IOCON_FUNC1);	//RX UART1
		Chip_IOCON_PinMux(LPC_IOCON, 0, 16, IOCON_MODE_INACT, IOCON_FUNC1);	//TX UART1
	} else if (Ux == LPC_UART2){
		/* Initialize UART2 pin connect */
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART2);
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_FUNC1 | IOCON_MODE_INACT));	//TX
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_FUNC1 | IOCON_MODE_INACT));	//RX
	}
	// Init UARTx for every parameter
	Chip_UART_Init(Ux);
	Chip_UART_TXEnable(Ux);
	Chip_UART_SetBaud(Ux , bt->baudrate);
	Ux->FCR &= ~(1<<0);
	Chip_UART_IntEnable(Ux , UART_IER_RBRINT );

	NVIC_SetPriority(bt->uart_irq_id , 1);
	NVIC_EnableIRQ(bt->uart_irq_id);

	// Init ring buffer
	RingBuffer_Init( &bt_rb , bt_Buffer, sizeof(char) , BT_RINGBUFFER_SIZE );
	xdev_in(UARTGetChar);
	xdev_out(UARTPutChar);
	__enable_irq();
}

/*
 * Baud accept value between 1 and 12
 * be careful you can be with no communication
 * after applying this command. Remember that if you are using
 * UART you need to change Baud Rate too.
 */
static void bt_GetDataStruct(BT_CMD_t* bt, CMD_TYPE_t br)
{
	switch (br)
	{
		case B1200:
			bt->cmd = CMD_B1200_1;
			bt->cmd_len = CMD_LEN(CMD_B1200_1);
			bt->response = RESP_B1200_1;
			bt->response_len = RESP_B1200_1_LEN;
			bt->cmd_type = B1200;
			break;
		case B2400:
			bt->cmd = CMD_B2400_2;
			bt->cmd_len = CMD_LEN(CMD_B2400_2);
			bt->response = RESP_B2400_2;
			bt->response_len = RESP_B2400_2_LEN;
			bt->cmd_type = B2400;
			break;
		case B4800:
			bt->cmd = CMD_B4800_3;
			bt->cmd_len = CMD_LEN(CMD_B4800_3);
			bt->response = RESP_B4800_3;
			bt->response_len = RESP_B4800_3_LEN;
			bt->cmd_type = B4800;
			break;
		case B9600:
			bt->cmd = CMD_B9600_4;
			bt->cmd_len = CMD_LEN(CMD_B9600_4);
			bt->response = RESP_B9600_4;
			bt->response_len = RESP_B9600_4_LEN;
			bt->cmd_type = B9600;
			break;
		case B19200:
			bt->cmd = CMD_B19200_5;
			bt->cmd_len = CMD_LEN(CMD_B19200_5);
			bt->response = RESP_B19200_5;
			bt->response_len = RESP_B19200_5_LEN;
			bt->cmd_type = B19200;
			break;
		case B38400:
			bt->cmd = CMD_B38400_6;
			bt->cmd_len = CMD_LEN(CMD_B38400_6);
			bt->response = RESP_B38400_6;
			bt->response_len = RESP_B38400_6_LEN;
			break;
		case B57600:
			bt->cmd = CMD_B57600_7;
			bt->cmd_len = CMD_LEN(CMD_B57600_7);
			bt->response = RESP_B57600_7;
			bt->response_len = RESP_B57600_7_LEN;
			bt->cmd_type = B57600;
			break;
		case B115200:
			bt->cmd = CMD_B115200_8;
			bt->cmd_len = CMD_LEN(CMD_B115200_8);
			bt->response = RESP_B115200_8;
			bt->response_len = RESP_B115200_8_LEN;
			bt->cmd_type = B115200;
			break;
		case B230400:
			bt->cmd = CMD_B230400_9;
			bt->cmd_len = CMD_LEN(CMD_B230400_9);
			bt->response = RESP_B230400_9;
			bt->response_len = RESP_B230400_9_LEN;
			bt->cmd_type = B230400;
			break;
		case B460800:
			bt->cmd = CMD_B460800_A;
			bt->cmd_len = CMD_LEN(CMD_B1200_1);
			bt->response = RESP_B1200_1;
			bt->response_len = RESP_B1200_1_LEN;
			bt->cmd_type = B460800;
			break;
		case B921600:
			bt->cmd = CMD_B921600_B;
			bt->cmd_len = CMD_LEN(CMD_B921600_B);
			bt->response = RESP_B921600_B;
			bt->response_len = RESP_B921600_B_LEN;
			bt->cmd_type = B921600;
			break;
		case B1382400:
			bt->cmd = CMD_B1382400_C;
			bt->cmd_len = CMD_LEN(CMD_B1382400_C);
			bt->response = RESP_B1382400_C;
			bt->response_len = RESP_B1382400_C_LEN;
			bt->cmd_type = B1382400;
			break;
		case AT:
			bt->cmd = CMD_AT;
			bt->cmd_len = CMD_LEN(CMD_AT);
			bt->response = RESP_AT;
			bt->response_len = RESP_AT_LEN;
			bt->cmd_type = AT;
			break;
		case ATPIN:
			bt->cmd = CMD_PIN;
			bt->cmd_len = CMD_LEN(CMD_PIN);
			bt->response = RESP_PIN;
			bt->response_len = RESP_PIN_LEN;
			bt->cmd_type = ATPIN;
			break;
		case ATNAME:
			bt->cmd = CMD_NAME;
			bt->cmd_len = CMD_LEN(CMD_NAME);
			bt->response = RESP_NAME;
			bt->response_len = RESP_NAME_LEN;
			bt->cmd_type = ATNAME;
			break;
		case ATPN:
			bt->cmd = CMD_PARITY_NONE;
			bt->cmd_len = CMD_LEN(CMD_PARITY_NONE);
			bt->response = RESP_PARITY_NONE;
			bt->response_len = RESP_PARITY_NONE_LEN;
			bt->cmd_type = ATPN;
			break;
		case ATPO:
			bt->cmd = CMD_PARITY_ODD;
			bt->cmd_len = CMD_LEN(CMD_PARITY_ODD);
			bt->response = RESP_PARITY_ODD;
			bt->response_len = RESP_PARITY_ODD_LEN;
			bt->cmd_type = ATPO;
			break;
		case ATPE:
			bt->cmd = CMD_PARITY_EVEN;
			bt->cmd_len = CMD_LEN(CMD_PARITY_EVEN);
			bt->response = RESP_PARITY_EVEN;
			bt->response_len = RESP_PARITY_EVEN_LEN;
			bt->cmd_type = ATPE;
			break;
		case ATVER:
			bt->cmd = CMD_GET_VERSION;
			bt->cmd_len = CMD_LEN(CMD_GET_VERSION);
			bt->response = RESP_GET_VERSION;
			bt->response_len = RESP_GET_VERSION_LEN;
			bt->cmd_type = ATVER;
			break;
		default:
			bt->cmd = 0;
			bt->cmd_len = 0;
			bt->response = 0;
			bt->response_len = 0;
			break;
	}
}

static char*  parse_hc06_command(BT_CMD_t* bt){
	UARTSendFrame(bt->cmd, bt->cmd_len);	// Send command frame to BT module
	startTimer0();							// start timer count for timeout
	while (!bt->cmd_result)					// wait for the character to arrive on UART port from BT module
	{										// in response to the command frame
		if (getTimer0_counter() >= TIMEOUT_VALUE){
			stopTimer0();					// this is timeout 2 seconds or 2000ms
			return ("Timeout...");
		}
	}
	while (RingBuffer_GetCount(&bt_rb) < bt->response_len);			// wait for ringbuffer to get all chars from BT module
	memset(bt->data_buffer, 0, BT_DATA_BUFF);						// Clen data buffer from CMD data
	RingBuffer_PopMult(&bt_rb, bt->data_buffer, bt->response_len);	// get all data out of ringbuffer
	if ( !memcmp (bt->data_buffer, (const char*)"OK", 2) ){			// compare the first 2 char from data buffer to OK
		bt->cmd_result = FALSE;
		return bt->data_buffer;
	}
	return 0;
}

static CMD_TYPE_t comm_scan_baudrate(BT_CMD_t* bt){
	int br;
	char* ptr;
	for (br=0;br<12;br++){
		bt->baudrate = Baud_Table[br];
		Chip_UART_SetBaud(Ux , bt->baudrate);
		bt_GetDataStruct(bt, AT);
		ptr = parse_hc06_command(bt);
		if ( !memcmp (ptr, (const char*)"OK", 2) ){			// compare the first 2 char from data buffer to OK
			return br+1;
		}
	}
	return 0;
}

static CMD_TYPE_t get_baudrate_type (int baudrate){
	CMD_TYPE_t brt;
	for (brt=0; brt<12; brt++){
		if (Baud_Table[brt] == baudrate)
			return (brt+1);
	}
	return 0;
}

//************************************************************
//			PUBLIC FUNCTION
//************************************************************

/*
 *BT need to be disconnected from host or data will pass thru
 */
Bool bt_init(BT_CMD_t* bt, LPC_USART_T* USARTx, int baudrate)
{
	__disable_irq();
	Lclbtx = bt;
	Ux = USARTx;
	bt->uart_irq_id = wsBoard_UART_Get_Interrupt_Index(USARTx);
	bt->baudrate = baudrate;
	bt->data_buffer = bt_data_buff;
	initTimer0(1000);
	init_uart(bt);
	if (!bt_Is_Alive(bt)){
		if (!comm_scan_baudrate(bt)){
			return FALSE;
		}
		else {
			xsprintf(bt_data_buff, "Connected @ %d: ", bt->baudrate);
			return true;
		}
	}
	return true;
}


Bool bt_Is_Alive(BT_CMD_t* bt){
	bt_GetDataStruct(bt, AT);
	memset(bt->data_buffer, 0, BT_DATA_BUFF);
	if ( !memcmp (bt->data_buffer, (const char*)"OK", 2) ) return TRUE;
	else return FALSE;
}


char* bt_Get_Version(BT_CMD_t* bt){
	if (bt->bt_state != BT_AT_MODE)
		return ("CMD Get Version - Not AT Mode...");
	bt_GetDataStruct(bt, ATVER);
	memset(bt->data_buffer, 0, BT_DATA_BUFF);
	return parse_hc06_command(bt);
}

char*  bt_ChangeDisplayName(BT_CMD_t* bt, const char* name){

	if (CMD_LEN(name) > 20)
		return ("Name Too long...");	// Name string lenght can not be greater than 20 bytes
	if (bt->bt_state != BT_AT_MODE)
		return ("CMD Change Display Name - Not AT Mode...");

	bt_GetDataStruct(bt, ATNAME);

	memset(bt_cmd_buffer, 0, BT_DATA_BUFF);

	strcat (bt_cmd_buffer, bt->cmd);
	strcat (bt_cmd_buffer, name);

	bt->cmd_len = CMD_LEN(bt_cmd_buffer);
	bt->cmd = bt_cmd_buffer;
	return parse_hc06_command(bt);
}


char* bt_ChangePIN(BT_CMD_t* bt, const char* pin){

	if (bt->bt_state != BT_AT_MODE)
		return ("CMD Change PIN - Not AT Mode...");
	if (CMD_LEN(pin) > 4)
		return ("PIN Too long...");	// Name string lenght can not be greater than 4 bytes

	bt_GetDataStruct(bt, ATPIN);

	memset(bt_cmd_buffer, 0, BT_DATA_BUFF);

	strcat (bt_cmd_buffer, bt->cmd);
	strcat (bt_cmd_buffer, pin);

	bt->cmd_len = CMD_LEN(bt_cmd_buffer);
	bt->cmd = bt_cmd_buffer;
	return parse_hc06_command(bt);
}


char* bt_Change_Baudrate(BT_CMD_t* bt, int baud){

	if (bt->bt_state != BT_AT_MODE)
		return ("CMD Change Baud Rate - Not AT Mode...");
	if (baud > 1382400)
		return ("BaudRate Too high, not supported...");	// Baud rate greater than 1382400, max rate

	CMD_TYPE_t cmd_type = get_baudrate_type(baud);

	bt_GetDataStruct(bt, cmd_type);

	memset(bt->data_buffer, 0, BT_DATA_BUFF);			// Clean data buffer to get proper response content

	return parse_hc06_command(bt);
}


BT_STATE_t bt_ModuleMonitor(BT_CMD_t* bt)
{
	if ( bt->bt_state == BT_CONN ){
		cli_init(msg);
		bt->bt_state = BT_ALREADY_CONN;
	}
	if (bt->bt_state == BT_ALREADY_CONN && bt_data_rx){
		cli_on_rx_char(bt_Rx_Data);
		bt_data_rx = FALSE;
	}
	return bt->bt_state;
}


//************************************************************
//			COMMAND DEFINITIONS
//************************************************************
static const char* bt_cmd_rtc_set_fn(u8_t argc, char* argv[]) {
	// rtc set dd/mm/yyyy 13:12:00
	RTC rtc;
	char tmp[2];
	char* ptr1,*ptr2;
	long pData;
	int i=0;
	if (argc != 0) {
		// Get arguments from command line
//		xprintf("Argument 1 %s\n", argv[1]);
		ptr2 = argv[0];
		memcpy(tmp,ptr2,2);
		ptr1 = tmp;
		xatoi(&ptr1, &pData);
		rtc.mday = (BYTE)pData;
		while (*ptr2++ != '/');
		memcpy(tmp,ptr2+i,2);
		ptr1 = tmp;
		xatoi(&ptr1, &pData);
		rtc.month = (BYTE)pData;



		xprintf("Argument 0 %s and Variable 1-> %u/%u\n", argv[0], rtc.mday,rtc.month);

//		rtc.mday = (BYTE)tmpdata;
//		while(*ptr++ != '/');
//		++argv;
//		xatoi(&ptr,&tmpdata);
//		rtc.month = (BYTE)tmpdata;
//		while(*ptr++ != '/');
//		++argv;
//		xatoi(&ptr, &tmpdata);
//		rtc.year = (BYTE)tmpdata;
//
//
//		// set the time on RTC on chip
//
//		//Get time and print it back
//		xprintf("Date set@ %02d:%02d:%02d\n", rtc.mday, rtc.month, rtc.year);
//		return "Time Set DONE...";
//	} else {
//		//Get time and print it back
//		return "NO arguments received...";
	}
	return NULL;
}


static const char* bt_cmd_rtc_set_time_fn(u8_t argc, char* argv[]) {
	RTCTIME_s time;
	if (argc != 0) {
		// Get arguments from command line
		cli_util_argv_to_u8(0, 0, 23);
		time.HOUR = cli_argv_val.u8;
		cli_util_argv_to_u8(1, 0, 59);
		time.MIN = cli_argv_val.u8;
		cli_util_argv_to_u8(2, 0, 59);
		time.SEC = cli_argv_val.u8;

		// set the time on RTC on chip
		SetClock_Time(&time);
		time.TIME_TYPE = _24HOUR;
		GetClock_Time(&time);

		//Get time and print it back
		xprintf("Time set@ %02d:%02d:%02d\n", time.HOUR, time.MIN, time.SEC);
		return "Time Set DONE...";
	} else {
		time.TIME_TYPE = _24HOUR;
		GetClock_Time(&time);
		//Get time and print it back
		xprintf("Time set@ %02d:%02d:%02d\n", time.HOUR, time.MIN, time.SEC);
		return "NO arguments received...";
	}
}

static const char* bt_cmd_rtc_set_date_fn(u8_t argc, char* argv[]) {
	RTCDATE_s date;

	if (argc != 0) {
		GetClock_Date(&date);

		// Get arguments from command line
		cli_util_argv_to_u8(0, 0, 31);
		date.DOM = cli_argv_val.u8;
		cli_util_argv_to_u8(1, 1, 12);
		date.MONTH = cli_argv_val.u8;
		cli_util_argv_to_u16(2, 2000, 2099);
		date.YEAR = cli_argv_val.u16;
		cli_util_argv_to_u8(3, 1, 7);
		date.DOW = cli_argv_val.u8;

		SetCLock_Date(&date);
		GetClock_Date(&date);
		xprintf("Date set@ %02d/%02d/%02d %s %s\n", date.DOM, date.MONTH,
				date.YEAR, date.sDOW, date.sMONTH);
		return "Date Set DONE...";
	} else {
		GetClock_Date(&date);
		xprintf("Date set@ %02d/%02d/%02d %s %s\n", date.DOM, date.MONTH,
				date.YEAR, date.sDOW, date.sMONTH);
		return "NO args received...";
	}
}

static const char* bt_cmd_rtc_show_datetime_fn(u8_t argc, char* argv[]) {
	RTCDATE_s date;
	RTCTIME_s time;

	time.TIME_TYPE = _24HOUR;
	GetClock_Date(&date);
	GetClock_Time(&time);

	xprintf("%02d/%s/%02d %s %02d:%02d:%02d\n", date.DOM, date.sMONTH,
			date.YEAR, date.sDOW, time.HOUR, time.MIN, time.SEC);
	return NULL;
}



/*
 * CLEAR SCREEN
 */
static const char* bt_cmd_clear_screen_fn(u8_t argc, char* argv[]) {
	vt100_clr_screen();
	return NULL;
}



// Declare CLI command list and add commands and groups
//----------------------------------
// RTC related commands
CLI_CMD_CREATE(bt_cmd_set, "set", 1, 1, "<dd/mm/yyyy> <hh:mm:ss>", "Set RTC date and time ")
CLI_CMD_CREATE(bt_cmd_time_set, "time", 0, 3, "<hh mm ss> in 24H format ", "Set RTC time ")
CLI_CMD_CREATE(bt_cmd_date_set, "date", 0, 4, "<dd mm yyyy day#> sun=1,...","Set RTC date ")
CLI_CMD_CREATE(bt_cmd_time_show, "show", 0, 0, "", "Show current time and date on system rtc")
//RTC commands grouped as RTC
CLI_GROUP_CREATE(bt_group_rtc, "rtc")
	CLI_CMD_ADD(bt_cmd_set, bt_cmd_rtc_set_fn)
	CLI_CMD_ADD(bt_cmd_time_set, bt_cmd_rtc_set_time_fn)
	CLI_CMD_ADD(bt_cmd_date_set, bt_cmd_rtc_set_date_fn)
	CLI_CMD_ADD(bt_cmd_time_show, bt_cmd_rtc_show_datetime_fn)
CLI_GROUP_END()






CLI_CMD_CREATE(bt_cmd_help, "help", 0, 1, "<cmd(s) starts with...>","Display list of commands with help. Optionally, the list can be reduced.")
CLI_CMD_CREATE(bt_cmd_cls,  "cls", 0, 0,"",	"Clear Display, must be used in VT100 emulation...")


CLI_CMD_LIST_CREATE()
	CLI_GROUP_ADD(bt_group_rtc)
	CLI_CMD_ADD (bt_cmd_cls, bt_cmd_clear_screen_fn)
	CLI_CMD_ADD (bt_cmd_help, cli_cmd_help_fn)
CLI_CMD_LIST_END()

//************************************************************
//			END OF COMMAND DEFINITIONS
//************************************************************

