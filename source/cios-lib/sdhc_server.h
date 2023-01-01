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

#ifndef _SDHC_H_
#define _SDHC_H_

#include "ipc.h"
#include "types.h"

/* SDHC descriptor */
#define SDHC_FD               0x5D4CFD

/* Prototypes */
s32 SDHC_RegisterDevice(s32 queuehandle);
s32 SDHC_CheckDevicePath(char *devpath);
s32 SDHC_Close(void);
s32 SDHC_Ioctlv(u32 cmd, ioctlv *vector, u32 inlen, u32 iolen);

#endif

