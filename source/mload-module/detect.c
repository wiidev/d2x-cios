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

#include <string.h>

#include "ios.h"
#include "swi.h"
#include "types.h"

#define DETECT_ERROR	((s32)error_msg)

static char *error_msg = "Version not detected";

typedef struct {
	u32 idAddress;
	u8  idValue[20];
	u32 version;
} moduleId;

static s32 __Detect_ModuleVersion(moduleId* moduleIds, u32 n)
{
	u32 i;
	for (i = 0; i < n; i++) {
		if (memcmp((u32 *)(moduleIds[i].idAddress), moduleIds[i].idValue, 17) == 0)
			return moduleIds[i].version;  
	}
	return 0;  
}

s32 Detect_DipModule(void)
{
	u32 dipAddr = *(vu32 *)0x20200040;

	/* Set DIP version */
	switch (dipAddr) {

	case 0x20207F40:		// IOS: 37v5662, 53v5662, 55v5662
		/* DIP: 07/11/08 14:34:26 */
		ios.dipVersion = 0x48776F72;
		break;

	case 0x20207C2C:		// IOS: 36v3607, 38v4123
		/* DIP: 07/24/08 20:08:44 */
		ios.dipVersion = 0x4888E14C;
		break;

	case 0x20207C48:		// vIOS: 38v4380
		/* DIP: 04/03/12 12:00:16 */
		ios.dipVersion = 0x4F7AE650;
		break;

	case 0x20207EA8:		// IOS: 56v5661, 57v5918, 58v6175, 61v5661, 70v6687, 80v6943
		/* DIP: 06/03/09 07:49:09 */
		ios.dipVersion = 0x4A262AF5;
		break;

	case 0x20207DB8:		// IOS: 60v6174
		/* DIP: 11/24/08 15:39:09 */
		ios.dipVersion = 0x492ACA9D;
		break;

	case 0x20207EC4:		// vIOS: 56v5918, 57v6175, 58v6432
		/* DIP: 04/02/12 14:03:54 */
		ios.dipVersion = 0x4F79B1CA;
		break;

	default:
		/* Unknown version */
		return DETECT_ERROR;
	}

	return 0;
}

s32 Detect_EsModule(void)
{
	u32 esAddr = *(vu32 *)0x20100044;

	/* Set ES version */
	switch (esAddr) {
	case 0x201015A5:		// IOS: 70v6687
		/* ES: 06/03/09 07:46:02 */
		ios.esVersion = 0x4A262A3A;
		break;

	case 0x201014D5:		// IOS: 60v6174
		/* ES: 11/24/08 15:36:08 */
		ios.esVersion = 0x492AC9E8;
		break;

	case 0x201015E9:
	{
		static moduleId esIds[2] = {
			{0x2010B8D2, "03/03/10 10:40:14", 0x4B8E90EE},  //  IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
			{0x2010BB2E, "04/02/12 14:00:51", 0x4F79B113}   // vIOS: 56v5918, 57v6175, 58v6432
		};
		
		ios.esVersion = __Detect_ModuleVersion(esIds, 2);
		if (ios.esVersion == 0)
			return DETECT_ERROR;

		break;
	}		

	case 0x2010142D:		// IOS: 37v5662, 53v5662, 55v5662		
		/* ES: 03/01/10 03:26:03 */
		ios.esVersion = 0x4B8B882B;
		break;

	case 0x2010139D:		//  IOS: 36v3607, 38v4123
							// vIOS: 38v4380
	{
		static moduleId esIds[2] = {
			{0x2010A3EE, "03/01/10 03:18:58", 0x4B8B3222},  //  IOS: 36v3607, 38v4123
			{0x2010A64A, "04/03/12 12:05:51", 0x4F7AE79F}   // vIOS: 38v4380
		};
		
		ios.esVersion = __Detect_ModuleVersion(esIds, 2);
		if (ios.esVersion == 0)
			return DETECT_ERROR;

		break;
	}

	default:
		/* Unknown version */
		return DETECT_ERROR;
	}

	return 0;
}

s32 Detect_FfsModule(void)
{
	u32 ffsAddr = *(vu32 *)0x20000044;

	/* Set FFS version */
	switch (ffsAddr) {
	case 0x20005D89:		// IOS: 37v5662, 53v5662, 55v5662	
		/* FFS: 12/24/08 13:48:17 */
		ios.ffsVersion = 0x49523DA1;
		break;

	case 0x2000200D:		// IOS: 36v3607, 38v4123
		/* FFS: 12/23/08 17:26:21 */
		ios.ffsVersion = 0x49511F3D;
		break;

	case 0x20001FE1:		// vIOS: 38v4380
		/* FFS: 04/03/12 11:58:08 */
		ios.ffsVersion = 0x4F7AE5D0;
		break;

	case 0x20006009:		// IOS: 56v5661, 57v5918, 58v6175, 60v6174, 61v5661, 70v6687, 80v6943 	
		/* FFS: 11/24/08 15:36:10 */
		ios.ffsVersion = 0x492AC9EA;
		break;

	case 0x20005FDD:		// vIOS: 56v5918, 57v6175, 58v6432
		/* FFS: 04/02/12 14:00:54 */
		ios.ffsVersion = 0x4F79B116;
		break;

	default:
		/* Unknown version */
		return DETECT_ERROR;
	}

	return 0;
}

s32 Detect_IopModule(void)
{
	u32 iopAddr = *(vu32 *)0xFFFF0028;

	/* Set IOP version */
	switch (iopAddr) {
	case 0xFFFF1D60:	//               IOS: 37v5662, 53v5662, 55v5662
	case 0xFFFF79B4:	// gecko patched IOS: 37v5662, 53v5662, 55v5662
		/* IOSP: 07/11/08 14:34:29 */
		ios.iopVersion  = 0x48776F75;
		ios.syscallBase = 0xFFFF91B0;

		break;

	case 0xFFFF1D10:	//               IOS: 36v3607, 38v4123
	                	//              vIOS: 38v4380
	case 0xFFFF7938:	// gecko patched IOS: 36v3607, 38v4123
	{
		static moduleId iospIds[2] = {
			{0xFFFF856B, "03/01/10 03:13:17", 0x4B8B30CD},  //  IOS: 36v3607, 38v4123
			{0xFFFF83f3, "04/03/12 12:00:18", 0x4F7AE652}   // vIOS: 38v4380
		};

		ios.iopVersion = __Detect_ModuleVersion(iospIds, 2);

		switch (ios.iopVersion) {
		case 0x4B8B30CD:   //  IOS: 36v3607, 38v4123
			ios.syscallBase = 0xFFFF9100;
			break;
		case 0x4F7AE652:   // vIOS: 38v4380
			ios.syscallBase = 0xFFFF8F80;
			break;
		default:
			/* Unknown version */
			return DETECT_ERROR;
		}

		break;
	}

	case 0xFFFF1F20:	//               IOS: 60v6174, 70v6687, 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
	                	//              vIOS: 56v5918, 57v6175, 58v6432
	case 0xFFFF7B98:	// gecko patched IOS: 60v6174, 70v6687
	case 0xFFFF7BD0:	// gecko patched IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
	{
		static moduleId iospIds[3] = {
			{0xFFFF880B, "03/03/10 10:43:18", 0x4B8E3D46},  //  IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
			{0xFFFF8693, "04/02/12 14:03:56", 0x4F79B1CC},  // vIOS: 56v5918, 57v6175, 58v6432
			{0xFFFF87D3, "11/24/08 15:39:12", 0x492ACAA0}   //  IOS: 60v6174, 70v6687
		};

		ios.iopVersion = __Detect_ModuleVersion(iospIds, 3);

		switch (ios.iopVersion) {
		case 0x4B8E3D46:   // IOS: 56v5661, 57v5918, 58v6175, 61v5661, 80v6943
			ios.syscallBase = 0xFFFF93D0;
			break;
		case 0x4F79B1CC:   // vIOS: 56v5918, 57v6175
			ios.syscallBase = 0xFFFF9250;
			break;
		case 0x492ACAA0:   // IOS: 60v6174, 70v6687
			ios.syscallBase = 0xFFFF9390;
			break;
		default:
			/* Unknown version */
			return DETECT_ERROR;
		}

		break;
	}

	default:
		/* Unknown version */
		return DETECT_ERROR;
	}

	return 0;
}

