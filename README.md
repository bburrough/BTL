# BTL

Bob's C++ template library. Provides single-header implementations of pair, list, AVL tree, and red-black tree. Implemented without recursion and without memory allocation for traversal.

## Pair

Store a pair of items (e.g. a key/value pair). This is provided for implementation of higher level data structures such as map.

## List

Yet another singly-linked list.

## AVL Tree

Height-balanced binary search tree. Provides O(log N) insertion, search, and delete. The type stored in the tree must have a meaningful operator==() and operator<() to facilitate storage in and retrieval from the tree.

The standard implementation uses parent pointers at each node. A Morris traversal edition which omits parent pointers is also provided.

## Red-Black Tree

Store items in a tree and retrieve them with O(log N) time complexity. The type stored in the tree must have a meaningful operator==() and operator<() to facilitate storage in and retrieval from the tree.

## Comparison of Tree Implementations

To understand which tree to select for your project:

                                        Aborted    Complete          Memory
                        Insert  Search  Traversal  Traversal         Usage
                        ------  ------  ---------  ---------         -----
    RBTree<T>            fast    slow     fast       fast     3 ptrs + 1 enum per node
    RBTreeMorris<T>      slow    slow    slowest     slow             TBD
    AVLTree<T>           fast    fast    fastest     fast     3 ptrs + 1 int per node
    AVLTreeMorris<T>     slow    fast     slow       slow     2 ptrs + 1 int per node
    
Note, if you are so memory constrained that you are considering using AVLTreeMorris<T>, make sure your compiler is configured for proper padding and alignment to benefit from AVLTreeMorris<T>'s smaller node sizes.
