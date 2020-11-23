cmake_minimum_required(VERSION 2.8)

project(asiotest)

# config c standard
if (NOT WINDOWS)
    if(NOT DEFINED C_STD)
        set(C_STD 11)
    endif()
    message(STATUS "C_STD=${C_STD}")
    set(CMAKE_C_STANDARD ${C_STD})
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# config c++ standard
if(NOT DEFINED CXX_STD)
    set(CXX_STD 14)
endif()
message(STATUS "CXX_STD=${CXX_STD}")
set(CMAKE_CXX_STANDARD ${CXX_STD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FILE(GLOB TEST_SRC  packages/kcp/*.c;packages/yasio/yasio/*.cpp;src/transports/*.cpp;src/utils/*.cpp;test/test_transp.cpp)

add_executable(asiotest ${TEST_SRC})

target_compile_definitions(asiotest PUBLIC YASIO_HAVE_KCP=1)

target_include_directories(asiotest 
    PUBLIC "packages/yasio"
    PUBLIC "packages/kcp"
    PUBLIC "packages/json/include"
    PUBLIC "include"
    PUBLIC "packages"
)