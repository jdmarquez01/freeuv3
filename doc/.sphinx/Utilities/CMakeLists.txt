configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/CreateNewExample.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/CreateNewExample.py
  @ONLY
  )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/CodeSwarm.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/CodeSwarm.sh
  @ONLY
)

if( BUILD_DOCUMENTATION )
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/SphinxExtensions/breathe/documentation/source/_static/breathe.css
    DESTINATION ${CMAKE_BINARY_DIR}/html/_static
    )
endif()
