#!/usr/bin/env bash

cp -f /opt/t2service/* /zooservices/

ldconfig

chown 48:48 /opt/zooservices_user
chown 48:48 /var/www/_run/res

# Starting Workflow Executor
echo "Starting workflow executor..."
nohup /srv/conda/envs/workflow_executor_env/bin/python -m workflow_executor.fastapiserver &
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


set -e
printf "\n\033[0;44m---> Starting the SSH server.\033[0m\n"
service ssh start
service ssh status
exec "$@"