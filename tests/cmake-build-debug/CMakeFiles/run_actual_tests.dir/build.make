# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/home/workspace/kvengine/tests

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/home/workspace/kvengine/tests/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/run_actual_tests.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/run_actual_tests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/run_actual_tests.dir/flags.make

CMakeFiles/run_actual_tests.dir/testdemo.o: CMakeFiles/run_actual_tests.dir/flags.make
CMakeFiles/run_actual_tests.dir/testdemo.o: ../testdemo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/home/workspace/kvengine/tests/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/run_actual_tests.dir/testdemo.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/run_actual_tests.dir/testdemo.o -c /mnt/d/home/workspace/kvengine/tests/testdemo.cpp

CMakeFiles/run_actual_tests.dir/testdemo.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_actual_tests.dir/testdemo.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/home/workspace/kvengine/tests/testdemo.cpp > CMakeFiles/run_actual_tests.dir/testdemo.i

CMakeFiles/run_actual_tests.dir/testdemo.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_actual_tests.dir/testdemo.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/home/workspace/kvengine/tests/testdemo.cpp -o CMakeFiles/run_actual_tests.dir/testdemo.s

# Object files for target run_actual_tests
run_actual_tests_OBJECTS = \
"CMakeFiles/run_actual_tests.dir/testdemo.o"

# External object files for target run_actual_tests
run_actual_tests_EXTERNAL_OBJECTS =

run_actual_tests: CMakeFiles/run_actual_tests.dir/testdemo.o
run_actual_tests: CMakeFiles/run_actual_tests.dir/build.make
run_actual_tests: lib/libgtestd.a
run_actual_tests: lib/libgtest_maind.a
run_actual_tests: lib/libgtestd.a
run_actual_tests: CMakeFiles/run_actual_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/home/workspace/kvengine/tests/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable run_actual_tests"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/run_actual_tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/run_actual_tests.dir/build: run_actual_tests

.PHONY : CMakeFiles/run_actual_tests.dir/build

CMakeFiles/run_actual_tests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/run_actual_tests.dir/cmake_clean.cmake
.PHONY : CMakeFiles/run_actual_tests.dir/clean

CMakeFiles/run_actual_tests.dir/depend:
	cd /mnt/d/home/workspace/kvengine/tests/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/home/workspace/kvengine/tests /mnt/d/home/workspace/kvengine/tests /mnt/d/home/workspace/kvengine/tests/cmake-build-debug /mnt/d/home/workspace/kvengine/tests/cmake-build-debug /mnt/d/home/workspace/kvengine/tests/cmake-build-debug/CMakeFiles/run_actual_tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/run_actual_tests.dir/depend

