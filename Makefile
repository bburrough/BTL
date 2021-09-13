
#CC=clang --analyze
#CXX=clang++ --analyze
CC=clang
CXX=clang++
#CC=i686-pc-mingw32-gcc
#CXX=i686-pc-mingw32-g++
#CC=i686-pc-cygwin-gcc
#CXX=i686-pc-cygwin-gcc
	
#CXX_FLAGS = -g3 -gdwarf-2 -DDEBUG -g -Wall -fanalyzer -Wanalyzer-too-complex
CXX_FLAGS = -Wall -g

# Final binary
BIN = main.exe
# Put all auto generated stuff to this build dir.
BUILD_DIR = ./build

# List of all .cpp source files.
CPP = $(wildcard *.cpp)

# All .o files go to build dir.
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)

# Default target named after the binary.
$(BIN) : $(BUILD_DIR)/$(BIN)

# Actual target of the binary - depends on all .o files.
$(BUILD_DIR)/$(BIN) : $(OBJ)
# Create build directories - same structure as sources.
	mkdir -p $(@D)
# Just link all the object files.
	$(CXX) $(CXX_FLAGS) $^ -o $@

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
# The -MMD flags additionaly creates a .d file with
# the same name as the .o file.
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

.PHONY : clean
clean :
# This should remove all generated files.
	-rm $(BUILD_DIR)/$(BIN) $(OBJ) $(DEP)
