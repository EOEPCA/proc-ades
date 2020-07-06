#!/usr/bin/env bash

curl  -v  -s -L "http://localhost:7777/watchjob/processes/eoepcaadesdeployprocess/jobs/$1" -H "accept: application/json"