rwildcard = $(shell find $(1) -type f -name $(2))

NAME = cfacts

PKG_CFLAGS = $(shell pkg-config --cflags libcurl)
PKG_LIBS = $(shell pkg-config --libs libcurl)

CFLAGS = $(PKG_CFLAGS) -std=c99 -O2 -Wall -Wextra
LIBS = $(PKG_LIBS)

CC = clang

ifeq ($(origin TARGET), undefined)
	OBJ_DIR = obj/native
	BIN_DIR = bin/native
else
	OBJ_DIR = obj/$(TARGET)
	BIN_DIR = bin/$(TARGET)
	CC = clang -target $(TARGET)
endif

OBJ = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(call rwildcard,src/,*.c))
BIN = $(BIN_DIR)/$(NAME)

.PHONY:
.DEFAULT: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(call rwildcard,$(OBJ_DIR)/,*.o) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: src/%.c $(call rwildcard,src/,*.h)
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)
