curl  -v --location --request POST "http://${THEHOST}:8888/rdirienzo/wps3/processes/vegetation_index_/jobs" \
--header 'Accept: application/json' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsImtpZCI6IlJTQTEifQ.eyJhY3RpdmUiOnRydWUsImV4cCI6MTU5MzUxNTU2NSwiaWF0IjoxNTkzNTExOTY1LCJuYmYiOm51bGwsInBlcm1pc3Npb25zIjpbeyJyZXNvdXJjZV9pZCI6ImI3Y2FkZTVjLTM3MmYtNGM4Ny1iZTgyLWE3OTU2NDk4ZTcyOSIsInJlc291cmNlX3Njb3BlcyI6WyJBdXRoZW50aWNhdGVkIiwib3BlbmlkIl0sImV4cCI6MTU5MzUxNTU2NCwicGFyYW1zIjpudWxsfV0sImNsaWVudF9pZCI6IjYxY2UyOGQ1LWFhMTYtNGRkYy04NDJmLWZjYzE1OGQzMTVmYSIsInN1YiI6bnVsbCwiYXVkIjoiNjFjZTI4ZDUtYWExNi00ZGRjLTg0MmYtZmNjMTU4ZDMxNWZhIiwiaXNzIjpudWxsLCJqdGkiOm51bGwsInBjdF9jbGFpbXMiOnsiYXVkIjpbIjYxY2UyOGQ1LWFhMTYtNGRkYy04NDJmLWZjYzE1OGQzMTVmYSJdLCJzdWIiOlsiZWIzMTQyMWUtMGEyZS00OTBmLWJiYWYtMDk3MWE0ZTliNzhhIl0sInVzZXJfbmFtZSI6WyJkZW1zZmRzZnNkZnNvcyJdLCJpc3MiOlsiaHR0cHM6Ly9lb2VwY2EtZGV2LmRlaW1vcy1zcGFjZS5jb20iXSwiZXhwIjpbIjE1OTM1MTU1NjQiXSwiaWF0IjpbIjE1OTM1MTE5NjQiXSwib3hPcGVuSURDb25uZWN0VmVyc2lvbiI6WyJvcGVuaWRjb25uZWN0LTEuMCJdfX0.Ljxc9vChcNBD--ht0ho6x6faLrE1b3v0-NxPOiAZM8c' \
-d '{
    "inputs": [
        {
            "id": "input_reference",
            "input": {
                "dataType": {
                    "name": "string"
                },
                "value": "https://catalog.terradue.com/sentinel2/search?uid=S2A_MSIL2A_20191216T004701_N0213_R102_T53HPA_20191216T024808"
            }
        },
        {
            "id": "aoi",
            "input": {
                "dataType": {
                    "name": "string"
                },
                "value": "POLYGON((136.112726861895 -36.227897298303,137.333826362695 -36.2103069464338,137.305145407058 -35.2211228310596,136.099040812374 -35.2380875358202,136.112726861895 -36.227897298303))"
            }
        }
    ],
    "outputs": [
        {
            "format": {
                "mimeType": "string",
                "schema": "string",
                "encoding": "string"
            },
            "id": "wf_output",
            "transmissionMode": "value"
        }
    ],
    "mode": "async",
    "response": "raw"
}'