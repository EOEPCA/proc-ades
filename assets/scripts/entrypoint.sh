#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

# Starting Workflow Executor
echo "Starting workflow executor..."
nohup /opt/app-root/src/miniconda3/bin/python -m workflow_executor.fastapiserver &
sleep 5
echo "Workflow executor started"

# Starting WPS ZOO
echo "Starting WPS ZOO. Listening for request..."
/usr/sbin/httpd -DFOREGROUND
