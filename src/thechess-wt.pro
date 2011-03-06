CONFIG -= release
CONFIG -= debug
TARGET = thechess.wt

SOURCES += chess/hex.cpp
SOURCES += chess/board.cpp
SOURCES += chess/field.cpp
SOURCES += chess/xy.cpp
SOURCES += chess/move.cpp
SOURCES += chess/moves.cpp
SOURCES += chess/cached_moves.cpp

SOURCES += model/td.cpp
SOURCES += model/EloPlayer.cpp
SOURCES += model/CookieSession.cpp
SOURCES += model/User.cpp
SOURCES += model/Game.cpp

SOURCES += widgets/MySpinBox.cpp
SOURCES += widgets/TimeDeltaWidget.cpp
SOURCES += widgets/GameCountdown.cpp
SOURCES += widgets/BoardWidget.cpp
SOURCES += widgets/MovesWidget.cpp
SOURCES += widgets/RegisterWidget.cpp
SOURCES += widgets/LoginWidget.cpp
SOURCES += widgets/PleaseLoginWidget.cpp
SOURCES += widgets/GameParametersWidget.cpp
SOURCES += widgets/GameWidget.cpp
SOURCES += widgets/GameCreateWidget.cpp
SOURCES += widgets/GameListWidget.cpp
SOURCES += widgets/UserWidget.cpp
SOURCES += widgets/RatingChanges.cpp
SOURCES += widgets/MainMenu.cpp

SOURCES += rand.cpp

SOURCES += time_intervals.cpp
SOURCES += TaskTracker.cpp
SOURCES += ThechessSession.cpp
SOURCES += ThechessApplication.cpp

SOURCES += thechess.cpp


#DEFINES += THECHESS_SQLITE
#DEFINES += THECHESS_MYSQL
DEFINES += THECHESS_POSTGRES

CONFIG += debug
#CONFIG += precompile_header
CONFIG += console
CONFIG -= qt

contains( CONFIG, release ) {
    QMAKE_LFLAGS_RELEASE += -flto
    QMAKE_CXXFLAGS_RELEASE += -flto
    QMAKE_POST_LINK += upx -9 thechess.wt
}
contains( CONFIG, debug ) {
    DEFINES += RUN_TESTS
    QMAKE_CXXFLAGS_DEBUG += -fdump-class-hierarchy
    SOURCES += tests.cpp
}

INCLUDEPATH = /usr/local/include/Wt/

QMAKE_CXX = ccache g++

LIBS += -lboost_signals -lboost_regex
LIBS += -lwt -lwthttp
LIBS += -lwtdbo -lwtdbosqlite3 -lwtdbopostgres # -lwtdbomysql
