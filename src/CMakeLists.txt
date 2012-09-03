
set(WT_CONNECTOR wthttp CACHE STRING "Wt connector used")
option(CAT_CPP "Cat .cpp before compilation (Unix only)" ON)

set(libs
    wtclasses
    wtdbosqlite3 wtdbopostgres wtdbo
    wt
    boost_filesystem
    boost_signals boost_regex boost_system boost_thread
    crypt pthread)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(CAT_CPP)
    file(GLOB all_1 "${CMAKE_CURRENT_SOURCE_DIR}/*/")
    file(GLOB all_2 "${CMAKE_CURRENT_SOURCE_DIR}/*/*/")
    set(all_cpps "")
    foreach(file_path ${CMAKE_CURRENT_SOURCE_DIR}/.;${all_1};${all_2})
        if (IS_DIRECTORY ${file_path})
            file(GLOB sources "${file_path}/*.cpp")
            string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} "" dir ${file_path})
            string(REPLACE "/" "_" dir_short ${dir})
            set(all_cpp ${CMAKE_CURRENT_BINARY_DIR}/${dir_short}_all.cpp)
            add_custom_command(
                OUTPUT ${all_cpp}
                COMMAND cat ${file_path}/*.cpp > ${all_cpp}
                DEPENDS ${sources}
            )
            list(APPEND all_cpps ${all_cpp})
        endif()
    endforeach()
    add_executable(thechess.wt ${all_cpps})
else()
    add_executable(thechess.wt ${thechess_sources})
endif()

target_link_libraries(thechess.wt ${WT_CONNECTOR} ${libs})

add_test(NAME self-test
    COMMAND thechess.wt --run-tests
)

