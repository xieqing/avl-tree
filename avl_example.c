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

	/* create a AVL tree */
	if ((avlt = avl_create(compare_func, destroy_func)) == NULL) {
		fprintf(stderr, "create AVL tree failed\n");
		return 1;
	}

	/* insert items */
	char a[] = {'R', 'E', 'D', 'S', 'O', 'X', 'C', 'U', 'B', 'T'};
	int i;
	mydata *data;
	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		if ((data = makedata(a[i])) == NULL || avl_insert(avlt, data) == NULL) {
			fprintf(stderr, "insert %c: out of memory\n", a[i]);
			free(data);
			break;
		}
		printf("insert %c", a[i]);
		avl_print(avlt, print_char_func);
		printf("\n");
	}

	/* delete item */
	avlnode *node;
	mydata query;
	query.key = 'O';
	printf("delete %c", query.key);
	if ((node = avl_find(avlt, &query)) != NULL)
		avl_delete(avlt, node, 0);
	avl_print(avlt, print_char_func);

	#ifdef AVL_MIN
	while ((node = AVL_MINIMAL(avlt))) {
		printf("\ndelete ");
		print_char_func(node->data);
		avl_delete(avlt, node, 0);
		avl_print(avlt, print_char_func);
	}
	#endif

	avl_destroy(avlt);
	return 0;
}

/*
 * usage: gcc avl_example.c avl_bf.c avl_data.c && ./a.out
 */