#!/usr/bin/bash

SCRIPT_NAME="$(basename "$0")"
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Help messages
read -r -d '' BRIEF_MSG << EOM
This script can generate a new library documentation template folder
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
ULIBS_DOCS_DIR="$ULIBS_DIR/docs"
ULIBS_DOCS_PAGES="$ULIBS_DOCS_DIR/.pages"

LIB_NAME="$1"
DOCS_DIR="$ULIBS_DOCS_DIR/$LIB_NAME"
ASSETS_DIR="$DOCS_DIR/assets"

[[ -d $DOCS_DIR ]] && panic "Directory already exists"

# Generate diretories
echo "Generating documentation directories..."

mkdir -p "$ASSETS_DIR"

# Generate files
echo "Generating documentation files..."

# Files
DOCS_FILE="$DOCS_DIR/$LIB_NAME.md"
DOCS_PAGES="$DOCS_DIR/.pages"

echo "    - $LIB_NAME.md"

LIB_TITLE=$(echo ${LIB_NAME^} | tr - ' ')

cat > "$DOCS_FILE" <<EOF
# $LIB_TITLE

[comment]: <>(DOCUMENTATION OF THE LIBRARY HERE)

EOF

echo "    - .pages"

cat > "$DOCS_PAGES" <<EOF
title: $LIB_TITLE
nav:
    - $LIB_NAME.md
EOF

# Add library folder to main pages
tail -n+4 "$ULIBS_DOCS_PAGES" | grep -q "$LIB_NAME" || echo "    - $LIB_NAME" >> "$ULIBS_DOCS_PAGES"

echo
echo "Library documentation template generation done!"

cd $LWD

