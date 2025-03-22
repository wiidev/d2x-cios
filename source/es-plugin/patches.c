/*
 * ES plugin for Custom IOS.
 *
 * Copyright (C) 2010 Waninkoko.
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
	u32 open;
	u32 ioctlv;
	u32 launchTitle;
	u32 memcpy;
	u32 printf;
	u32 snprintf;
} esAddrInfo;

/* Addresses */
u32 addrOpen   = 0;
u32 addrIoctlv = 0;

/* Function pointers */
s32   (*ES_LaunchTitle)(u32 tidh, u32 tidl, void *view, u32 reset) = 0;
void *(*ES_memcpy)(void *dst, void *src, s32 n)                    = 0;
s32   (*ES_printf)(const char * fmt, ...)                          = 0;
s32   (*ES_snprintf)(char *str, u32 size, const char *fmt, ...)    = 0;


static void __Patch_EsModule(esAddrInfo *aInfo)
{
	/* Patch OPEN handler */
	DCWrite32(aInfo->open    , 0x4B004718);
	DCWrite32(aInfo->open + 4, (u32)ES_EmulateOpen);

	/* Patch IOCTLV handler */
	DCWrite32(aInfo->ioctlv    , 0x4B004718);
	DCWrite32(aInfo->ioctlv + 4, (u32)ES_EmulateIoctlv);

	/* Set addresses */
	addrOpen   = aInfo->open   + 8 + 1;
	addrIoctlv = aInfo->ioctlv + 8 + 1;

	/* Set function pointers */
	ES_LaunchTitle = (void *)aInfo->launchTitle + 1;
	ES_memcpy      = (void *)aInfo->memcpy      + 0;
	ES_printf      = (void *)aInfo->printf      + 1;
	ES_snprintf    = (void *)aInfo->snprintf    + 1;
}

s32 Patch_EsModule(void)
{
	switch (ios.esVersion) {
	/** 06/03/09 07:46:02 **/
	case 0x4A262A3A: {	// IOS: 70v6687
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104CA0,	// launchTitle
			0x2010A5E0,	// memcpy
			0x2010A728,	// printf
			0x2010ABD0	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	// FIX d2x v7 beta1
	// Added missing base identification and patch
	/** 11/24/08 15:36:08 **/
	case 0x492AC9E8: {	// IOS: 60v6174
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104770,	// launchTitle
			0x201098FC,	// memcpy
			0x20109A44,	// printf
			0x20109EEC	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	/** 03/03/10 10:40:14 **/
	case 0x4B8E90EE: {	// IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104CF4,	// launchTitle
			0x2010A6A8,	// memcpy
			0x2010A7F0,	// printf
			0x2010AC98	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	// FIX d2x v5 beta1
	// Fixed wrong base identification
	/** 03/01/10 03:26:03 **/
	case 0x4B8B882B: {	// IOS: 37v5662, 53v5662, 55v5662
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104818,	// launchTitle
			0x20109870,	// memcpy
			0x201099B8,	// printf
			0x20109E60	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	/** 03/01/10 03:18:58 **/
	case 0x4B8B3222: {	// IOS: 36v3607, 38v4123
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104544,	// launchTitle
			0x201091F4,	// memcpy
			0x2010933C,	// printf
			0x201097E4	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	/** 04/03/12 12:05:51 **/
	case 0x4F7AE79F: {	// vIOS: 38v4380
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104600,	// launchTitle
			0x20109450,	// memcpy
			0x20109598,	// printf
			0x20109A40	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	/** 04/02/12 14:00:51 **/
	case 0x4F79B113: {	// vIOS: 56v5918, 57v6175, 58v6432
		static esAddrInfo addrInfo = {
			0x20100048,	// open
			0x201000CC,	// ioctlv
			0x20104DB0,	// launchTitle 
			0x2010A904,	// memcpy
			0x2010AA4C,	// printf
			0x2010AEF4	// snprintf
		};
		__Patch_EsModule(&addrInfo);
		break;
	}

	default:
		/* Unknown version */
		return IOS_ERROR_ES;
	}

	return 0;
}
