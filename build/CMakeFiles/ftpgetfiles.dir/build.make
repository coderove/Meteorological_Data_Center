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
include CMakeFiles/ftpgetfiles.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ftpgetfiles.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ftpgetfiles.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ftpgetfiles.dir/flags.make

CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o: CMakeFiles/ftpgetfiles.dir/flags.make
CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o: /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_ftp.cpp
CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o: CMakeFiles/ftpgetfiles.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o -MF CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o.d -o CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o -c /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_ftp.cpp

CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_ftp.cpp > CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.i

CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/coderove/workspace/code/Meteorological_Center/clion/project/public/_ftp.cpp -o CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.s

CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o: CMakeFiles/ftpgetfiles.dir/flags.make
CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o: /home/coderove/workspace/code/Meteorological_Center/clion/project/tools/src/ftpgetfiles.cpp
CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o: CMakeFiles/ftpgetfiles.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o -MF CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o.d -o CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o -c /home/coderove/workspace/code/Meteorological_Center/clion/project/tools/src/ftpgetfiles.cpp

CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/coderove/workspace/code/Meteorological_Center/clion/project/tools/src/ftpgetfiles.cpp > CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.i

CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/coderove/workspace/code/Meteorological_Center/clion/project/tools/src/ftpgetfiles.cpp -o CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.s

# Object files for target ftpgetfiles
ftpgetfiles_OBJECTS = \
"CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o" \
"CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o"

# External object files for target ftpgetfiles
ftpgetfiles_EXTERNAL_OBJECTS =

/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/ftpgetfiles: CMakeFiles/ftpgetfiles.dir/public/_ftp.cpp.o
/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/ftpgetfiles: CMakeFiles/ftpgetfiles.dir/tools/src/ftpgetfiles.cpp.o
/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/ftpgetfiles: CMakeFiles/ftpgetfiles.dir/build.make
/home/coderove/workspace/code/Meteorological_Center/clion/project/bin/ftpgetfiles: CMakeFiles/ftpgetfiles.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable /home/coderove/workspace/code/Meteorological_Center/clion/project/bin/ftpgetfiles"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ftpgetfiles.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ftpgetfiles.dir/build: /home/coderove/workspace/code/Meteorological_Center/clion/project/bin/ftpgetfiles
.PHONY : CMakeFiles/ftpgetfiles.dir/build

CMakeFiles/ftpgetfiles.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ftpgetfiles.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ftpgetfiles.dir/clean

CMakeFiles/ftpgetfiles.dir/depend:
	cd /home/coderove/workspace/code/Meteorological_Center/clion/project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/coderove/workspace/code/Meteorological_Center/clion/project /home/coderove/workspace/code/Meteorological_Center/clion/project /home/coderove/workspace/code/Meteorological_Center/clion/project/build /home/coderove/workspace/code/Meteorological_Center/clion/project/build /home/coderove/workspace/code/Meteorological_Center/clion/project/build/CMakeFiles/ftpgetfiles.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ftpgetfiles.dir/depend

