set(
   target
   signature_manager_tests
)

set(
   lib_links
   trecs
   test_utils
)

set(
   config_files
)

build_trecs_test( ${target} "${target}.cpp" "" "${lib_links}" "${config_files}")
