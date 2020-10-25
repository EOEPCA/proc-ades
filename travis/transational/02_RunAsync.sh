#!/usr/bin/env bash

curl -v -L -X POST "http://localhost:7777/rdirienzo/wps3/processes/eoepcaadesdeployprocess/jobs" -H  \
  "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d@${1}
