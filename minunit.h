/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#ifndef _MINUNIT_HEADER
#define _MINUNIT_HEADER

#define mu_test(_s, _c) \
do { \
	printf("#%03d %s \n", ++mu_tests, _s); \
	if (_c) { \
		printf("PASSED\n"); \
	} else { \
		printf("FAILED\n"); \
		mu_fails++; \
	} \
} while (0)

extern int mu_tests, mu_fails;

#endif /* _MINUNIT_HEADER */