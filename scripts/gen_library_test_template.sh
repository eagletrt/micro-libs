#!/usr/bin/bash

SCRIPT_NAME="$(basename "$0")"
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Help messages
read -r -d '' BRIEF_MSG << EOM
This script can generate a new library unit testing template folder
for the micro-libs given the name of library.

A valid library name should contains only number and lower case characters
as well as hypens(-) to separate names.
A library name cannot start with a number nor an hypen and cannot be ended with an hypen.
EOM
USAGE_MSG="Usage: $SCRIPT_NAME lib_name"

function panic {
    local ERROR_MSG="$1"

    >&2 echo "ERROR: $ERROR_MSG"
    >&2 echo
    >&2 echo "$BRIEF_MSG"
    >&2 echo
    >&2 echo "$USAGE_MSG"

    exit 1
}

LWD=$(pwd)
cd $SCRIPT_DIR

# Check parameters
[[ $# -ne 1 ]] && panic "Wrong number of parameters"
! ./validate_library_name.sh "$1" && panic "Invalid library name"

# Dirs
ULIBS_DIR="$(dirname "$SCRIPT_DIR")"

LIB_NAME="$1"
LIB_DIR="$ULIBS_DIR/$LIB_NAME"

TEST_DIR="$LIB_DIR/test"

[[ -d $TEST_DIR ]] && panic "Directory already exists"

# Generate directories
echo "Generating test directories..."

mkdir -p "$TEST_DIR"

# Generate files
echo "Generating $LIB_NAME test template..."

# Generate source file
echo "    - test-$LIB_NAME.c"

TEST_FILE="$TEST_DIR/test-$LIB_NAME.c"

cat > "$TEST_FILE" <<EOF
/**
 * @file test-$LIB_NAME.c
 * @brief 
 *
 * @date $(LC_ALL=C date '+%d %b %Y')
 * @author Name Surname [your@email.here]
 */

#include "unity.h"
#include "$LIB_NAME.h"

void setUp(void) {

}

void tearDown(void) {

}

void test_unity(void) {
    TEST_ASSERT(1);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_unity);

    UNITY_END();
}

EOF

# Generate Makefile
echo "    - Makefile"

UNITY_DIR="$ULIBS_DIR/Unity"
CMOCK_DIR="$ULIBS_DIR/CMock"
MAKEFILE="$TEST_DIR/Makefile"

TAB="$(printf '\t')"

cat > "$MAKEFILE" <<EOF
TARGET=test-$LIB_NAME

BUILD_DIR=build
SRC_DIR=../src
INC_DIR=../inc
UNITY_DIR=../../Unity/src

CC=\$(shell command -v gcc || command -v clang || echo /bin/gcc)
SZ=\$(shell command -v size)

OPT = -Og

C_SOURCES = \\
\$(UNITY_DIR)/unity.c \\
\$(wildcard \$(SRC_DIR)/*.c) \\
\$(wildcard *.c)

C_INCLUDES = \\
\$(UNITY_DIR) \\
\$(INC_DIR)

C_DEFINES = \\
UNITY_OUTPUT_COLOR=1

CFLAGS = \$(addprefix -I,\$(C_INCLUDES)) \$(OPT) -Wall \$(addprefix -D,\$(C_DEFINES))

# List of object files
OBJECTS = \$(addprefix \$(BUILD_DIR)/, \$(notdir \$(C_SOURCES:.c=.o)))
vpath %.c \$(sort \$(dir \$(C_SOURCES)))


# Build
\$(BUILD_DIR)/\$(TARGET): \$(OBJECTS) Makefile
${TAB}\$(CC) \$(OBJECTS) -o \$@
${TAB}@[ -n \$(SZ) ] && \$(SZ) \$@

\$(BUILD_DIR)/%.o: %.c Makefile | \$(BUILD_DIR)
${TAB}\$(CC) -c \$(CFLAGS) $< -o \$@

\$(BUILD_DIR):
${TAB}mkdir \$@

# Run test
test_all: \$(BUILD_DIR)/\$(TARGET)
${TAB}./\$<

# Clean all
clean:
${TAB}rm -rf \$(BUILD_DIR)

EOF

echo
echo "Library test template generation done!"

cd $LWD

# Giacomo Mazzucchi in data giovedì 14 dicembre 2023 all'ora 18:03:30 ha detto: "Pusha pure sul master, Enrico"

