
hunter_config(RapidJSON VERSION "1.1.0-66eb606-p0")

hunter_config(Snappy 
URL https://github.com/google/snappy/archive/1.1.8.tar.gz
SHA1 1eeb144e7b564dd726136c97f869b453d2fb7924
CMAKE_ARGS
#force feed snappy with parent c/c++ runtime+compiler needed by leveldb
CMAKE_C_COMPILER=${CMAKE_C_COMPILER}
CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
CMAKE_CXX_FLAGS=-stdlib=libc++)


hunter_config(leveldb VERSION "1.22"
CMAKE_ARGS
#force feed leveldb with parent c/c++ runtime+compiler
CMAKE_C_COMPILER=${CMAKE_C_COMPILER}
CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
CMAKE_CXX_FLAGS=-stdlib=libc++
_ITERATOR_DEBUG_LEVEL=0)

hunter_config(date VERSION 2.4.1)
hunter_config(OpenSSL VERSION "1.1.1d"
CMAKE_ARGS
EXTRA_FLAGS=--with-sandbox=rlimit)


#hunter_config(mbedtls 
#URL https://github.com/ARMmbed/mbedtls/archive/abc460236f17be148036e2c7e07e6a05f938b656.tar.gz
#SHA1 a0a5dc51e3737e56efe0910b5bae6e759c1109a7)

#hunter_config(mbedtls 
#URL https://github.com/edwardbr/mbedtls/archive/Branch_abc46023.tar.gz
#SHA1 a0a5dc51e3737e56efe0910b5bae6e759c1109a7)

#hunter_config(nng 
#URL https://github.com/nanomsg/nng/archive/master.tar.gz
#SHA1 0377e7a318fd090d9ad23c719ecbeb7da5a77391
#CMAKE_ARGS
#NNG_ENABLE_TLS=ON)


#hunter_config(nng 
#URL https://github.com/nanomsg/nng/archive/f09864b1343e4ce4b8fd7d7df0ed63061a39ef6c.tar.gz
#SHA1 3165aa27a7ea7723cc25654e7f1b6aeab0251636
#CMAKE_ARGS
#NNG_ENABLE_TLS=ON)

hunter_config(fmt 
URL https://github.com/fmtlib/fmt/archive/6.1.2.tar.gz
SHA1 dc59b27d461f1af12daf73f3e00d18bf1e9eed78)

hunter_config(GTest VERSION "1.10.0-p1"
CMAKE_ARGS
#force feed gtest with parent c/c++ runtime+compiler
CMAKE_C_COMPILER=${CMAKE_C_COMPILER}
CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
CMAKE_CXX_FLAGS=-stdlib=libc++
)

hunter_config(flatbuffers
URL https://github.com/google/flatbuffers/archive/v1.12.0.tar.gz
SHA1 8c047d1d843a29072702ee09ec7ecbce00636433
    CMAKE_ARGS
    FLATBUFFERS_BUILD_FLATC=ON
)