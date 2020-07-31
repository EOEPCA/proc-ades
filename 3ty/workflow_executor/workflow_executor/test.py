import os
from kubernetes import client, config


# Configs can be set in Configuration class directly or using helper utility
#config.load_kube_config()


# /var/snap/microk8s/current/credentials/kubelet.config
#config.load_kube_config("/home/bla/.kube/config")
#config.load_kube_config("/etc/rancher/k3s/k3s.yaml")
configuration = client.Configuration()
configuration.verify_ssl = False
# configuration.debug = False
client.Configuration.set_default(configuration)



v1 = client.CoreV1Api()
print("Listing pods with their IPs:")
ret = v1.list_pod_for_all_namespaces(watch=False)
for i in ret.items:
    print("%s\t%s\t%s" % (i.status.pod_ip, i.metadata.namespace, i.metadata.name))