LIBS = gtk+-3.0

CC = gcc
CFLAGS = -Wall -Wextra -Wno-deprecated-declarations -O2 -std=c23 -flto

ifneq ($(LIBS),)
    CFLAGS += $(shell pkgconf -cflags $(LIBS))
    LDLIBS += $(shell pkgconf -libs $(LIBS))
endif

.EXTRA_PREREQS = Makefile

.PHONY: all clean

all: tray-run

clean:
	$(RM) tray-run
