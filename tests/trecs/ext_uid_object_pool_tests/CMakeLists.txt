set(
   target
   ext_uid_object_pool_tests
)

set(
   lib_links
   trecs
)

set(
   config_files
)

build_trecs_test( ${target} "${target}.cpp" "" "${lib_links}" "${config_files}")
