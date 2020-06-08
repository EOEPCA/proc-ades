#!/usr/bin/env bash

if [ -z "$FOLDER" ]
then
	echo "FOLDER env is empty"
	exit 1
fi

echo ${FOLDER}

rm -fvR ${FOLDER}
mkdir -p ${FOLDER}

cd ${FOLDER}

yum install -y epel-release
if [ $? -ne 0 ]
then
	echo "yum install epel-release failed"
	exit 1
fi 

yum update -y
yum install -y zlib-devel libxml2 libxml2-devel bison openssl  python-devel subversion libxslt-devel libcurl-devel gdal gdal-devel proj-devel libuuid-devel openssl-devel fcgi-devel wget unzip autoconf flex
yum install -y json-c json-c-devel git

if [ $? -ne 0 ]
then
	echo "yum install dependencies failed"
	exit 1
fi 

echo "*******************"
echo "prepare libfcgi"
cp /work/assets/libfcgi-2.4.0.orig.tar.gz .
if [ $? -ne 0 ]
then
	echo "cp libfcgi-2.4.0 failed"
	exit 1
fi 

tar -zxvf libfcgi-2.4.0.orig.tar.gz 
if [ $? -ne 0 ]
then
	echo "tar libfcgi-2.4.0 failed"
	exit 1
fi 
echo "****************"

echo "***************"
echo "install wps-zoo"

git clone https://github.com/OSGeo/zoo-project.git ZOO-Project
if [ $? -ne 0 ]
then
	echo "wget zoo failed"
	exit 1
fi

cd  ${FOLDER}/ZOO-Project/thirds/cgic206
make libcgic.a
if [ $? -ne 0 ]
then
	echo "make libcgic.a failed"
	exit 1
fi

make install
if [ $? -ne 0 ]
then
	echo "make install libcgic.a  failed"
	exit 1
fi


cd ${FOLDER}/ZOO-Project/zoo-project/zoo-kernel/

autoconf
if [ $? -ne 0 ]
then
	echo "autoconf failed"
	exit 1
fi

#./configure  --with-json=/usr/ --with-fastcgi=${FOLDER}/libfcgi-2.4.0.orig/OUT/lib --with-xml2config=/usr/bin/xml2-config  --with-cgi-dir=/var/www/cgi-bin --with-etc-dir=yes --sysconfdir=/etc/zoo-project
./configure  --with-json=/usr/ --with-fastcgi=${FOLDER}/libfcgi-2.4.0.orig/OUT/lib --with-xml2config=/usr/bin/xml2-config  --with-cgi-dir=/var/www/cgi-bin --with-etc-dir=yes --sysconfdir=/zooservices
if [ $? -ne 0 ]
then
	echo "configure failed"
	exit 1
fi

make
if [ $? -ne 0 ]
then
	echo "make zoo failed"
	exit 1
fi

make install
if [ $? -ne 0 ]
then
	echo "make install zoo failed"
	exit 1
fi

ln -s  libzoo_service.so   libzoo_service.so.1.5
if [ $? -ne 0 ]
then
	echo "ln libzoo failed"
	exit 1
fi

echo "************************************************************"
echo "Build status service and demo"
HERE=$PWD
cd ${FOLDER}/ZOO-Project/zoo-project/zoo-services/utils/status
make
if [ $? -ne 0 ]
then
	echo "make status service failed"
	exit 1
fi

mkdir -p ${FOLDER}/demo
if [ $? -ne 0 ]
then
	echo "mkdir demo folder failed"
	exit 1
fi

rsync -av cgi-env/* ${FOLDER}/demo
if [ $? -ne 0 ]
then
	echo "rsync failed"
	exit 1
fi

cd ${HERE}
echo "************************************************************"


mkdir -p /etc/zoo-project/
if [ $? -ne 0 ]
then
	echo "mkdir zoo-project failed"
	exit 1
fi

cp main.cfg /etc/zoo-project/
if [ $? -ne 0 ]
then
	echo "cp main.cfg failed"
	exit 1
fi

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:.

echo "*************************************************************"
echo "RUN local test"
./zoo_loader.cgi "service=WPS&version=1.0.0&request=GetCapabilities"
if [ $? -ne 0 ]
then
	echo "TEST BUILD failed"
	exit 1
fi
echo "-------------------------------------------------------------"

