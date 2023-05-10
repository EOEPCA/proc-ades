#! /bin/sh

# prerequisites:
# - helm
# - kubectl
# - kind
# - nfs-common
# - curl


oneTimeSetUp() {

  echo "creating a cluster called 'ades-kind-cluster'"
  #kind create cluster --name ades-kind-cluster
  kind create cluster --config=kind.cfg --name ades-kind-cluster

  echo "Set kubectl context to 'kind-ades-kind-cluster'"
  kubectl cluster-info --context kind-ades-kind-cluster

  echo "installing open-iscsi on kind control plane pod"
  docker exec ades-kind-cluster-control-plane apt-get update
  docker exec ades-kind-cluster-control-plane apt-get install -y open-iscsi
  docker exec ades-kind-cluster-control-plane systemctl enable --now iscsid

#  echo "installing longhorn"
#  helm repo add longhorn https://charts.longhorn.io
#  helm repo update
#  helm install longhorn longhorn/longhorn --set persistence.defaultClassReplicaCount=1 --namespace longhorn-system --create-namespace --version 1.4.1


  echo "installing openebs"
  helm repo add openebs https://openebs.github.io/charts
  helm repo update
  helm install openebs --namespace openebs openebs/openebs --create-namespace


  # takes 3 mins to install longhorn
  sleep 220

  echo "creating namespace 'ades-test'"
  kubectl create ns ades-test
}

oneTimeTearDown() {
  echo "Delete kind cluster 'kind-ades-kind-cluster'"
  #kind delete cluster --name ades-kind-cluster
}

testDeployingAdesHelmChart() {
  helm install -f ./values.ades-kind-test.yaml ades ../../charts/ades/ -n ades-test
  STATUS_STRING=$(helm status ades -n ades-test)
  assertContains "$STATUS_STRING" 'STATUS: deployed'
}

testAdesIsRunning() {
  sleep 15
  POD_NAME=$(kubectl get pods --namespace ades-test -l "app.kubernetes.io/name=ades,app.kubernetes.io/instance=ades" -o jsonpath="{.items[0].metadata.name}")
  echo "pod name is: $POD_NAME"
  n=0
  until [ "$n" -ge 10 ]; do
    STATUS="$(kubectl get pod $POD_NAME -n ades-test -o custom-columns=':status.phase' | tr '\n' ' ' | xargs)"
    if [ $STATUS = "Running" ]; then
      break
    fi
    echo "Pod is not running yet. Status: ${STATUS}"
    n=$((n + 1))
    sleep 20
  done

  assertEquals "$STATUS" "Running"
}

testPortforwardAdesOn8080() {
  export POD_NAME=$(kubectl get pods --namespace ades-test -l "app.kubernetes.io/name=ades,app.kubernetes.io/instance=ades" -o jsonpath="{.items[0].metadata.name}")
  kubectl --namespace ades-test port-forward $POD_NAME 8080:80 >/dev/null 2>&1 &

}

testAdesIsReady() {
  sleep 5
  POD_NAME=$(kubectl get pods --namespace ades-test -l "app.kubernetes.io/name=ades,app.kubernetes.io/instance=ades" -o jsonpath="{.items[0].metadata.name}")
  n=0
  until [ "$n" -ge 5 ]; do
    READY=$(kubectl get pod $POD_NAME -n ades-test -o jsonpath="{.status.containerStatuses[0].ready}")

    if [ "$READY" = "true" ]; then
      break
    fi
    echo "Pod is not ready yet "
    n=$((n + 1))
    sleep 20
  done

  assertEquals "$READY" "true"
}

testInstallMinio() {
  kubectl create ns minio
  kubectl create -f my-minio-fs.yaml -n minio
  kubectl expose deployment/my-minio-fs --type="NodePort" --port 9000 -n minio
  kubectl port-forward svc/my-minio-fs 9000:9000 -n minio >/dev/null 2>&1 &
}

testDeploySnuggsApp() {
  result=$(curl --location --request POST 'http://127.0.0.1:8080/terradue/wps3/processes' \
    --header 'Accept: application/json' \
    --header 'Content-Type: application/json' \
    --data-raw '{
    "executionUnit": {
      "href": "https://raw.githubusercontent.com/EOEPCA/app-snuggs/main/app-package.cwl",
      "type": "application/cwl"
    }
  }')
  expected_result=$(cat ./data_assertions/testDeploySnuggsApp_assertion.txt)
  assertEquals "$expected_result" "$result"
}

testExecuteSnuggsApp() {


  #kubectl create -f ./pv.yaml

  result=$(curl -v --location --request POST 'http://127.0.0.1:8080/terradue/wps3/processes/snuggs-0_3_0/execution' \
    --header 'Accept: application/json' \
    --header 'Content-Type: application/json' \
    --header 'Prefer: respond-async' \
    --data-raw '{
  "inputs": {
    "input_reference": "https://earth-search.aws.element84.com/v0/collections/sentinel-s2-l2a-cogs/items/S2B_36RTT_20191205_0_L2A",
    "s_expression": [
      "nbr:(/ (- B05 B02) (+ B05 B02))",
      "ndvi:(/ (- B05 B03) (+ B05 B03))"
    ]
  }
}')

  echo $result


  #expected_result=$(cat ./data_assertions/testExecuteSnuggsApp_assertion.txt)
  #assertEquals "$expected_result" "$result"

}

# Load shUnit2.
. $()./shunit2
