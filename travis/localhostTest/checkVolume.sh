kubectl create -f eoepcademoshell.yaml -n wf-bd22ffd8-cb1c-11ea-8a3d-a0c5899f98fe
sleep 10
kubectl -n wf-bd22ffd8-cb1c-11ea-8a3d-a0c5899f98fe exec -it redis-demo -- /bin/bash