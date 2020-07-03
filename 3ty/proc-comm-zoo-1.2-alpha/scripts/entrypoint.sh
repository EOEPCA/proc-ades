#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

/usr/sbin/httpd -DFOREGROUND
#/usr/sbin/httpd -DBACKGROUND
