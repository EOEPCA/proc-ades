#!/usr/bin/env bash

curl -v \
  --request POST \
  --data '@../examples/vegetation-index-workflow.json' \
  http://127.0.0.1:8000/execute




curl -v \
  --request POST \
  --data '@wbb.json' \
  http://127.0.0.1:8000/execute
