Containers
==========

This is a library containing C implementations of binary trees.

So far we have the following implementations:

- bstree  - a binary search tree
- dstree  - a digital search tree
- trie    - a Fredkin tree
- critbit - a crit-bit tree

Not yet implemented:

- patricia - a PATRICIA trie

See 'Algorithms' by Sedgewick for details of the algorithms.

Two additional libraries are provided for comparison with the tree data
structures:

- linkedlist   - a linked list in which elements are kept sorted
- orderedarray - an ordered array (again, sorted)

Organisation
------------

The code is divided into two layers. You can either call the data structure manager functions directly using the interfaces defined in:

- bstree.h
- critbit.h
- dstree.h
- linkedlist.h
- orderedarray.h
- patricia.h
- trie.h

Or you can use the higher-level "Containers" interface defined in:

- icontainer.h

To create a container use one of:

- container-bstree.h::`container_create_bstree`
- container-critbit.h::`container_create_critbit`
- container-dstree.h::`container_create_dstree`
- container-linkedlist.h::`container_create_linkedlist`
- container-orderedarray.h::`container_create_orderedarray`
- container-patricia.h::`container_create_patricia`
- container-trie.h::`container_create_trie`

These are all `icontainer_maker` functions as defined in:

- icontainer-maker.h

Maker functions accept pointers to key and value interfaces then allocate and populate an `icontainer_t` interface. Key and value interfaces are specified using `icontainer_key_t` and `icontainer_value_t`. They are respectively defined in:

- icontainer-key.h
- icontainer-value.h

(Interfaces common to both key and value icontainers live in icontainer-kv.h).

Pre-prepared key and value implementations are available in:

- char-kv.h
- int-kv.h
- string-kv.h

(Implementations common to these live in kv-common.h).

License
-------
Copyright (C) David Thomas, 2011-2012. 
Proper license to be sorted out in due course.

