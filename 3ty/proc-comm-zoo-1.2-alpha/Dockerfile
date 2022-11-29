FROM centos/devtoolset-7-toolchain-centos7 as builder
USER root

RUN yum install -y epel-release
RUN yum update -y
RUN yum install -y json-c json-c-devel zlib-devel libxml2 libxml2-devel bison openssl  python-devel subversion libxslt-devel libcurl-devel gdal gdal-devel proj-devel libuuid-devel openssl-devel fcgi-devel wget unzip autoconf flex cmake3

RUN yum install -y bzip2 kernel-devel which && ln -s /opt/rh/devtoolset-7/enable /etc/profile.d/rhgccenable.sh && chmod +x /etc/profile.d/rhgccenable.sh

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

COPY  assets/patch/zoo/response_print.c /opt/zoo-project/zoo-project/zoo-kernel/response_print.c_
COPY  assets/patch/zoo/zoo_service_loader.c /opt/zoo-project/zoo-project/zoo-kernel/zoo_service_loader.c_
COPY  assets/patch/zoo/service_json.c /opt/zoo-project/zoo-project/zoo-kernel/service_json.c_
RUN  if [ -n "$addUserFeature" ]; then rm /opt/zoo-project/zoo-project/zoo-kernel/zoo_service_loader.c && mv /opt/zoo-project/zoo-project/zoo-kernel/zoo_service_loader.c_ /opt/zoo-project/zoo-project/zoo-kernel/zoo_service_loader.c; fi
RUN  if [ -n "$addUserFeature" ]; then rm /opt/zoo-project/zoo-project/zoo-kernel/service_json.c && mv /opt/zoo-project/zoo-project/zoo-kernel/service_json.c_ /opt/zoo-project/zoo-project/zoo-kernel/service_json.c; fi
RUN  if [ -n "$addUserFeature" ]; then rm /opt/zoo-project/zoo-project/zoo-kernel/response_print.c && mv /opt/zoo-project/zoo-project/zoo-kernel/response_print.c_ /opt/zoo-project/zoo-project/zoo-kernel/response_print.c; fi

RUN cd /opt/zoo-project/thirds/cgic206 && make libcgic.a && make install

RUN cd /opt/zoo-project/zoo-project/zoo-kernel/ && autoconf && \
./configure  --with-json=/usr/ --with-fastcgi=${FOLDER}/libfcgi-2.4.0.orig/OUT/lib --with-xml2config=/usr/bin/xml2-config  --with-cgi-dir=/var/www/zoo-bin --with-etc-dir=yes --sysconfdir=/zooservices

RUN cd /opt/zoo-project/zoo-project/zoo-kernel/ && ls -ltr && make && make install

RUN mkdir -p /opt/t2service /opt/watchjob /var/www/zoo-bin/ /var/www/data/ /etc/zoo-project/ /var/www/_run/res/statusInfos /var/www/t2dep/ /opt/watchjob/
COPY src/* /opt/watchjob/

RUN cd /opt/zoo-project/zoo-project/zoo-services/utils/status && make && cd /opt/zoo-project/zoo-project/zoo-services/utils/status/cgi-env && \
    cp longProcess.zcfg wps_status.zo GetStatus.zcfg /opt/t2service/ && cp /opt/zoo-project/zoo-project/zoo-services/utils/status/cgi-env/updateStatus.xsl   /var/www/data/updateStatus.xsl && \
    cd /opt/watchjob/ && make && make install

# ------------------------------------------------------
# Production Build
# ------------------------------------------------------

FROM centos

RUN cd /etc/yum.repos.d/ && \
    sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-* && \
    sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-*
RUN yum update -y
RUN yum install -y epel-release
RUN yum update -y
RUN yum install -y json-c libxml2 bison openssl wget unzip bzip2 make gcc-c++
ENV PATH "$PATH:/usr/local/bin"

##install built zoo
COPY --from=builder /opt /opt
COPY --from=builder /var/www/data /var/www/data
COPY --from=builder /var/www/t2dep /var/www/t2dep

RUN yum install -y vim httpd \
	&& mkdir -p /opt/watchjob /var/www/zoo-bin/ /var/www/data/ /etc/zoo-project/ /var/www/_run/res/statusInfos \
	&& mkdir -p /zooservices &&  chmod 777 /zooservices \
	&& mkdir -p /var/www/fcgi/ /var/www/_run/zoo  /var/www/_run/wps3 /var/www/t2dep /var/www/_run/watchjob \
	&& chown -R 48:48 /var/www/zoo-bin/ /var/www/zoo-bin/ /var/www/data/ /var/www/_run/res  \
	&& echo '/usr/local/lib' > /etc/ld.so.conf.d/zoo.conf && ldconfig \
	&& mkdir -p /opt/t2build/includes  /opt/t2service/ /opt/opt/t2service/t2scripts/ \
	&& mkdir -p /var/www/zoo-bin/ /var/www/_run/zoo/

COPY assets/zoo/httpd/htaccess_html /var/www/html/.htaccess
COPY assets/zoo/httpd/htaccess_wps3 /var/www/_run/wps3/.htaccess
COPY assets/zoo/httpd/htaccess_watchjob /var/www/_run/watchjob/.htaccess
COPY assets/zoo/httpd/htaccess /var/www/_run/zoo/.htaccess
COPY assets/zoo/httpd/zoo.conf	/etc/httpd/conf.d/zoo.conf
COPY assets/main.cfg /etc/zoo-project/main.cfg
COPY assets/oas.cfg /etc/zoo-project/oas.cfg
COPY assets/main.cfg /opt/t2service/main.cfg
COPY assets/oas.cfg /opt/t2service/oas.cfg

COPY scripts/entrypoint.sh /opt/t2scripts/entrypoint.sh

CMD ["/opt/t2scripts/entrypoint.sh"]





