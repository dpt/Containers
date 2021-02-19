Containers
==========

This is a library of associative array data structures implemented as binary trees in C.

We implement the following types of binary tree:

- 'bstree'   - a binary search tree
- 'dstree'   - a digital search tree
- 'trie'     - a Fredkin tree
- 'critbit'  - a crit-bit tree
- 'patricia' - a PATRICIA trie

Each have their own strengths and (dis)advantages.

See 'Algorithms' by Sedgewick for details of the algorithms.

Two additional associative data structures are provided for comparison with the binary tree data structures:

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

The benefit of the Containers interface is a regular interface which makes it possible to swap between data structures with little or no changes to the calling code. The cost is more code which may perform redundant work depending on your use case.

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

Testing
-------

The containers layer was developed largely to simplify testing of the lower-level data structures. container-test.c uses the containers interface to perform the same tests across all available data structures:

- string test:
	1. create container
	1. insert test data (given in testdata.h)
	1. delete test data
	1. insert test data again
		1. at each step: look up every inserted key, ensure that all can be found and that the values match
	1. print whatever the fifth element contains
	1. enumerate keys by prefix
	1. look up a nonexistent key
	1. dump data
	1. dump data to Graphviz format
	1. remove every other key
	1. remove remaining keys
	1. dump data (which ought to be empty)
	1. destroy container

- common prefix test:
	1. create container
	1. insert test data (strings with common prefixes)
	1. dump data
	1. dump data to Graphviz format
	1. destroy container

- integer test:
	1. create container
	1. insert test data (mapping numbers to names)
		1. at each step: look up every inserted key, ensure that all can be found and that the values match
	1. print whatever the fifth element contains
	1. enumerate keys by prefix
	1. look up a nonexistent key
	1. dump data
	1. dump data to Graphviz format
	1. remove every other key
	1. remove remaining keys
	1. dump data (which ought to be empty)
	1. destroy container

- character test:
	1. create container
	1. insert test data (mapping characters to their ordinals)
		1. at each step: look up every inserted key, ensure that all can be found and that the values match
	1. print whatever the fifth element contains
	1. enumerate keys by prefix
	1. look up a nonexistent key
	1. dump data
	1. dump data to Graphviz format
	1. remove every other key
	1. remove remaining keys
	1. dump data (which ought to be empty)
	1. destroy container

Graphs
------

The critbit, dstree, patricia and trie tests dump representations of their data structures to [Graphviz](http://www.graphviz.org/) format. The produced .gv files can be run through Graphviz's 'dot' tool, rendering the directed graph into boxes, lines and arrows.

To turn these into, for example, a PDF file:

    dot -Tpdf -O dstree-string.gv

The above command will write out dstree-string.gv.pdf.

License
-------

Copyright (c) 2009-2021, David Thomas. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

