#!/bin/bash

# test avl_destroy with valgrind --leak-check=yes ./a.out

gcc avl_bf.c avl_data.c avl_test.c && time valgrind --leak-check=yes ./a.out || time ./a.out
