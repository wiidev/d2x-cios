/*   
	Custom IOS Module (MLOAD)

	Copyright (C) 2008 neimod.
	Copyright (C) 2010 Hermes.
	Copyright (C) 2010 Waninkoko.
	Copyright (C) 2011 davebaol.

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

#include "ios.h"
#include "swi.h"
#include "tools.h"
#include "types.h"


typedef struct {
	u32 signatureCheck1;
	u32 signatureCheck2;
	u32 identifyCheck1;
	u32 identifyCheck2;
	u32 openContentPerm1;
	u32 openContentPerm2;
	u32 openContentPerm3;
	u32 readContentPerm;
	u32 seekContentPerm;
	u32 setUidCheck;
	u32 titleVersionCheck;
	u32 titleDeleteCheck;
	u32 decryptCheck;
} esAddrInfo;

static void __Patch_EsModule(esAddrInfo *aInfo)
{
	/* Signature check */
	DCWrite16(aInfo->signatureCheck1, 0x2000);
	DCWrite16(aInfo->signatureCheck2, 0x2000);

	/* Identify check */
	DCWrite16(aInfo->identifyCheck1, 0x2803);
	DCWrite16(aInfo->identifyCheck2, 0x2803);

	/* Open content permissions */
	DCWrite8(aInfo->openContentPerm1, 0xE0);
	DCWrite8(aInfo->openContentPerm2, 0xE0);
	if (aInfo->openContentPerm3)	// Missing in base 36 and 38
		DCWrite8(aInfo->openContentPerm3, 0xE0);

	/* Read content permissions */
	DCWrite16(aInfo->readContentPerm,     0x46C0);
	DCWrite16(aInfo->readContentPerm + 4, 0x46C0);
	DCWrite8 (aInfo->readContentPerm + 8, 0xE0);

	/* Close content permissions */
	DCWrite16(aInfo->seekContentPerm    , 0x46C0);
	DCWrite16(aInfo->seekContentPerm + 4, 0x46C0);
	DCWrite8 (aInfo->seekContentPerm + 8, 0xE0);

	/* Set UID check */
	DCWrite16(aInfo->setUidCheck, 0x46C0);

	/* Title version check */
	DCWrite8(aInfo->titleVersionCheck, 0xE0);

	/* Title delete check */
	DCWrite8(aInfo->titleDeleteCheck, 0xE0);

	/* Decrypt check */
	if (aInfo->decryptCheck)		// Missing in base 36 and 38
		DCWrite8(aInfo->decryptCheck, 0xE0);
}

s32 Patch_DipModule(void)
{
	switch (ios.dipVersion) {
	/** 07/11/08 14:34:26 **/
	case 0x48776F72:      // IOS: 37v5662, 53v5662, 55v5662
		/* Unencrypted read limit */
		DCWrite32(0x20206640, 0x7ED40000);

		break;

	/** 07/24/08 20:08:44 **/
	case 0x4888E14C:      // IOS: 36v3607, 38v4123
		/* Unencrypted read limit */
		DCWrite32(0x2020636C, 0x7ED40000);

		break;

	/** 04/03/12 12:00:16 **/
	case 0x4F7AE650:      // vIOS: 38v4380
		/* Unencrypted read limit */
		DCWrite32(0x20206380, 0x7ED40000);

		break;

	/** 11/24/08 15:39:09 **/
	case 0x492ACA9D:       // IOS: 60v6174
	/** 06/03/09 07:49:09 **/
	case 0x4A262AF5:       // IOS: 56v5661, 57v5918, 58v6175, 61v5661, 70v6687, 80v6943
		/* Unencrypted read limit */
		DCWrite32(0x202066E4, 0x7ED40000);

		break;

	/** 04/02/12 14:03:54 **/
	case 0x4F79B1CA:       // vIOS: 56v5918, 57v6175, 58v6432
		/* Unencrypted read limit */
		DCWrite32(0x202066F8, 0x7ED40000);

		break;

	default:
		/* Unknown module version */
		return IOS_ERROR_DIP;
	}

	return 0;
}

s32 Patch_EsModule(void)
{
	switch (ios.esVersion) {
	/** 06/03/09 07:46:02 **/
	case 0x4A262A3A: {	// IOS: 70v6687
		static esAddrInfo addrInfo = {
			0x13A7547A,	// signatureCheck1 (added in d2x v8 r42)
			0x13A75626,	// signatureCheck2
			0x20100E74,	// identifyCheck1
			0x20100EEC,	// identifyCheck2
			0x20105290,	// openContentPerm1
			0x201052D0,	// openContentPerm2
			0x201052F4,	// openContentPerm3
			0x201053FC,	// readContentPerm
			0x20105498,	// seekContentPerm
			0x2010576A,	// setUidCheck
			0x20102C74,	// titleVersionCheck
			0x2010849A,	// titleDeleteCheck
			0x2010650C	// decryptCheck
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	/** 11/24/08 15:36:08 **/
	case 0x492AC9E8: {	// IOS: 60v6174
		static esAddrInfo addrInfo = {
			0x13A754FA,	// signatureCheck1
			0x13A756A6,	// signatureCheck2
			0x20100DA4,	// identifyCheck1
			0x20100E1C,	// identifyCheck2
			0x20104D60,	// openContentPerm1
			0x20104DA0,	// openContentPerm2
			0x20104DC4,	// openContentPerm3
			0x20104ECC,	// readContentPerm
			0x20104F68,	// seekContentPerm
			0x2010523A,	// setUidCheck
			0x20102800,	// titleVersionCheck
			0x20107B32,	// titleDeleteCheck
			0x20105FD0	// decryptCheck
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	/** 03/03/10 10:40:14 **/
	case 0x4B8E90EE: {	// IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
		static esAddrInfo addrInfo = {
			0x13A7547A,	// signatureCheck1  (added in d2x v8 r42)
			0x13A75626,	// signatureCheck2
			0x20100E74,	// identifyCheck1
			0x20100EEC,	// identifyCheck2
			0x201052E4,	// openContentPerm1
			0x20105324,	// openContentPerm2
			0x20105348,	// openContentPerm3
			0x20105450,	// readContentPerm
			0x201054A0,	// seekContentPerm
			0x201057BE,	// setUidCheck
			0x20102CB8,	// titleVersionCheck
			0x20108562,	// titleDeleteCheck
			0x20106560	// decryptCheck      (added in d2x v8 r42)
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	/** 03/01/10 03:26:03 **/
	case 0x4B8B882B: {	// IOS: 37v5662, 53v5662, 55v5662
		static esAddrInfo addrInfo = {
			0x13A750DE,	// signatureCheck1  (added in d2x v8 r42)
			0x13A752E6,	// signatureCheck2
			0x20100D46,	// identifyCheck1
			0x20100DBE,	// identifyCheck2
			0x20104DF6,	// openContentPerm1
			0x20104E1A,	// openContentPerm2
			0x20104E3E,	// openContentPerm3
			0x20104F38,	// readContentPerm
			0x20104F88,	// seekContentPerm
			0x201052A6,	// setUidCheck
			0x20102818,	// titleVersionCheck
			0x20107BAA,	// titleDeleteCheck
			0x20106048	// decryptCheck      (added in d2x v8 r42)
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	/** 03/01/10 03:18:58 **/
	case 0x4B8B8682: {	// IOS: 36v3607, 38v4123
		static esAddrInfo addrInfo = {
			0x13A750DE,	// signatureCheck1  (added in d2x v8 r42)
			0x13A75266,	// signatureCheck2
			0x20100CC4,	// identifyCheck1
			0x20100D3C,	// identifyCheck2
			0x20104B20,	// openContentPerm1
			0x20104B44,	// openContentPerm2
			0x0,		// openContentPerm3
			0x20104C3C,	// readContentPerm
			0x20104C8C,	// seekContentPerm
			0x20104FAA,	// setUidCheck
			0x201026CC,	// titleVersionCheck
			0x20107642,	// titleDeleteCheck
			0x0		// decryptCheck
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	/** 04/03/12 12:05:51 **/
	case 0x4F7AE79F: {	// vIOS: 38v4380
		static esAddrInfo addrInfo = {
			0x13A750DE,	// signatureCheck1  (added in d2x v8 r42)
			0x13A75266,	// signatureCheck2
			0x20100CC4,	// identifyCheck1
			0x20100D3C,	// identifyCheck2
			0x20104D7C,	// openContentPerm1
			0x20104DA0,	// openContentPerm2
			0x0,		// openContentPerm3
			0x20104E98,	// readContentPerm
			0x20104EE8,	// seekContentPerm
			0x20105206,	// setUidCheck
			0x201026EC,	// titleVersionCheck
			0x2010789E,	// titleDeleteCheck
			0x0		// decryptCheck
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	/** 04/02/12 14:00:51 **/
	case 0x4F79B113: {	// vIOS: 56v5918, 57v6175, 58v6432
		static esAddrInfo addrInfo = {
			0x13A7547A,	// signatureCheck1
			0x13A75626,	// signatureCheck2
			0x20100E74,	// identifyCheck1
			0x20100EEC,	// identifyCheck2
			0x20105540,	// openContentPerm1
			0x20105580,	// openContentPerm2
			0x201055A4,	// openContentPerm3
			0x201056AC,	// readContentPerm
			0x201056FC,	// seekContentPerm 
			0x20105A1A,	// setUidCheck
			0x20102CD8,	// titleVersionCheck
			0x201087BE,	// titleDeleteCheck
			0x201067BC	// decryptCheck
		};

		__Patch_EsModule(&addrInfo);

		break;
	}

	default:
		/* Unknown module version */
		return IOS_ERROR_ES;
	}

	return 0;
}

s32 Patch_FfsModule(void)
{
	switch (ios.ffsVersion) {
	/** 12/24/08 13:48:17 **/
	case 0x49523DA1:
		/* Permissions check */
		DCWrite8(0x200012F2, 0xE0);

		break;

	/** 12/23/09 17:26:21 **/
	case 0x49511F3D:           //  IOS: 38v4123
		/* Permissions check */
		DCWrite8(0x2000347E, 0xE0);

		break;

	/** 04/03/12 11:58:08 **/
	case 0x4F7AE5D0:           // vIOS: 38v4380
		/* Permissions check */
		DCWrite8(0x20003452, 0xE0);

		break;

	/** 11/24/08 15:36:10 **/
	case 0x492AC9EA:           //  IOS: 56v5661, ....
	/** 04/02/12 14:00:54 **/
	case 0x4F79B116:           // vIOS: 56v5918, 57v6175, 58v6432
		/* Permissions check */
		DCWrite8(0x20001306, 0xE0);

		break;

	default:
		/* Unknown module version */
		return IOS_ERROR_FFS;
	}

	return 0;
}

s32 Patch_IopModule(void)
{
	switch (ios.iopVersion) {
	/** 07/11/08 14:34:29 **/
	/** 11/24/08 15:39:12 **/
	/** 12/23/08 17:28:32 **/
	/** 03/03/10 10:43:18 **/
	/** 03/01/10 03:28:58 **/
	/** 03/01/10 03:13:17 **/
	case 0x48776F75:        // IOS: 37v5662, 53v5662, 55v5662
	case 0x492ACAA0:        // IOS: 60v6174, 70v6687
	case 0x49511FC0:        // IOS: ???
	case 0x4B8E3D46:        // IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943 
	case 0x4B8B30CD:        // IOS: 36v3607, 38v4123
	/** 04/03/12 12:00:18 **/
	case 0x4F7AE652:        // vIOS: 38v4380
	/** 04/02/12 14:03:56 **/
	case 0x4F79B1CC:        // vIOS: 56v5918, 57v6175, 58v6432
		/* SWI handler */
		DCWrite32(0xFFFF0028, (u32)SwiVector);

		break;

	default:
		/* Unknown module version */
		return IOS_ERROR_IOP;
	}

	return 0;
}

