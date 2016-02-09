cmake_minimum_required (VERSION 3.2.2)

set(PMR_VERSION_MAJOR 0)
set(PMR_VERSION_MINOR 1)
set(PMR_VERSION_PATCH 1)
set(PMR_VERSION ${PMR_VERSION_MAJOR}.${PMR_VERSION_MINOR}.${PMR_VERSION_PATCH})

project(pmr VERSION ${PMR_VERSION} LANGUAGES CXX)


file(GLOB_RECURSE pmr_srcs src/*.cpp)
add_library(pmr SHARED ${pmr_srcs})
target_include_directories(pmr PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)
set_target_properties(pmr PROPERTIES
    CXX_STANDARD 11
    VERSION ${PMR_VERSION}
    SOVERSION ${PMR_VERSION_MAJOR})
add_compile_options(-Wall -Wpedantic -Wextra -Werror)


include(CTest)
add_subdirectory(test)


include(FindDoxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(
        docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating doxygen project documentation in ${CMAKE_CURRENT_BINARY_DIR}." VERBATIM
    )
else()
    ADD_CUSTOM_TARGET(docs COMMENT "Doxygen not found. Please install doxygen to use this target." VERBATIM)
endif()
