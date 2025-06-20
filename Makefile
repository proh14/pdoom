CC=gcc
TARGET_EXEC := pdoom
BUILD_DIR := ./build
INSTALL_DIR := /usr/local/bin
SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) 
CFLAGS := $(INC_FLAGS) -MMD -MP -Wall --debug -Wextra -pedantic -std=c99
LDFLAGS := $(shell pkg-config --cflags --libs sdl3) -lm

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean install uninstall

clean:
	rm -r $(BUILD_DIR)

install: $(BUILD_DIR)/$(TARGET_EXEC)
	sudo install -m 755 $< $(INSTALL_DIR)/$(TARGET_EXEC)

uninstall: $(INSTALL_DIR)/$(TARGET_EXEC)
	rm $(INSTALL_DIR)/$(TARGET_EXEC) 



-include $(DEPS)
