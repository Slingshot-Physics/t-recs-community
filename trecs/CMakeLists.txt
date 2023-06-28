set(
    headers
    include/allocator.hpp
    include/archetype.hpp
    include/byte_pool.hpp
    include/component_array_wrapper.hpp
    include/component_manager.hpp
    include/ecs_types.hpp
    include/entity_component_buffer.hpp
    include/entity_manager.hpp
    include/ext_uid_byte_pool.hpp
    include/ext_uid_object_pool.hpp
    include/pool_allocator_interface.hpp
    include/query_manager.hpp
    include/signature_manager.hpp
    include/system_manager.hpp
    include/system.hpp
)

set(
    sources
    src/allocator.cpp
    src/component_manager.cpp
    src/entity_manager.cpp
    src/system_manager.cpp
)

set(
    target
    trecs
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library(
   ${target}
   SHARED
   ${sources}
   ${headers}
)

target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-function)

target_include_directories(
   ${target}
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
)
