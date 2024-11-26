# ********************************************************************************
# 
# CMake file used to build the project.
# 
# Created on Wed Nov 20 2024
# 
# Daniel Figueira <daniel.castro.figueira@gmail.com>
# 
# ********************************************************************************

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

# Compiler options
target_compile_options(app PRIVATE -Wall -Werror)

# Header files
zephyr_library_include_directories(include)

# Source files
file(GLOB_RECURSE APP_SRC "src/*.c")
target_sources(app PRIVATE ${APP_SRC} main.c)