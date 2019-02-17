/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#ifndef _AVL_DATA_HEADER
#define _AVL_DATA_HEADER

typedef struct {
	int key;
} mydata;

mydata *makedata(int key);
int compare_func(const void *d1, const void *d2);
void destroy_func(void *d);
void print_func(void *d);
void print_char_func(void *d);

#endif /* _AVL_DATA_HEADER */

