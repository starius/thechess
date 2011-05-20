
mode=$1
if [ ! $mode ]; then mode='debug'; fi

mkdir -p build
cd build
if [ ! -d ./locales ]; then ln -s ../locales ./locales; fi
if [ ! -d ./files ]; then ln -s ../files ./files; fi

qmake "CONFIG-=release" "CONFIG-=debug" "CONFIG+=$mode" ../src/thechess-wt.pro && make -j 3


