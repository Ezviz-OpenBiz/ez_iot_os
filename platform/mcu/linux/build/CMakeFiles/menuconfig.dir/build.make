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
CMAKE_COMMAND = /opt/cmake-3.13.0/bin/cmake

# The command to remove a file.
RM = /opt/cmake-3.13.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xurongjun/git/ez_iot_os/platform/mcu/linux

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xurongjun/git/ez_iot_os/platform/mcu/linux/build

# Utility rule file for menuconfig.

# Include the progress variables for this target.
include CMakeFiles/menuconfig.dir/progress.make

CMakeFiles/menuconfig:
	python3 /home/xurongjun/git/ez_iot_os/tools/kconfig/genconfig.py --kconfig /home/xurongjun/git/ez_iot_os/Kconfig --menuconfig True --env EZOS_PATH=/home/xurongjun/git/ez_iot_os --env PROJECT_PATH=/home/xurongjun/git/ez_iot_os/platform/mcu/linux --output config /home/xurongjun/git/ez_iot_os/platform/mcu/linux/config/.config --output cmake /home/xurongjun/git/ez_iot_os/platform/mcu/linux/config/ezos_gconfig.cmake --output header /home/xurongjun/git/ez_iot_os/platform/mcu/linux/config/ezos_gconfig.h

menuconfig: CMakeFiles/menuconfig
menuconfig: CMakeFiles/menuconfig.dir/build.make

.PHONY : menuconfig

# Rule to build all files generated by this target.
CMakeFiles/menuconfig.dir/build: menuconfig

.PHONY : CMakeFiles/menuconfig.dir/build

CMakeFiles/menuconfig.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/menuconfig.dir/cmake_clean.cmake
.PHONY : CMakeFiles/menuconfig.dir/clean

CMakeFiles/menuconfig.dir/depend:
	cd /home/xurongjun/git/ez_iot_os/platform/mcu/linux/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xurongjun/git/ez_iot_os/platform/mcu/linux /home/xurongjun/git/ez_iot_os/platform/mcu/linux /home/xurongjun/git/ez_iot_os/platform/mcu/linux/build /home/xurongjun/git/ez_iot_os/platform/mcu/linux/build /home/xurongjun/git/ez_iot_os/platform/mcu/linux/build/CMakeFiles/menuconfig.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/menuconfig.dir/depend

