/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avl_bf.h"
#include "avl_data.h"
#include "minunit.h"

#define MIN 0
#define MAX 99999

int mu_tests= 0, mu_fails = 0;

avltree *ab_create()
{
	avltree *avlt;
	if ((avlt = avl_create(compare_func, destroy_func)) == NULL)
		fprintf(stderr, "out of memory\n");
	return avlt;
}

avlnode *ab_insert(avltree *avlt, int key)
{
	avlnode *n;
	mydata *p;

	if (key < MIN || key > MAX) {
		fprintf(stderr, "invalid key\n");
		return NULL;
	}
		
	if ((p = makedata(key)) == NULL) {
		fprintf(stderr, "out of memory\n");
		return NULL;
	}

	if ((n = avl_insert(avlt, p)) == NULL) {
		fprintf(stderr, "out of memory\n");
		free(p);
	}

	return n;
}

avlnode *ab_find(avltree *avlt, int key)
{
	mydata data;
	data.key = key;
	return avl_find(avlt, &data);
}

int ab_check(avltree *avlt)
{
	mydata min, max;
	int rc;

	min.key = MIN;
	max.key = MAX;
	rc = 1;

	if (avl_check_order(avlt, &min, &max) != 1) {
		fprintf(stderr, "invalid order\n");
		rc = 0;
	}

	if (avl_check_height(avlt) != 1) {
		fprintf(stderr, "invalid height\n");
		rc = 0;
	}

	if (avl_check_bf(avlt) != 1) {
		fprintf(stderr, "invalid balance factor\n");
		rc = 0;
	}

	return rc;
}

void ab_print(avltree *avlt)
{
	mydata min, max;

	min.key = MIN;
	max.key = MAX;

	avl_print(avlt, print_func);
}

/*
 * test case for avl_create
 */
int unit_test_avl_create()
{
	avltree *avlt;

	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if (avlt->compare != compare_func || \
		avlt->destroy != destroy_func || \
		
		avlt->nil.left != AVL_NIL(avlt) || \
		avlt->nil.right != AVL_NIL(avlt) || \
		avlt->nil.parent != AVL_NIL(avlt) || \
		avlt->nil.bf != 0 || \
		avlt->nil.data != NULL || \
		avlt->root.left != AVL_NIL(avlt) || \
		avlt->root.right != AVL_NIL(avlt) || \
		avlt->root.parent != AVL_NIL(avlt) || \
		avlt->root.bf != 0 || \
		avlt->root.data != NULL) {
		fprintf(stderr, "init 1\n");
		avl_destroy(avlt);
		return 0;
	}

	#ifdef AVL_MIN
	if (avlt->min != NULL) {
		fprintf(stderr, "init 2\n");
		avl_destroy(avlt);
		return 0;
	}
	#endif

	avl_destroy(avlt);
	return 1;
}

/*
 * test case for avl_find
 */
int unit_test_avl_find()
{
	avltree *avlt;
	avlnode *a, *b, *c, *d, *e, *f, *g;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if ((c = ab_insert(avlt, 'C')) == NULL || \
		(b = ab_insert(avlt, 'B')) == NULL || \
		(f = ab_insert(avlt, 'F')) == NULL || \
		(a = ab_insert(avlt, 'A')) == NULL || \
		(d = ab_insert(avlt, 'D')) == NULL || \
		(g = ab_insert(avlt, 'G')) == NULL || \
		(e = ab_insert(avlt, 'E')) == NULL || \
		ab_check(avlt) != 1) {
		fprintf(stderr, "init\n");
		avl_destroy(avlt);
		return 0;
	}

	if (avl_find(avlt, a->data) != a || \
		avl_find(avlt, b->data) != b || \
		avl_find(avlt, c->data) != c || \
		avl_find(avlt, d->data) != d || \
		avl_find(avlt, e->data) != e || \
		avl_find(avlt, f->data) != f || \
		avl_find(avlt, g->data) != g) {
		fprintf(stderr, "find\n");
		avl_destroy(avlt);
		return 0;
	}

	avl_destroy(avlt);
	return 1;
}

/*
 * test case for avl_successor
 */
int unit_test_avl_successor()
{
	avltree *avlt;
	avlnode *a, *b, *c, *d, *e, *f, *g;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if ((c = ab_insert(avlt, 'C')) == NULL || \
		(b = ab_insert(avlt, 'B')) == NULL || \
		(f = ab_insert(avlt, 'F')) == NULL || \
		(a = ab_insert(avlt, 'A')) == NULL || \
		(d = ab_insert(avlt, 'D')) == NULL || \
		(g = ab_insert(avlt, 'G')) == NULL || \
		(e = ab_insert(avlt, 'E')) == NULL || \
		ab_check(avlt) != 1) {
		fprintf(stderr, "init\n");
		avl_destroy(avlt);
		return 0;
	}

	if (avl_successor(avlt, a) != b || \
		avl_successor(avlt, b) != c || \
		avl_successor(avlt, c) != d || \
		avl_successor(avlt, d) != e || \
		avl_successor(avlt, e) != f || \
		avl_successor(avlt, f) != g || \
		avl_successor(avlt, g) != NULL) {
		fprintf(stderr, "successor\n");
		avl_destroy(avlt);
		return 0;
	}

	avl_destroy(avlt);
	return 1;
}

int unit_test_avl_insert_left_left()
{
	avltree *avlt;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	char items[] = {'C', 'B', 'A'};
	int i;
	for (i = 0; i < sizeof(items) / sizeof(char); i++) {
		if (ab_insert(avlt, items[i]) == NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "insert %d\n", items[i]);
			goto err;
		}
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_insert_left_right()
{
	avltree *avlt;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	char items[] = {'C', 'A', 'B'};
	int i;
	for (i = 0; i < sizeof(items) / sizeof(char); i++) {
		if (ab_insert(avlt, items[i]) == NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "insert %d\n", items[i]);
			goto err;
		}
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_insert_right_left()
{
	avltree *avlt;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	char items[] = {'A', 'C', 'B'};
	int i;
	for (i = 0; i < sizeof(items) / sizeof(char); i++) {
		if (ab_insert(avlt, items[i]) == NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "insert %d\n", items[i]);
			goto err;
		}
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_insert_right_right()
{
	avltree *avlt;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	char items[] = {'A', 'B', 'C'};
	int i;
	for (i = 0; i < sizeof(items) / sizeof(char); i++) {
		if (ab_insert(avlt, items[i]) == NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "insert %d\n", items[i]);
			goto err;
		}
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_delete_left_left()
{
	avltree *avlt;
	avlnode *a, *b, *c, *d;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if ((c = ab_insert(avlt, 'C')) == NULL || \
		(b = ab_insert(avlt, 'B')) == NULL || \
		(a = ab_insert(avlt, 'A')) == NULL || \
		(d = ab_insert(avlt, 'D')) == NULL || \
		ab_check(avlt) != 1) {
		fprintf(stderr, "init\n");
		goto err;
	}

	avl_delete(avlt, d, 0);

	if (ab_find(avlt, 'D') != NULL || ab_check(avlt) != 1) {
		fprintf(stderr, "delete\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_delete_left_right()
{
	avltree *avlt;
	avlnode *a, *b, *c, *d;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if ((c = ab_insert(avlt, 'C')) == NULL || \
		(a = ab_insert(avlt, 'A')) == NULL || \
		(b = ab_insert(avlt, 'B')) == NULL || \
		(d = ab_insert(avlt, 'D')) == NULL || \
		ab_check(avlt) != 1) {
		fprintf(stderr, "init\n");
		goto err;
	}

	avl_delete(avlt, d, 0);

	if (ab_find(avlt, 'D') != NULL || ab_check(avlt) != 1) {
		fprintf(stderr, "delete\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_delete_right_left()
{
	avltree *avlt;
	avlnode *a, *b, *c, *d;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if ((b = ab_insert(avlt, 'B')) == NULL || \
		(a = ab_insert(avlt, 'A')) == NULL || \
		(d = ab_insert(avlt, 'D')) == NULL || \
		(c = ab_insert(avlt, 'C')) == NULL || \
		ab_check(avlt) != 1) {
		fprintf(stderr, "init\n");
		goto err;
	}

	avl_delete(avlt, a, 0);

	if (ab_find(avlt, 'A') != NULL || ab_check(avlt) != 1) {
		fprintf(stderr, "delete\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_delete_right_right()
{
	avltree *avlt;
	avlnode *a, *b, *c, *d;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	if ((b = ab_insert(avlt, 'B')) == NULL || \
		(a = ab_insert(avlt, 'A')) == NULL || \
		(c = ab_insert(avlt, 'C')) == NULL || \
		(d = ab_insert(avlt, 'D')) == NULL || \
		ab_check(avlt) != 1) {
		fprintf(stderr, "init\n");
		goto err;
	}

	avl_delete(avlt, a, 0);

	if (ab_find(avlt, 'A') != NULL || ab_check(avlt) != 1) {
		fprintf(stderr, "delete\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_insert_delete_random()
{
	avltree *avlt;
	avlnode *n;
	int i, key, insert_count, delete_count;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}

	srand((unsigned int) time(NULL));
	
	for (i = 1, insert_count = 0; i <= 9999; i++) {
		key = rand() % MAX;
		if (ab_find(avlt, key) != NULL)
			continue;
		insert_count++;
		printf("insert random: %d\n", key);
		if (ab_insert(avlt, key) == NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "insert random\n");
			goto err;
		}
	}

	for (i = 1, delete_count = 0; i < MAX; i++) {
		key = rand() % MAX;
		if ((n = ab_find(avlt, key)) == NULL)
			continue;
		delete_count++;
		printf("delete random: %d\n", key);
		avl_delete(avlt, n, 0);
		if (ab_find(avlt, key) != NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "delete random\n");
			goto err;
		}
	}

	printf("\tinsert random count: %d\n", insert_count);
	printf("\tdelete random count: %d\n", delete_count);

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

int unit_test_avl_delete()
{
	avltree *avlt;
	
	if ((avlt = ab_create()) == NULL) {
		fprintf(stderr, "create\n");
		return 0;
	}
	
	char items[] = {'Q', 'F', 'T', 'C', 'N', 'S', 'Z', 'E', 'G', 'P', 'W', 'K'};
	int i;

	for (i = 0; i < sizeof(items) / sizeof(char); i++) {
		if (ab_insert(avlt, items[i]) == NULL || ab_check(avlt) != 1) {
			fprintf(stderr, "init\n");
			goto err;
		}
	}

	avlnode *n = ab_find(avlt, 'S');

	avl_delete(avlt, n, 0);

	if (ab_find(avlt, 'S') != NULL || ab_check(avlt) != 1) {
		fprintf(stderr, "delete S\n");
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
	return 0;
}

void all_tests()
{

	/* avl_create */
	mu_test("unit_test_avl_create", unit_test_avl_create());

	/* avl_insert */
	mu_test("unit_test_avl_insert_left_left", unit_test_avl_insert_left_left());
	mu_test("unit_test_avl_insert_left_right", unit_test_avl_insert_left_right());
	mu_test("unit_test_avl_insert_right_left", unit_test_avl_insert_right_left());
	mu_test("unit_test_avl_insert_right_right", unit_test_avl_insert_right_right());

	/* avl_find */
	mu_test("unit_test_avl_find", unit_test_avl_find());

	/* avl_successor */
	mu_test("unit_test_avl_successor", unit_test_avl_successor());

	/* avl_delete */
	mu_test("unit_test_avl_delete_left_left", unit_test_avl_delete_left_left());
	mu_test("unit_test_avl_delete_left_right", unit_test_avl_delete_left_right());
	mu_test("unit_test_avl_delete_right_left", unit_test_avl_delete_right_left());
	mu_test("unit_test_avl_delete_right_right", unit_test_avl_delete_right_right());

	mu_test("unit_test_avl_delete", unit_test_avl_delete());

	/* random */
	mu_test("unit_test_random", unit_test_insert_delete_random());
}

int main(int argc, char **argv)
{
	all_tests();

	if (mu_fails) {
		printf("*** %d/%d TESTS FAILED ***\n", mu_fails, mu_tests);
		return 1;
	} else {
		printf("ALL TESTS PASSED\n");
		return 0;
	}
}