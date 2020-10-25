#!/usr/bin/env bash

curl  -v  -s -L "http://localhost:7777/rdirienzo/watchjob/processes/eoepcaadesundeployprocess/jobs/$1/result" -H "accept: application/json"
