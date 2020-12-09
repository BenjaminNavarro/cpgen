set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink 
        ${CMAKE_CURRENT_SOURCE_DIR}/build/compile_commands.json 
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
)