MUNIT_SOURCE=munit/munit.c
LIB_INCLUDE=munit

SRC=../

INC=../
INC_PARAMS=$(foreach d, $(INC), -I$d)

BUILD_DIR=build
EXECUTABLE=main
TARGET=$(BUILD_DIR)/$(EXECUTABLE)

SOURCES=main.c $(SRC)llist.c
OBJECTS=test_list.o
OBJ=$(BUILD_DIR)/$(OBJECTS)

CC?=gcc

CFLAGS=-I $(LIB_INCLUDE) $(INC_PARAMS) -Wall# -fprofile-arcs -ftest-coverage

.PHONY: all
all: $(TARGET)

$(TARGET): $(SOURCES) $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(MUNIT_SOURCE) $(CFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)
	rm -d $(BUILD_DIR)

.PHONY: test
test: all
	$(TARGET)
