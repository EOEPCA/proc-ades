#!/usr/bin/env bash

curl -v \
  --request POST \
  --data '{"runID": "runID-123","serviceID": "service-id-123"}' \
  http://localhost:8000/prepare

