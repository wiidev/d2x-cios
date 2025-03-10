/*
 * DIP plugin for Custom IOS.
 *
 * Copyright (C) 2008-2010 Waninkoko, WiiGator.
 * Copyright (C) 2011 davebaol.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ios.h"
#include "plugin.h"
#include "syscalls.h"
#include "tools.h"


typedef struct {
	u32 init;
	u32 ioctl;
	u32 cmd;
	u32 readHash;
	u32 alloc;
	u32 free;
	u32 printf;
	u32 readCtrl;
} dipAddrInfo;

/* Addresses */
u32 addr_handleInitDrive = 0;
u32 addr_handleIoctl     = 0;
u32 addr_handleCmd       = 0;
u8 *dip_readctrl         = 0;

/* Function pointers */
s32   (*DI_Printf)(const char * fmt, ...) = 0;
s32   (*DI_ReadHash)(void)                = 0; 
void *(*DI_Alloc)(u32 size, u32 align)    = 0;
void  (*DI_Free)(void *ptr)               = 0;


static void __Patch_DipModule(dipAddrInfo *aInfo)
{
	/* Patch DVD driver init stage 2 */
	DCWrite32(aInfo->init    , 0x4B004718);
	DCWrite32(aInfo->init + 4, (u32)DI_EmulateInitDrive);

	/* Patch IOCTL handler */
	DCWrite32(aInfo->ioctl,     0x4B004718);
	DCWrite32(aInfo->ioctl + 4, (u32)DI_EmulateIoctl);
		
	/* Patch command handler */
	DCWrite32(aInfo->cmd,     0x4B004718);
	DCWrite32(aInfo->cmd + 4, (u32)DI_EmulateCmd);

	/* Set addresses */
	addr_handleInitDrive = (aInfo->init  +  8) + 1;
	addr_handleIoctl     = (aInfo->ioctl + 12) + 1;
	addr_handleCmd       = (aInfo->cmd   + 12) + 1;
	dip_readctrl         = (u8 *) aInfo->readCtrl;

	/* Set function pointers */
	DI_ReadHash = (void *)aInfo->readHash + 1;
	DI_Alloc    = (void *)aInfo->alloc    + 1;
	DI_Free     = (void *)aInfo->free     + 1;
	DI_Printf   = (void *)aInfo->printf   + 1;
}

s32 Patch_DipModule(void)
{
	switch (ios.dipVersion) {
	/** 07/11/08 14:34:26 **/
	case 0x48776F72: {      // IOS: 37v5662, 53v5662, 55v5662
		static dipAddrInfo aInfo = {
			0x20200074,	// init
			0x20200400,	// ioctl
			0x20200EF8,	// cmd
			0x20202A70,	// readHash
			0x2020096C,	// alloc
			0x2020093C,	// free
			0x2020387C,	// printf
			0x2022DD60	// readCtrl
		};
		__Patch_DipModule(&aInfo);
		break;
	}

	/** 07/24/08 20:08:44 **/
	case 0x4888E14C: {      // IOS: 36v3607, 38v4123
		static dipAddrInfo aInfo = {
			0x20200068,	// init
			0x202003B8,	// ioctl
			0x20200D2C,	// cmd
			0x20202874,	// readHash
			0x202008C4,	// alloc
			0x20200898,	// free
			0x2020365C,	// printf
			0x2022CDAC	// readCtrl
		};
		__Patch_DipModule(&aInfo);
		break;
	}

	/** 04/03/12 12:00:16 **/
	case 0x4F7AE650: {      // vIOS: 38v4380
		static dipAddrInfo aInfo = {
			0x20200068,	// init
			0x202003B8,	// ioctl
			0x20200D2C,	// cmd
			0x20202888,	// readHash
			0x202008C4,	// alloc
			0x20200898,	// free
			0x20203670,	// printf
			0x2022CDAC	// readCtrl
		};
		__Patch_DipModule(&aInfo);
		break;
	}

	/** 11/24/08 15:39:09 **/
	case 0x492ACA9D:        // IOS: 60v6174
	/** 06/03/09 07:49:09 **/
	case 0x4A262AF5: {       // IOS: 56v5661, 57v5918, 58v6175, 61v5661, 70v6687, 80v6943
		static dipAddrInfo aInfo = {
			0x20200074,	// aInit
			0x20200400,	// aIoctl
			0x20200EF8,	// aCmd
			0x20202944,	// aReadHash
			0x2020096C,	// aAlloc
			0x2020093C,	// aFree
			0x20203750,	// aPrintf
			0x2022CD60	// aReadCtrl
		};
		__Patch_DipModule(&aInfo);
		break;
	}

	/** 04/02/12 14:03:54 **/
	case 0x4F79B1CA: {       // vIOS: 56v5918, 57v6175, 58v6432
		static dipAddrInfo aInfo = {
			0x20200074,	// aInit
			0x20200400,	// aIoctl
			0x20200EF8,	// aCmd
			0x20202958,	// aReadHash
			0x2020096C,	// aAlloc
			0x2020093C,	// aFree
			0x20203764,	// aPrintf
			0x2022DD60	// aReadCtrl
		};
		__Patch_DipModule(&aInfo);
		break;
	}

	default:
		/* Unknown version */
		return IOS_ERROR_DIP;
	}

	return 0;
}
