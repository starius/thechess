# thechess, chess game web application written in C++ and based on Wt
# Copyright (C) 2010 Boris Nagaev
# 
# thechess is licensed under the GNU GPL Version 2.
# Other versions of the GPL do not apply.
# See the LICENSE file for terms of use.

mode=$1
if [ ! $mode ]; then mode='release'; fi
./compile.sh $mode

mkdir -p /usr/games
mkdir -p /usr/share/thechess
mkdir -p /etc/thechess

cp build/thechess.wt thechess-run /usr/games
cp -r files/ locales/ /usr/share/thechess
cp wt_config.xml /etc/thechess
cp args-release /etc/thechess/args

