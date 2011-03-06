
mkdir -p build
cd build
if [ ! -d ./locales ]; then ln -s ../locales ./locales; fi
if [ ! -d ./files ]; then ln -s ../files ./files; fi

qmake ../src/thechess-wt.pro && make VERBOSE=0 -j 3


