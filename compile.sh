# thechess, chess game web application written in C++ and based on Wt
# Copyright (C) 2010 Boris Nagaev
# 
# thechess is licensed under the GNU GPL Version 2.
# Other versions of the GPL do not apply.
# See the LICENSE file for terms of use.

mode=$1
if [ ! $mode ]; then mode='debug'; fi

mkdir -p build
cd build
if [ ! -d ./locales ]; then ln -s ../locales ./locales; fi
if [ ! -d ./files ]; then ln -s ../files ./files; fi

qmake "CONFIG-=release" "CONFIG-=debug" "CONFIG+=$mode" ../src/thechess-wt.pro && make -j 3


