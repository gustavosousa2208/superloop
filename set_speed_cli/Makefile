
CC = gcc
CFLAGS = -g -Iinclude -w
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
PFLAGS = -lncurses -ltinfo -lmenu -pthread -levdev -lm -std=gnu99

CONFIG ?= 0
EXPERIMENTAL ?= exp

# List of source files (excluding .c extension)
SRC_FILES = $(notdir $(basename $(wildcard $(SRC_DIR)/*.c)))

# List of object files (with .o extension)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_FILES)))

# Name of the executable
TARGET = $(BIN_DIR)/set_speed_cli

all: clean $(TARGET) run

compile: clean $(TARGET)

# Configuration selection
ifeq ($(CONFIG), debug)
    CFLAGS += -DDEBUG
endif

ifeq ($(EXPERIMENTAL), exp)
    CFLAGS += -DEXPERIMENTAL
endif


$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR) 
	$(CC) -g $^ -o $@ $(PFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: compile
	sudo ./bin/set_speed_cli v 10

run:
	sudo ./bin/set_speed_cli
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
