cmd_Release/obj.target/libtransport/src/transports/base.o := g++ -o Release/obj.target/libtransport/src/transports/base.o ../src/transports/base.cpp '-DNODE_GYP_MODULE_NAME=libtransport' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-DV8_DEPRECATION_WARNINGS' '-DV8_IMMINENT_DEPRECATION_WARNINGS' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-D__STDC_FORMAT_MACROS' '-DOPENSSL_NO_PINSHARED' '-DOPENSSL_THREADS' '-DNAPI_DISABLE_CPP_EXCEPTIONS' '-DBUILDING_NODE_EXTENSION' -I/home/ivuser/.cache/node-gyp/14.15.1/include/node -I/home/ivuser/.cache/node-gyp/14.15.1/src -I/home/ivuser/.cache/node-gyp/14.15.1/deps/openssl/config -I/home/ivuser/.cache/node-gyp/14.15.1/deps/openssl/openssl/include -I/home/ivuser/.cache/node-gyp/14.15.1/deps/uv/include -I/home/ivuser/.cache/node-gyp/14.15.1/deps/zlib -I/home/ivuser/.cache/node-gyp/14.15.1/deps/v8/include -I/home/ivuser/git/_test/asiotest2/node_modules/node-addon-api -I../include -I../src_gyp -I../packages -I../packages/yasio -I../packages/kcp -I../packages/json/include  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -m64 -O3 -fno-omit-frame-pointer -std=gnu++1y -std=c++17 -O0 -Wall -fexceptions -march=native -fPIC -MMD -MF ./Release/.deps/Release/obj.target/libtransport/src/transports/base.o.d.raw   -c
Release/obj.target/libtransport/src/transports/base.o: \
 ../src/transports/base.cpp ../include/transports/base.h \
 ../include/types.h ../packages/json/include/nlohmann/json.hpp \
 ../packages/json/include/nlohmann/adl_serializer.hpp \
 ../packages/json/include/nlohmann/detail/conversions/from_json.hpp \
 ../packages/json/include/nlohmann/detail/exceptions.hpp \
 ../packages/json/include/nlohmann/detail/input/position_t.hpp \
 ../packages/json/include/nlohmann/detail/macro_scope.hpp \
 ../packages/json/include/nlohmann/thirdparty/hedley/hedley.hpp \
 ../packages/json/include/nlohmann/detail/meta/cpp_future.hpp \
 ../packages/json/include/nlohmann/detail/meta/type_traits.hpp \
 ../packages/json/include/nlohmann/detail/iterators/iterator_traits.hpp \
 ../packages/json/include/nlohmann/detail/meta/void_t.hpp \
 ../packages/json/include/nlohmann/detail/meta/detected.hpp \
 ../packages/json/include/nlohmann/json_fwd.hpp \
 ../packages/json/include/nlohmann/detail/value_t.hpp \
 ../packages/json/include/nlohmann/detail/conversions/to_json.hpp \
 ../packages/json/include/nlohmann/detail/iterators/iteration_proxy.hpp \
 ../packages/json/include/nlohmann/byte_container_with_subtype.hpp \
 ../packages/json/include/nlohmann/detail/hash.hpp \
 ../packages/json/include/nlohmann/detail/input/binary_reader.hpp \
 ../packages/json/include/nlohmann/detail/input/input_adapters.hpp \
 ../packages/json/include/nlohmann/detail/input/json_sax.hpp \
 ../packages/json/include/nlohmann/detail/input/lexer.hpp \
 ../packages/json/include/nlohmann/detail/meta/is_sax.hpp \
 ../packages/json/include/nlohmann/detail/input/parser.hpp \
 ../packages/json/include/nlohmann/detail/iterators/internal_iterator.hpp \
 ../packages/json/include/nlohmann/detail/iterators/primitive_iterator.hpp \
 ../packages/json/include/nlohmann/detail/iterators/iter_impl.hpp \
 ../packages/json/include/nlohmann/detail/iterators/json_reverse_iterator.hpp \
 ../packages/json/include/nlohmann/detail/json_pointer.hpp \
 ../packages/json/include/nlohmann/detail/json_ref.hpp \
 ../packages/json/include/nlohmann/detail/output/binary_writer.hpp \
 ../packages/json/include/nlohmann/detail/output/output_adapters.hpp \
 ../packages/json/include/nlohmann/detail/output/serializer.hpp \
 ../packages/json/include/nlohmann/detail/conversions/to_chars.hpp \
 ../packages/json/include/nlohmann/ordered_map.hpp \
 ../packages/json/include/nlohmann/detail/macro_unscope.hpp \
 ../packages/json/include/nlohmann/thirdparty/hedley/hedley_undef.hpp
../src/transports/base.cpp:
../include/transports/base.h:
../include/types.h:
../packages/json/include/nlohmann/json.hpp:
../packages/json/include/nlohmann/adl_serializer.hpp:
../packages/json/include/nlohmann/detail/conversions/from_json.hpp:
../packages/json/include/nlohmann/detail/exceptions.hpp:
../packages/json/include/nlohmann/detail/input/position_t.hpp:
../packages/json/include/nlohmann/detail/macro_scope.hpp:
../packages/json/include/nlohmann/thirdparty/hedley/hedley.hpp:
../packages/json/include/nlohmann/detail/meta/cpp_future.hpp:
../packages/json/include/nlohmann/detail/meta/type_traits.hpp:
../packages/json/include/nlohmann/detail/iterators/iterator_traits.hpp:
../packages/json/include/nlohmann/detail/meta/void_t.hpp:
../packages/json/include/nlohmann/detail/meta/detected.hpp:
../packages/json/include/nlohmann/json_fwd.hpp:
../packages/json/include/nlohmann/detail/value_t.hpp:
../packages/json/include/nlohmann/detail/conversions/to_json.hpp:
../packages/json/include/nlohmann/detail/iterators/iteration_proxy.hpp:
../packages/json/include/nlohmann/byte_container_with_subtype.hpp:
../packages/json/include/nlohmann/detail/hash.hpp:
../packages/json/include/nlohmann/detail/input/binary_reader.hpp:
../packages/json/include/nlohmann/detail/input/input_adapters.hpp:
../packages/json/include/nlohmann/detail/input/json_sax.hpp:
../packages/json/include/nlohmann/detail/input/lexer.hpp:
../packages/json/include/nlohmann/detail/meta/is_sax.hpp:
../packages/json/include/nlohmann/detail/input/parser.hpp:
../packages/json/include/nlohmann/detail/iterators/internal_iterator.hpp:
../packages/json/include/nlohmann/detail/iterators/primitive_iterator.hpp:
../packages/json/include/nlohmann/detail/iterators/iter_impl.hpp:
../packages/json/include/nlohmann/detail/iterators/json_reverse_iterator.hpp:
../packages/json/include/nlohmann/detail/json_pointer.hpp:
../packages/json/include/nlohmann/detail/json_ref.hpp:
../packages/json/include/nlohmann/detail/output/binary_writer.hpp:
../packages/json/include/nlohmann/detail/output/output_adapters.hpp:
../packages/json/include/nlohmann/detail/output/serializer.hpp:
../packages/json/include/nlohmann/detail/conversions/to_chars.hpp:
../packages/json/include/nlohmann/ordered_map.hpp:
../packages/json/include/nlohmann/detail/macro_unscope.hpp:
../packages/json/include/nlohmann/thirdparty/hedley/hedley_undef.hpp:
