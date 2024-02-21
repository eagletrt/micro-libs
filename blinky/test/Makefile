TARGET=test-blinky

BUILD_DIR=build
SRC_DIR=../src
INC_DIR=../inc
UNITY_DIR=../../Unity/src

CC=$(shell command -v gcc || command -v clang || echo /bin/gcc)
SZ=$(shell command -v size)

OPT = -Og

C_SOURCES = \
$(UNITY_DIR)/unity.c \
$(wildcard $(SRC_DIR)/*.c) \
$(wildcard *.c)

C_INCLUDES = \
$(UNITY_DIR) \
$(INC_DIR)

C_DEFINES = \
UNITY_OUTPUT_COLOR=1

CFLAGS = $(addprefix -I,$(C_INCLUDES)) $(OPT) -Wall $(addprefix -D,$(C_DEFINES))

# List of object files
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))


# Build
$(BUILD_DIR)/$(TARGET): $(OBJECTS) Makefile
	$(CC) $(OBJECTS) -o $@
	@[ -n $(SZ) ] && $(SZ) $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

# Run test
test_all: $(BUILD_DIR)/$(TARGET)
	./$<

# Clean all
clean:
	rm -rf $(BUILD_DIR)

