#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

chown 48:48 /opt/zooservices_user
chown 48:48 /var/www/_run/res

# Starting Workflow Executor
echo "Starting workflow executor..."
nohup /opt/miniconda/bin/python -m workflow_executor.workflow_executor.fastapiserver &
sleep 5
echo "Workflow executor started"


#rm -f /var/log/httpd/access_log
#rm -f /var/log/httpd/error_log
#
#ln -snf /dev/stdout /var/log/httpd/access_log \
#    && ln -snf /dev/stderr /var/log/httpd/error_log

# Starting WPS ZOO
echo "Starting WPS ZOO. Listening for request..."
/usr/sbin/httpd -DBACKGROUND
sleep 2
tail -f /var/log/httpd/error_log /var/log/httpd/access_log
