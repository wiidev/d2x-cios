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

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "di.h"
#include "es_calls.h"
#include "ioctl.h"
#include "isfs.h"
#include "ipc.h"
#include "plugin.h"
#include "stealth.h"
#include "swi_mload.h"
#include "syscalls.h"
#include "types.h"

/* Constants */
#define ES_SIG_RSA4096		0x10000
#define ES_SIG_RSA2048		0x10001
#define TIK_SIZE		676


/* RSA (2048 bits) signature structure */
typedef struct {
	u32 type;
	u8 sig[256];
	u8 fill[60];
} ATTRIBUTE_PACKED sig_rsa2048;

/* RSA (4096 bits) signature structure */
typedef struct {
	u32 type;
	u8  sig[512];
	u8  fill[60];
} ATTRIBUTE_PACKED sig_rsa4096;

/* Ticket limit structure */
typedef struct {
	u32 tag;
	u32 value;
} ATTRIBUTE_PACKED tiklimit;

/* Ticket view structure */
typedef struct {
	u32 view;
	u64 ticketid;
	u32 devicetype;
	u64 titleid;
	u16 access_mask;
	u8  reserved[0x3c];
	u8  cidx_mask[0x40];
	u16 padding;
	tiklimit limits[8];
} ATTRIBUTE_PACKED tikview;

/* Ticket structure */
typedef struct {
	char issuer[0x40];
	u8  fill[63];
	u8  cipher_title_key[16];
	u8  fill2;
	u64 ticketid;
	u32 devicetype;
	u64 titleid;
	u16 access_mask;
	u8  reserved[0x3c];
	u8  cidx_mask[0x40];
	u16 padding;
	tiklimit limits[8];
} ATTRIBUTE_PACKED tik;

/* TMD content structure */
typedef struct {
	u32 cid;
	u16 index;
	u16 type;
	u64 size;
	u8 hash[20];
} ATTRIBUTE_PACKED tmd_content;

/* TMD structure */
typedef struct {
	char issuer[0x40];
	u8 version;
	u8 ca_crl_version;
	u8 signer_crl_version;
	u8 fill2;
	u64 sys_version;
	u64 title_id;
	u32 title_type;
	u16 group_id;
	u16 zero;
	u16 region;
	u8 ratings[16];
	u8 reserved[12];
	u8 ipc_mask[12];
	u8 reserved2[18];
	u32 access_rights;
	u16 title_version;
	u16 num_contents;
	u16 boot_index;
	u16 fill3;
	tmd_content contents[];
} ATTRIBUTE_PACKED tmd;


/* Signature Macros */
#define SIGNATURE_SIZE(x) (\
	((*(x))==ES_SIG_RSA2048) ? sizeof(sig_rsa2048) : ( \
	((*(x))==ES_SIG_RSA4096) ? sizeof(sig_rsa4096) : 0 ))

#define SIGNATURE_PAYLOAD(x) ((void *)(((u8*)(x)) + SIGNATURE_SIZE(x)))

#define FAKE_IOS_TICKET_VIEWS 1

/* Disc-based games have title IDs of 00010000xxxxxxxx and 00010004xxxxxxxx */
#define IS_DISC_BASED_GAME(TID) ((TID) >> 32 == 0x00010000 || (TID) >> 32 == 0x00010004) 


/* Global config */
struct esConfig config = { 0, 0, 0, 0 };

static s32 __ES_GetTitleID(u64 *tid)
{
	// FIX v7
	// Now these 2 variables are static and aligned.
	// Without this fix COD3 crashes on ios reload block 
	// when using the new ehci module.
	static ipcmessage message ATTRIBUTE_ALIGN(32);
	static ioctlv     vector  ATTRIBUTE_ALIGN(32);
	static u64        tmp_tid ATTRIBUTE_ALIGN(32);

	s32 ret;

	/* Clear buffer */
	memset(&message, 0, sizeof(message));

	/* Setup vector */
	vector.data = &tmp_tid;
	vector.len  = sizeof(tmp_tid);

	/* Setup message */
	message.ioctlv.command = 0x20;
	message.ioctlv.num_io  = 1;
	message.ioctlv.vector  = &vector;

	/* Call handler */
	ret = ES_HandleIoctlv(&message);
	
	/* Set title ID */
	if (tid)
		*tid = ret < 0 ? 0 : tmp_tid;

	return ret;
}

static s32 __ES_GetTicketView(u32 tidh, u32 tidl, u8 *view)
{
	static u8 buffer[TIK_SIZE] ATTRIBUTE_ALIGN(32);

	char path[ISFS_MAXPATH];
	s32  fd, ret;

	/* Generate ticket path forcing real nand access with '#' */
	ES_snprintf(path, sizeof(path), "#ticket/%08x/%08x.tik", tidh, tidl);

	/* Open ticket */
	fd = os_open(path, 1);
	if (fd < 0)
		return fd;

	/* Read ticket */
	ret = os_read(fd, buffer, sizeof(buffer));

	/* Close ticket */
	os_close(fd);

	/* Read error */
	if (ret < 0)
		return ret;

	/* Generate ticket view */
	*(u32 *) (view + 0x00) = (*(u32 *) (buffer + 0x1BC)) & 0xFF000000;
	ES_memcpy(view + 0x04, buffer + 0x1D0, sizeof(tikview) - 0x04);
	*(u16 *) (view + 0x96) = 0;


	return 0;
}

static s32 __ES_CustomLaunch(u32 tidh, u32 tidl)
{
	static tikview view ATTRIBUTE_ALIGN(32) = { 0 };
	s32 ret;

	/* Get ticket view */
	ret = __ES_GetTicketView(tidh, tidl, (void *)&view);
	if (ret < 0)
		return ret;

	/* Launch title */
	return ES_LaunchTitle(tidh, tidl, &view, 0);
}

static s32 __ES_SetFakeIosLaunch(u32 mode, u32 ios)
{
	config.fakelaunch = mode;
	config.ios        = ios;
	config.title_id   = 0;

	return 0;
}

static void __ES_RemoveError002(void *data, u32 len)
{
	/* Check whether the cios has been reloaded by a disc-based game */
	if (config.fakelaunch != 0 && config.title_id != 0) {
		/* Get TitleMetaData */
		if (data != NULL && len >= sizeof(sig_rsa2048) + sizeof(tmd)) {
			tmd *titleMetaData = (tmd *)(data + sizeof(sig_rsa2048));
 
			/* Not matched title ID */
			if(titleMetaData->title_id != config.title_id) {

				/* Disable ios reload block */
				config.fakelaunch = 0;
			}
			else {

				// Fix d2x v7 beta1
				// This line 
				// *(u32 *)0x00003140 = (*((u32 *)0x00003188)) | 0xFFFF;
				// has been made more general by taking the required IOS
				// from TMD rather than from MEM1. This way all those
				// games that reload different IOSs are now supported.
				// For example in Wii Fit Plus IOS53 is required for the game 
				// while IOS33 is required for channel installation.
				// 
				// Also, since d2x v8 beta1, the syscall os_kernel_set_version
				// is used instead of assigning address 0x00003140 directly.

				/* Remove error 002 */
				os_kernel_set_version((((u32)titleMetaData->sys_version)<<16) | 0xFFFF);
			}
		}

		/* Reset title ID */
		config.title_id = 0;
	}
}

static s32 __ES_FixMissingTitle(s32 justCounting, ioctlv *vector, s32 ret)
{
	s32 expectedRet   = justCounting ? 0 : -1017;
	s32 expectedCount = justCounting ? 0 : FAKE_IOS_TICKET_VIEWS;
	u32 *count        = vector[1].data;
	u64 *tid          = (u64 *)vector[0].data;

	/* Check input */
	if (count == NULL || tid == NULL)
		return ret;

	/* Check missing IOS for future fake launch, see bug http://code.google.com/p/d2x-cios/issues/detail?id=2 */
	if (ret == expectedRet && *count == expectedCount) {
		u32 tidh = (u32)(*tid >> 32);
		u32 tidl = (u32) *tid;

		/* System title */
		if (tidh == 1) {
			u64 fake_tid = 0;

			/* System menu */
			if (tidl == 2 && config.sm_title_id != 0) {
				fake_tid = config.sm_title_id;
			}


			/* IOS */
			if (tidl >= 3 && tidl <= 255 && config.fakelaunch != 0 && config.title_id == 0) {
				u64 running_tid;
				s32 ret2;

				/* Get title ID */
				ret2 = __ES_GetTitleID(&running_tid);
				
				/* Check a disc-based game is running */
				if (ret2 >= 0 && IS_DISC_BASED_GAME(running_tid)) {

					/* The required title matches the game IOS */
					if (tidl == ((*(vu32 *)0x00003140) >> 16)) {
						fake_tid = 0x0000000100000000LL | config.ios;
					}
				}
			}

			/* Use fake title id */
			if (fake_tid != 0) {
				if (justCounting) {
					ES_printf("__ES_Ioctlv: GetNumTicketViews: Setting fake count...\n");

					/* Set fake count*/
					*count = FAKE_IOS_TICKET_VIEWS;

					/* Success */
					ret = 0;
				}
				else {
					s32 ret2;

					tikview *view = (tikview *)vector[2].data;
					ES_printf("__ES_Ioctlv: GetNumTicketViews: Creating fake ticket view...\n");

					/* Create fake ticket view */
					ret2 = __ES_GetTicketView((u32)(fake_tid >> 32), (u32)fake_tid , (void *)view);

					/* Success */
					if (ret2 >= 0)
						ret = 0;
				}
			}
		}
	}

	return ret;
}

s32 ES_EmulateOpen(ipcmessage *message)
{
	static s32 stage2_done = 0;

	/* Initialize ES (stage 2) */
	if (!stage2_done) {
		s32 tid, ret;

		/* Enable ios reload block */
		if (config.title_id == 0) {
			s32 kver;

			/* Get kernel version */
			kver = os_kernel_get_version();

			/* Set fake ios launch */
			__ES_SetFakeIosLaunch(1, (kver >> 16) & 0xFF);
		}

		/* Get current thread id */
		tid = os_get_thread_id();

		/* Add thread rights for stealth mode */
		ret = Swi_AddThreadRights(tid, TID_RIGHTS_FORCE_REAL_NAND);

		/* Enable PPC HW access */
		os_set_ahbprot(1);

		/* Mark stage 2 as initilized */
		stage2_done = 1;
	}

	/* Call OPEN handler */
	return ES_HandleOpen(message);
}

s32 ES_EmulateIoctlv(ipcmessage *message)
{
	ioctlv *vector = message->ioctlv.vector;
	u32     inlen  = message->ioctlv.num_in;
	//u32     iolen  = message->ioctlv.num_io;
	u32     cmd    = message->ioctlv.command;

	s32 ret;
  
	/* Parse command */
	switch (cmd) {
	case IOCTL_ES_DIVERIFY: {
		ES_printf("__ES_Ioctlv: DIVerify()\n");

		/* Remove error 002 after IOS reload block */
		__ES_RemoveError002(vector[3].data, vector[3].len);

		/* Call IOCTLV handler */
		ret = ES_HandleIoctlv(message);

		/* Set running title for stealth mode */
		Swi_SetRunningTitle(!ret);

		ES_printf("__ES_Ioctlv: DIVerify: ret = %d\n", ret);

		return ret;
	}

	case IOCTL_ES_GETNUMTICKETVIEWS:
	case IOCTL_ES_GETTICKETVIEWS: {
		s32 justCounting  = (cmd == IOCTL_ES_GETNUMTICKETVIEWS);

		ES_printf("__ES_Ioctlv: Get%sTicketViews()\n", (justCounting ? "Num": ""));

		/* Call IOCTLV handler */
		ret = ES_HandleIoctlv(message);

		/* Fix missing title for fake launch */
		ret = __ES_FixMissingTitle(justCounting, vector, ret);

		ES_printf("__ES_Ioctlv: Get%sTicketViews: ret = %d\n", (justCounting ? "Num": ""), ret);

		return ret;
	}

	case IOCTL_ES_LAUNCH: {
		ES_printf("__ES_Ioctlv: LaunchTitle()\n");

		u64 tid = *(u64 *)vector[0].data;

		u32 tidh = (u32)(tid >> 32);
		u32 tidl = (u32) tid;

		/* System title launch */
		if (tidh == 1) {

			/* System menu launch */
			if (tidl == 2) {

				/* Set ES status for stealth mode */
				Swi_SetEsRequest(1);

				/* Disable NAND emulation */
				ISFS_SetMode(ISFS_MODE_NAND, "");

				/* Clear ES status for stealth mode */
				Swi_SetEsRequest(0);

				/* Launch title (fake ID) */
				if (config.sm_title_id != 0)
					return __ES_CustomLaunch((u32) (config.sm_title_id>>32), (u32) config.sm_title_id);
			}

			/* Fake IOS launch */
			if (config.fakelaunch != 0 && tidl >= 3 && tidl <= 255) {

				/* Reload the cIOS in place of the requested IOS */
				if (config.title_id == 0) {
					s32 ret;

					/* Get title ID */
					ret = __ES_GetTitleID(&config.title_id);
					if (ret >= 0 && IS_DISC_BASED_GAME(config.title_id)) {

						/* Set ES status for stealth mode */
						Swi_SetEsRequest(1);

						/* Save DI and FFS config after disabling nand emulation */
						DI_Config_Save();

						/* Clear ES status for stealth mode */
						Swi_SetEsRequest(0);

						/* Save ES config */
						Config_Save(&config, sizeof(config));

						/* Launch title (fake ID) */
						return __ES_CustomLaunch(tidh, config.ios);
					}

					/* Reset title ID */
					config.title_id = 0;
				}
			}
		}

		break;
	}

	case IOCTL_ES_KOREANCHECK: {
		/* Return error */
		return -1017;
	}

	case IOCTL_ES_SET_FAKE_IOS_LAUNCH: {
		ES_printf("__ES_Ioctlv: SetFakeIosLaunch()\n");

		/* Block custom command if a title is running */
		ret = Stealth_CheckRunningTitle("IOCTL_ES_SET_FAKE_IOS_LAUNCH");
		if (ret)
			return IPC_ENOENT;

		u32 mode = *(u32 *)vector[0].data;
		u32 ios = inlen>1 ? *(u32 *)vector[1].data : 249;

		/* Set fake ios launch */
		return __ES_SetFakeIosLaunch(mode, ios);
	}

	case IOCTL_ES_SET_FAKE_SM_LAUNCH: {
		ES_printf("__ES_Ioctlv: SetFakeSystemMenuLaunch()\n");

		/* Block custom command if a title is running */
		ret = Stealth_CheckRunningTitle("IOCTL_ES_SET_FAKE_SM_LAUNCH");
		if (ret)
			return IPC_ENOENT;

		u64 sm_title_id = *(u64 *)vector[0].data;

		/* Set fake system menu launch */
		config.sm_title_id = sm_title_id;

		return 0;
	}

	default:
		break;
	}

	ES_printf("__ES_Ioctlv(cmd = 0x%x) ...\n", cmd);

	/* Call IOCTLV handler */
	ret = ES_HandleIoctlv(message);

	ES_printf("__ES_Ioctlv(cmd = 0x%x): ret = %d\n", cmd, ret);

	return ret;
}

