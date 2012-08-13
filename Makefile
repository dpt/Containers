# GNU makefile for Containers
#
# Builds the exe from two parts: a library and a non-library or host part.
# Allows 'release' and 'debug' versions of the app to be built side-by-side.
# Debug objects are suffixed 'odbg'.

# Tools

# set $(prefix) to point to cross-compiling gcc etc

cc_		= $(prefix)gcc
ar_		= $(prefix)ar
link_		= $(prefix)gcc

# Tool flags

ccflags		= -c -std=c99 $(cpu) $(warnings) $(includes) -MMD
arflags		= rc
linkflags	=

# Compiler options

cpu		=
warnings        = -Wall -Wundef -Wpointer-arith -Wuninitialized \
		  -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wunused \
		  -Wmissing-prototypes -Wmissing-declarations \
		  -Wnested-externs -Winline -Wno-unused -Wno-long-long -W \
		  -Wshadow
# -Wcast-qual disabled due to the amount of spam
includes	= -Iinclude

# Combined tool and flags

cc		= $(cc_) $(ccflags)
ar		= $(ar_) $(arflags)
link		= $(link_) $(linkflags)

# Rule patterns

.SUFFIXES:	.o .odbg

.c.o:;		$(cc) -Os -DNDEBUG $< -o $@
.c.odbg:;	$(cc) -g $< -o $@

# Names

lib		= libcontainer.a
debuglib	= libcontainerdbg.a
testlib		= libcontainertest.a
debugtestlib	= libcontainertestdbg.a

ctestexe	= container-test
debugctestexe	= $(ctestexe)dbg

wtestexe	= word-test
debugwtestexe	= $(wtestexe)dbg

# Objects

src		= $(shell find libraries -path '*/test/*' -o -name 'apps' -prune -o -name '*.c' -print)
objs		= $(src:.c=.o)
debugobjs	= $(src:.c=.odbg)
deps		= $(src:.c=.d)

testsrc		= $(shell find libraries -path '*/test/*' -name '*.c' -print)
testobjs	= $(testsrc:.c=.o)
debugtestobjs	= $(testsrc:.c=.odbg)
testdeps	= $(testsrc:.c=.d)

ctestsrc	= $(shell find apps/container-test -name '*.c')
ctestobjs	= $(ctestsrc:.c=.o)
debugctestobjs	= $(ctestsrc:.c=.odbg)
ctestdeps	= $(ctestsrc:.c=.d)

wtestsrc	= $(shell find apps/word-test -name '*.c')
wtestobjs	= $(wtestsrc:.c=.o)
debugwtestobjs	= $(wtestsrc:.c=.odbg)
wtestdeps	= $(wtestsrc:.c=.d)

# Targets

.PHONY:	release debug apps debugapps all clean 

$(lib):		$(objs)
		$(ar) $@ $(objs)

$(debuglib):	$(debugobjs)
		$(ar) $@ $(debugobjs)

$(testlib):	$(testobjs)
		$(ar) $@ $(testobjs)

$(debugtestlib):	$(debugtestobjs)
		$(ar) $@ $(debugtestobjs)

release:	$(lib)
		@echo 'release' built

debug:		$(debuglib)
		@echo 'debug' built

$(ctestexe):	$(ctestobjs) $(testlib) $(lib)
		$(link) -o $@ $^ $(extlibs)

$(debugctestexe):	$(debugctestobjs) $(debugtestlib) $(debuglib)
		$(link) -g -o $@ $^ $(extlibs)

$(wtestexe):	$(wtestobjs) $(testlib) $(lib)
		$(link) -o $@ $^ $(extlibs)

$(debugwtestexe):	$(debugwtestobjs) $(debugtestlib) $(debuglib)
		$(link) -g -o $@ $^ $(extlibs)

apps:		$(ctestexe) $(wtestexe)
		@echo 'apps' built

debugapps:	$(debugctestexe) $(debugwtestexe)
		@echo 'debugapps' built

all:		release debug apps debugapps
		@echo 'all' built

clean:
		-rm -f $(objs) $(debugobjs) $(deps)
		-rm -f $(lib) $(debuglib)
		-rm -f $(testobjs) $(debugtestobjs) $(testdeps)
		-rm -f $(testlib) $(debugtestlib) 
		-rm -f $(ctestexe) $(debugctestexe) 
		-rm -f $(ctestobjs) $(debugctestobjs) $(ctestdeps)
		-rm -f $(wtestexe) $(debugwtestexe)
		-rm -f $(wtestobjs) $(debugwtestobjs) $(wtestdeps)
		@echo Cleaned

# Dependencies

-include	$(deps) $(ctestdeps)

