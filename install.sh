
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

