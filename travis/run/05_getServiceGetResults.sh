#!/usr/bin/env bash

curl  -v  -s -L "http://localhost:7777/watchjob/processes/eo_metadata_generation_1_0/jobs/$1/result" -H "accept: application/json"