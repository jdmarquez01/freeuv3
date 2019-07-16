configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
  ${CMAKE_BINARY_DIR}/Doxyfile
  @ONLY
)
 set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)
