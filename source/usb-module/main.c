/*   
	Custom IOS Module (USB)

	Copyright (C) 2008 neimod.
	Copyright (C) 2009 WiiGator.
	Copyright (C) 2009 Waninkoko.
	Copyright (C) 2011 davebaol.
	Copyright (C) 2022 blackb0x.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>

#include "ipc.h"
#include "mem.h"
#include "module.h"
#include "sdhc_server.h"
#include "stealth.h"
#include "syscalls.h"
#include "timer.h"
#include "types.h"
#include "usb2.h"
#include "usbglue.h"
#include "wbfs.h"


/* Variables */
char *moduleName = "USBS/SDHC";
s32 queuehandle = -1;

/* Async replies */
areply usbCb[2] = { { -1, -1 } };


static s32 __USB_Ioctlv(u32 cmd, ioctlv *vector, u32 inlen, u32 iolen)
{
        s32 ret = IPC_EINVAL;

        /* Invalidate cache */
        InvalidateVector(vector, inlen, iolen);

        /* Parse IOCTLV command */
        switch (cmd) {
        /** Initialize USB **/
        case IOCTL_USB_INIT:
	case IOCTL_UMS_INIT: {
		/* Initialize USB */
		ret = !usbstorage_Startup();

		break;
	}

	/** Read USB sectors **/
	case IOCTL_USB_READ:
	case IOCTL_UMS_READ_SECTORS: {
		void *buffer = vector[2].data;

		u32 sector = *(u32 *)vector[0].data;
		u32 count  = *(u32 *)vector[1].data;

		/* Read sectors */
		ret = !usbstorage_ReadSectors(sector, count, buffer);

		break;
	}

	/** Write USB sectors **/
	case IOCTL_USB_WRITE:
	case IOCTL_UMS_WRITE_SECTORS: {
		void *buffer = vector[2].data;

		u32 sector = *(u32 *)vector[0].data;
		u32 count  = *(u32 *)vector[1].data;

		/* Write sectors */
		ret = !usbstorage_WriteSectors(sector, count, buffer);

		break;
	}

	/** Get USB capacity **/
	case IOCTL_UMS_GET_CAPACITY: {
		u32 *output = (u32 *)vector[0].data;

		u32 nbSector, sectorSz;

		/* Get device capacity */
		ret = !usbstorage_ReadCapacity(&sectorSz, &nbSector);

		/* Copy data */
		if (!ret) {
			*output = sectorSz;
			ret     = nbSector;
		}

		break;
	}

	/** Check for USB device **/
	case IOCTL_USB_ISINSERTED: {
		/* Check if USB device is inserted */
		ret = !usbstorage_IsInserted();

		break;
	}

	/** Unmount USB device **/
	case IOCTL_USB_UNMOUNT: {
		/* Shutdown USB */
		ret = !usbstorage_Shutdown();

		break;
	}

	/** Open WBFS disc **/
	case IOCTL_WBFS_OPEN_DISC: {
		u8 *discid = (u8 *)(vector[0].data);

		/* Open WBFS disc */
		ret = WBFS_OpenDisc(discid);

		break;
	}

	/** Read WBFS disc **/
	case IOCTL_WBFS_READ_DISC: {
		u32   offset = *(u32 *)(vector[0].data);
		u32   len    = *(u32 *)(vector[1].data);
		void *buffer = vector[2].data;

		/* Read WBFS disc */
		ret = WBFS_Read(buffer, len, offset);
		if (ret)
			ret = 0x8000;

		break;
	}

	default:
		break;
	}

	/* Flush cache */
	FlushVector(vector, inlen, iolen);

	return ret;
}

static s32 __USB_Callback(u32 message)
{
	/* Device change callback */
	if (message == MESSAGE_DEVCHANGE) {
		USB_DeviceChange(usbCb[0].result);
		return 0;
	}

	/* Attach finish callback */
	if (message == MESSAGE_ATTACH) {
		USB_AttachFinish(usbCb[1].result);
		return 0;
	}

	/* Mount device callback */
	if (message == MESSAGE_MOUNT) {
		usbstorage_IsInserted();
		return 0;
	}

	return -1;
}

s32 __USB_Initialize(void)
{
	/* Heap space */
	static u32 heapspace[0x2000] ATTRIBUTE_ALIGN(32);

	void *buffer = NULL;
	s32   ret;

	/* Initialize memory heap */
	ret = Mem_Init(heapspace, sizeof(heapspace));
	if (ret < 0)
		return ret;

	/* Initialize timer subsystem */
	ret = Timer_Init();
	if (ret < 0)
		return ret;

	/* Allocate queue buffer */
	buffer = Mem_Alloc(0x80);
	if (!buffer)
		return IPC_ENOMEM;

	/* Create message queue */
	ret = os_message_queue_create(buffer, 32);
	if (ret < 0)
		return ret;

	/* Register devices */
	os_device_register(DEVICE_NAME, ret);

	/* Copy queue handler */
	queuehandle = ret;

	/* Register SDHC device */
	SDHC_RegisterDevice(queuehandle);

	return 0;
}


int main(void)
{
	s32 ret;

	/* Print info */
	svc_write("$IOSVersion: USBS: " __DATE__ " " __TIME__ " 64M$\n");

	/* Initialize module */
	ret = __USB_Initialize();
	if (ret < 0)
		return ret;

	/* Main loop */
	while (1) {
		ipcmessage *message = NULL;

		/* Wait for message */
		os_message_queue_receive(queuehandle, (void *)&message, 0);

		/* Check callback */
		ret = __USB_Callback((u32)message);
		if (!ret)
			continue;

		switch (message->command) {
		case IOS_OPEN: {

			/* Block opening request if a title is running */
			ret = Stealth_CheckRunningTitle(NULL);
			if (ret) {
				ret = IPC_ENOENT;
				break;
			}

			char *devpath = message->open.device;

			/* Check device path */
			if (!strcmp(devpath, DEVICE_NAME)) {
				ret = message->open.resultfd;
				break;
			}

			/* SDHC module device */
			if (SDHC_CheckDevicePath(devpath)) {
				ret = SDHC_FD;
				break;
			}

			/* Wrong device */
			ret = IPC_ENOENT;

			break;
		}

		case IOS_CLOSE: {
			if (message->fd == SDHC_FD) {
				/* Close SDHC device */
				ret = SDHC_Close();
			} else {
				/* Do nothing */
				ret = 0;
			}
			break;
		}

		case IOS_IOCTLV: {
			ioctlv *vector = message->ioctlv.vector;
			u32     inlen  = message->ioctlv.num_in;
			u32     iolen  = message->ioctlv.num_io;
			u32     cmd    = message->ioctlv.command;

			/* Parse IOCTLV message */
			if (message->fd == SDHC_FD)
				ret = SDHC_Ioctlv(cmd, vector, inlen, iolen);
			else
				ret = __USB_Ioctlv(cmd, vector, inlen, iolen);

			break;
		}

		default:
			/* Unknown command */
			ret = IPC_EINVAL;
		}

		/* Acknowledge message */
		os_message_queue_ack(message, ret);
	}
   
	return 0;
}
