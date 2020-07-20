import sys

from kubernetes import client
from kubernetes.client.rest import ApiException
from pprint import pprint

from workflow_executor import helpers


def run(args):
    namespace = args.namespace_name
    volumeSize = args.volumeSize
    volumeName = args.volumeName

    print("Preparing " + namespace + " volumeSize: " + str(volumeSize) + "Gi volumeName: " + volumeName, file=sys.stderr)

    v1 = client.CoreV1Api()

    configuration = client.Configuration()
    api_instance = client.RbacAuthorizationV1Api(client.ApiClient(configuration))

    ### Creating namespace
    print("####################################", file=sys.stderr)
    print("######### Creating namespace", file=sys.stderr)
    try:
        body = client.V1Namespace(metadata=client.V1ObjectMeta(name=args.namespace_name))
        namespace_json = v1.create_namespace(body=body, async_req=False)
        print(str(namespace_json),file=sys.stderr)
    except ApiException as e:
        print("Exception when creating namespace: %s\n" % e, file=sys.stderr)
        return 1

    #### Creating pod manager role
    print("####################################", file=sys.stderr)
    print("######### Creating pod_manager_role", file=sys.stderr)
    metadata = client.V1ObjectMeta(name='pod-manager-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods'],
                               verbs=['create', 'patch', 'delete', 'list', 'watch'])
    rules = []
    rules.append(rule)
    body = client.V1Role(metadata=metadata, rules=rules)
    pretty = True

    try:
        api_response = api_instance.create_namespaced_role(namespace, body, pretty=pretty)
        pprint(api_response,sys.stderr)
    except ApiException as e:
        print("Exception when creating pod-manager-role: %s\n" % e, file=sys.stderr)
        return 1

    #### Creating log-reader-role
    print("####################################", file=sys.stderr)
    print("######### Creating log-reader-role", file=sys.stderr)
    metadata = client.V1ObjectMeta(name='log-reader-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods'],
                               verbs=['create', 'patch', 'delete', 'list', 'watch'])
                               #verbs=['get', 'list'])
    rules = []
    rules.append(rule)
    body = client.V1Role(metadata=metadata, rules=rules)
    pretty = True

    try:
        api_response = api_instance.create_namespaced_role(namespace, body, pretty=pretty)
        pprint(api_response,sys.stderr)
    except ApiException as e:
        print("Exception when creating pod-manager-role: %s\n" % e, file=sys.stderr)
        return 1

    print("####################################", file=sys.stderr)
    print("######### Creating pod-manager-default-binding", file=sys.stderr)
    metadata = client.V1ObjectMeta(name='pod-manager-default-binding', namespace=namespace)

    role_ref = client.V1RoleRef(api_group='', kind='Role', name='pod-manager-role')

    subject = client.models.V1Subject(api_group='', kind='ServiceAccount', name='default', namespace=namespace)
    subjects = []
    subjects.append(subject)

    body = client.V1RoleBinding(metadata=metadata, role_ref=role_ref, subjects=subjects)
    pretty = True
    try:
        api_response = api_instance.create_namespaced_role_binding(namespace, body, pretty=pretty)
        pprint(api_response,sys.stderr)
    except ApiException as e:
        print("Exception when creating pod-manager-default-binding: %s\n" % e)
        return 1

    print("####################################", file=sys.stderr)
    print("######### Creating log-reader-default-binding", file=sys.stderr)
    metadata = client.V1ObjectMeta(name='log-reader-default-binding', namespace=namespace)

    role_ref = client.V1RoleRef(api_group='', kind='Role', name='log-reader-role')

    subject = client.models.V1Subject(api_group='', kind='ServiceAccount', name='default', namespace=namespace)
    subjects = []
    subjects.append(subject)

    body = client.V1RoleBinding(metadata=metadata, role_ref=role_ref, subjects=subjects)
    pretty = True
    try:
        api_response = api_instance.create_namespaced_role_binding(namespace, body, pretty=pretty)
        pprint(api_response,sys.stderr)
    except ApiException as e:
        print("Exception when creating log-reader-default-binding: %s\n" % e)
        return 1

    print("####################################", file=sys.stderr)
    print("######### Creating Persistent Volume Claims", file=sys.stderr)

    metadata1 = client.V1ObjectMeta(name=f"{volumeName}-input-data", namespace=namespace)
    spec1 = client.V1PersistentVolumeClaimSpec(
        # must be ReadWriteOnce for EBS
        #access_modes=["ReadWriteOnce", "ReadOnlyMany"],
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
    )
    body1 = client.V1PersistentVolumeClaim(metadata=metadata1, spec=spec1)

    metadata2 = client.V1ObjectMeta(name=f"{volumeName}-tmpout", namespace=namespace)
    spec2 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
    )
    body2 = client.V1PersistentVolumeClaim(metadata=metadata2, spec=spec2)

    metadata3 = client.V1ObjectMeta(name=f"{volumeName}-output-data", namespace=namespace)
    spec3 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": f"{volumeSize}Gi"}
        )
    )
    body3 = client.V1PersistentVolumeClaim(metadata=metadata3, spec=spec3)

    pretty = True
    try:
        api_response1 = v1.create_namespaced_persistent_volume_claim(namespace, body1, pretty=pretty)
        api_response2 = v1.create_namespaced_persistent_volume_claim(namespace, body2, pretty=pretty)
        api_response3 = v1.create_namespaced_persistent_volume_claim(namespace, body3, pretty=pretty)
        pprint(api_response1,sys.stderr)
        pprint(api_response2,sys.stderr)
        pprint(api_response3,sys.stderr)
    except ApiException as e:
        print("Exception when creating persistent_volume_claim: %s\n" % e)
        return 1


