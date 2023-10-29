#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

static unsigned long long rand_next;

static void memset_inner(void *dest, unsigned char c, size_t size)
{
	unsigned int *addr;
	unsigned int offset;

	if ((uintptr_t)dest < MEM1_SIZE) {
		for (; size != 0; size--) {
			addr = (unsigned int *)((uintptr_t)dest & ~3);
			offset = 24 - ((uintptr_t)dest & 3) * 8;
			*addr = (*addr & ~(0xFF << offset)) | (c << offset);
			dest = (void *)((uintptr_t)dest + 1);
		}
	} else {
		for (; size != 0; size--) {
			*(unsigned char *)dest = c;
			dest = (void *)((uintptr_t)dest + 1);
		}
	}
}

void *memset(void *dest, int c, size_t size)
{
	size_t cnt;
	unsigned int data32;
	void *const dst = dest;
	unsigned int data = c & 0xFF;

	if (size == 0)
		return dst;

	if (((uintptr_t)dest & 3) != 0) {
		cnt = 4 - ((uintptr_t)dest & 3);
		if (size <= cnt) {
			memset_inner(dest, data, size);
			return dst;
		}
		memset_inner(dest, data, cnt);
		size -= cnt;
		dest = (void *)((uintptr_t)dest + cnt);
	}

	if (size >= 4) {
		data32 = (data << 8) | data;
		data32 = (data32 << 16) | data32;
		do {
			*(unsigned int *)dest = data32;
			dest = (void *)((uintptr_t)dest + 4);
			size -= 4;
		} while (size >= 4);
	}

	if (size > 0)
		memset_inner(dest, data, size);

	return dst;
}

void *memcpy(void *__restrict dest, const void *__restrict src, size_t size)
{
	unsigned int *addr;
	unsigned int offset;
	unsigned int tmp1;
	unsigned int tmp2;
	unsigned int tmp3;
	unsigned int tmp4;
	unsigned int tmp5;
	unsigned int tmp6;
	unsigned int tmp7;
	unsigned int tmp8;
	void *const dst = dest;

	if (size == 0)
		return dst;

	if ((((uintptr_t)dest | (uintptr_t)src) & 3) == 0) {
		if (size > 15) {
			if (size > 31) {
				do {
					tmp1 = *(unsigned int *)((uintptr_t)src);
					tmp2 = *(unsigned int *)((uintptr_t)src + 4);
					tmp3 = *(unsigned int *)((uintptr_t)src + 8);
					tmp4 = *(unsigned int *)((uintptr_t)src + 0xc);
					tmp5 = *(unsigned int *)((uintptr_t)src + 0x10);
					tmp6 = *(unsigned int *)((uintptr_t)src + 0x14);
					tmp7 = *(unsigned int *)((uintptr_t)src + 0x18);
					tmp8 = *(unsigned int *)((uintptr_t)src + 0x1c);
					src = (void *)((uintptr_t)src + 0x20);
					*(unsigned int *)dest = tmp1;
					*(unsigned int *)((uintptr_t)dest + 4) = tmp2;
					*(unsigned int *)((uintptr_t)dest + 8) = tmp3;
					*(unsigned int *)((uintptr_t)dest + 0xc) = tmp4;
					*(unsigned int *)((uintptr_t)dest + 0x10) = tmp5;
					*(unsigned int *)((uintptr_t)dest + 0x14) = tmp6;
					*(unsigned int *)((uintptr_t)dest + 0x18) = tmp7;
					*(unsigned int *)((uintptr_t)dest + 0x1c) = tmp8;
					dest = (void *)((uintptr_t)dest + 0x20);
					size = size - 0x20;
				} while (size >= 32);
			}

			for (; size > 15; size -= 16) {
				tmp1 = *(unsigned int *)((uintptr_t)src);
				tmp2 = *(unsigned int *)((uintptr_t)src + 4);
				tmp3 = *(unsigned int *)((uintptr_t)src + 8);
				tmp4 = *(unsigned int *)((uintptr_t)src + 0xc);
				src = (void *)((uintptr_t)src + 0x10);
				*(unsigned int *)dest = tmp1;
				*(unsigned int *)((uintptr_t)dest + 4) = tmp2;
				*(unsigned int *)((uintptr_t)dest + 8) = tmp3;
				*(unsigned int *)((uintptr_t)dest + 0xc) = tmp4;
				dest = (void *)((uintptr_t)dest + 0x10);
			}
		}

		for (; size > 3; size -= 4) {
			*(unsigned int *)dest = *(unsigned int *)src;
			dest = (void *)((uintptr_t)dest + 4);
			src = (void *)((uintptr_t)src + 4);
		}
	}

	if ((uintptr_t)dest < MEM1_SIZE) {
		for (; size != 0; size--) {
			addr = (unsigned int *)((uintptr_t)dest & ~3);
			offset = 24 - ((uintptr_t)dest & 3) * 8;
			*addr = (*addr & ~(0xFF << offset)) | (*(unsigned char *)src << offset);
			dest = (void *)((uintptr_t)dest + 1);
			src = (void *)((uintptr_t)src + 1);
		}
	} else {
		for (; size != 0; size--) {
			*(unsigned char *)dest = *(unsigned char *)src;
			dest = (void *)((uintptr_t)dest + 1);
			src = (void *)((uintptr_t)src + 1);
		}
	}

	return dst;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char u1, u2;

	for (; n--; s1++, s2++) {
		u1 = *(unsigned char *)s1;
		u2 = *(unsigned char *)s2;
		if (u1 != u2)
			return u1 - u2;
	}

	return 0;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s1 == *s2) {
		s1++;
		s2++;
	}

  return (*(unsigned char *)s1) - (*(unsigned char *)s2);
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while (n-- != 0 && *s1 == *s2) {
		if (n == 0 || *s1 == '\0')
			break;
		s1++;
		s2++;
	}

	return (*(unsigned char *)s1) - (*(unsigned char *)s2);
}

size_t strlen(const char *str)
{
	const char *s = str;

	while (*s)
		s++;

	return s - str;
}

size_t strnlen(const char *s, size_t maxlen)
{
	size_t len = 0;

	while (len < maxlen) {
		if (!*s)
			break;
		len++;
		s++;
	}

	return len;
}

char *strcat(char *__restrict s1, const char *__restrict s2)
{
	unsigned int offset;
	unsigned int *addr;
	char *const s = s1;
	char data;

	while (*s1)
		s1++;

	if ((uintptr_t)s1 < MEM1_SIZE) {
		do {
			offset = 24 - ((uintptr_t)s1 & 3) * 8;
			addr = (unsigned int *)((uintptr_t)s1 & ~3);
			data = *s2;
			*addr = (*addr & ~(0xFF << offset)) | (((unsigned int)(unsigned char)data) << offset);
			s1++;
			s2++;
		} while (data != '\0');
	} else {
		while ((*s1++ = *s2++) != '\0')
			;
	}

	return s;
}

char *strchr(const char *s1, int i)
{
	const unsigned char *s = (const unsigned char *)s1;
	unsigned char c = i;

	while (*s && *s != c)
		s++;

	if (*s == c)
		return (char *)s;

	return NULL;
}

char *strcpy(char *__restrict s1, const char *s2)
{
	unsigned int offset;
	unsigned int *addr;
	char *const s = s1;
	char data;

	if ((uintptr_t)s1 < MEM1_SIZE) {
		do {
			offset = 24 - ((uintptr_t)s1 & 3) * 8;
			addr = (unsigned int *)((uintptr_t)s1 & ~3);
			data = *s2;
			*addr = (*addr & ~(0xFF << offset)) | (((unsigned int)(unsigned char)data) << offset);
			s1++;
			s2++;
		} while (data != '\0');
	} else {
		while ((*s1++ = *s2++) != '\0')
			;
	}

	return s;
}

char *strncpy(char *__restrict dest, const char *src, size_t num)
{
	unsigned int offset;
	unsigned int *addr;
	char *const dst = dest;
	char data;

	if ((uintptr_t)dest < MEM1_SIZE) {
		if (num != 0) {
			do {
				offset = 24 - ((uintptr_t)dest & 3) * 8;
				addr = (unsigned int *)((uintptr_t)dest & ~3);
				data = *src;
				*addr = (*addr & ~(0xFF << offset)) | (((unsigned int)(unsigned char)data) << offset);
				num--;
				dest++;
				if (data == '\0')
					break;
				src++;
			} while (num != 0);
		}
		for (; num != 0; num--) {
			offset = 24 - ((uintptr_t)dest & 3) * 8;
			addr = (unsigned int *)((uintptr_t)dest & ~3);
			*addr = *addr & ~(0xFF << offset);
			dest++;
		}
	} else {
		if (num != 0) {
			do {
				data = *src;
				*dest = data;
				num--;
				dest++;
				if (data == '\0')
					break;
				src++;
			} while (num != 0);
		}
		for (; num != 0; num--) {
			*dest = '\0';
			dest++;
		}
	}

	return dst;
}

void srand(unsigned int seed)
{
	rand_next = seed;
}

int rand(void)
{
	/* This multiplier was obtained from Knuth, D.E., "The Art of
		Computer Programming," Vol 2, Seminumerical Algorithms, Third
		Edition, Addison-Wesley, 1998, p. 106 (line 26) & p. 108 */
	rand_next = rand_next * 6364136223846793005LL + 1;
	return (int)((rand_next >> 32) & RAND_MAX);
}
