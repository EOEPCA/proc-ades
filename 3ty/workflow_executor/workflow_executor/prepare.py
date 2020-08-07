import json
import sys

from kubernetes import client, config
from kubernetes.client.rest import ApiException
from pprint import pprint

import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


def run(namespace, volumeSize, volumeName, stageout_config_file, state=None):
    print("Preparing " + namespace + " volumeSize: " + str(volumeSize) + "Gi volumeName: " + volumeName)

    if state:
        config.load_kube_config(state.kubeconfig)
    configuration = client.Configuration()
    configuration.verify_ssl = False
    client.Configuration.set_default(configuration)
    api_instance = client.RbacAuthorizationV1Api(client.ApiClient(configuration))
    v1 = client.CoreV1Api()

    ### Creating namespace
    print("####################################")
    print("######### Creating namespace")
    try:
        body = client.V1Namespace(metadata=client.V1ObjectMeta(name=namespace))
        namespace_json = v1.create_namespace(body=body, async_req=False)
        print(str(namespace_json))
    except ApiException as e:
        print("Exception when creating namespace: %s\n" % e, file=sys.stderr)
        error = {"debug": "Exception when creating namespace", "error": e.body}
        return error

    #### Creating pod manager role
    print("####################################")
    print("######### Creating pod_manager_role")
    metadata = client.V1ObjectMeta(name='pod-manager-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods'],
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
        error = {"debug": "Exception when creating pod-manager-role", "error": e.body}
        return error

    #### Creating log-reader-role
    print("####################################")
    print("######### Creating log-reader-role")
    metadata = client.V1ObjectMeta(name='log-reader-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods'],
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
        error = {"debug": "Exception when creating pod-manager-role", "error": e.body}
        return error

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
        error = {"debug": "Exception when creating pod-manager-default-binding", "error": e}
        return error

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
        error = {"debug": "Exception when creating log-reader-default-binding", "error": e.body}
        return error

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
        , storage_class_name="glusterfs-storage"
    )
    body1 = client.V1PersistentVolumeClaim(metadata=metadata1, spec=spec1)

    metadata2 = client.V1ObjectMeta(name=f"{volumeName}-tmpout", namespace=namespace)
    spec2 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
        , storage_class_name="glusterfs-storage"
    )
    body2 = client.V1PersistentVolumeClaim(metadata=metadata2, spec=spec2)

    metadata3 = client.V1ObjectMeta(name=f"{volumeName}-output-data", namespace=namespace
                                    )
    spec3 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
        , storage_class_name="glusterfs-storage"
    )
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
        error = {"debug": "Exception when creating persistent_volume_claim", "error": e.body}
        return error



    if stageout_config_file != None:
        try:

            with open(stageout_config_file, 'r') as f:
                stageout_config_json = json.load(f)

            secret = client.V1Secret()
            secret.metadata = client.V1ObjectMeta(name="procades-secret")
            secret.type = "Opaque"
            secret.data = stageout_config_json["stageout"]

            v1.create_namespaced_secret(namespace, secret)
        except rest.ApiException as e:
            # Status appears to be a string.
            if e.status == 409:
                logging.info("procades-secret has already been installed")
            else:
                raise

    return {"status": 200}
