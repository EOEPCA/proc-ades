#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

nohup /opt/app-root/src/miniconda3/bin/python -m workflow_executor.fastapiserver &

/usr/sbin/httpd -DFOREGROUND
#/usr/sbin/httpd -DBACKGROUND

