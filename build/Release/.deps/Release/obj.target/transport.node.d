cmd_Release/obj.target/transport.node := g++ -o Release/obj.target/transport.node -shared -pthread -rdynamic -m64  -Wl,-soname=transport.node -Wl,--start-group Release/obj.target/libtransport/src/utils/random.o Release/obj.target/libtransport/src/utils/time.o Release/obj.target/libtransport/src/transports/base.o Release/obj.target/libtransport/src/transports/kcp.o Release/obj.target/libtransport/src_gyp/nodetransport.o Release/obj.target/libtransport/src_gyp/exportclass.o Release/obj.target/libtransport/packages/kcp/ikcp.o Release/obj.target/libtransport/packages/yasio/yasio/ibstream.o Release/obj.target/libtransport/packages/yasio/yasio/obstream.o Release/obj.target/libtransport/packages/yasio/yasio/xxsocket.o Release/obj.target/libtransport/packages/yasio/yasio/yasio.o -Wl,--end-group -lpthread -levent -lstdc++fs