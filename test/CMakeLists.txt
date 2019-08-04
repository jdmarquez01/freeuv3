if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/catch.hpp")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/catchorg/Catch2/raw/v2.9.1/single_include/catch2/"
    )
  file(
    DOWNLOAD
    "https://github.com/catchorg/Catch2/raw/v2.9.1/single_include/catch2/catch.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/catch.hpp")

  file(
    DOWNLOAD
    "https://github.com/catchorg/Catch2/raw/v2.9.1/contrib/ParseAndAddCatchTests.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ParseAndAddCatchTests.cmake")

endif()

include(${CMAKE_CURRENT_BINARY_DIR}/ParseAndAddCatchTests.cmake)

add_executable(testuv3 main.cpp)
target_link_libraries(testuv3 PRIVATE uv3decoder)
target_include_directories(testuv3 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(CODE_COVERAGE)
    target_code_coverage(testuv3)
endif()

parseandaddcatchtests(testuv3)
