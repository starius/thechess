
set(WT_CONNECTOR wthttp CACHE STRING "Wt connector used")

set(libs
    wtclasses
    wtdbosqlite3 wtdbopostgres wtdbo
    wt
    boost_filesystem
    boost_signals boost_regex boost_system boost_thread
    crypt pthread)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(thechess.wt ${thechess_sources})
target_link_libraries(thechess.wt ${WT_CONNECTOR} ${libs})

add_test(NAME self-test
    COMMAND thechess.wt --run-tests
)

