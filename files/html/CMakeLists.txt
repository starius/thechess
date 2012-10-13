file(GLOB html_pages "${CMAKE_CURRENT_SOURCE_DIR}/*.html")

foreach(html_page ${html_pages})
    get_filename_component(html_page_name ${html_page} NAME)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${html_page_name}
        ${CMAKE_CURRENT_BINARY_DIR}/${html_page_name})
endforeach()

