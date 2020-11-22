cmake_minimum_required(VERSION 3.0)
# Note that C language specifier required due to CEF
project(
  nar-server
  VERSION 0.1.0
  LANGUAGES CXX)
include(cmake/StandardProjectSettings.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(WITH_DOCS "Compile documentation" OFF)
if (WITH_DOCS)
	include(cmake/Doxygen.cmake)
	enable_doxygen()
endif()

option(ENABLE_TESTING "Enable Test Builds" OFF)
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

add_subdirectory(src)
