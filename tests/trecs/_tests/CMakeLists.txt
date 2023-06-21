
set( source variadic_test.cpp)

set( header )

set( target variadic_test )

if( EXISTS ${target}.hpp)
   set( header ${target}.hpp)
endif()

add_executable(
   ${target}
   ${source}
   ${header}
)
