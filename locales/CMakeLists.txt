execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB thechess_locales "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")

file(DOWNLOAD https://raw.github.com/kdeforche/wt/master/src/xml/wt.xml
     ${CMAKE_CURRENT_BINARY_DIR}/wt.xml)
file(DOWNLOAD https://raw.github.com/kdeforche/wt/master/src/xml/auth.xml
     ${CMAKE_CURRENT_BINARY_DIR}/auth.xml)

add_test(NAME locales-test
    COMMAND locales-test
    --prefix=tc --sections comment common competition forum game title user
    --sources ${thechess_sources} --locales ${thechess_locales}
    --wt ${CMAKE_CURRENT_BINARY_DIR}/wt.xml ${CMAKE_CURRENT_BINARY_DIR}/auth.xml
)

