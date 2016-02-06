cmake_minimum_required (VERSION 3.2.2)
project(pmr)

include(CTest)
include(FindDoxygen)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(src)
add_subdirectory(test)


if(DOXYGEN_FOUND)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(
            docs
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating doxygen project documentation in ${CMAKE_CURRENT_BINARY_DIR}." VERBATIM
        )
    else()
        add_custom_target(docs COMMENT "Doxyfile.in not found. Please generate a doxygen configuration file to use this target." VERBATIM)
    endif()
else()
    ADD_CUSTOM_TARGET(docs COMMENT "Doxygen not found. Please install doxygen to use this target." VERBATIM)
endif()
