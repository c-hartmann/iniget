cmake_minimum_required(VERSION 3.10)

project(
	iniget
	VERSION 0.0.3
  LANGUAGES CXX
  DESCRIPTION "C++ based CLI program to get and set values from INI-style files"
  HOMEPAGE_URL https://gitlab.com/c-hartmann/iniget/
 )

set(SOURCE_FILES iniget.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
