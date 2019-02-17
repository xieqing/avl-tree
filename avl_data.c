/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "avl_data.h"

mydata *makedata(int key)
{
	mydata *p;

	p = (mydata *) malloc(sizeof(mydata));
	if (p != NULL)
		p->key = key;

	return p;
}

int compare_func(const void *d1, const void *d2)
{
	mydata *p1, *p2;
	
	assert(d1 != NULL);
	assert(d2 != NULL);
	
	p1 = (mydata *) d1;
	p2 = (mydata *) d2;
	
	return p1->key - p2->key;
}

void destroy_func(void *d)
{
	mydata *p;
	
	assert(d != NULL);
	
	p = (mydata *) d;
	free(p);
}

void print_func(void *d)
{
	mydata *p;
	
	assert(d != NULL);
	
	p = (mydata *) d;
	printf("%d", p->key);
}

void print_char_func(void *d)
{
	mydata *p;
	
	assert(d != NULL);
	
	p = (mydata *) d;
	printf("%c", p->key & 127);
}