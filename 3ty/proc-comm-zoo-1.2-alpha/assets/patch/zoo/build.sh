#!/usr/bin/env bash

#cp /work/3ty/proc-comm-zoo-1.2-alpha/assets/patch/zoo/build.sh /opt/zoo-project/zoo-project/zoo-kernel/c && chmod +x c
cp /work/3ty/proc-comm-zoo-1.2-alpha/assets/patch/zoo/zoo_service_loader.c .
cp /work/3ty/proc-comm-zoo-1.2-alpha/assets/patch/zoo/service_json.c  .
make && make install && cp zoo_loader.cgi /var/www/zoo-bin/zoo_loader.cgi