find_program(DOXYGEN doxygen)
if(NOT DOXYGEN)
  message(STATUS "Target 'doc' disabled (requires doxygen)")
  return()
endif()

add_custom_target(doc
                  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build.py '1.0.0'
                  SOURCES build.py conf.py _templates/layout.html)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
        DESTINATION share/doc/fmt
        OPTIONAL)
