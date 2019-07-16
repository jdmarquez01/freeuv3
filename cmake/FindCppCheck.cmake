find_program(CPPCHECK_EXE NAMES cppcheck)
if (NOT CPPCHECK_EXE)
  message(STATUS "cppcheck not found.")
else()
message(STATUS "cppcheck found: ${CPPCHECK_EXE}")
set (CMAKE_CXX_CPPCHECK ${CPPCHECK_EXE}
            "--enable=warning"
            "--inconclusive"
            "--force" 
            "--inline-suppr"
			
            
    )
	#"--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
endif()
