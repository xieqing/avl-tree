/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "avl_bf.h"
#include "avl_data.h"
#include "minunit.h"

#define MIN INT_MIN
#define MAX INT_MAX
#define CHARS "ABCDEFGHIJ"

int mu_tests= 0, mu_fails = 0;

int permutation_error = 0;

static avltree *tree_create();
static avlnode *tree_find(avltree *avlt, int key);
static void tree_print(avltree *avlt);
static int tree_check(avltree *avlt);
static avlnode *tree_insert(avltree *avlt, int key);
static int tree_delete(avltree *avlt, int key);

static void swap(char *x, char *y);
static void permute(char *a, int start, int end, void func(char *));
static void permutation_insert(char *a);
static void permutation_delete(char *a);

static int unit_test_create();
static int unit_test_find();
static int unit_test_successor();
static int unit_test_atomic_insertion();
static int unit_test_atomic_deletion();
static int unit_test_chain_insertion();
static int unit_test_chain_deletion();
static int unit_test_permutation_insertion();
static int unit_test_permutation_deletion();
static int unit_test_random_insertion_deletion();

static int unit_test_dup();
#ifdef AVL_MIN
static int unit_test_min();
#endif

void all_tests()
{
	mu_test("unit_test_create", unit_test_create());

	mu_test("unit_test_find", unit_test_find());

	mu_test("unit_test_successor", unit_test_successor());

	mu_test("unit_test_atomic_insertion", unit_test_atomic_insertion());
	mu_test("unit_test_atomic_deletion", unit_test_atomic_deletion());

	mu_test("unit_test_chain_insertion", unit_test_chain_insertion());
	mu_test("unit_test_chain_deletion", unit_test_chain_deletion());

	mu_test("unit_test_permutation_insertion", unit_test_permutation_insertion());
	mu_test("unit_test_permutation_deletion", unit_test_permutation_deletion());

	mu_test("unit_test_random_insertion_deletion", unit_test_random_insertion_deletion());

	mu_test("unit_test_dup", unit_test_dup());

	#ifdef AVL_MIN
	mu_test("unit_test_min", unit_test_min());
	#endif
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

avltree *tree_create()
{
	return avl_create(compare_func, destroy_func);
}

avlnode *tree_find(avltree *avlt, int key)
{
	mydata query;
	query.key = key;
	return avl_find(avlt, &query);
}

void tree_print(avltree *avlt)
{
	avl_print(avlt, print_func);
}

int tree_check(avltree *avlt)
{
	mydata min, max;
	int rc;

	min.key = MIN;
	max.key = MAX;
	rc = 1;

	if (avl_check_order(avlt, &min, &max) != 1) {
		fprintf(stdout, "tree_check: invalid order\n");
		rc = 0;
	}

	if (avl_check_height(avlt) != 1) {
		fprintf(stdout, "tree_check: invalid height\n");
		rc = 0;
	}

	return rc;
}

avlnode *tree_insert(avltree *avlt, int key)
{
	avlnode *node;
	mydata *data;

	if (key < MIN || key > MAX) {
		fprintf(stdout, "tree_insert: invalid key %d\n", key);
		return NULL;
	}
	
	if ((data = makedata(key)) == NULL || (node = avl_insert(avlt, data)) == NULL) {
		fprintf(stdout, "tree_insert: insert %d failed\n", key);
		free(data);
		return NULL;
	}

	return node;
}

int tree_delete(avltree *avlt, int key)
{
	avlnode *node;

	if ((node = tree_find(avlt, key)) == NULL) {
		fprintf(stdout, "tree_delete: %d not found\n", key);
		return 0;
	}

	avl_delete(avlt, node, 0);

	if (tree_find(avlt, key) == node) {
		fprintf(stdout, "tree_delete: delete %d failed\n", key);
		return 0;
	}

	return 1;
}

void swap(char *x, char *y)
{
	char temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

void permute(char *a, int start, int end, void func(char *))
{
	if (start == end) {
		func(a);
		return;
	}

	int i;
	for (i = start; i <= end; i++) {
		swap(a + start, a + i);
		permute(a, start + 1, end, func);
		swap(a + start, a + i);
	}
}

void permutation_insert(char *a)
{
	avltree *avlt;
	avlnode *node;
	int i;
	
	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		permutation_error++;
		return;
	}

	for (i = 0; i < strlen(a); i++) {
		if ((node = tree_insert(avlt, a[i])) == NULL || tree_find(avlt, a[i]) != node || tree_check(avlt) != 1) {
			fprintf(stdout, "insert %c failed\n", a[i]);
			permutation_error++;
			return;
		}
	}

	avl_destroy(avlt);
}

void permutation_delete(char *a)
{
	avltree *avlt;
	avlnode *node;
	int i;
	
	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		permutation_error++;
		return;
	}

	char b[] = CHARS;

	for (i = 0; i < strlen(b); i++) {
		if ((node = tree_insert(avlt, b[i])) == NULL || tree_find(avlt, b[i]) != node || tree_check(avlt) != 1) {
			fprintf(stdout, "insert %c failed\n", b[i]);
			permutation_error++;
			return;
		}
	}

	for (i = 0; i < strlen(a); i++) {
		if (tree_delete(avlt, a[i]) != 1 || tree_check(avlt) != 1) {
			fprintf(stdout, "delete %c failed\n", a[i]);
			permutation_error++;
			return;
		}
	}

	avl_destroy(avlt);
}

int unit_test_create()
{
	avltree *avlt;

	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
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
		fprintf(stdout, "init 1\n");
		avl_destroy(avlt);
		return 0;
	}

	avl_destroy(avlt);
	return 1;
}

int unit_test_find()
{
	avltree *avlt;
	avlnode *r, *e, *d, *s, *o, *x, *c, *u, *b, *t;
	
	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		goto err0;
	}

	if ((r = tree_insert(avlt, 'R')) == NULL || \
		(e = tree_insert(avlt, 'E')) == NULL || \
		(d = tree_insert(avlt, 'D')) == NULL || \
		(s = tree_insert(avlt, 'S')) == NULL || \
		(o = tree_insert(avlt, 'O')) == NULL || \
		(x = tree_insert(avlt, 'X')) == NULL || \
		(c = tree_insert(avlt, 'C')) == NULL || \
		(u = tree_insert(avlt, 'U')) == NULL || \
		(b = tree_insert(avlt, 'B')) == NULL || \
		(t = tree_insert(avlt, 'T')) == NULL || \
		tree_check(avlt) != 1) {
		fprintf(stdout, "init failed\n");
		goto err;
	}

	if (avl_find(avlt, r->data) != r || \
		avl_find(avlt, e->data) != e || \
		avl_find(avlt, d->data) != d || \
		avl_find(avlt, s->data) != s || \
		avl_find(avlt, o->data) != o || \
		avl_find(avlt, x->data) != x || \
		avl_find(avlt, c->data) != c || \
		avl_find(avlt, u->data) != u || \
		avl_find(avlt, b->data) != b || \
		avl_find(avlt, t->data) != t) {
		fprintf(stdout, "find failed\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

int unit_test_successor()
{
	avltree *avlt;
	avlnode *r, *e, *d, *s, *o, *x, *c, *u, *b, *t;
	
	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		goto err0;
	}

	if ((r = tree_insert(avlt, 'R')) == NULL || \
		(e = tree_insert(avlt, 'E')) == NULL || \
		(d = tree_insert(avlt, 'D')) == NULL || \
		(s = tree_insert(avlt, 'S')) == NULL || \
		(o = tree_insert(avlt, 'O')) == NULL || \
		(x = tree_insert(avlt, 'X')) == NULL || \
		(c = tree_insert(avlt, 'C')) == NULL || \
		(u = tree_insert(avlt, 'U')) == NULL || \
		(b = tree_insert(avlt, 'B')) == NULL || \
		(t = tree_insert(avlt, 'T')) == NULL || \
		tree_delete(avlt, 'O') != 1 || \
		tree_check(avlt) != 1) {
		fprintf(stdout, "init failed\n");
		goto err;
	}

	if (avl_successor(avlt, b) != c || \
		avl_successor(avlt, c) != d || \
		avl_successor(avlt, d) != e || \
		avl_successor(avlt, e) != r || \
		avl_successor(avlt, r) != s || \
		avl_successor(avlt, s) != t || \
		avl_successor(avlt, t) != u || \
		avl_successor(avlt, u) != x || \
		avl_successor(avlt, x) != NULL) {
		fprintf(stdout, "successor failed\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

int unit_test_atomic_insertion()
{
	avltree *avlt;
	int i, j;

	char *cases[] = {

	/* we could make a full tree by inserting A-Z and a-e */

	/* balanced */
		
		/* height insreased */
		"P",
		"PH",
		"PX",

		/* height unchanged
		"PHX",
		"PXH",

	/* arrangements required: insertion under P.left */

		/* P.left is left-heavy */

		/* H.height = 0 + 2 */
		"PHD"

		/* H.height = 1 + 2 */
		"PHXDLB",
		"PHXDLF",

		/* P.left is right-heavy */

		/* H.height = 0 + 2 */
		"PHL",

		/* H.height = 1 + 2 */
		"PHXDLJ",
		"PHXDLN",

	/* arrangements required: insertion under P.right */

		/* P.right is right-heavy */
		
		/* X.height = 0 + 2 */
		"PXb",

		/* X.height = 1 + 2 */
		"PHXTbZ",
		"PHXTbd",

		/* P.right is left-heavy */

		/* X.height = 0 + 2 */
		"PHXT",

		/* X.height = 1 + 2 */
		"PHXTbRV",
		"PHXTbR",
		"PHXTbV",

	/* end */
		0

	};

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]) && cases[i]; i++) {
		if ((avlt = tree_create()) == NULL) {
			fprintf(stdout, "%s - create AVL tree failed\n", cases[i]);
			goto err0;
		}

		for (j = 0; j < strlen(cases[i]); j++) {
			if (tree_insert(avlt, cases[i][j]) == NULL || tree_check(avlt) != 1) {
				fprintf(stdout, "%s - insert %c failed\n", cases[i], cases[i][j]);
				goto err;
			}
		}

		avl_destroy(avlt);
	}
	
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

int unit_test_atomic_deletion()
{
	avltree *avlt;
	int i, j;

	char *cases[] = {

	/* we could make a full tree by inserting A-Z and a-e */

	/* balanced */

		/* height decreased */
		"P", "P", 
		"PH", "H",
		"PH", "P",
		"PX", "P",
		"PX", "X",

		/* height unchanged */
		"PHX", "H",
		"PHX", "P",
		"PHX", "X",

	/* arrangements required: deletion under P.right */

		/* P.left is left-heavy */  /* height decreased */

		/* H.height = 0 + 2 */
		"PHXD", "X",

		/* H.height = 1 + 2 */
		"PHXDLTBF", "X", 
		"PHXDLTB", "X",
		"PHXDLTF", "X",		

		/* P.left is balanced */

		/* H.height = 0 + 2 */
		"PHXDL", "X",

		/* H.height = 1 + 2 */
		"PHXDLTBFJN", "X",
		"PHXDLTBFJ", "X",
		"PHXDLTBFN", "X",
		"PHXDLTBJN", "X",
		"PHXDLTBJ", "X",
		"PHXDLTBN", "X",
		"PHXDLTFJN", "X",
		"PHXDLTFJ", "X",
		"PHXDLTFN", "X",

		/* P.left is right-heavy */  /* height decreased */

		/* H.height = 0 + 2 */
		"PHXL", "X",

		/* H.height = 1 + 2 */
		"PHXDLTJN", "X",
		"PHXDLTJ", "X",
		"PHXDLTN", "X",

	/* arrangements required: deletion under P.left */

		/* P.right is right-heavy */  /* height decreased */
		
		/* X.height = 0 + 2 */
		"PHXb", "H",

		/* X.height = 1 + 2 */
		"PHXDTbZd", "H",
		"PHXDTbZ", "H",
		"PHXDTbd", "H",

		/* P.right is balanced */
		
		/* X.height = 0 + 2 */
		"PHXTb", "H",

		/* X.height = 1 + 2 */
		"PHXDTbRVZd", "H",
		"PHXDTbRVZ", "H",
		"PHXDTbRVd", "H",
		"PHXDTbRZd", "H",
		"PHXDTbRZ", "H",
		"PHXDTbRd", "H",
		"PHXDTbVZd", "H",
		"PHXDTbVZ", "H",
		"PHXDTbVd", "H",

		/* P.right is left-heavy */  /* height decreased */

		/* X.height = 0 + 2 */
		"PHXT", "H",

		/* X.height = 1 + 2 */
		"PHXDTbRV", "H",
		"PHXDTbR", "H",
		"PHXDTbV", "H",

	/* end */
		0, 0

	};

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]) && cases[i] != 0; i += 2) {
		if ((avlt = tree_create()) == NULL) {
			fprintf(stdout, "%s-%s - create AVL tree failed\n", cases[i], cases[i + 1]);
			goto err0;
		}

		for (j = 0; j < strlen(cases[i]); j++) {
			if (tree_insert(avlt, cases[i][j]) == NULL || tree_check(avlt) != 1) {
				fprintf(stdout, "%s-%s - insert %c failed\n", cases[i], cases[i + 1], cases[i][j]);
				goto err;
			}
		}

		for (j = 0; j < strlen(cases[i + 1]); j++) {
			if (tree_delete(avlt, cases[i + 1][j]) == 0 || tree_check(avlt) != 1) {
				fprintf(stdout, "%s-%s - delete %c failed\n", cases[i], cases[i + 1], cases[i + 1][j]);
				goto err;
			}
		}

		avl_destroy(avlt);
	}
	
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

/*
 * chain insertion
 * move up until height unchanged (one rebalancing operation is performed) or root reached
 */
int unit_test_chain_insertion()
{
	avltree *avlt;
	int i, j;

	char *cases[] = {
		"PHXDB",
		"PHXDF",
		"PHXLJ",
		"PHXLN",
		"PHXTR",
		"PHXTV",
		"PHXbZ",
		"PHXbd",
		"PHXDLB",
		"PHXDLF",
		"PHXDLJ",
		"PHXDLN",
		"PHXTbR",
		"PHXTbV",
		"PHXTbV",
		"PHXTbZ",
		"PHXTbd",
		0
	};
	
	for (i = 0; i < sizeof(cases) / sizeof(cases[0]) && cases[i] != 0; i++) {
		if ((avlt = tree_create()) == NULL) {
			fprintf(stdout, "%s - create AVL tree failed\n", cases[i]);
			goto err0;
		}

		for (j = 0; j < strlen(cases[i]); j++) {
			if (tree_insert(avlt, cases[i][j]) == NULL || tree_check(avlt) != 1) {
				fprintf(stdout, "%s - insert %c failed\n", cases[i], cases[i][j]);
				goto err;
			}
		}

		avl_destroy(avlt);
	}

	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

/*
 * chain deletion
 * move up until height unchanged (after one or more rebalancing operations) or root reached
 */
int unit_test_chain_deletion()
{
	avltree *avlt;
	int i, j;

	char *cases[] = {
		"PHXDLTbFJNZK", "T",
		"PHXDLTbFJNZdK", "T",
		"PHXDLTbFJNdK", "T", /* equivalents to PHXDLTzFJNbK */
		"PHXDLTbFRVZU", "L",
		"PHXDLTbBFRVZU", "L",
		"PHXDLTbBRVZU", "L", /* equivalents to PHXFLTbDRVZU */
		0, 0
	};

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]) && cases[i] != 0; i += 2) {
		if ((avlt = tree_create()) == NULL) {
			fprintf(stdout, "%s-%s - create AVL tree failed\n", cases[i], cases[i + 1]);
			goto err0;
		}

		for (j = 0; j < strlen(cases[i]); j++) {
			if (tree_insert(avlt, cases[i][j]) == NULL || tree_check(avlt) != 1) {
				fprintf(stdout, "%s-%s - insert %c failed\n", cases[i], cases[i + 1], cases[i][j]);
				goto err;
			}
		}

		for (j = 0; j < strlen(cases[i + 1]); j++) {
			if (tree_delete(avlt, cases[i + 1][j]) == 0 || tree_check(avlt) != 1) {
				fprintf(stdout, "%s-%s - delete %c failed\n", cases[i], cases[i + 1], cases[i + 1][j]);
				goto err;
			}
		}

		avl_destroy(avlt);
	}

	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

int unit_test_permutation_insertion()
{
	char a[] = CHARS;
	
	permutation_error = 0;
	permute(a, 0, strlen(a) - 1, permutation_insert);
	return (permutation_error == 0);
}

int unit_test_permutation_deletion()
{
	char a[] = CHARS;
	
	permutation_error = 0;
	permute(a, 0, strlen(a) - 1, permutation_delete);
	return (permutation_error == 0);
}

int unit_test_random_insertion_deletion()
{
	avltree *avlt;
	int ninsert, ndelete;
	int i, key, max;
	
	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		goto err0;
	}
	
	ninsert = 0;
	ndelete = 0;
	max = 9999;

	srand((unsigned int) time(NULL));

	for (i = 1; i <= 1999; i++) {
		key = rand() % max;
		if (tree_find(avlt, key) != NULL)
			continue;
		ninsert++;
		if (tree_insert(avlt, key) == NULL || tree_check(avlt) != 1) {
			fprintf(stdout, "insert %d failed\n", key);
			goto err;
		}
	}

	for (i = 1; i < max; i++) {
		key = rand() % max;
		if (tree_find(avlt, key) == NULL)
			continue;
		ndelete++;
		if (tree_delete(avlt, key) != 1 || tree_check(avlt) != 1) {
			fprintf(stdout, "delete %d failed\n", key);
			goto err;
		}
	}

	printf("\tstat: ninsert=%d, ndelete=%d\n", ninsert, ndelete);

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}

#ifdef AVL_MIN
int unit_test_min()
{
	avltree *avlt;

	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		goto err0;
	}

	if (AVL_MINIMAL(avlt) != NULL || \
		tree_insert(avlt, 'B') == NULL || AVL_MINIMAL(avlt) != tree_find(avlt, 'B') || \
		tree_insert(avlt, 'A') == NULL || AVL_MINIMAL(avlt) != tree_find(avlt, 'A') || \
		tree_insert(avlt, 'C') == NULL || AVL_MINIMAL(avlt) != tree_find(avlt, 'A') || \
		tree_delete(avlt, 'B') != 1 || AVL_MINIMAL(avlt) != tree_find(avlt, 'A') || \
		tree_delete(avlt, 'A') != 1 || AVL_MINIMAL(avlt) != tree_find(avlt, 'C') || \
		tree_delete(avlt, 'C') != 1 || AVL_MINIMAL(avlt) != NULL) {
		fprintf(stdout, "invalid min\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}
#endif

int unit_test_dup()
{
	avltree *avlt;
	avlnode *n1, *n2;

	if ((avlt = tree_create()) == NULL) {
		fprintf(stdout, "create AVL tree failed\n");
		goto err0;
	}

	if ((n1 = tree_insert(avlt, 'N')) == NULL || (n2 = tree_insert(avlt, 'N')) == NULL) {
		fprintf(stdout, "insert failed\n");
		goto err;
	}

	#ifdef AVL_DUP
	if (n1 == n2 || n1->right != n2) {
	#else
	if (n1 != n2) {
	#endif
		fprintf(stdout, "invalid dup\n");
		goto err;
	}

	avl_destroy(avlt);
	return 1;

err:
	avl_destroy(avlt);
err0:
	return 0;
}