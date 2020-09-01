#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

/usr/sbin/httpd -DFOREGROUND
#/usr/sbin/httpd -DBACKGROUND

/opt/app-root/src/miniconda3/bin/python -m workflow_executor.fastapiserver