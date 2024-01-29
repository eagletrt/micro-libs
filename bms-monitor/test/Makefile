.PHONY: all test_all clear

# Build dirs
BUILD_DIR=build
BUILD_DEPS_DIR=$(BUILD_DIR)/deps

# Source and include dirs
SRC_DIR=../src
INC_DIR=../inc
UNITY_DIR=../../Unity/src

# Tools
CC=$(shell command -v gcc || command -v clang || echo /bin/gcc)
SZ=$(shell command -v size)

# Sources
C_SOURCES = $(wildcard *.c)
DEPS_SOURCES = $(wildcard $(SRC_DIR)/*.c $(UNITY_DIR)/unity.c)
SOURCES=$(C_SOURCES) $(DEPS_SOURCES)

# Include directories
C_INCLUDES = \
$(UNITY_DIR) \
$(INC_DIR)

# Executables
TARGETS=$(addprefix $(BUILD_DIR)/, $(basename $(C_SOURCES)))

OPT = -Og

C_DEFINES = \
UNITY_OUTPUT_COLOR=1

CFLAGS = $(addprefix -I,$(C_INCLUDES)) $(OPT) -Wall $(addprefix -D,$(C_DEFINES))

# List of object files
C_OBJECTS=$(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
DEPS_OBJECTS=$(addprefix $(BUILD_DEPS_DIR)/, $(notdir $(DEPS_SOURCES:.c=.o)))

OBJECTS=$(C_OBJECTS) $(DEPS_OBJECTS)
vpath %.c $(sort $(dir $(SOURCES)))

# File of the final test results
TEST_RESULTS=$(BUILD_DIR)/results.txt

all: $(TARGETS)

# Build
$(TARGETS): $(OBJECTS) Makefile
	$(CC) $@.o $(DEPS_OBJECTS) -g -o $@

$(BUILD_DEPS_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR): $(BUILD_DEPS_DIR)

$(BUILD_DEPS_DIR):
	mkdir -p $@

# Run all test
test_all: $(TARGETS)
	@echo -n "" > $(TEST_RESULTS)
	@for target in $?; do \
		./$$target | tee -a $(TEST_RESULTS); \
	done

# Clean all
clean:
	rm -rf $(BUILD_DIR)

