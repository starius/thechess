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
SOURCES += model/UserClassification.cpp
SOURCES += model/GameParameters.cpp
SOURCES += model/Game.cpp
SOURCES += model/Competition.cpp

SOURCES += widgets/MySpinBox.cpp
SOURCES += widgets/TimeDeltaWidget.cpp
SOURCES += widgets/ClassificationWidget.cpp
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
SOURCES += utils.cpp

SOURCES += time_intervals.cpp
SOURCES += TaskTracker.cpp
SOURCES += ThechessSession.cpp
SOURCES += ThechessApplication.cpp
SOURCES += ThechessOptions.cpp

SOURCES += thechess.cpp

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
    QMAKE_CXXFLAGS_DEBUG += -O0
    QMAKE_LFLAGS_DEBUG += -O0
    SOURCES += tests.cpp
}

INCLUDEPATH = /usr/local/include/Wt/

QMAKE_CXX = ccache g++

LIBS += -lboost_signals -lboost_regex
LIBS += -lwt -lwthttp
LIBS += -lwtdbo -lwtdbosqlite3 -lwtdbopostgres # -lwtdbomysql

