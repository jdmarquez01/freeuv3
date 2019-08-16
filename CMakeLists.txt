# ------------------------------------------------------------------------------
# Project information
#

cmake_minimum_required(VERSION 3.12.0)

# Get Version and filename
file(READ ${CMAKE_SOURCE_DIR}/PROJECT PRJ_NAME)
string(REGEX
       REPLACE "\n"
               ""
               PRJ_NAME
               "${PRJ_NAME}") # get rid of the newline at the end

file(READ ${CMAKE_SOURCE_DIR}/VERSION PRJ_VERSION)

string(REGEX
       REPLACE "\n"
               ""
               PRJ_VERSION
               "${PRJ_VERSION}") # get rid of the newline at the end
string(REPLACE "."
               ";"
               PRJ_VERSION_STR
               ${PRJ_VERSION})
set(PRJ_VERSION_LIST ${PRJ_VERSION_STR})
separate_arguments(PRJ_VERSION_LIST)

list(LENGTH PRJ_VERSION_LIST PRJ_VERSION_SIZE)
list(GET PRJ_VERSION_LIST 0 PRJ_VERSION_MAJOR)
list(GET PRJ_VERSION_LIST 1 PRJ_VERSION_MINOR)

set(PRJ_VERSION_BUGFIX 0)
set(PRJ_VERSION_TWEAK 0)

if(PRJ_VERSION_SIZE EQUAL 3)
  list(GET PRJ_VERSION_LIST 2 PRJ_VERSION_BUGFIX)
else(PRJ_VERSION_SIZE EQUAL 3)
  set(PRJ_VERSION_BUGFIX 0)
endif(PRJ_VERSION_SIZE EQUAL 3)

# ------------------------------------------------------------------------------
# -----------
# override rules
set(CMAKE_USER_MAKE_RULES_OVERRIDE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)
set(CMAKE_INSTALL_PREFIX
    "${CMAKE_BINARY_DIR}/install"
    CACHE FILEPATH "${CMAKE_BINARY_DIR}/install")

# ------------------------------------------------------------------------------
# -----------
# Name of the Project
project(
  ${PRJ_NAME}
  VERSION
    ${PRJ_VERSION_MAJOR}.${PRJ_VERSION_MINOR}.${PRJ_VERSION_BUGFIX}.${PRJ_VERSION_TWEAK}
  )

message("-------------------------------------------------------------")
message(
  "Building Project ${PROJECT_NAME} - ${PROJECT_VERSION} for ${CMAKE_SYSTEM}")
message("-------------------------------------------------------------")
message("git SHA1 ID: ${GIT_REVISION}")

# ------------------------------------------------------------------------------
# -----------
# CMake global modules
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# ------------------------------------------------------------------------------
# -----------
# Find Git Version tweak
find_package(Git REQUIRED)
if(GIT_FOUND)
  execute_process(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                  OUTPUT_VARIABLE GIT_OUT
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT GIT_OUT)
    set(GIT_OUT 0)
  endif()
  set(GIT_REVISION "${GIT_OUT}")
endif(GIT_FOUND)

macro(trace variable)
  message("${variable} = ${${variable}}")
endmacro()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)



option(CODE_COVERAGE "Enable coverage reporting" OFF)

if(CODE_COVERAGE)
  include(code-coverage)
endif()

# code analisys
#find_package(Tidy)
# find_package(CppCheck)
#find_package(IWYU)

option(BUILD_DOC "Build documentation" OFF)

# Builds the documentation.
if(BUILD_DOC)

  find_package (Python3 COMPONENTS Interpreter)
  add_subdirectory(doc)

endif(BUILD_DOC)

macro(preserveFolder target)
  file(RELATIVE_PATH
       relativeOutDir
       ${CMAKE_SOURCE_DIR}
       "${CMAKE_CURRENT_SOURCE_DIR}/..")
  set_target_properties(${target} PROPERTIES FOLDER ${relativeOutDir})
endmacro()

# ------------------------------------------------------------------------------
# -----------
# Conan
if(false)
  if(EXISTS ${CMAKE_SOURCE_DIR}/conanfile.txt)
    # conan dependencies
    if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
      include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    else()
      if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(
          STATUS
            "Downloading conan.cmake from https://github.com/conan-io/cmake-conan"
          )
        file(DOWNLOAD
             "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
             "${CMAKE_BINARY_DIR}/conan.cmake")
      endif()
      include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)
      conan_check()
      conan_cmake_run(CONANFILE
                      conanfile.txt # or relative build/conanfile.txt
                      BASIC_SETUP
                      CMAKE_TARGETS
                      BUILD
                      missing)
      include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    endif()
    conan_basic_setup()
    trace(CONAN_LIBS)
  endif()
endif()

# ------------------------------------------------------------------------------
# -----------
# src
add_subdirectory(src)

# ------------------------------------------------------------------------------
# -----------
# testing
option(BUILD_TESTING "build test" OFF)
set (BUILD_TESTING ON)
if(BUILD_TESTING)
  configure_file("${CMAKE_SOURCE_DIR}/data/test.UVData" "${CMAKE_BINARY_DIR}/test/test.UVData" COPYONLY)
  #INCLUDE(CTest)
  enable_testing()
  add_subdirectory(test)
endif()




# ------------------------------------------------------------------------------
# -----------
# install
