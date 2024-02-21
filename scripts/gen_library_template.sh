#!/usr/bin/bash

SCRIPT_NAME="$(basename "$0")"
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Help messages
read -r -d '' BRIEF_MSG << EOM
This script can generate a new library template folder
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

LWD=$(pwd) # Last working directory
cd $SCRIPT_DIR

# Check parameters
[[ $# -ne 1 ]] && panic "Wrong number of parameters"
! ./validate_library_name.sh "$1" && panic "Invalid library name"

# Dirs
ULIBS_DIR="$(dirname "$SCRIPT_DIR")"

LIB_NAME="$1"
LIB_DIR="$ULIBS_DIR/$LIB_NAME"

[[ -d $LIB_DIR ]] && panic "Directory already exists"

SRC_DIR="$LIB_DIR/src"
INC_DIR="$LIB_DIR/inc"

echo "Generating library $LIB_NAME in directory $LIB_DIR"
echo

# Generate directories
echo "Generating directories..."

mkdir -p "$SRC_DIR" "$INC_DIR"

# Generate files
echo "Generating files..."

# Files
SRC_FILE="$SRC_DIR/$LIB_NAME.c"
INC_FILE="$INC_DIR/$LIB_NAME.h"

echo "    - Readme"

cat > "$LIB_DIR/README.md" <<EOF
# ${LIB_NAME^^}

[comment]: <> (EXPLANATION OF THE LIBRARY HERE)

EOF

echo "    - $LIB_NAME.c"

cat > "$SRC_FILE" <<EOF
/**
 * @file $LIB_NAME.c
 * @brief 
 *
 * @date $(LC_ALL=C date '+%d %b %Y')
 * @author Name Surname [your@email.here]
 */

#include "$LIB_NAME.h"

// CODE HERE

EOF

echo "    - $LIB_NAME.h"

SAFEGUARD="$( echo ${LIB_NAME^^} | tr - _ )_H"

cat > "$INC_FILE" <<EOF
/**
 * @file $LIB_NAME.h
 * @brief 
 *
 * @date $(LC_ALL=C date '+%d %b %Y')
 * @author Name Surname [your@email.here]
 */

#ifndef $SAFEGUARD
#define $SAFEGUARD

// CODE HERE

#endif  // $SAFEGUARD

EOF

./gen_library_test_template.sh "$LIB_NAME"
./gen_library_docs.sh "$LIB_NAME"

echo
echo "Library template generation done!"

cd $LWD

