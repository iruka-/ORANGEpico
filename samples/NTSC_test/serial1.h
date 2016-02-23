#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <p32xxxx.h>			// always in first place to avoid conflict with const.h ON
#include "utype.h"
#include "fifo.h"

    #ifndef INPUT
        #define INPUT       1
    #endif

    #ifndef OUTPUT
        #define OUTPUT      0
    #endif

#define UART1			1
#define UART2			2
// bit 15 ON: UARTx Enable bit
#define UART_DISABLE							0x0000
#define UART_ENABLE								0x8000

// bit 13 SIDL: Stop in Idle Mode bit
#define UART_ENABLE_STOP_ON_IDLE				0x2000

// bit 12 IREN: IrDA Encoder and Decoder Enable bit
#define UART_ENABLE_IRDA						0x1000

// bit 11 RTSMD: Mode Selection for UxRTS Pin bit
#define UART_RTS_WHEN_RX_NOT_FULL			0x000
#define UART_RTS_WHEN_TX_NOT_EMPTY			0x800

// bit 9-8 UEN<1:0>: UARTx Enable bits
#define UART_ENABLE_PINS_BIT_CLOCK			0x300 // UxTX, UxRX, and UxBCLK pins are enabled and used; UxCTS pin is controlled by port latches
#define UART_ENABLE_PINS_CTS_RTS				0x200 // UxTX, UxRX, UxCTS, and UxRTS pins are enabled and used
#define UART_ENABLE_PINS_RTS					0x100 // UxTX, UxRX and UxRTS pins are enabled and used; UxCTS pin is controlled by port latches
#define UART_ENABLE_PINS_TX_RX_ONLY			0x000 // UxTX and UxRX pins are enabled and used; UxCTS and UxRTS/UxBCLK pins are controlled by port latches

// bit 7 WAKE: Enable Wake-up on Start bit Detect During Sleep mode bit
#define UART_ENABLE_WAKE_ON_START			0x80

// bit 6 LPBACK: UARTx Loopback Mode Select bit
#define UART_ENABLE_LOOPBACK					0x40

// bit 5 ABAUD: Auto-Baud Enable bit

// bit 4 RXINV: Receive Polarity Inversion bit
#define UART_INVERT_RECEIVE_POLARITY		0x10

// bit 3 BRGH: High Baud Rate Enable bit
//#define UART_ENABLE_HIGH_SPEED		0x00000008
#define UART_ENABLE_HIGH_SPEED				1
#define UART_ENABLE_STANDARD_SPEED			0

// bit 2-1 PDSEL<1:0>: Parity and Data Selection bits
 
#define UART_9_BITS_NO_PARITY					0x06
#define UART_8_BITS_ODD_PARITY				0x04
#define UART_8_BITS_EVEN_PARITY				0x02
#define UART_8_BITS_NO_PARITY					0x00

// bit 0 STSEL: Stop Selection bit
#define UART_STOP_BITS_2						0x01	// Enables generation of 2 stop bits per frame.
#define UART_STOP_BITS_1						0x00	// Enables generation of 1 stop bit per frame (default).

// -------------------------------------------------------------------------
// UxSTA
// -------------------------------------------------------------------------

// bit 24 ADM_EN: Automatic Address Detect Mode Enable bit

// bit 15-14 UTXISEL<1:0>: Tx Interrupt Mode Selection bits
#define UART_INTERRUPT_ON_TX_BUFFER_EMPTY	0x8000
#define UART_INTERRUPT_ON_TX_DONE			0x4000
#define UART_INTERRUPT_ON_TX_NOT_FULL		0x0000

// bit 13 UTXINV: Transmit Polarity Inversion bit
#define UART_INVERT_TRANSMIT_POLARITY		0x2000

// bit 12 URXEN: Receiver Enable bit
#define UART_RX_ENABLED							0x1000	// UARTx receiver is enabled, UxRX pin controlled by UARTx (if ON = 1)
#define UART_RX_DISABLED						0x0000	// UARTx receiver is disabled, the UxRX pin is ignored by the UARTx module. UxRX pin controlled by PORT.

// bit 11 UTXBRK: Transmit Break bit

// bit 10 UTXEN: Transmit Enable bit
#define UART_TX_ENABLED							0x400		// UARTx transmitter enabled, UxTX pin controlled by UARTx (if ON = 1)
#define UART_TX_DISABLED						0x000		// UARTx transmitter disabled, any pending transmission is aborted and buffer is reset. UxTX pin controlled by PORT.

#define UART_RX_TX_ENABLED						0x1400

// bit 7-6 URXISEL<1:0>: Receive Interrupt Mode Selection bit
#define UART_INTERRUPT_ON_RX_FULL			0xC0
#define UART_INTERRUPT_ON_RX_HALF_FULL		0x40
#define UART_INTERRUPT_ON_RX_NOT_EMPTY		0x00

// bit 5 ADDEN: Address Character Detect (bit 8 of received data = 1)

// UART_CONFIGURATION
#define UART_SUPPORT_IEEE_485					0x00000900

// UART_LINE_STATUS;
#define UART_TRANSMITTER_NOT_FULL			0x00000200	// The transmitter is able to accept data to transmit.
#define UART_TRANSMITTER_EMPTY				0x00000100	// The transmitter is empty (no data is available to transmit).
#define UART_RECEIVER_IDLE						0x00000010	// The receiver is currently idle.
#define UART_PARITY_ERROR						0x00000008	// A received data parity error was detected.
#define UART_FRAMING_ERROR						0x00000004	// Data was received that violated the framing protocol
#define UART_OVERRUN_ERROR						0x00000002	// The UART has received more data than it can buffer.  Data has been lost.
#define UART_DATA_READY							0x00000001	// UART data has been received and is avaiable in the FIFO.


void	SerialSetDataRate(u8 port, u32 baudrate);
//void	SerialEnable(u8 port, u32 config);
void	SerialSetLineControl(u8 port, u32 config);
void	SerialFlush(u8 port);
void	SerialPinConfigure(u8 port);
void	SerialIntConfigure(u8 port, u8 priority, u8 subpriority);
void	SerialConfigure(u8 port, u32 config, u32 enable, u32 baudrate);

void _MIPS32 Serial1WriteChar(char c);
int		Serial1Available();
int		Serial1Read();
int		Serial1GetKey();
void	Serial1Interrupt(void);


void UARTwrite(unsigned char *buf,int len);
int	 UARTgetpacket(char *buf,int size);

#define	GetPeripheralClock() get_PeripheralClock()

#endif	/* __SERIAL__ */
