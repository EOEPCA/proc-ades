#!/usr/bin/env bash

curl -v \
  --request POST \
  --data '{"runID": "string","serviceID": "string"}' \
  http://127.0.0.1:8000/prepare

