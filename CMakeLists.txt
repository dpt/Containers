# CMakeLists.txt
#
# Containers
#
# Copyright (c) David Thomas, 2020-2021
#
# vim: sw=4

# TODO
# - Fortify
# - Rename library to DPTHashLib or something
# - add_compile_definitions(_POSIX_C_SOURCE=200809L) was previously needed for strdup() not now?

cmake_minimum_required(VERSION 3.18)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/)

project(Containers VERSION 0.1.8 DESCRIPTION "Containers library" LANGUAGES C)

# The values set in the toolchain file aren't available until this point.

if(TARGET_RISCOS AND NOT DEFINED ENV{GCCSDK_INSTALL_ENV})
    message(FATAL_ERROR "GCCSDK_INSTALL_ENV is not set")
endif()

if(TARGET_RISCOS)
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "" FORCE) # overwrite default of '-O3 -NDEBUG'
    
    # I tried to get this to live in riscos.cmake but was defeated.
    set(CMAKE_EXECUTABLE_SUFFIX ,ff8)
endif()


# If this is enabled then the final binary will need to be linked against
# Fortify too.
option(USE_FORTIFY "Use Fortify" OFF)

# Referencing CMAKE_TOOLCHAIN_FILE avoids a warning on rebuilds.
if(NOT ${CMAKE_TOOLCHAIN_FILE} STREQUAL "")
    message(STATUS "Containers: Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()

set(LIBRARY ${PROJECT_NAME})

add_library(${LIBRARY})

set_target_properties(${LIBRARY} PROPERTIES
    VERSION 0.2.7
    DESCRIPTION "Containers library"
    C_STANDARD 99
    PREFIX ""  # remove 'lib' prefix
    ARCHIVE_OUTPUT_NAME_DEBUG ${LIBRARY}-debug
    ARCHIVE_OUTPUT_NAME_RELEASE ${LIBRARY}
    ARCHIVE_OUTPUT_NAME_RELWITHDEBINFO ${LIBRARY}-relwithdebinfo
    ARCHIVE_OUTPUT_NAME_MINSIZEREL ${LIBRARY}-minsizerel)

target_include_directories(${LIBRARY}
    PUBLIC
    include)

set(PUBLIC_HEADERS
    include/container/bstree.h
    include/container/critbit.h
    include/container/dstree.h
    include/container/hash.h
    include/container/linkedlist.h
    include/container/orderedarray.h
    include/container/patricia.h
    include/container/trie.h
    include/datastruct/bstree.h
    include/datastruct/critbit.h
    include/datastruct/dstree.h
    include/datastruct/hash.h
    include/datastruct/linkedlist.h
    include/datastruct/orderedarray.h
    include/datastruct/patricia.h
    include/datastruct/queue.h
    include/datastruct/trie.h
    include/keyval/char.h
    include/keyval/common.h
    include/keyval/int.h
    include/keyval/kv.h
    include/keyval/string.h)

set(DATASTRUCT_SOURCES
    libraries/datastruct/bstree/count.c
    libraries/datastruct/bstree/create.c
    libraries/datastruct/bstree/destroy.c
    libraries/datastruct/bstree/impl.h
    libraries/datastruct/bstree/insert.c
    libraries/datastruct/bstree/lookup-prefix.c
    libraries/datastruct/bstree/lookup.c
    libraries/datastruct/bstree/node-create.c
    libraries/datastruct/bstree/node-destroy.c
    libraries/datastruct/bstree/remove.c
    libraries/datastruct/bstree/select.c
    libraries/datastruct/bstree/show-viz.c
    libraries/datastruct/bstree/show.c
    libraries/datastruct/bstree/walk-internal.c
    libraries/datastruct/bstree/walk.c
    libraries/datastruct/critbit/count.c
    libraries/datastruct/critbit/create.c
    libraries/datastruct/critbit/destroy.c
    libraries/datastruct/critbit/extnode-create.c
    libraries/datastruct/critbit/extnode-destroy.c
    libraries/datastruct/critbit/impl.h
    libraries/datastruct/critbit/insert.c
    libraries/datastruct/critbit/lookup-prefix.c
    libraries/datastruct/critbit/lookup.c
    libraries/datastruct/critbit/node-create.c
    libraries/datastruct/critbit/node-destroy.c
    libraries/datastruct/critbit/remove.c
    libraries/datastruct/critbit/select.c
    libraries/datastruct/critbit/show-viz.c
    libraries/datastruct/critbit/show.c
    libraries/datastruct/critbit/walk-internal.c
    libraries/datastruct/critbit/walk.c
    libraries/datastruct/dstree/breadthwalk-internal.c
    libraries/datastruct/dstree/count.c
    libraries/datastruct/dstree/create.c
    libraries/datastruct/dstree/destroy.c
    libraries/datastruct/dstree/impl.h
    libraries/datastruct/dstree/insert.c
    libraries/datastruct/dstree/lookup-prefix.c
    libraries/datastruct/dstree/lookup.c
    libraries/datastruct/dstree/node-clear.c
    libraries/datastruct/dstree/node-create.c
    libraries/datastruct/dstree/node-destroy.c
    libraries/datastruct/dstree/remove.c
    libraries/datastruct/dstree/remove2.c
    libraries/datastruct/dstree/select.c
    libraries/datastruct/dstree/show-viz.c
    libraries/datastruct/dstree/show.c
    libraries/datastruct/dstree/walk-internal.c
    libraries/datastruct/dstree/walk.c
    libraries/datastruct/hash/count.c
    libraries/datastruct/hash/create.c
    libraries/datastruct/hash/destroy.c
    libraries/datastruct/hash/impl.h
    libraries/datastruct/hash/insert.c
    libraries/datastruct/hash/lookup-node.c
    libraries/datastruct/hash/lookup.c
    libraries/datastruct/hash/remove.c
    libraries/datastruct/hash/show.c
    libraries/datastruct/hash/walk-cont.c
    libraries/datastruct/hash/walk-internal.c
    libraries/datastruct/hash/walk.c
    libraries/datastruct/linkedlist/count.c
    libraries/datastruct/linkedlist/create.c
    libraries/datastruct/linkedlist/destroy.c
    libraries/datastruct/linkedlist/impl.h
    libraries/datastruct/linkedlist/insert.c
    libraries/datastruct/linkedlist/lookup-prefix.c
    libraries/datastruct/linkedlist/lookup.c
    libraries/datastruct/linkedlist/node-create.c
    libraries/datastruct/linkedlist/node-destroy.c
    libraries/datastruct/linkedlist/remove.c
    libraries/datastruct/linkedlist/select.c
    libraries/datastruct/linkedlist/show.c
    libraries/datastruct/linkedlist/walk-internal.c
    libraries/datastruct/linkedlist/walk.c
    libraries/datastruct/orderedarray/count.c
    libraries/datastruct/orderedarray/create.c
    libraries/datastruct/orderedarray/destroy.c
    libraries/datastruct/orderedarray/impl.h
    libraries/datastruct/orderedarray/insert.c
    libraries/datastruct/orderedarray/lookup-prefix.c
    libraries/datastruct/orderedarray/lookup.c
    libraries/datastruct/orderedarray/node-destroy.c
    libraries/datastruct/orderedarray/remove.c
    libraries/datastruct/orderedarray/select.c
    libraries/datastruct/orderedarray/show.c
    libraries/datastruct/orderedarray/walk-internal.c
    libraries/datastruct/orderedarray/walk.c
    libraries/datastruct/patricia/count.c
    libraries/datastruct/patricia/create.c
    libraries/datastruct/patricia/destroy.c
    libraries/datastruct/patricia/impl.h
    libraries/datastruct/patricia/insert.c
    libraries/datastruct/patricia/lookup-prefix.c
    libraries/datastruct/patricia/lookup.c
    libraries/datastruct/patricia/node-clear.c
    libraries/datastruct/patricia/node-create.c
    libraries/datastruct/patricia/node-destroy.c
    libraries/datastruct/patricia/remove.c
    libraries/datastruct/patricia/select.c
    libraries/datastruct/patricia/show-viz.c
    libraries/datastruct/patricia/show.c
    libraries/datastruct/patricia/walk-internal.c
    libraries/datastruct/patricia/walk.c
    libraries/datastruct/queue/count.c
    libraries/datastruct/queue/create.c
    libraries/datastruct/queue/dequeue.c
    libraries/datastruct/queue/destroy.c
    libraries/datastruct/queue/empty.c
    libraries/datastruct/queue/enqueue.c
    libraries/datastruct/queue/full.c
    libraries/datastruct/queue/impl.h
    libraries/datastruct/trie/breadthwalk-internal.c
    libraries/datastruct/trie/count.c
    libraries/datastruct/trie/create.c
    libraries/datastruct/trie/destroy.c
    libraries/datastruct/trie/impl.h
    libraries/datastruct/trie/insert.c
    libraries/datastruct/trie/lookup-prefix.c
    libraries/datastruct/trie/lookup.c
    libraries/datastruct/trie/node-clear.c
    libraries/datastruct/trie/node-create.c
    libraries/datastruct/trie/node-destroy.c
    libraries/datastruct/trie/remove.c
    libraries/datastruct/trie/select.c
    libraries/datastruct/trie/show-viz.c
    libraries/datastruct/trie/show.c
    libraries/datastruct/trie/walk-internal.c
    libraries/datastruct/trie/walk.c)

set(CONTAINERS_SOURCES
    libraries/container/bstree/bstree.c
    libraries/container/critbit/critbit.c
    libraries/container/dstree/dstree.c
    libraries/container/hash/hash.c
    libraries/container/linkedlist/linkedlist.c
    libraries/container/orderedarray/orderedarray.c
    libraries/container/patricia/patricia.c
    libraries/container/trie/trie.c)

set(KEYVAL_SOURCES
    libraries/keyval/char.c
    libraries/keyval/common.c
    libraries/keyval/int.c
    libraries/keyval/string.c)

set(UTILS_SOURCES
    libraries/utils/primes.c
    libraries/utils/utils.c)

set(ALL_SOURCES 
    ${PUBLIC_HEADERS}
    ${DATASTRUCT_SOURCES}
    ${CONTAINERS_SOURCES}
    ${KEYVAL_SOURCES}
    ${UTILS_SOURCES})

target_sources(${LIBRARY} PRIVATE ${ALL_SOURCES})

target_compile_options(${LIBRARY} PRIVATE -Wall -Wextra -pedantic)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${LIBRARY} PRIVATE $<UPPER_CASE:${LIBRARY}>_DEBUG)
endif()

# Library dependencies
#

if(USE_FORTIFY)
    target_compile_definitions(${LIBRARY} PUBLIC FORTIFY)
    target_link_libraries(${LIBRARY} PUBLIC Fortify)
endif()


# Self-test executable
#

set(TEST ${PROJECT_NAME}Test)

set(TEST_SOURCES
    apps/container-test/main.c
    apps/container-test/test.c
    apps/container-test/test.h
    apps/container-test/testdata.h
    libraries/datastruct/queue/test/test.c)

# Avoid a warning from CMake
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

add_executable(${TEST} ${TEST_SOURCES})

set_target_properties(${TEST} PROPERTIES
    DESCRIPTION "Containers test program"
    C_STANDARD 99
    OUTPUT_NAME_DEBUG ${TEST}-debug
    OUTPUT_NAME_RELEASE ${TEST}
    OUTPUT_NAME_RELWITHDEBINFO ${TEST}-relwithdebinfo
    OUTPUT_NAME_MINSIZEREL ${TEST}-minsizerel)

# ContainersTest dependencies
# Of course, ContainersTest depends on the Containers library.
target_link_libraries(${TEST} ${LIBRARY})


# Installation
#

# Note: This affects everything built by this file.
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "CMake install prefix" FORCE)

# Install library & test program into root of build directory.
install(TARGETS ${LIBRARY} ${TEST}
    CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel
    ARCHIVE DESTINATION .
    RUNTIME DESTINATION .)
