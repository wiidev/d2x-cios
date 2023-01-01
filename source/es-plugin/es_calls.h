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

#ifndef _ES_CALLS_H_
#define _ES_CALLS_H_

#include "types.h"

//#define DEBUG

/* Prototypes */
#ifdef DEBUG
s32 (*ES_printf)(const char * fmt, ...);
#else
#define ES_printf(fmt, ...)     do {} while (0)
#endif
extern s32   (*ES_snprintf)(char *str, u32 size, const char *fmt, ...);
extern s32   (*ES_LaunchTitle)(u32 tidh, u32 tidl, void *view, u32 reset);
extern void *(*ES_memcpy)(void *dst, void *src, s32 n);

/* ES handlers */
extern s32 ES_HandleOpen(void *message);
extern s32 ES_HandleIoctlv(void *message);

#endif
