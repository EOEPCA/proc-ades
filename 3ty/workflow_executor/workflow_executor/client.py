import uuid

import workflow_executor
from workflow_executor import execute, prepare
from workflow_executor import result
from workflow_executor import status
from workflow_executor import helpers
import click


################################
### COMMON OPTIONS

class State(object):

    def __init__(self):
        self.verbosity = 0
        self.debug = False
        self.kubeconfig = "~/.kube/config"


pass_state = click.make_pass_decorator(State, ensure=True)


def verbosity_option(f):
    def callback(ctx, param, value):
        state = ctx.ensure_object(State)
        state.verbosity = value
        return value

    return click.option('-v', '--verbose', count=True,
                        expose_value=False,
                        help='Enables verbosity.',
                        callback=callback)(f)


def debug_option(f):
    def callback(ctx, param, value):
        state = ctx.ensure_object(State)
        state.debug = value
        return value

    return click.option('--debug/--no-debug',
                        expose_value=False,
                        help='Enables or disables debug mode.',
                        callback=callback)(f)


def kubeconfig_option(f):
    def callback(ctx, param, value):
        state = ctx.ensure_object(State)
        state.kubeconfig = value
        return value

    return click.option('--kubeconfig',
                        expose_value=False,
                        help='Sets kube config path.',
                        callback=callback, type=click.Path(exists=True))(f)


def common_options(f):
    f = verbosity_option(f)
    f = debug_option(f)
    f = kubeconfig_option(f)
    return f


#####################
## PREPARE
@click.group()
def prepare_cli():
    pass


@prepare_cli.command()
@click.argument('namespace')
@click.argument('volume_size', default=1, type=int)
@click.argument('volume_name', default="eoepca-volume-" + str(uuid.uuid4()))
@click.option('-s','--stageout_config_file', type=click.Path(exists=True))
@common_options
@pass_state
def prepare(state, namespace, volume_size, volume_name,stageout_config_file):
    """Prepares the workflow namespace in a kubernetes environment

    \b
    NAMESPACE   is the name of the namespace that Kubernetes will create.
    VOLUME_SIZE is the size that kubernetes will allocate to the volumes.
    VOLUME_NAME is the prefix of the volumes that Kubernetes will create.
    """
    click.echo('Verbosity: %s' % state.verbosity)
    click.echo('Debug: %s' % state.debug)
    click.echo('Kube config: %s' % state.kubeconfig)
    click.echo('namespace: %s' % namespace)
    click.echo('volume_size: %d' % volume_size)
    click.echo('volume_name: %s' % volume_name)

    resp_status = workflow_executor.prepare.run(namespace=namespace, volumeSize=volume_size, volumeName=volume_name,stageout_config_file=stageout_config_file,
                                                state=state)
    click.echo(resp_status)


#######################
## EXECUTE
@click.group()
def execute_cli():
    pass


@execute_cli.command()
@common_options
@pass_state
@click.argument('input_json', required=True, type=click.Path(exists=True))
@click.argument('cwl_file', required=True, type=click.Path(exists=True))
@click.argument('volume_name_prefix', required=True)
@click.argument('namespace', required=True)
@click.argument('workflow_name', required=True)
@click.argument('mount_folder', required=True)
def execute(state, input_json, cwl_file, volume_name_prefix, namespace, workflow_name, mount_folder):
    """Executes the cwl workflow

    \b
    INPUT_JSON          is the path to the input json file.
    CWL_FILE            is the path to the cwl file.
    VOLUME_NAME_PREFIX  is the volume name prefix. eg. <volume_name_prefix>-input-data.
    NAMESPACE           is the name of the namespace.
    WORKFLOW_NAME       is the id of the workflow to execute.
    MOUNT_FOLDER        is the folder path where the kubernetes will store the application's files.

    """
    click.echo('Verbosity: %s' % state.verbosity)
    click.echo('Debug: %s' % state.debug)
    click.echo('Kubeconfig: %s' % state.kubeconfig)

    resp_status = workflow_executor.execute.run(job_input_json=input_json, cwl_document=cwl_file,
                                                volume_name_prefix=volume_name_prefix, namespace=namespace,
                                                mount_folder=mount_folder,
                                                workflow_name=workflow_name, state=state)
    click.echo(resp_status)


#######################
## GETSTATUS
@click.group()
def status_cli():
    pass


@status_cli.command()
@common_options
@pass_state
@click.argument('namespace', required=True)
@click.argument('workflow_name', required=True)
def status(state, namespace, workflow_name):
    """Gets the workflow status

    \b
    WORKFLOW_NAME       is the id of the workflow.
    NAMESPACE           is the name of the namespace.
    """
    click.echo('Verbosity: %s' % state.verbosity)
    click.echo('Debug: %s' % state.debug)

    resp = workflow_executor.status.run(namespace, workflow_name, state)
    return resp


#######################
## GETRESULT
@click.group()
def result_cli():
    pass


@result_cli.command()
@common_options
@pass_state
@click.argument('namespace_name',  required=True)
@click.argument('mount_folder',  required=True)
@click.argument('volume_name_prefix', required=True)
@click.argument('workflowname',  required=True)
def result(state, namespace_name, mount_folder, volume_name_prefix, workflowname):
    """Command on result"""

    resp = workflow_executor.result.run(namespace=namespace_name, mount_folder=mount_folder,
                                        volume_name_prefix=volume_name_prefix, workflowname=workflowname, state=state)
    return resp


cli = click.CommandCollection(sources=[prepare_cli, execute_cli, status_cli, result_cli])


def main(args=None):
    cli()


if __name__ == '__main__':
    cli()
