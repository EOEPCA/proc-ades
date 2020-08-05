#!/usr/bin/env bash

curl -v \
  --request POST \
  --data '{"runID": "string","serviceID": "string", "prepareID":"uuid"    }' \
  http://localhost:8080/execute

