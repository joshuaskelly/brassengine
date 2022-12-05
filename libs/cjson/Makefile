CJSON_OBJ = cJSON.o
CJSON_LIBNAME = libcjson

LDLIBS = -lm

LIBVERSION = 1.7.15
CJSON_SOVERSION = 1

CJSON_SO_LDFLAG=-Wl,-soname=$(CJSON_LIBNAME).so.$(CJSON_SOVERSION)

PREFIX ?= /usr/local
INCLUDE_PATH ?= include/cjson
LIBRARY_PATH ?= lib

INSTALL_INCLUDE_PATH = $(DESTDIR)$(PREFIX)/$(INCLUDE_PATH)
INSTALL_LIBRARY_PATH = $(DESTDIR)$(PREFIX)/$(LIBRARY_PATH)

INSTALL ?= cp -a

CC = gcc -std=c89

PIC_FLAGS = -fPIC
R_CFLAGS = $(PIC_FLAGS) -pedantic -Wall -Werror -Wstrict-prototypes -Wwrite-strings -Wshadow -Winit-self -Wcast-align -Wformat=2 -Wmissing-prototypes -Wstrict-overflow=2 -Wcast-qual -Wc++-compat -Wundef -Wswitch-default -Wconversion $(CFLAGS)

uname := $(shell sh -c 'uname -s 2>/dev/null || echo false')

#library file extensions
SHARED = so
STATIC = a

## create dynamic (shared) library on Darwin (base OS for MacOSX and IOS)
ifeq (Darwin, $(uname))
	SHARED = dylib
	CJSON_SO_LDFLAG = ""
endif

#cJSON library names
CJSON_SHARED = $(CJSON_LIBNAME).$(SHARED)
CJSON_SHARED_VERSION = $(CJSON_LIBNAME).$(SHARED).$(LIBVERSION)
CJSON_SHARED_SO = $(CJSON_LIBNAME).$(SHARED).$(CJSON_SOVERSION)
CJSON_STATIC = $(CJSON_LIBNAME).$(STATIC)

SHARED_CMD = $(CC) -shared -o

.PHONY: all shared static clean install

all: shared static

shared: $(CJSON_SHARED)

static: $(CJSON_STATIC)

.c.o:
	$(CC) -c $(R_CFLAGS) $<

#static libraries
#cJSON
$(CJSON_STATIC): $(CJSON_OBJ)
	$(AR) $@ $<

#shared libraries .so.1.0.0
#cJSON
$(CJSON_SHARED_VERSION): $(CJSON_OBJ)
	$(CC) -shared -o $@ $< $(CJSON_SO_LDFLAG) $(LDFLAGS)

#objects
#cJSON
$(CJSON_OBJ): cJSON.c cJSON.h


#links .so -> .so.1 -> .so.1.0.0
#cJSON
$(CJSON_SHARED_SO): $(CJSON_SHARED_VERSION)
	ln -s $(CJSON_SHARED_VERSION) $(CJSON_SHARED_SO)
$(CJSON_SHARED): $(CJSON_SHARED_SO)
	ln -s $(CJSON_SHARED_SO) $(CJSON_SHARED)

#install
#cJSON
install-cjson:
	mkdir -p $(INSTALL_LIBRARY_PATH) $(INSTALL_INCLUDE_PATH)
	$(INSTALL) cJSON.h $(INSTALL_INCLUDE_PATH)
	$(INSTALL) $(CJSON_SHARED) $(CJSON_SHARED_SO) $(CJSON_SHARED_VERSION) $(INSTALL_LIBRARY_PATH)

install: install-cjson

#uninstall
#cJSON
uninstall-cjson:
	$(RM) $(INSTALL_LIBRARY_PATH)/$(CJSON_SHARED)
	$(RM) $(INSTALL_LIBRARY_PATH)/$(CJSON_SHARED_VERSION)
	$(RM) $(INSTALL_LIBRARY_PATH)/$(CJSON_SHARED_SO)
	$(RM) $(INSTALL_INCLUDE_PATH)/cJSON.h

remove-dir:
	$(if $(wildcard $(INSTALL_LIBRARY_PATH)/*.*),,rmdir $(INSTALL_LIBRARY_PATH))
	$(if $(wildcard $(INSTALL_INCLUDE_PATH)/*.*),,rmdir $(INSTALL_INCLUDE_PATH))

uninstall: uninstall-cjson remove-dir

clean:
	$(RM) $(CJSON_OBJ) #delete object files
	$(RM) $(CJSON_SHARED) $(CJSON_SHARED_VERSION) $(CJSON_SHARED_SO) $(CJSON_STATIC) #delete cJSON
