#!/usr/bin/bash

SCRIPT_NAME="$(basename "$0")"

# Help messages
read -r -d '' BRIEF_MSG << EOM
This script checks if a library name is valid.
The exit value of the script is 0 if the name is valid 1 otherwise.

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

[[ $# -ne 1 ]] && panic "Wrong number of parameters"

[[ $1 =~ ^[a-z][a-z0-9-]*[a-z0-9]$ ]] && exit 0
exit 1 
