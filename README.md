
# An AVL Tree Implementation In C

There are several choices when implementing AVL trees:
- store height or balance factor
- store parent reference or not
- recursive or non-recursive (iterative)

This implementation's choice:
- store balance factor
- store parent reference
- non-recursive (iterative)

Files:
- avl_bf.h - AVL tree header
- avl_bf.c - AVL tree library
- avl_data.h - data header
- avl_data.c - data library
- avl_example.c - example code for AVL tree application
- avl_test.c - unit test program
- avl_test.sh - unit test shell script
- README.md - implementation note

If you have suggestions, corrections, or comments, please get in touch with [xieqing](https://github.com/xieqing).

## DEFINITION

The AVL tree is named after its two Soviet inventors, Georgy Adelson-Velsky and Evgenii Landis, who published it in their 1962 paper "An algorithm for the organization of information". It was the first such data structure to be invented.

In an AVL tree, the heights of the two child subtrees of any node differ by at most one; each node stores its height (alternatively, can just store difference in heights), if at any time they differ by more than one, rebalancing is done to restore this property.

In a binary search tree the balance factor of a node N is defined to be the height difference of its two child subtrees.

```
BalanceFactor(N) = Height(RightSubtree(N)) – Height(LeftSubtree(N))
```

A binary search tree is defined to be an AVL tree if the invariant holds for every node N in the tree.

```
BalanceFactor(N) ∈ {–1,0,+1}
```

A node N with BalanceFactor(N) < 0 is called "left-heavy", one with BalanceFactor(N) > 0 is called "right-heavy", and one with BalanceFactor(N) = 0 is sometimes simply called "balanced".

Balance factors can be kept up-to-date by knowning the previous balance factors and the change in height - it is not necesary to know the absolute height.

Two main properties of AVL trees:
- Binary Search Property: in-order sequence of the keys, ensures that we can search for any value in O(height);
- Balance Factor Property: the heights of two child subtrees of any node differ by at most one, ensures that the height of an AVL tree is always O(log N).

## ROTATION

It is easy to check that a single rotation preserves the ordering requirement for a binary search tree. The keys in subtree A are less than or equal to x, the keys in tree C are greater than or equal to y, and the keys in B are between x and y.

```
Before rotation

      x
     / \
    A   y
       / \
      B   C

After rotation

        y
       / \
      x   C
     / \
    A   B
```

## SEARCH

Searching for a specific key in an AVL tree can be done the same way as that of a normal binary search tree.

## MODIFICATION

After a modifying operation (e.g. insertion, deletion) it is necessary to update the balance factors of all nodes, a little thought should convince you that all nodes requiring correction must be on the path from the root to the modified node, and these nodes are ancestors of the modified node.

If a temporary height difference of more than one arises between two child subtrees, the parent subtree has to be rebalanced by rotations.

Rotations never violate the binary search property and the balance factor property, insertions and deletions never violate the binary search property, and violations of the balance factor property can be restored by rotations.

### INSERTION

The effective insertion of the new node increases the height of the corresponding child tree from 0 to 1. Starting at this subtree, it is necessary to check each of the ancestors for consistency with the invariants of AVL trees.
1. insert as in simple binary search tree.
2. backtrack the top-down path from the root to the new node: update the balance factor of parent node; rebalance if the balance factor of parent node temporarily becomes +2 or -2 (parent subtree has the same height as before, thus backtracking terminate immediately); terminate if the height of that parent subtree remains unchanged (has the same height as before insertion).

**Inserting**

```
Replace the termination NIL pointer with the new node

Before insertion
      
    parent   
      |
     NIL (current)    

After insertion

      parent (height increased)
        |
     new_node (current)
       / \
    NIL   NIL
```

**Rebalancing**

```
Let x be the lowest node that violates the AVL property and let h be the height of its shorter subtree.

The first case: insert under x.left

1. insert under x.left.left

    Before insertion

                x (h+2)
               / \
        (h+1) y   C (h)
             / \
        (h) A   B (h)
            ^ insert (A may be NIL)

        bf(y) = 0; bf(x) = -1; height = h+2

    After insertion (y's balance factor has been updated)

                  x (h+3)
                 / \
          (h+2) y   C (h)
               / \
        (h+1) A   B (h)

        bf(y) = -1; bf(x) = -1; height = h+2

    After right rotation

                y (h+2)
               / \
        (h+1) A   x (h+1)
                 / \
            (h) B   C (h)

        bf(x) = 0; bf(y) = 0; height = h+2 (height unchanged)

2. insert under x.left.right

    Before insertion

                x (h+2)
               / \
        (h+1) y   C (h)
             / \
        (h) A   B (h)
                ^ insert (B may be NIL)

        bf(y) = 0; bf(x) = -1; height = h+2

    After insertion (y's balance factor has been updated)

                x (h+3)
               / \
        (h+2) y   C (h)
             / \
        (h) A   B (h+1)

        bf(y) = 1; bf(x) = -1; height = h+2

    Let's expand B one more level (since B has height h+1, it cannot be empty)

                      x (h+3)
                     / \
              (h+2) y   C (h)
                   / \
              (h) A   z' (h+1)
                     / \
        (h/h-1/h=0) U   V (h-1/h/h=0)

    After left rotation

              x
             / \
            z   C
           / \
          y   V
         / \
        A   U

    After right rotation

                  z (h+2)
                 / \
                /   \
         (h+1) y     x (h+1)
              / \   / \
         (h) A   U V   C (h)
        (h/h-1/h=0)(h-1/h/h=0)

        bf(z') = -1; bf(y) = 0; bf(x) = 1; bf(z) = 0; height = h+2 (height unchanged)
        bf(z') = 1; bf(y) = -1; bf(x) = 0; bf(z) = 0; height = h+2 (height unchanged)
        bf(z') = 0; bf(y) = 0; bf(x) = 0; bf(z) = 0; height = h+2 (height unchanged)

The second case: insert under x.right

1. insert under x.right.right

    Before insertion

              x (h+2)
             / \
        (h) A   y (h+1)
               / \
          (h) B   C (h)
                  ^ insert (C may be NIL)

        bf(y) = 0; bf(x) = 1; height = h+2

    After insertion (y's balance factor has been updated)

              x
             / \
        (h) A   y (h+2)
               / \
          (h) B   C (h+1)
        
        bf(y) = 1; bf(x) = 1; height = h+2

    After left rotation

                y (h+2)
               / \
        (h+1) x   C (h+1)
             / \
        (h) A   B (h)

        bf(x) = 0; bf(y) = 0; height = h+2 (height unchanged)

2. insert under x.right.left

    Before insertion

              x (h+2)
             / \
        (h) A   y (h+1)
               / \
          (h) B   C (h)
              ^ insert (B may be NIL)

        bf(y) = 0; bf(x) = 1; height = h+2

    After insertion (y's balance factor has been updated)

              x
             / \
        (h) A   y (h+2)
               / \
        (h+1) B   C (h)

        bf(y) = -1; bf(x) = 1; height = h+2

    Let's expand it one more level (since B has height h+1, it cannot be empty)

                      x (h+3)
                     / \
                (h) A   y (h+2)
                       / \
                (h+1) z'  C (h)
                     / \
        (h/h-1/h=0) U   V (h-1/h/h=0)

    After right rotation

          x
         / \
        A   z
           / \
          U   y
             / \
            V   C

    After left rotation

                  z (h+2)
                 / \
                /   \
         (h+1) y     x (h+1)
              / \   / \
         (h) A   U V   C (h)
        (h/h-1/h=0)(h-1/h/h=0)

        bf(z') = -1; bf(y) = 0; bf(x) = 1; bf(z) = 0; height = h+2 (height unchanged)
        bf(z') = 1; bf(y) = -1; bf(x) = 0; bf(z) = 0; height = h+2 (height unchanged)
        bf(z') = 0; bf(y) = 0; bf(x) = 0; bf(z) = 0; height = h+2 (height unchanged)
```

### DELETION

The effective deletion of the subject node or the replacement node decreases the height of the corresponding child tree either from 1 to 0 or from 2 to 1, if that node had a child. Starting at this subtree, it is necessary to check each of the ancestors for consistency with the invariants of AVL trees.
1. find the subject node or its replacement node (in-order successor) if the subject node has two children, not to remove it for the time being.
2. backtrack the top-down path from the root to the subject node or the replacement node: update the balance factor of parent node; rebalance if the balance factor of parent node temporarily becomes +2 or -2; terminate if the height of that parent subtree remains unchanged (has the same height as before deletion).
3. remove the subject node or the replacement node.

**Rebalancing**

```
Let x be the lowest node that violates the AVL property and let h+1 be the height of its shorter subtree.

The first case: delete under x.right

1. x.left is left-heavy or balanced

    Before deletion
    
                      x (h+3)
                     / \
              (h+2) y   C (h+1)
                   / \  ^ delete
        (h+1/h+1) A   B (h/h+1)
        
        bf(y) = -1; bf(x) = -1; height = h+3
        bf(y) = 0; bf(x) = -1; height = h+3

    After deletion
    
                      x
                     / \
              (h+2) y'  C (h)
                   / \
        (h+1/h+1) A   B (h/h+1)

    After right rotation

                    y (h+2/h+3)
                   / \
        (h+1/h+1) A   x (h+1/h+2)
                     / \
            (h/h+1) B   C (h)
        
        bf(y') = -1; bf(x) = 0; bf(y) = 0; height = h+2 (height decreased)
        bf(y') = 0; bf(x) = -1; bf(y) = 1; height = h+3 (height unchanged)

2. x.left is right-heavy

    Before deletion
    
                    x (h+3)
                   / \
            (h+2) y   C (h+1)
                 / \  ^ delete
            (h) A   B (h+1)
        
        bf(y) = 1; bf(x) = -1; height = h+3

    After deletion

                    x (h+1)
                   / \
            (h+2) y   C (h)
                 / \
            (h) A   B (h+1)

    Let's expand B one more level (since B has height h+1, it cannot be empty)

              (h+3) x
                   / \
            (h+2) y   C (h)
                 / \
            (h) A   z' (h+1)
                   / \
        (h/h-1/h) U   V (h-1/h/h)

    After left rotation

              x
             / \
            z   C
           / \
          y   V
         / \
        A   U
        
    After right rotation

                 z (h+2)
                / \
               /   \
        (h+1) y     x (h+1)
             / \   / \
        (h) A   U V   C (h)
        (h/h-1/h)(h-1/h/h)
        
        bf(z') = -1; bf(y) = 0; bf(x) = 1; bf(z) = 0; height = h+2 (height decreased)
        bf(z') = 1; bf(y) = -1; bf(x) = 0; bf(z) = 0; height = h+2 (height decreased)
        bf(z') = 0; bf(y) = 0; bf(x) = 0; bf(z) = 0; height = h+2 (height decreased)

The sencond case: delete under x.left

1. x.right is right-heavy or balanced

    Before deletion
    
                 x (h+3)
                / \
         (h+1) A   y (h+2)
        delete ^  / \
         (h/h+1) B   C (h+1/h+1)
        
        bf(y) = 1; bf(x) = 1; height = h+3
        bf(y) = 0; bf(x) = 1; height = h+3

    After deletion

                x (h+3)
               / \
          (h) A   y' (h+2)
                 / \
        (h/h+1) B   C (h+1/h+1)

    After left rotation

                    y (h+2/h+3)
                   / \
        (h+1/h+2) x   C (h+1/h+1)
                 / \
            (h) A   B (h/h+1)
            
        bf(y') = 1; bf(x) = 0; bf(y) = 0; height = h+2 (height decreased)
        bf(y') = 0; bf(x) = 1; bf(y) = -1; height = h+3 (height unchanged)

2. x.right is left-heavy

    Before deletion
    
                 x (h+3)
                / \
         (h+1) A   y (h+2)
        delete ^  / \
           (h+1) B   C (h)

        bf(y) = -1; bf(x) = 1; height = h+3

    After deletion

              x (h+3)
             / \
        (h) A   y (h+2)
               / \
        (h+1) B   C (h)

    Let's expand B one more level (since B has height h+1, it cannot be empty)
    
                    x (h+3)
                   / \
              (h) A   y (h+2)
                     / \
              (h+1) z'  C (h)
                   / \
        (h/h-1/h) U   V (h-1/h/h)

    After right rotation

          x
         / \
        A   z
           / \
          U   y
             / \
            V   C
            
    After left rotation

                 z (h+2)
                / \
               /   \
        (h+1) y     x (h+1)
             / \   / \
        (h) A   U V   C (h)
         (h/h-1/h)(h-1/h/h)
        
        bf(z') = -1; bf(y) = 0; bf(x) = 1; bf(z) = 0; height = h+2 (height decreased)
        bf(z') = 1; bf(y) = -1; bf(x) = 0; bf(z) = 0; height = h+2 (height decreased)
        bf(z') = 0; bf(y) = 0; bf(x) = 0; bf(z) = 0; height = h+2 (height decreased)
```

**Removing**

```
Replace the subject node or the replacement node with its child (which may be NIL)

            parent
              |                        parent
             node               ->       |
             / \                     child/NIL
child/NIL/NIL   NIL/child/NIL
```

## References

1. [https://en.wikipedia.org/wiki/AVL_tree](https://en.wikipedia.org/wiki/AVL_tree)
2. [https://www.cs.usfca.edu/~galles/visualization/AVLtree.html](https://www.cs.usfca.edu/~galles/visualization/AVLtree.html)

## License

Copyright (c) 2019 xieqing. https://github.com/xieqing

May be freely redistributed, but copyright notice must be retained.
