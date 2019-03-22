SRC_DIR=./src
BIN_DIR=./bin
SRC=$(shell find $(SRC_DIR) -name "*.c")
BIN=$(SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%.out)

all: $(BIN)

$(BIN_DIR)/%.out: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	gcc $< -o $@ -ggdb3 -Wall

.PHONY: clean all

clean:
	rm -rf bin
