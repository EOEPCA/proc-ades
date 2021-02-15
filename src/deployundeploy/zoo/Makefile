CFLAGS= -I../includes -I/project/3ty/proc-comm-lib-ows-1.04/includes/ -I/usr/include/libxml2 -Iinclude  -I/opt/zoo-project/zoo-project/zoo-kernel -L/var/lib64   `cgicc-config --cxxflags`   -I`cgicc-config --includedir` -L`cgicc-config  --libdir` -I/usr/include/
all:
	mkdir -p build
	g++ -std=c++17 ${CFLAGS} -fpic -c sha1.cpp -o build/sha1.o
	g++ -std=c++17 ${CFLAGS} -fpic -c zooconverter.cpp -o build/zooconverter.o
	g++ -std=c++17 ${CFLAGS} -fpic -c xmlmemorywritewrapper.cpp -o build/xmlmemorywritewrapper.o
	g++ -std=c++17 ${CFLAGS}  -fpic -c deploy.cpp -o build/deploy.o
	g++ -std=c++17 build/*.o ${CFLAGS}  -shared  -lpthread -ldl -lzoo_service -obuild/libepcatransactional.zo

install:
	cp build/libepcatransactional.zo /project/zooservices/
	cp *.zcfg /project/zooservices/

local:
	cp *.zcfg /zooservices/
	cp build/*.zo /zooservices/
	cp build/libepcatransactional.zo /opt/t2service/libepcatransactional.zo




