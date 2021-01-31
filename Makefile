ifeq ($(OS),)
OS = $(shell uname -s)
endif
PREFIX = /usr/local
CC   = gcc
CPP  = g++
AR   = ar
LIBPREFIX = lib
LIBEXT = .a
ifeq ($(OS),Windows_NT)
BINEXT = .exe
SOLIBPREFIX =
SOEXT = .dll
else ifeq ($(OS),Darwin)
BINEXT =
SOLIBPREFIX = lib
SOEXT = .dylib
else
BINEXT =
SOLIBPREFIX = lib
SOEXT = .so
endif
INCS += -Iinclude
CFLAGS += $(INCS) -Os
CPPFLAGS += $(INCS) -Os
LIBS =
LDFLAGS = -largtable2
ifeq ($(OS),Darwin)
STRIPFLAG =
else
STRIPFLAG = -s
endif
MKDIR = mkdir -p
RM = rm -f
RMDIR = rm -rf
CP = cp -f
CPDIR = cp -rf
DOXYGEN = $(shell which doxygen)

OSALIAS := $(OS)
ifeq ($(OS),Windows_NT)
ifneq (,$(findstring x86_64,$(shell gcc --version)))
OSALIAS := win64
else
OSALIAS := win32
endif
endif

TOOLS_BIN = showdate$(BINEXT)

COMMON_PACKAGE_FILES = README.md LICENSE Changelog.txt
SOURCE_PACKAGE_FILES = $(COMMON_PACKAGE_FILES) Makefile *.c build/*.cbp build/*.depend

default: all

all: $(TOOLS_BIN)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

showdate$(BINEXT): %$(BINEXT): showdate.o
	$(CC) $(STRIPFLAG) -o $@ $^ $(LDFLAGS)

install: all doc
	$(MKDIR) $(PREFIX)/bin
	$(CP) $(TOOLS_BIN) $(PREFIX)/bin/

.PHONY: version
version:
	sed -ne "s/^#define\s*SHOWDATE_VERSION_[A-Z]*\s*\([0-9]*\)\s*$$/\1./p" showdate.c | tr -d "\n" | sed -e "s/\.$$//" > version

.PHONY: package
package: version
	tar cfJ showdate-$(shell cat version).tar.xz --transform="s?^?showdate-$(shell cat version)/?" $(SOURCE_PACKAGE_FILES)

.PHONY: package
binarypackage: version
ifneq ($(OS),Windows_NT)
	$(MAKE) PREFIX=binarypackage_temp_$(OSALIAS) install
	tar cfJ showdate-$(shell cat version)-$(OSALIAS).tar.xz --transform="s?^binarypackage_temp_$(OSALIAS)/??" $(COMMON_PACKAGE_FILES) binarypackage_temp_$(OSALIAS)/*
else
	$(MAKE) PREFIX=binarypackage_temp_$(OSALIAS) install DOXYGEN=
	cp -f $(COMMON_PACKAGE_FILES) binarypackage_temp_$(OSALIAS)
	rm -f showdate-$(shell cat version)-$(OSALIAS).zip
	cd binarypackage_temp_$(OSALIAS) && zip -r9 ../showdate-$(shell cat version)-$(OSALIAS).zip $(COMMON_PACKAGE_FILES) * && cd ..
endif
	rm -rf binarypackage_temp_$(OSALIAS)

.PHONY: clean
clean:
	$(RM) *.o $(TOOLS_BIN) version showdate-*.tar.xz
