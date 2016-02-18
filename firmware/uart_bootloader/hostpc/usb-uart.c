/****************************************************************************
 *	usb to uart interface conversion
 ****************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <setupapi.h>
#include <dbt.h>

#include  "rs232c.h"
#include  "util.h"
#include  "usb-uart.h"

int	usbOpen(int vendorID,int productID)
{
	return 0;
}

int	usbWrite(uchar *usbBuf,int len,uchar *readBuf,int readlen)
{
	if(len) {
		uchar buf[256];
		memcpy(buf+1,usbBuf,len);
		buf[0]=0x01;
		RS_putdata(buf,len+1);
	}

	if(readlen) {
		RS_getdata(readBuf,readlen);
	}
	return 0;	// Ok.
}


void usbClose(void)
{
}
