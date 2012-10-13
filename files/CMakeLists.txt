execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/tinymce
    ${CMAKE_CURRENT_BINARY_DIR}/resources/tiny_mce
)

subdirs(css)
subdirs(js)
subdirs(sound)
subdirs(html)
subdirs(img)

