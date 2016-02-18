/****************************************************************************
 File        : mphidflash.h
 Description : Common header file for all mphidflash sources.

 History     : 2009-02-19  Phillip Burgess
                 * Initial implementation
               2009-04-16  Phillip Burgess
                 * Bug fix for non-Intel and 64-bit platforms.
               2009-12-26  Thomas Fischl, Dominik Fisch (www.FundF.net)
                 * Renamed 'ubw32' to 'mphidflash'
               
 License     : Copyright (C) 2009 Phillip Burgess
               Copyright (C) 2009 Thomas Fischl, Dominik Fisch (www.FundF.net)

               This file is part of 'mphidflash' program.

               'mphidflash' is free software: you can redistribute it and/or
               modify it under the terms of the GNU General Public License
               as published by the Free Software Foundation, either version
               3 of the License, or (at your option) any later version.

               'mphidflash' is distributed in the hope that it will be useful,
               but WITHOUT ANY WARRANTY; without even the implied warranty
               of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
               See the GNU General Public License for more details.

               You should have received a copy of the GNU General Public
               License along with 'mphidflash' source code.  If not,
               see <http://www.gnu.org/licenses/>.

 ****************************************************************************/

#ifndef _USB_UART_H_
#define _USB_UART_H_

/* Values derived from Microchip HID Bootloader source */

/* Bootloader commands */
#define	QUERY_DEVICE      0x02
#define	UNLOCK_CONFIG     0x03
#define ERASE_DEVICE      0x04
#define PROGRAM_DEVICE    0x05
#define	PROGRAM_COMPLETE  0x06
#define GET_DATA          0x07
#define	RESET_DEVICE      0x08
#define PUT_DATA          0x09
#define EXEC_CODE         0x0a

/* Sub-commands for the ERASE_DEVICE command */
#define UNLOCKCONFIG      0x00
#define LOCKCONFIG        0x01

/* Response types for QUERY_DEVICE command */
#define	TypeProgramMemory 0x01
#define TypeEEPROM        0x02
#define TypeConfigWords   0x03
#define	TypeEndOfTypeList 0xFF

/* Error codes returned by various functions */

typedef enum
{
	ERR_NONE = 0,        /* Success (non-error) */
	ERR_CMD_ARG,
	ERR_CMD_UNKNOWN,
	ERR_DEVICE_NOT_FOUND,
	ERR_USB_INIT1,
	ERR_USB_INIT2,
	ERR_USB_OPEN,
	ERR_USB_WRITE,
	ERR_USB_READ,
	ERR_HEX_OPEN,
	ERR_HEX_STAT,
	ERR_HEX_MMAP,
	ERR_HEX_SYNTAX,
	ERR_HEX_CHECKSUM,
	ERR_HEX_RECORD,
	ERR_VERIFY,
	ERR_EOL              /* End-of-list, not actual error code */
} ErrorCode;

/* Function prototypes */

int		usbOpen(int vendorID,int productID);
int		usbWrite(uchar *buf,int size,uchar *read,int readsize);
void	usbClose(void);

#define	PACKET_SIZE		64

#endif /* _MPHIDFLASH_H_ */
