file(GLOB_RECURSE lib_SRCS *.cpp)
add_library(pmr SHARED ${lib_SRCS})

set_target_properties(pmr PROPERTIES CXX_STANDARD 11)
add_compile_options(-Wall -Wpedantic -Wextra -Werror)
