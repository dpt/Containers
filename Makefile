# GNU makefile for Containers
#
# Builds the exe from two parts: a library and a non-library or host part.
# Allows 'release' and 'debug' versions of the app to be built side-by-side.
# Debug objects are suffixed 'odbg'.

# Tools

# set $(prefix) to point to cross-compiling gcc etc

cc_		= $(prefix)clang
ar_		= $(prefix)ar
link_		= $(prefix)clang

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
exe		= container-test
debugexe	= container-testdbg

# Objects

src		= $(shell find libraries -path '*/test/*' -o -name 'apps' -prune -o -name '*.c' -print)
objs		= $(src:.c=.o)
debugobjs	= $(src:.c=.odbg)
deps		= $(src:.c=.d)

testsrc		= $(shell find libraries -path '*/test/*' -name '*.c' -print)
testobjs	= $(testsrc:.c=.o)
debugtestobjs	= $(testsrc:.c=.odbg)
testdeps	= $(testsrc:.c=.d)

appsrc		= $(shell find apps -name '*.c')
appobjs		= $(appsrc:.c=.o)
debugappobjs	= $(appsrc:.c=.odbg)
appdeps		= $(appsrc:.c=.d)

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

$(exe):		$(appobjs) $(testlib) $(lib)
		$(link) -o $@ $^ $(extlibs)

$(debugexe):	$(debugappobjs) $(debugtestlib) $(debuglib)
		$(link) -g -o $@ $^ $(extlibs)

apps:		$(exe)
		@echo 'apps' built

debugapps:	$(debugexe)
		@echo 'debugapps' built

all:		release debug apps debugapps
		@echo 'all' built

clean:
		-rm $(lib) $(debuglib) $(testlib) $(debugtestlib) $(exe) $(debugexe)
		-rm $(objs) $(debugobjs) $(deps)
		-rm $(testobjs) $(debugtestobjs) $(testdeps)
		-rm $(appobjs) $(debugappobjs) $(appdeps)
		@echo Cleaned

# Dependencies

-include	$(deps) $(appdeps)

