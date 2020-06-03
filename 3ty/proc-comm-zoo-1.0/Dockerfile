FROM centos/devtoolset-7-toolchain-centos7
USER root
#RUN ln -s /opt/rh/devtoolset-7/enable /etc/profile.d/rhgccenable.sh && chmod +x /etc/profile.d/rhgccenable.sh

RUN yum install -y epel-release
RUN yum update -y
RUN yum install -y json-c json-c-devel zlib-devel libxml2 libxml2-devel bison openssl  python-devel subversion libxslt-devel libcurl-devel gdal gdal-devel proj-devel libuuid-devel openssl-devel fcgi-devel wget unzip autoconf flex cmake3

COPY assets/libfcgi-2.4.0.orig.tar.gz /opt/libfcgi-2.4.0.orig.tar.gz
RUN cd /opt/ && gzip -d libfcgi-2.4.0.orig.tar.gz && tar -xvf /opt/libfcgi-2.4.0.orig.tar

RUN yum install -y git

RUN yum install -y automake && \
	cd && \
	wget http://ftp.gnu.org/gnu/cgicc/cgicc-3.2.19.tar.gz && \
	tar -zxvf cgicc-3.2.19.tar.gz && \
	cd cgicc-3.2.19 && ./autogen && ./configure && make && make install && \
	cd && rm -fvR cgicc*
ENV PATH "$PATH:/usr/local/bin"

COPY assets/zoo-project.tar.gz /opt/zoo-project.tar.gz
RUN cd /opt/ && tar -zxvf zoo-project.tar.gz && rm -f zoo-project.tar.gz

RUN cd /opt/zoo-project/thirds/cgic206 && make libcgic.a && make install

RUN cd /opt/zoo-project/zoo-project/zoo-kernel/ && autoconf && \
./configure  --with-json=/usr/ --with-fastcgi=${FOLDER}/libfcgi-2.4.0.orig/OUT/lib --with-xml2config=/usr/bin/xml2-config  --with-cgi-dir=/var/www/zoo-bin --with-etc-dir=yes --sysconfdir=/zooservices

RUN cd /opt/zoo-project/zoo-project/zoo-kernel/ && ls -ltr && make && make install

##install zoo
RUN yum install -y vim httpd \
	&& mkdir -p /opt/watchjob /var/www/zoo-bin/ /var/www/data/ /etc/zoo-project/ /var/www/html/res/statusInfos \
	&& mkdir -p /zooservices &&  chmod 777 /zooservices \
	&& mkdir -p /var/www/fcgi/ /var/www/html/zoo  /var/www/html/wps3 /var/www/t2dep /var/www/html/watchjob \
	&& chown -R 48:48 /var/www/zoo-bin/ /var/www/zoo-bin/ /var/www/data/ /var/www/html/res  \
	&& echo '/usr/local/lib' > /etc/ld.so.conf.d/zoo.conf && ldconfig \
	&& mkdir -p /opt/t2build/includes  /opt/t2service/ /opt/opt/t2service/t2scripts/ \
	&& mkdir -p /var/www/zoo-bin/ /var/www/html/zoo/

COPY assets/zoo/httpd/htaccess_wps3 /var/www/html/wps3/.htaccess
COPY assets/zoo/httpd/htaccess_watchjob /var/www/html/watchjob/.htaccess
COPY assets/zoo/httpd/htaccess /var/www/html/zoo/.htaccess
COPY assets/zoo/httpd/zoo.conf	/etc/httpd/conf.d/zoo.conf
COPY assets/main.cfg /etc/zoo-project/main.cfg
COPY assets/oas.cfg /etc/zoo-project/oas.cfg
COPY assets/main.cfg /opt/t2service/main.cfg
COPY assets/oas.cfg /opt/t2service/oas.cfg
COPY src/* /opt/watchjob/

RUN cd /opt/zoo-project/zoo-project/zoo-services/utils/status && make && cd /opt/zoo-project/zoo-project/zoo-services/utils/status/cgi-env && \
    cp longProcess.zcfg wps_status.zo GetStatus.zcfg /opt/t2service/ && cp /opt/zoo-project/zoo-project/zoo-services/utils/status/cgi-env/updateStatus.xsl   /var/www/data/updateStatus.xsl && \
    cd /opt/watchjob/ && make && make install

COPY scripts/entrypoint.sh /opt/t2scripts/entrypoint.sh

CMD ["/opt/t2scripts/entrypoint.sh"]


