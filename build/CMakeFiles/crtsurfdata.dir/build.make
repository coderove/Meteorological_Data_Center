# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/coderove/workspace/code/Meteorological_Center/clion/project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/coderove/workspace/code/Meteorological_Center/clion/project/build

# Include any dependencies generated for this target.
include CMakeFiles/crtsurfdata.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/crtsurfdata.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/crtsurfdata.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/crtsurfdata.dir/flags.make

CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o: CMakeFiles/crtsurfdata.dir/flags.make
CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o: /home/coderove/workspace/code/Meteorological_Center/clion/project/idc/src/crtsurfdata.cpp
CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o: CMakeFiles/crtsurfdata.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o -MF CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o.d -o CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o -c /home/coderove/workspace/code/Meteorological_Center/clion/project/idc/src/crtsurfdata.cpp

CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/coderove/workspace/code/Meteorological_Center/clion/project/idc/src/crtsurfdata.cpp > CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.i

CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/coderove/workspace/code/Meteorological_Center/clion/project/idc/src/crtsurfdata.cpp -o CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.s

CMakeFiles/crtsurfdata.dir/public/_public.cpp.o: CMakeFiles/crtsurfdata.dir/flags.make
CMakeFiles/crtsurfdata.dir/public/_public.cpp.o: /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_public.cpp
CMakeFiles/crtsurfdata.dir/public/_public.cpp.o: CMakeFiles/crtsurfdata.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/crtsurfdata.dir/public/_public.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/crtsurfdata.dir/public/_public.cpp.o -MF CMakeFiles/crtsurfdata.dir/public/_public.cpp.o.d -o CMakeFiles/crtsurfdata.dir/public/_public.cpp.o -c /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_public.cpp

CMakeFiles/crtsurfdata.dir/public/_public.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/crtsurfdata.dir/public/_public.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_public.cpp > CMakeFiles/crtsurfdata.dir/public/_public.cpp.i

CMakeFiles/crtsurfdata.dir/public/_public.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/crtsurfdata.dir/public/_public.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_public.cpp -o CMakeFiles/crtsurfdata.dir/public/_public.cpp.s

# Object files for target crtsurfdata
crtsurfdata_OBJECTS = \
"CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o" \
"CMakeFiles/crtsurfdata.dir/public/_public.cpp.o"

# External object files for target crtsurfdata
crtsurfdata_EXTERNAL_OBJECTS =

/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/crtsurfdata: CMakeFiles/crtsurfdata.dir/idc/src/crtsurfdata.cpp.o
/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/crtsurfdata: CMakeFiles/crtsurfdata.dir/public/_public.cpp.o
/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/crtsurfdata: CMakeFiles/crtsurfdata.dir/build.make
/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/crtsurfdata: CMakeFiles/crtsurfdata.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable /home/coderove/workspace/code/Meteorological_Center/clion/project/bin/crtsurfdata"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/crtsurfdata.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/crtsurfdata.dir/build: /home/coderove/workspace/code/Meteorological_Center/clion/project/bin/crtsurfdata
.PHONY : CMakeFiles/crtsurfdata.dir/build

CMakeFiles/crtsurfdata.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/crtsurfdata.dir/cmake_clean.cmake
.PHONY : CMakeFiles/crtsurfdata.dir/clean

CMakeFiles/crtsurfdata.dir/depend:
	cd /home/coderove/workspace/code/Meteorological_Center/clion/project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/coderove/workspace/code/Meteorological_Center/clion/project /home/coderove/workspace/code/Meteorological_Center/clion/project /home/coderove/workspace/code/Meteorological_Center/clion/project/build /home/coderove/workspace/code/Meteorological_Center/clion/project/build /home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles/crtsurfdata.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/crtsurfdata.dir/depend

