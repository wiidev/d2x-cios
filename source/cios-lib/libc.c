#include <stdio.h>
#include <stdlib.h>

static unsigned long long rand_next;

void *memset(void *s, int c, size_t n)
{
	char *p = s;

	while (n) {
		*p++ = c;
		n--;
	}

	return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	const char *s = src;
	char *d = dest;

	while (n) {
		*d++ = *s++;
		n--;
	}

	return dest;
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
	char *s = s1;

	while (*s1)
		s1++;

	while ((*s1++ = *s2++))
		;
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

char *strcpy(char *s1, const char *s2)
{
	char *s = s1;
	while ((*s++ = *s2++) != 0)
		;
	return s1;
}

char *strncpy(char *__restrict dst0, const char *__restrict src0, size_t count)
{
	char *dscan = dst0;
	const char *sscan = src0;

	while (count > 0) {
		--count;
		if ((*dscan++ = *sscan++) == '\0')
			break;
	}

	while (count-- > 0)
		*dscan++ = '\0';

	return dst0;
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
