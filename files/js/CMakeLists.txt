file(DOWNLOAD
    https://raw.github.com/starius/wt-classes/gh-pages/jquery.countdown-1.6.1.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/jquery.countdown.min.js
)

file(DOWNLOAD
    https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js
    ${CMAKE_CURRENT_BINARY_DIR}/jquery.min.js
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/fix_tinymce_change_lang.js
    ${CMAKE_CURRENT_BINARY_DIR}/fix_tinymce_change_lang.js
    COPYONLY
)

