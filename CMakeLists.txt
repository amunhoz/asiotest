cmake_minimum_required(VERSION 2.8)

project(asiotest)

FILE(GLOB TEST_SRC  packages/kcp/*.c;packages/yasio/yasio/*.cpp;src/transports/*.cpp;src/utils/*.cpp;test/test_transp.cpp)

add_executable(asiotest ${TEST_SRC})

target_include_directories(asiotest 
    PUBLIC "packages/yasio"
    PUBLIC "packages/kcp"
    PUBLIC "packages/json/include"
    PUBLIC "include"
    PUBLIC "packages"
)