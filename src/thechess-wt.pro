CONFIG -= qt

TARGET = thechess.wt

SOURCES = $$system("find -name '*.cpp'")
HEADERS = $$system("find -name '*.hpp'")

CONFIG(release) {
    QMAKE_LFLAGS_RELEASE += -flto
    QMAKE_CXXFLAGS_RELEASE += -flto
    QMAKE_POST_LINK += upx -9 thechess.wt
    SOURCES -= tests.cpp
}
CONFIG(debug) {
    DEFINES += RUN_TESTS
    QMAKE_CXXFLAGS_DEBUG += -fdump-class-hierarchy
    QMAKE_CXXFLAGS_DEBUG += -O0
    QMAKE_LFLAGS_DEBUG += -O0
}

QMAKE_CXX = ccache g++

LIBS += -lcrypt
LIBS += -lboost_signals -lboost_regex
LIBS += -lwt -lwthttp
LIBS += -lwtdbo -lwtdbosqlite3 -lwtdbopostgres

