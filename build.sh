g++ -fPIC -std=c++17 -O0 -Wall -fexceptions -g $PWD/packages/github/kcp/*.c $PWD/packages/github/yasio/yasio/*.cpp \
$PWD/src/transports/*.cpp $PWD/src/utils/*.cpp $PWD/test/test_transp.cpp -o $PWD/test/test -I $PWD/packages/github \
-I $PWD/packages/github/yasio -I $PWD/packages/github/kcp -I $PWD/packages/github/json/include -I $PWD/include -lpthread -levent -lstdc++fs -lrt