
# AVL tree implementation

## overview

The AVL tree is named after its two Soviet inventors, Georgy Adelson-Velsky and Evgenii Landis, who published it in their 1962 paper "An algorithm for the organization of information". an AVL tree is a self-balancing binary search tree. It was the first such data structure to be invented.

In an AVL tree, the heights of the two child subtrees of any node differ by at most one; each node stores its height (alternatively, can just store difference in heights), if at any time they differ by more than one, rebalancing is done to restore this property until height unchanged or root reached.

Balance factors can be kept up-to-date by knowning the previous balance factors and the change in height - it is not necesary to know the absolute height.

## files

* avl_bf.h - AVL tree header
* avl_bf.c - AVL tree library
* avl_data.h - tree node data header
* avl_data.c - tree node data library
* avl_example.c - AVL tree example code
* avl_test.c - unit test program
* avl_test.sh - unit test shell script
* avl_note.txt - implementation note