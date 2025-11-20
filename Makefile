# Makefile for building directiveKILL on Linux (uses pkg-config + raylib)
# Usage:
#   make         -> build optimized binary 'main'
#   make debug   -> build with debug symbols
#   make run     -> build and run
#   make clean   -> remove objects and binary


CC := gcc

# Try to get raylib flags from pkg-config. If pkg-config or raylib.pc
# is missing, allow overrides via environment variables and fall back
# to common installation paths.

# Require pkg-config/raylib by default. Allow environment override via
# RAYLIB_CFLAGS/RAYLIB_LIBS if you want to point to a custom install.
RAYLIB_CFLAGS ?= $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS   ?= $(shell pkg-config --libs   raylib 2>/dev/null)

# Try to get the library directory from pkg-config so we can embed an rpath
# in the final binary. This makes the executable runnable for users who
# installed raylib to a non-standard prefix (for example /usr/local).
PKG_LIBDIR := $(shell pkg-config --variable=libdir raylib 2>/dev/null)
ifneq ($(strip $(PKG_LIBDIR)),)
RPATH_FLAG := -Wl,-rpath,$(PKG_LIBDIR)
else
RPATH_FLAG :=
endif

ifeq ($(strip $(RAYLIB_CFLAGS)$(RAYLIB_LIBS)),)
$(error pkg-config could not find 'raylib'. Ensure raylib is installed and either set PKG_CONFIG_PATH so pkg-config finds raylib.pc or set RAYLIB_CFLAGS/RAYLIB_LIBS environment variables.)
endif

CFLAGS := -O2 -Wall -Wextra $(RAYLIB_CFLAGS) -I include
LDFLAGS := $(RAYLIB_LIBS) -lm $(RPATH_FLAG)

SRCS := main.c $(shell find src -name '*.c')
OBJS := $(patsubst %.c,%.o,$(SRCS))

TARGET := main

.PHONY: all clean run debug check

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS := -g -O0 -Wall -Wextra $(RAYLIB_CFLAGS) -I include
debug: LDFLAGS := $(RAYLIB_LIBS) -lm $(RPATH_FLAG)
debug: clean all

run: $(TARGET)
	# Prefer the pkg-config detected libdir for runtime lookup when available.
	# If PKG_LIBDIR is empty (pkg-config missing), just run the binary normally
	# (user may need to set LD_LIBRARY_PATH or run ldconfig).
	@if [ -z "$(PKG_LIBDIR)" ]; then \
		./$(TARGET); \
	else \
		LD_LIBRARY_PATH=$(PKG_LIBDIR):$$LD_LIBRARY_PATH ./$(TARGET); \
	fi

.PHONY: check
check:
	@echo "pkg-config flags: $(RAYLIB_CFLAGS) $(RAYLIB_LIBS)"
	@if [ -n "$(PKG_LIBDIR)" ]; then \
		echo "detected raylib libdir: $(PKG_LIBDIR)"; \
		ls -ld "$(PKG_LIBDIR)" || true; \
		pkg-config --modversion raylib || true; \
	else \
		echo "pkg-config did not detect raylib. You can set RAYLIB_CFLAGS/RAYLIB_LIBS to override."; \
		echo "Example: make RAYLIB_CFLAGS='-I/path/to/include' RAYLIB_LIBS='-L/path/to/lib -lraylib -lm'"; \
	fi

clean:
	rm -f $(OBJS) $(TARGET)
