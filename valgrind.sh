
cd ./build/;
valgrind --leak-check=full ./thechess.wt --docroot ../files/ --http-address 0.0.0.0 --http-port 8004


