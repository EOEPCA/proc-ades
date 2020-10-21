import json
import sys

from kubernetes import client, config
from kubernetes.client.rest import ApiException
from pprint import pprint

import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)



def run(namespace, volumeSize, volumeName, workflow_config=None, state=None):
    print("Preparing " + namespace + " volumeSize: " + str(volumeSize) + "Gi volumeName: " + volumeName)

    config.load_kube_config()
    api_instance = client.RbacAuthorizationV1Api(client.ApiClient())
    v1 = client.CoreV1Api()

    print("####################################")
    print("######### Checking if namespace already exists")
    try:
        v1.read_namespace(namespace, pretty=True)
        print("Namespace already exists")
        return {"status": "success"}
    except ApiException as e:
        if e.status == 404:
            print("Namespace does not exists and will be created")
        else:
            print("Exception when creating namespace: %s\n" % e, file=sys.stderr)
            raise e

    ### Creating namespace
    print("####################################")
    print("######### Creating namespace")
    try:
        body = client.V1Namespace(metadata=client.V1ObjectMeta(name=namespace))
        namespace_json = v1.create_namespace(body=body, async_req=False)
        print(str(namespace_json))
    except ApiException as e:
        print("Exception when creating namespace: %s\n" % e, file=sys.stderr)
        raise e

    ### Configuring storage_class and creating kubernetes secrets for stageout
    storage_class_name = ""
    if workflow_config != None:
        storage_class_name = workflow_config["storageclass"]


    #### Creating pod manager role
    print("####################################")
    print("######### Creating pod_manager_role")
    metadata = client.V1ObjectMeta(name='pod-manager-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods','pods/log'],
                               verbs=['create', 'patch', 'delete', 'list', 'watch'])
    rules = []
    rules.append(rule)
    body = client.V1Role(metadata=metadata, rules=rules)
    pretty = True

    try:
        api_response = api_instance.create_namespaced_role(namespace, body, pretty=pretty)
        pprint(api_response)
    except ApiException as e:
        print("Exception when creating pod-manager-role: %s\n" % e, file=sys.stderr)
        raise e

    #### Creating log-reader-role
    print("####################################")
    print("######### Creating log-reader-role")
    metadata = client.V1ObjectMeta(name='log-reader-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods','pods/log'],
                               verbs=['create', 'patch', 'delete', 'list', 'watch'])
    # verbs=['get', 'list'])
    rules = []
    rules.append(rule)
    body = client.V1Role(metadata=metadata, rules=rules)
    pretty = True

    try:
        api_response = api_instance.create_namespaced_role(namespace, body, pretty=pretty)
        pprint(api_response)
    except ApiException as e:
        print("Exception when creating pod-manager-role: %s\n" % e, file=sys.stderr)
        raise e

    print("####################################")
    print("######### Creating pod-manager-default-binding")
    metadata = client.V1ObjectMeta(name='pod-manager-default-binding', namespace=namespace)

    role_ref = client.V1RoleRef(api_group='', kind='Role', name='pod-manager-role')

    subject = client.models.V1Subject(api_group='', kind='ServiceAccount', name='default', namespace=namespace)
    subjects = []
    subjects.append(subject)

    body = client.V1RoleBinding(metadata=metadata, role_ref=role_ref, subjects=subjects)
    pretty = True
    try:
        api_response = api_instance.create_namespaced_role_binding(namespace, body, pretty=pretty)
        pprint(api_response)
    except ApiException as e:
        print("Exception when creating pod-manager-default-binding: %s\n" % e, file=sys.stderr)
        raise e

    print("####################################")
    print("######### Creating log-reader-default-binding")
    metadata = client.V1ObjectMeta(name='log-reader-default-binding', namespace=namespace)

    role_ref = client.V1RoleRef(api_group='', kind='Role', name='log-reader-role')

    subject = client.models.V1Subject(api_group='', kind='ServiceAccount', name='default', namespace=namespace)
    subjects = []
    subjects.append(subject)

    body = client.V1RoleBinding(metadata=metadata, role_ref=role_ref, subjects=subjects)
    pretty = True
    try:
        api_response = api_instance.create_namespaced_role_binding(namespace, body, pretty=pretty)
        pprint(api_response)
    except ApiException as e:
        print("Exception when creating log-reader-default-binding: %s\n" % e, file=sys.stderr)
        raise e


    print("####################################")
    print("######### Creating cluster-role-binding")
    metadata = client.V1ObjectMeta(name=f"{namespace}-rbac", namespace=namespace)

    role_ref = client.V1RoleRef(api_group='rbac.authorization.k8s.io', kind='ClusterRole', name='cluster-admin')

    subject = client.models.V1Subject(api_group='', kind='ServiceAccount', name='default', namespace=namespace)
    subjects = []
    subjects.append(subject)

    body = client.V1ClusterRoleBinding(metadata=metadata, role_ref=role_ref, subjects=subjects)
    pretty = True
    try:
        api_response = api_instance.create_cluster_role_binding(body=body, pretty=pretty)
        pprint(api_response)
    except ApiException as e:
        if e.status == 409:
            print(f"cluster-role-binding {namespace}-rbac has already been installed")
        else:
            print("Exception when creating cluster-role-binding: %s\n" % e, file=sys.stderr)
            raise e


    print("####################################")
    print("######### Creating Persistent Volume Claims")

    metadata1 = client.V1ObjectMeta(name=f"{volumeName}-input-data", namespace=namespace)
    spec1 = client.V1PersistentVolumeClaimSpec(
        # must be ReadWriteOnce for EBS
        # access_modes=["ReadWriteOnce", "ReadOnlyMany"],
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
    )

    if storage_class_name:
        spec1.storage_class_name = storage_class_name

    body1 = client.V1PersistentVolumeClaim(metadata=metadata1, spec=spec1)

    metadata2 = client.V1ObjectMeta(name=f"{volumeName}-tmpout", namespace=namespace)
    spec2 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
    )
    if storage_class_name:
        spec2.storage_class_name = storage_class_name

    body2 = client.V1PersistentVolumeClaim(metadata=metadata2, spec=spec2)

    metadata3 = client.V1ObjectMeta(name=f"{volumeName}-output-data", namespace=namespace
                                    )
    spec3 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
    )
    if storage_class_name:
        spec3.storage_class_name = storage_class_name

    body3 = client.V1PersistentVolumeClaim(metadata=metadata3, spec=spec3)

    pretty = True
    try:
        api_response1 = v1.create_namespaced_persistent_volume_claim(namespace, body1, pretty=pretty)
        api_response2 = v1.create_namespaced_persistent_volume_claim(namespace, body2, pretty=pretty)
        api_response3 = v1.create_namespaced_persistent_volume_claim(namespace, body3, pretty=pretty)
        pprint(api_response1)
        pprint(api_response2)
        pprint(api_response3)
    except ApiException as e:
        print("Exception when creating persistent_volume_claim: %s\n" % e, file=sys.stderr)
        raise e

    return {"status": "success"}


def get(namespace, state=None):
    config.load_kube_config()
    api_instance = client.RbacAuthorizationV1Api(client.ApiClient())
    v1 = client.CoreV1Api()

    # Things to check:
    # namespace
    # kubernetes volume claim

    # NAMESPACE CHECK
    pretty = True
    try:
        api_response = v1.read_namespace_status(namespace, pretty=pretty)
        pprint(api_response)

    except ApiException as e:
        print("Exception when calling CoreV1Api->read_namespace_status: %s\n" % e)
        raise e

    # VOLUME CLAIM CHECK

    volumeclaim1 = f"{namespace}-volume-input-data"
    volumeclaim2 = f"{namespace}-volume-tmpout"
    volumeclaim3 = f"{namespace}-volume-output-data"

    try:
        response1 = v1.read_namespaced_persistent_volume_claim(volumeclaim1, namespace, pretty=True)
        if response1.status.phase == "Pending":
            return {"status": "pending"}
        response2 = v1.read_namespaced_persistent_volume_claim(volumeclaim2, namespace, pretty=True)
        if response1.status.phase == "Pending":
            return {"status": "pending"}
        response3 = v1.read_namespaced_persistent_volume_claim(volumeclaim3, namespace, pretty=True)
        if response1.status.phase == "Pending":
            return {"status": "pending"}

        pprint(response1)
        pprint(response2)
        pprint(response3)
    except ApiException as e:
        print("Exception when calling CoreV1Api->read_namespaced_persistent_volume_claim: %s\n" % e)
        raise e

    return {"status": "success"}
