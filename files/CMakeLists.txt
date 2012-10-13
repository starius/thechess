execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/tinymce
    ${CMAKE_CURRENT_BINARY_DIR}/resources/tiny_mce
)

file(DOWNLOAD
    https://raw.github.com/starius/wt-classes/gh-pages/jquery.countdown.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/js/jquery.countdown.min.js
)

file(DOWNLOAD
    https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/js/jquery.min.js
)

