file(DOWNLOAD
    https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/jquery.min.js.inc
)
list(APPEND js_files_1 ${CMAKE_CURRENT_BINARY_DIR}/jquery.min.js.inc)

file(DOWNLOAD
    https://raw.github.com/starius/wt-classes/gh-pages/jquery.countdown-1.6.1.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/jquery.countdown.min.js.inc
)
list(APPEND js_files_1 ${CMAKE_CURRENT_BINARY_DIR}/jquery.countdown.min.js.inc)

file(GLOB js_files_2 "${CMAKE_CURRENT_SOURCE_DIR}/*.js")
list(REMOVE_ITEM js_files_2 "${CMAKE_CURRENT_SOURCE_DIR}/tiny_mce_src.js")

foreach(js_file ${js_files_2})
    list(APPEND js_files_1 ${js_file})
endforeach()

add_custom_target(all_in_one_js ALL
    COMMAND cat ${js_files_1} > ${CMAKE_CURRENT_BINARY_DIR}/all_in_one.js
    DEPENDS ${js_files_1}
)

