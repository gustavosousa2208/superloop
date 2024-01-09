
CC = gcc
CFLAGS = -g -Iinclude -I/usr/include/libevdev-1.0/ -w
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
PFLAGS = -lncurses -ltinfo -lmenu -pthread -levdev -std=gnu99

# List of source files (excluding .c extension)
SRC_FILES = $(notdir $(basename $(wildcard $(SRC_DIR)/*.c)))

# List of object files (with .o extension)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_FILES)))

# Name of the executable
TARGET = $(BIN_DIR)/superloop

all: clean $(TARGET) run

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR) 
	$(CC) -g $^ -o $@ $(PFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

compile: clean $(TARGET)

run:
	sudo ./bin/superloop
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
