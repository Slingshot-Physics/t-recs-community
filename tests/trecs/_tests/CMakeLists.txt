set(
   test_names
   variadic_test
   assignment_overload_test
)

foreach( test_name ${test_names} )

   set( source ${test_name}.cpp)

   set( header )

   set( target ${test_name} )

   if( EXISTS ${target}.hpp)
      set( header ${target}.hpp)
   endif()

   add_executable(
      ${target}
      ${source}
      ${header}
   )
endforeach()
