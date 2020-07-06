CFLAGS= -L/var/lib64   `cgicc-config --cxxflags`   -I`cgicc-config --includedir` -L`cgicc-config  --libdir` -I/usr/include/
all:
	g++ -std=c++14  IniReader.h watchjob.cpp ${CFAGS} -lcgicc -ljson-c  -owatchjob
clean:

package:


install:
	cp watchjob /var/www/t2dep/

