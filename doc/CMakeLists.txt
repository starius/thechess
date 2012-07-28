FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/Doxyfile
        ${PROJECT_BINARY_DIR}/Doxyfile)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc/html/index.html
        COMMAND ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${PROJECT_BINARY_DIR}/Doxyfile ${thechess_headers})
    add_custom_target(doc ALL DEPENDS ${PROJECT_BINARY_DIR}/doc/html/index.html)
endif(DOXYGEN_FOUND)

