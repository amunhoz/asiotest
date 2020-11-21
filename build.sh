g++ -fPIC -std=c++17 -O0 -Wall -fexceptions -g $PWD/packages/kcp/*.c $PWD/packages/yasio/yasio/*.cpp \
$PWD/src/transports/*.cpp $PWD/src/utils/*.cpp $PWD/test/test_transp.cpp -o $PWD/test/test -I $PWD/packages \
-I $PWD/packages/yasio -I $PWD/packages/kcp -I $PWD/packages/json/include -I $PWD/include -lpthread -levent -lstdc++fs -lrt