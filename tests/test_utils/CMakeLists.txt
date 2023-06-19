set(
   target
   test_utils
)

set(
   sources
)

set(
   headers
   blank_systems.hpp
   complicated_types.hpp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library(
   ${target}
   SHARED
   ${sources}
   ${headers}
)

target_include_directories(
   ${target}
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
   ${target}
   trecs
)

set_target_properties(
   ${target}
   PROPERTIES
   # Prompt the compiler/linker about the linker language
   LINKER_LANGUAGE CXX
)

target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-function)
