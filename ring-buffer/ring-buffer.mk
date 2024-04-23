###############################################################################
# Ring buffer makefile
# Generated by gen_library_template.sh, for more information refer to:
# https://github.com/eagletrt/micro-libs/blob/master/README.md
#
# Generation date: 2024-04-14 19:33:16 +0200
# To use include this file in your Makefile as follows:
#     include path/to/library/root/dir/ring-buffer.mk
###############################################################################


###############################################################################
### DIRECTORIES                                                             ###
###############################################################################

# Root directory of the library
RING_BUFFER_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

###############################################################################
### SOURCES    								                                ###
###############################################################################

# List of all the library sources
RING_BUFFER_C_SOURCES?=$(shell find $(RING_BUFFER_DIR)src -name "*.c")

###############################################################################
### HEADERS                                                                 ###
###############################################################################

# List of all the library include directories
RING_BUFFER_C_INCLUDE_DIRS?=$(shell find $(RING_BUFFER_DIR)inc -type d)
# List of all the library include directories with the -I prefix
RING_BUFFER_C_INCLUDE_DIRS_PREFIX?=$(addprefix -I, $(RING_BUFFER_C_INCLUDE_DIRS))
