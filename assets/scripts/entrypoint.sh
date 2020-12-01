#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

chown 48:48 /opt/zooservices_user
chown 48:48 /var/www/_run/res

# Starting Workflow Executor
echo "Starting workflow executor..."
nohup /opt/miniconda/bin/python -m workflow_executor.fastapiserver &
sleep 5
echo "Workflow executor started"

# Starting WPS ZOO
echo "Starting WPS ZOO. Listening for request..."
/usr/sbin/httpd -DBACKGROUND

tail -f /dev/null
