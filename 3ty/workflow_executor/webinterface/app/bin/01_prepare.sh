#!/usr/bin/env bash

curl -v \
  --request POST \
  --data '{"runID": "string","serviceID": "string"}' \
  http://localhost:8080/prepare

