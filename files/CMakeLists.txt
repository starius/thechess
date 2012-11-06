execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/tinymce
    ${CMAKE_CURRENT_BINARY_DIR}/resources/tiny_mce
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/js/tiny_mce_src.js
    ${CMAKE_CURRENT_BINARY_DIR}/resources/tiny_mce/tiny_mce.js
)

execute_process(COMMAND ${CMAKE_COMMAND} -E tar x
    ${CMAKE_CURRENT_SOURCE_DIR}/js/tinymce_language_pack.tar.gz
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources/tiny_mce/
)

subdirs(css)
subdirs(js)
subdirs(sound)
subdirs(html)
subdirs(img)

