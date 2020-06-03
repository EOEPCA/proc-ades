#!/usr/bin/env bash

curl -v -L -X POST "http://localhost/wps3/processes/eo_metadata_generation_1_0/jobs" -H  \
  "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d@run.json
