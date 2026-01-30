CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lSDL2 -lSDL2_ttf

# Folders
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin


SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/shiu

all: $(TARGET) strip

# Link the binary
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

strip:
	strip $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

install: all
	@sudo mkdir -p /usr/local/bin
	@sudo ln -sf "$(PWD)/$(TARGET)" /usr/local/bin/shiu
	@echo "Installed successfully! Have fun using my editor :)"

uninstall:
	@sudo rm -f /usr/local/bin/shiu
	@echo "Uninstalled successfully! Thank you for your time."
	
.PHONY: all clean strip install uninstall