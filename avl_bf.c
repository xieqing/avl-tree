/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <stdlib.h>
#include "avl_bf.h"

static avlnode *rotate_left(avltree *avlt, avlnode *x);
static avlnode *rotate_right(avltree *avlt, avlnode *x);

static avlnode *fix_insert_leftimbalance(avltree *avlt, avlnode *p);
static avlnode *fix_insert_rightimbalance(avltree *avlt, avlnode *p);
static avlnode *fix_delete_leftimbalance(avltree *avlt, avlnode *p);
static avlnode *fix_delete_rightimbalance(avltree *avlt, avlnode *p);

static int check_order(avltree *avlt, avlnode *n, void *min, void *max);
static int check_height(avltree *avlt, avlnode *n);
static int check_balance_factor(avltree *avlt, avlnode *n);

static void print(avltree *avlt, avlnode *n, void (*print_func)(void *), int depth, char *label);
static void destroy(avltree *avlt, avlnode *n);

/*
 * construction
 * return NULL if out of memory
 */
avltree *avl_create(int (*compare_func)(const void *, const void *), void (*destroy_func)(void *))
{
	avltree *avlt;

	avlt = (avltree *) malloc(sizeof(avltree));
	if (avlt == NULL)
		return NULL; /* out of memory */

	avlt->compare = compare_func;
	avlt->destroy = destroy_func;

	/* sentinel node nil */
	avlt->nil.left = avlt->nil.right = avlt->nil.parent = AVL_NIL(avlt);
	avlt->nil.bf = 0;
	avlt->nil.data = NULL;

	/* sentinel node root */
	avlt->root.left = avlt->root.right = avlt->root.parent = AVL_NIL(avlt);
	avlt->root.bf = 0;
	avlt->root.data = NULL;

	#ifdef AVL_MIN
	avlt->min = NULL;
	#endif

	return avlt;
}

/*
 * destruction
 */
void avl_destroy(avltree *avlt)
{
	destroy(avlt, AVL_FIRST(avlt));
	free(avlt);
}

/*
 * look up
 * return NULL if not found
 */
avlnode *avl_find(avltree *avlt, void *data)
{
	avlnode *p;

	p = AVL_FIRST(avlt);

	while (p != AVL_NIL(avlt)) {
		int cmp;
		cmp = avlt->compare(data, p->data);
		if (cmp == 0)
			return p; /* found */
		p = (cmp < 0) ? p->left : p->right;
	}

	return NULL; /* not found */
}

/*
 * next larger
 * return NULL if not found
 */
avlnode *avl_successor(avltree *avlt, avlnode *node)
{
	avlnode *p;

	p = node->right;

	if (p != AVL_NIL(avlt)) {
		/* move down until we find it */
		for ( ; p->left != AVL_NIL(avlt); p = p->left) ;
	} else {
		/* move up until we find it or hit the root */
        for (p = node->parent; node == p->right; node = p, p = p->parent) ;

		if (p == AVL_ROOT(avlt))
			p = NULL; /* not found */
	}

	return p;
}

/*
 * apply func
 * return non-zero if error
 */
int avl_apply(avltree *avlt, avlnode *node, int (*func)(void *, void *), void *cookie, enum avltraversal order)
{
	int err;

	if (node != AVL_NIL(avlt)) {
		if (order == PREORDER && (err = func(node->data, cookie)) != 0) /* preorder */
			return err;
		if ((err = avl_apply(avlt, node->left, func, cookie, order)) != 0) /* left */
			return err;
		if (order == INORDER && (err = func(node->data, cookie)) != 0) /* inorder */
			return err;
		if ((err = avl_apply(avlt, node->right, func, cookie, order)) != 0) /* right */
			return err;
		if (order == POSTORDER && (err = func(node->data, cookie)) != 0) /* postorder */
			return err;
	}

	return 0;
}

/*
 * print tree
 */
void avl_print(avltree *avlt, void (*print_func)(void *))
{
	printf("\n--\n");
	print(avlt, AVL_FIRST(avlt), print_func, 0, "T");
	printf("\nheight = %d, bf = %d\n", check_height(avlt, AVL_FIRST(avlt)), check_balance_factor(avlt, AVL_FIRST(avlt)));
}

/*
 * check order of tree
 */
int avl_check_order(avltree *avlt, void *min, void *max)
{
	return check_order(avlt, AVL_FIRST(avlt), min, max);
}

/*
 * check height of tree
 */
int avl_check_height(avltree *avlt)
{
	int height;
	height = check_height(avlt, AVL_FIRST(avlt));

	return (height < 0) ? 0 : 1;
}

/*
 * check balance factor of tree
 */
int avl_check_bf(avltree *avlt)
{
	int bf;
	bf = check_balance_factor(avlt, AVL_FIRST(avlt));

	return (bf < -1 || bf > 1) ? 0 : 1;
}

/*
 * insert (or update) data
 * return NULL if out of memory
 */
avlnode *avl_insert(avltree *avlt, void *data)
{
	avlnode *current, *parent;
	avlnode *new_node;

	/* do a binary search to find where it should be */

	current = AVL_FIRST(avlt);
	parent = AVL_ROOT(avlt);

	while (current != AVL_NIL(avlt)) {
		int cmp;
		cmp = avlt->compare(data, current->data);

		#ifndef AVL_DUP
		if (cmp == 0) {
			avlt->destroy(current->data);
			current->data = data;
			return current; /* updated */
		}
		#endif

		parent = current;
		current = (cmp < 0) ? current->left : current->right;
	}
	
	/* replace the termination NIL pointer with the new node pointer */

	current = new_node = (avlnode *) malloc(sizeof(avlnode));
	if (current == NULL)
		return NULL; /* out of memory */

	current->left = current->right = AVL_NIL(avlt);
	current->parent = parent;
	current->bf = 0;
	current->data = data;

	if (parent == AVL_ROOT(avlt) || avlt->compare(data, parent->data) < 0)
		parent->left = current;
	else
		parent->right = current;

	#ifdef AVL_MIN
	if (avlt->min == NULL || avlt->compare(current->data, avlt->min->data) < 0)
		avlt->min = current;
	#endif

	/* rebalance */
	
	while (parent != AVL_ROOT(avlt)) {
		if (current == parent->left) {
			if (parent->bf == 1) {
				parent->bf = 0; /* height unchanged, goto break */
				break;
			} else if (parent->bf == 0) {
				parent->bf = -1; /* height increased, goto loop */
			} else if (parent->bf == -1) {
				fix_insert_leftimbalance(avlt, parent); /* height unchanged, goto break */
				break;
			}
		} else {
			if (parent->bf == -1) {
				parent->bf = 0; /* height unchanged, goto break */
				break;
			} else if (parent->bf == 0) {
				parent->bf = 1; /* height increased, goto loop */
			} else if (parent->bf == 1) {
				fix_insert_rightimbalance(avlt, parent); /* height unchanged, goto break */
				break;
			}
		}

		/* move up */
		current = parent;
		parent = current->parent;
	}

	return new_node; /* inserted */
}

/*
 * delete node
 * return NULL if keep is zero (already freed)
 */
void *avl_delete(avltree *avlt, avlnode *node, int keep)
{
	avlnode *current, *parent;
	avlnode *target;
	void *data;

	/* choose node's in-order successor if it has two children */

	data = node->data;
	
	if (node->left == AVL_NIL(avlt) || node->right == AVL_NIL(avlt)) {
		target = node;

		#ifdef AVL_MIN
		if (avlt->min == node)
			avlt->min = avl_successor(avlt, node); /* deleted, thus min = successor */
		#endif
	} else {
		target = avl_successor(avlt, node); /* node->right must not be NIL, thus move down */

		node->data = target->data; /* swapped */

		#ifdef AVL_MIN
		/* idle, thus commented
		if (avlt->min == node)
			avlt->min = target;
		*/
		if (avlt->min == target)
			avlt->min = node; /* swapped, thus min = node */
		#endif
	}

	/* rebalance if needed */

	current = target;
	parent = current->parent;

	while (parent != AVL_ROOT(avlt)) {
		if (current == parent->left) {
			if (parent->bf == -1) {
				parent->bf = 0; /* height decreased, goto loop */
			} else if (parent->bf == 0) {
				parent->bf = 1;
				break; /* height unchanged, goto break */
			} else if (parent->bf == 1) {
				parent = fix_delete_rightimbalance(avlt, parent);
				if (parent->bf == -1)
					break; /* height unchanged, goto break */
				/* height decreased, goto loop */
			}
		} else {
			if (parent->bf == 1) {
				parent->bf = 0; /* height decreased, goto loop */
			} else if (parent->bf == 0) {
				parent->bf = -1;
				break; /* height unchanged, goto break */
			} else if (parent->bf == -1) {
				parent = fix_delete_leftimbalance(avlt, parent);
				if (parent->bf == 1)
					break; /* height unchanged, goto break */
				/* height decreased, goto loop */
			}
		}

		/* move up */
		current = parent;
		parent = current->parent;
	}

	/* replace the target node with its child (may be NIL) */

	avlnode *child; /* child of target */

	child = (target->left == AVL_NIL(avlt)) ? target->right : target->left; /* child may be NIL */

	if (child != AVL_NIL(avlt))
		child->parent = target->parent;

	if (target == target->parent->left)
		target->parent->left = child;
	else
		target->parent->right = child;

	free(target);

	/* keep or discard data */

	if (keep == 0) {
		avlt->destroy(data);
		data = NULL; /* freed */
	}

	return data;
}

/*
 * rotate left about x
 * return the new root
 */
avlnode *rotate_left(avltree *avlt, avlnode *x)
{
	avlnode *y;

	y = x->right; /* child */

	/* tree x */
	x->right = y->left;
	if (x->right != AVL_NIL(avlt))
		x->right->parent = x;

	/* tree y */
	y->parent = x->parent;
	if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	/* assemble tree x and tree y */
	y->left = x;
	x->parent = y;

	return y;
}

/*
 * rotate right about x
 * return the new root
 */
avlnode *rotate_right(avltree *avlt, avlnode *x)
{
	avlnode *y;

	y = x->left; /* child */

	/* tree x */
	x->left = y->right;
	if (x->left != AVL_NIL(avlt))
		x->left->parent = x;

	/* tree y */
	y->parent = x->parent;
	if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	/* assemble tree x and tree y */
	y->right = x;
	x->parent = y;

	return y;
}

/*
 * fix left imbalance after insertion
 * return the new root
 */
avlnode *fix_insert_leftimbalance(avltree *avlt, avlnode *p)
{
	/* p->left->bf updated here */

	if (p->left->bf == p->bf) { /* -1, -1 */
		p = rotate_right(avlt, p);
		p->bf = p->right->bf = 0;
	} else { /* 1, -1 */
		int oldbf;
		oldbf = p->left->right->bf;
		rotate_left(avlt, p->left);
		p = rotate_right(avlt, p);
		p->bf = 0;
		if (oldbf == -1) {
			p->left->bf = 0;
			p->right->bf = 1;
		} else if (oldbf == 1) {
			p->left->bf = -1;
			p->right->bf = 0;
		} else if (oldbf == 0) {
			p->left->bf = p->right->bf = 0;
		}
	}
	return p;
}

/*
 * fix right imbalance after insertion
 * return the new root
 */
avlnode *fix_insert_rightimbalance(avltree *avlt, avlnode *p)
{
	if (p->right->bf == p->bf) { /* 1, 1 */
		p = rotate_left(avlt, p);
		p->bf = p->left->bf = 0;
	} else { /* -1, 1 */
		int oldbf;
		oldbf = p->right->left->bf;
		rotate_right(avlt, p->right);
		p = rotate_left(avlt, p);
		p->bf = 0;
		if (oldbf == -1) {
			p->left->bf = 0;
			p->right->bf = 1;
		} else if (oldbf == 1) {
			p->left->bf = -1;
			p->right->bf = 0;
		} else if (oldbf == 0) {
			p->left->bf = p->right->bf = 0;
		}
	}
	return p;
}

/*
 * fix left imbalance after deletion
 * return the new root
 */
avlnode *fix_delete_leftimbalance(avltree *avlt, avlnode *p)
{
	if (p->left->bf == -1) {
		p = rotate_right(avlt, p);
		p->bf = p->right->bf = 0;
	} else if (p->left->bf == 0) {
		p = rotate_right(avlt, p);
		p->bf = 1;
		p->right->bf = -1;
	} else if (p->left->bf == 1) {
		int oldbf;
		oldbf = p->left->right->bf;
		rotate_left(avlt, p->left);
		p = rotate_right(avlt, p);
		p->bf = 0;
		if (oldbf == -1) {
			p->left->bf = 0;
			p->right->bf = 1;
		} else if (oldbf == 1) {
			p->left->bf = -1;
			p->right->bf = 0;
		} else if (oldbf == 0) {
			p->left->bf = p->right->bf = 0;
		}
	}
	return p;
}

/*
 * fix right imbalance after deletion
 * return the new root
 */
avlnode *fix_delete_rightimbalance(avltree *avlt, avlnode *p)
{
	if (p->right->bf == 1) {
		p = rotate_left(avlt, p);
		p->bf = p->left->bf = 0;
	} else if (p->right->bf == 0) {
		p = rotate_left(avlt, p);
		p->bf = -1;
		p->left->bf = 1;
	} else if (p->right->bf == -1) {
		int oldbf;
		oldbf = p->right->left->bf;
		rotate_right(avlt, p->right);
		p = rotate_left(avlt, p);
		p->bf = 0;
		if (oldbf == -1) {
			p->left->bf = 0;
			p->right->bf = 1;
		} else if (oldbf == 1) {
			p->left->bf = -1;
			p->right->bf = 0;
		} else if (oldbf == 0) {
			p->left->bf = p->right->bf = 0;
		}
	}
	return p;
}

/*
 * check order recursively
 */
int check_order(avltree *avlt, avlnode *n, void *min, void *max)
{
	if (n == AVL_NIL(avlt))
		return 1;

	#ifdef AVL_DUP
	if (avlt->compare(n->data, min) < 0 || avlt->compare(n->data, max) > 0)
	#else
	if (avlt->compare(n->data, min) <= 0 || avlt->compare(n->data, max) >= 0)
	#endif
		return 0;

	return check_order(avlt, n->left, min, n->data) && check_order(avlt, n->right, n->data, max);
}

/*
 * check height recursively
 */
int check_height(avltree *avlt, avlnode *n)
{
	int lh, rh, cmp;

	if (n == AVL_NIL(avlt))
		return 0;

	lh = check_height(avlt, n->left);
	if (lh < 0)
		return lh;
	
	rh = check_height(avlt, n->right);
	if (rh < 0)
		return rh;
	
	cmp = rh - lh;
	if (cmp < -1 || cmp > 1 || cmp != n->bf) /* check recomputed/cached balance factor */
		return -1;
	
	return 1 + ((lh > rh) ? lh : rh);
}

/*
 * check balance factor recursively
 */
int check_balance_factor(avltree *avlt, avlnode *n)
{
	int lbf, rbf;

	if (n == AVL_NIL(avlt))
		return 0;

	lbf = check_balance_factor(avlt, n->left);
	if (lbf < -1 || lbf > 1)
		return lbf;

	rbf = check_balance_factor(avlt, n->right);
	if (rbf < -1 || rbf > 1)
		return rbf;

	return n->bf;
}

/*
 * print node recursively
 */
void print(avltree *avlt, avlnode *n, void (*print_func)(void *), int depth, char *label)
{
	if (n != AVL_NIL(avlt)) {
		print(avlt, n->right, print_func, depth + 1, "R");
		printf("%*s", 8 * depth, "");
		if (label)
			printf("%s: ", label);
		print_func(n->data);
		printf(" (%s%d)\n", (n->bf >= 0) ? "+" : "", n->bf);
		print(avlt, n->left, print_func, depth + 1, "L");
	}
}

/*
 * destroy node recursively
 */
void destroy(avltree *avlt, avlnode *n)
{
	if (n != AVL_NIL(avlt)) {
		destroy(avlt, n->left);
		destroy(avlt, n->right);
		avlt->destroy(n->data);
		free(n);
	}
}
