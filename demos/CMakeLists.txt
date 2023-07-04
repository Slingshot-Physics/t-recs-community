
set(
   exec_folders
   ecb_system
   multi_component_multi_system
   multi_component_system
)

foreach( exec_folder ${exec_folders} )
   set( source "${exec_folder}/main.cpp")
   set( target ${exec_folder} )

   add_executable(
      ${target}
      ${source}
   )

   target_link_libraries(
      ${target}
      trecs
   )
endforeach()
