
CFLAGS= -Iinclude -I/project/3ty/jwt-cpp/include -I/opt/zoo-project/zoo-project/zoo-kernel -L/var/lib64   `cgicc-config --cxxflags`   -I`cgicc-config --includedir` -L`cgicc-config  --libdir` -I/usr/include/
all:
	#g++ -std=c++17 argolib.cpp  -shared -fpic -obuild/libargo_interface.so
	g++ -std=c++17 -Wunused-function -DSERVICENAME=${COMPILE} template.cpp ${CFLAGS} -lcgicc -ljson-c -shared -fpic  -lpthread -ldl -lzoo_service -o${USERPATH}${SERVICENAMEFILE}.zo

interface:
	g++ -std=c++17 -I/usr/include/json/  interface.cpp ${CFLAGS} -lcgicc -ljson-c -shared -fpic  -lpthread -ldl -lzoo_service -olibinterface.so

argo:
	mkdir -p  build
	#g++ -std=c++17 argolib.cpp  -shared -fpic -obuild/libargo_interface.so
	g++ -std=c++17 argo.cpp ${CFLAGS} -lcgicc -ljson-c -shared -fpic  -lpthread -ldl -lzoo_service -obuild/argo.zo

argoinstall:


clean:
	rm -fvR build/*
package:

build:

install:

installinterface:
	cp libinterface.so /opt/t2libs/libinterface.so

local:
	cp -f * /opt/t2template/

