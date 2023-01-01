/*   
	Custom IOS Library

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

#include "ipc.h"
#include "sdio.h"
#include "syscalls.h"
#include "types.h"
#include "libwbfs/libwbfs.h"

/* Constants */
#define SECTOR_SIZE			0x200
#define WBFS_BASE			(('W'<<24)|('F'<<16)|('S'<<8))

/* IOCTL commands */
#define SDHC_IOCTL_INIT			0x01
#define SDHC_IOCTL_READ			0x02
#define SDHC_IOCTL_WRITE		0x03
#define SDHC_IOCTL_ISINSERTED		0x04
#define SDHC_IOCTL_WBFS_OPEN_DISC	(WBFS_BASE + 0x1)
#define SDHC_IOCTL_WBFS_READ_DISC	(WBFS_BASE + 0x2)


/* Variables */
static char        *__sdhc_device = "/dev/sdio/sdhc";
static wbfs_t      *hdd           = NULL;
static wbfs_disc_t *disc          = NULL;


static int __SDHC_WBFS_ReadSector(void *cbdata, u32 lba, u32 count, void *buffer)
{
	s32 ret;

	/* Read data */
	ret = sdio_ReadSectors(lba, count, buffer);
	if (!ret)
		return 1;

	/* Invalidate range */
	os_sync_before_read(buffer, SECTOR_SIZE * count);

	return 0;
}

static s32 __SDHC_WBFS_OpenDisc(u8 *discid)
{
	s32 ret;

	/* Initialize SDIO */
	ret = sdio_Startup();
	if (!ret)
		return 1;

	/* Close disc */
	if (disc)
		wbfs_close_disc(disc);

	/* Close SD */
	if (hdd)
		wbfs_close(hdd);

	/* Open SD */
	hdd = wbfs_open_hd(__SDHC_WBFS_ReadSector, NULL, NULL, SECTOR_SIZE, 0, 0);
	if (!hdd)
		return 2;

	/* Open disc */
	disc = wbfs_open_disc(hdd, discid);
	if (!disc)
		return 3;

	return 0;
}

static s32 __SDHC_WBFS_Read(void *buffer, u32 len, u32 offset)
{
	/* No disc opened */
	if (!disc)
		return 1;

	/* Disc read */
	return wbfs_disc_read(disc, offset, buffer, len);
}

s32 SDHC_RegisterDevice(s32 queuehandle)
{
	/* Register SDHC device */
	return os_device_register(__sdhc_device, queuehandle);
}

s32 SDHC_CheckDevicePath(char *devpath)
{
	return !strcmp(devpath, __sdhc_device);
}

s32 SDHC_Close(void)
{
	return !sdio_Shutdown();
}

s32 SDHC_Ioctlv(u32 cmd, ioctlv *vector, u32 inlen, u32 iolen)
{
	s32 ret = IPC_EINVAL;

	/* Invalidate cache */
	InvalidateVector(vector, inlen, iolen);

	/* Parse IOCTLV command */
	switch (cmd) {
	/** Initialize SDHC **/
	case SDHC_IOCTL_INIT: {
		/* Initialize SDIO */
		ret = !sdio_Startup();

		break;
	}

	/** Read sectors **/
	case SDHC_IOCTL_READ: {
		u32   sector     = *(u32 *)(vector[0].data);
		u32   numSectors = *(u32 *)(vector[1].data);
		void *buffer     = vector[2].data;

		/* Read sectors */
		ret = !sdio_ReadSectors(sector, numSectors, buffer);

		break;
	}

	/** Write sectors **/
	case SDHC_IOCTL_WRITE: {
		u32   sector     = *(u32 *)(vector[0].data);
		u32   numSectors = *(u32 *)(vector[1].data);
		void *buffer     = vector[2].data;

		/* Write sectors */
		ret = !sdio_WriteSectors(sector, numSectors, buffer);

		break;
	}

	/** Check for SD card **/
	case SDHC_IOCTL_ISINSERTED: {
		/* Check if SD card is inserted */
		ret = !sdio_IsInserted();

		break;
	}

	/** Open WBFS disc **/
	case SDHC_IOCTL_WBFS_OPEN_DISC: {
		u8 *discid = (u8 *)(vector[0].data);

		/* Open WBFS disc */
		ret = __SDHC_WBFS_OpenDisc(discid);

		break;
	}

	/** Read WBFS disc **/
	case SDHC_IOCTL_WBFS_READ_DISC: {
		u32   offset = *(u32 *)(vector[0].data);
		u32   len    = *(u32 *)(vector[1].data);
		void *buffer = vector[2].data;

		/* Read WBFS disc */
		ret = __SDHC_WBFS_Read(buffer, len, offset);
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
