#!/usr/bin/env bash

curl -v \
  --request GET \
  http://localhost:8000/result/service-id-123/runid-123/prepare-id-123/job-id-123 \
  -H "Content-type: application/json"


