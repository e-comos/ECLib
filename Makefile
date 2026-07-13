# Makefile for ECLib
# Builds libeclib.a (static) and optionally libeclib.so (shared)

-include config.mk

CC  ?= gcc
AR  ?= ar
RM  ?= rm -rf

PREFIX          ?= /usr/local
LIB_INSTALL_DIR ?= $(PREFIX)/lib
INC_INSTALL_DIR ?= $(PREFIX)/include

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
LIB_DIR   := lib

CFLAGS ?= -O2 -Wall -Wextra -fPIC -Iinclude
LDLIBS ?=

ifeq ($(DEBUG),1)
CFLAGS := -g -O0 -Wall -Iinclude
endif

SRCS := $(shell find src -type f -name '*.c' 2>/dev/null)
OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TARGET_STATIC := $(LIB_DIR)/libeclib.a
TARGET_SHARED := $(LIB_DIR)/libeclib.so

.PHONY: all static shared clean install uninstall config

all: static

static: $(TARGET_STATIC)

$(TARGET_STATIC): $(OBJS) | $(LIB_DIR)
	@echo "Creating static library $@"
	$(AR) rcs $@ $(OBJS)

shared: $(TARGET_SHARED)

$(TARGET_SHARED): $(OBJS) | $(LIB_DIR)
	@echo "Creating shared library $@"
	$(CC) -shared -o $@ $(OBJS) $(LDLIBS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR):
	@mkdir -p $@

install: all
	install -d $(DESTDIR)$(LIB_INSTALL_DIR)
	install -d $(DESTDIR)$(INC_INSTALL_DIR)/eclib
	install -m 644 $(TARGET_STATIC) $(DESTDIR)$(LIB_INSTALL_DIR)/
	install -m 644 include/slfd.h   $(DESTDIR)$(INC_INSTALL_DIR)/
	install -m 644 include/eclib/*.h $(DESTDIR)$(INC_INSTALL_DIR)/eclib/
	if [ -d include/sys ]; then \
		install -d $(DESTDIR)$(INC_INSTALL_DIR)/sys; \
		install -m 644 include/sys/*.h $(DESTDIR)$(INC_INSTALL_DIR)/sys/; \
	fi

uninstall:
	rm -f  $(DESTDIR)$(LIB_INSTALL_DIR)/libeclib.a
	rm -f  $(DESTDIR)$(INC_INSTALL_DIR)/slfd.h
	rm -rf $(DESTDIR)$(INC_INSTALL_DIR)/eclib
	rm -rf $(DESTDIR)$(INC_INSTALL_DIR)/sys

config:
	python3 configure

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) config.mk
