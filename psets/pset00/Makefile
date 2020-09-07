# C/C++ compilation involves two stages:
# - compiling: convert each source (*.cpp) file independently into binary
#   object (.o) files
# - linking: assemble all the object files into a single executable
#
# Since any C++ project contains multiple source files, the above
# process involves firing multiple commands. Makefiles help us
# write all the compilation stages in a single file and run them
# as one big script.
#
# See http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# and many other tutorials on makefiles.

# === Variables ================================================================

# We define some variables, so we don't repeat ourselves in the rest of
# the script. It also makes it easier to change the configuration later on if
# we decide to do so.
BUILD_DIR  :=_build
EXECUTABLE := a0
OUTPUT := Output

# We declare the C++ compiler/linker to be used. 
# We define it as a variable so we can change
# it easily if needed.
CXX := g++ -Wall

# === Targets ==================================================================

# In the remainder of the Makefile, we define a list of 'targets' and their dependencies.
# When we type the command 'make <target>' in the command line for a given <target>, 
# Make will figure out the dependencies needed by <target> and specified in this Makefile.
# The basic syntax is:
# <target>: <dependencies>
# 	<command to run>


# 'make' or 'make all' runs the default target 'all' which requires that
# the EXECUTABLE must be available; if the EXECUTABLE is already available
# then nothing happens. Otherwise, make will follow the rules to for
# EXECUTABLE, which you can see below. .PHONY tells make that "all" is not a file or 
# and executable but really just the name of a recipe

all: $(EXECUTABLE)
.PHONY: all

# ------------------------------------------------------------------------------

# 'make run' runs the target 'run' which calls for running the executable
# if the executable is not available, then the rules for creating it are run
# this is a superset of the target 'all' because it creates the executable
# and also runs it

run: $(EXECUTABLE)
	./$(EXECUTABLE)
.PHONY: run

# ------------------------------------------------------------------------------

# 'make clean' runs the target 'clean' which in turn removes the
# intermediate .o files and the executable

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) $(OUTPUT)
.PHONY: clean


# ------------------------------------------------------------------------------

# rule for creating the executable: this "links" the .o files using the g++ linker.
# If .o files are not available, then the rules for creating .o files are run.

$(EXECUTABLE): $(BUILD_DIR)/a0_main.o $(BUILD_DIR)/a0.o $(BUILD_DIR)/Image.o $(BUILD_DIR)/lodepng.o
	$(CXX) $(BUILD_DIR)/a0_main.o $(BUILD_DIR)/a0.o $(BUILD_DIR)/Image.o $(BUILD_DIR)/lodepng.o -o $(EXECUTABLE)
	mkdir -p $(OUTPUT)

# ------------------------------------------------------------------------------

# rules for creating the .o files:  compile each of the .cpp files and create a
# corresponding .o file. Each .o depends upon the corresponding .cpp.
# They also implicitly depend on the .h files, but since the .h is included in the .cpp,
# we don't have to explicitely list this dependency.

$(BUILD_DIR)/a0.o: a0.cpp 
	mkdir -p $(BUILD_DIR)
	$(CXX) -c a0.cpp -o $(BUILD_DIR)/a0.o

$(BUILD_DIR)/a0_main.o: a0_main.cpp 
	mkdir -p $(BUILD_DIR)
	$(CXX) -c a0_main.cpp -o $(BUILD_DIR)/a0_main.o

$(BUILD_DIR)/Image.o: Image.cpp 
	mkdir -p $(BUILD_DIR)
	$(CXX) -c Image.cpp -o $(BUILD_DIR)/Image.o

$(BUILD_DIR)/lodepng.o: lodepng.cpp 
	mkdir -p $(BUILD_DIR)
	$(CXX) -c lodepng.cpp -o $(BUILD_DIR)/lodepng.o
