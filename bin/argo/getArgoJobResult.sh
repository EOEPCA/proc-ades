#!/usr/bin/env bash

JOBID=''

if [ $# -gt 0 ]; then
  JOBID=${1}
else
  echo "insert jobid"
  exit
fi

curl -v -s -L "http://localhost:7777/watchjob/processes/argo/jobs/${JOBID}/result" -H "accept: application/json"
