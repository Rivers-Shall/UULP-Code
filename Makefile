SRC_DIR=./src
BIN_DIR=./bin
SRC=$(shell find $(SRC_DIR) -name "*.c")
BIN=$(SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%.out)

$(BIN_DIR)/%.out: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	gcc $< -o $@ -ggdb3 -Wall

.PHONY: clean all
.DEFAULT_GOAL :=all

clean:
	rm -rf bin

all: $(BIN)
