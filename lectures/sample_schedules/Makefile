#
# Modify HALIDE_DIR to the appropriate path on your machine.
#
# Special instructions for Mac users
# ==================================
# You need to or must have installed libpng through Macports or Homebrew.
# Assuming that the installation succeeded, you should be able to run
#
# The brew command for installing libpng is
# brew install libpng
#
# libpng-config --I_opts
# libpng-config --L_opts
#
# Please add the output of the above commands to the following variables:
# PNG_INC
# PNG_LIB
#

MKDIR	:= mkdir -p
RM		:= rm -f
CP		:= cp -f
CXX		:= g++ -std=c++11

HALIDE_DIR ?= $(HOME)/Projects/Halide/
HALIDE_LIB := $(HALIDE_DIR)/bin/libHalide.a

PNG_INC :=
PNG_LIB :=

all: test

CXXFLAGS := $(PNG_INC) -I$(HALIDE_DIR)/include/ -I. -g -Wall
LDFLAGS  := $(PNG_LIB) -L$(HALIDE_DIR)/bin/     -lz -lpthread -ldl -lncurses -lpng

test: main.cpp $(HALIDE_LIB)
	$(CXX) $(CXXFLAGS) main.cpp $(HALIDE_LIB) $(LDFLAGS) -o $@

run: test
	./test

clean:
	$(RM) test
