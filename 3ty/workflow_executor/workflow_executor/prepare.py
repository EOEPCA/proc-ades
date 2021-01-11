import json
import sys
import time

from kubernetes import client, config
from kubernetes.client.rest import ApiException
from pprint import pprint
from workflow_executor import helpers
import urllib3

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


def run(namespace, tmpVolumeSize, outputVolumeSize, volumeName, storage_class_name=None, imagepullsecrets=None,ades_namespace=None,
        state=None):
    print(
        f"Preparing {namespace} tmpVolumeSize: {tmpVolumeSize} outputVolumeSize: {outputVolumeSize}  volumeName: {volumeName}")

    apiclient = helpers.get_api_client()
    api_instance = client.RbacAuthorizationV1Api(apiclient)
    v1 = client.CoreV1Api(api_client=apiclient)

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

    #### Creating pod manager role
    print("####################################")
    print("######### Creating pod_manager_role")
    metadata = client.V1ObjectMeta(name='pod-manager-role', namespace=namespace)
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods', 'pods/log'],
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
    rule = client.V1PolicyRule(api_groups=['*'], resources=['pods', 'pods/log'],
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

    # metadata1 = client.V1ObjectMeta(name=f"{volumeName}-input-data", namespace=namespace)
    # spec1 = client.V1PersistentVolumeClaimSpec(
    #     # must be ReadWriteOnce for EBS
    #     # access_modes=["ReadWriteOnce", "ReadOnlyMany"],
    #     access_modes=["ReadWriteMany"],
    #     resources=client.V1ResourceRequirements(
    #         requests={"storage": inputVolumeSize}
    #     )
    # )
    #
    # if storage_class_name:
    #     spec1.storage_class_name = storage_class_name
    #
    # body1 = client.V1PersistentVolumeClaim(metadata=metadata1, spec=spec1)

    metadata2 = client.V1ObjectMeta(name=f"{volumeName}-tmpout", namespace=namespace)
    spec2 = client.V1PersistentVolumeClaimSpec(
        access_modes=["ReadWriteMany"],
        resources=client.V1ResourceRequirements(
            requests={"storage": tmpVolumeSize}
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
            requests={"storage": outputVolumeSize}
        )
    )
    if storage_class_name:
        spec3.storage_class_name = storage_class_name

    body3 = client.V1PersistentVolumeClaim(metadata=metadata3, spec=spec3)

    pretty = True
    try:
        #    api_response1 = v1.create_namespaced_persistent_volume_claim(namespace, body1, pretty=pretty)
        api_response2 = v1.create_namespaced_persistent_volume_claim(namespace, body2, pretty=pretty)
        api_response3 = v1.create_namespaced_persistent_volume_claim(namespace, body3, pretty=pretty)
        #    pprint(api_response1)
        pprint(api_response2)
        pprint(api_response3)
    except ApiException as e:
        print("Exception when creating persistent_volume_claim: %s\n" % e, file=sys.stderr)
        raise e

    # we copy the secret from ades namespace to the new job namespace
    if imagepullsecrets is not None and ades_namespace is not None:
        for imagepullsecret in imagepullsecrets:
            # Create an instance of the API class
            secretname = imagepullsecret["name"]
            pretty = True  # str | If 'true', then the output is pretty printed. (optional)
            exact = False  # bool | Should the export be exact.  Exact export maintains cluster-specific fields like 'Namespace'. Deprecated. Planned for removal in 1.18. (optional)
            export = True  # bool | Should this value be exported.  Export strips fields that a user can not specify. Deprecated. Planned for removal in 1.18. (optional)

            secret_export = None
            try:
                secret_export = v1.read_namespaced_secret(secretname, ades_namespace, pretty=pretty, exact=exact, export=export)
            except ApiException as e:
                print("Exception when retrieving image pull secret from eoepca: %s\n" % e)

            time.sleep(5)
            try:
                api_response = v1.create_namespaced_secret(namespace, secret_export, pretty=pretty)
            except ApiException as e:
                print("Exception when creating image pull secret: %s\n" % e)

            time.sleep(5)

            name = 'default'
            try:
                service_account_body = v1.read_namespaced_service_account(name, namespace, pretty=True)
                pprint(api_response)
                time.sleep(5)


                if "secrets" not in service_account_body:
                    service_account_body["secrets"] = []

                if "image_pull_secrets" not in service_account_body:
                    service_account_body["image_pull_secrets"] = []

                service_account_body.secrets.append({"name": secretname})
                service_account_body.image_pull_secrets.append({"name": secretname})

                api_response = v1.patch_namespaced_service_account(name, namespace, service_account_body, pretty=True)
                pprint(api_response)
            except ApiException as e:
                print("Exception when calling CoreV1Api->patch_namespaced_service_account: %s\n" % e)

    return {"status": "success"}


def get(namespace, state=None):
    apiclient = helpers.get_api_client()
    v1 = client.CoreV1Api(api_client=apiclient)

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
    pretty = True
    try:
        pvc_list = v1.list_namespaced_persistent_volume_claim(namespace, pretty=pretty)
        for pvc in pvc_list.items:
            print("%s\t%s" % (pvc.metadata.name, pvc.status.phase))
            if pvc.status.phase == "Pending":
                return {"status": "pending"}
            elif pvc.status.phase == "Failed":
                return {"status": "failed"}
    except ApiException as e:
        print("Exception when calling CoreV1Api->list_namespaced_persistent_volume_claim: %s\n" % e)
        raise e

    return {"status": "success"}
