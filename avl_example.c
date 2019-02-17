/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <stdlib.h>
#include "avl_bf.h"
#include "avl_data.h"

int main(int argc, char *argv[])
{
	avltree *avlt;
	mydata *data;

	if ((avlt = avl_create(compare_func, destroy_func)) == NULL) {
		fprintf(stderr, "out of memory\n");
		return 1;
	}

	if ((data = makedata(1)) != NULL)
		if (avl_insert(avlt, data) == NULL)
			free(data);

	if ((data = makedata(3)) != NULL)
		if (avl_insert(avlt, data) == NULL)
			free(data);

	if ((data = makedata(5)) != NULL)
		if (avl_insert(avlt, data) == NULL)
			free(data);

	if ((data = makedata(7)) != NULL)
		if (avl_insert(avlt, data) == NULL)
			free(data);

	avl_print(avlt, print_func);

	avlnode *n;
	mydata query;
	query.key = 1;
	if ((n = avl_find(avlt, &query)) != NULL)
		avl_delete(avlt, n, 0);

	avl_print(avlt, print_func);

	avl_destroy(avlt);
	return 0;
}

/*
 * usage: gcc avl_example.c avl_bf.c avl_data.c && ./a.out
 */