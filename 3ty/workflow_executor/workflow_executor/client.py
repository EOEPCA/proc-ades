import argparse
import uuid
from kubernetes import client, config
import sys
from workflow_executor import prepare
from workflow_executor import stagein
from workflow_executor import execute
from workflow_executor import result
from workflow_executor import status

from workflow_executor import helpers

__version__ = '1.0.0'
__author__ = 'Blasco Brauzzi'


def get_parser():
    """
    Creates a new argument parser.
    """
    parser = argparse.ArgumentParser('workflow_executor')
    version = '%(prog)s ' + __version__
    parser.add_argument('--version', '-v', action='version', version=version)
    parser.add_argument(
        '--config',
        help='Path to kube configuration file'
    )

    parser.set_defaults(
        func=prepare.run
    )

    subparsers = parser.add_subparsers(
        help='sub-command help'
    )

    ### PREPARE
    prepare_parser = subparsers.add_parser(
        'prepare',
        help='Prepares workflow workspace'
    )

    prepare_parser.add_argument(
        'namespace_name',
        help='Name of the namespace to create'
    )



    prepare_parser.add_argument(
        'volumeSize',
        nargs='?',
        default=1,
        help='Optionally specify a volume size in Gibabytes'
    )
    prepare_parser.add_argument(
        'volumeName',
        nargs='?',
        default="eoepca-volume-" + str(uuid.uuid4()),
        help='Optionally specify a volume name'
    )
    prepare_parser.set_defaults(
        func=prepare.run
    )

    ####
    execute_parser = subparsers.add_parser(
        'execute',
        help='Executes cwl workflow'
    )
    execute_parser.add_argument(
        '-i', '--input_json',
        help='The path to the input json'
    )

    execute_parser.add_argument(
        '-c', '--cwl-file',
        help='The path to the cwl file'
    )

    execute_parser.add_argument(
        '-v', '--volume_name_prefix',
        help='The volume name prefix. Volume name: <prefix>-input-data',
        required=True
    )

    execute_parser.add_argument(
        '-n', '--namespace',
        default=None,
        help='The name of the namespace where to execute the cwl'
    )

    execute_parser.add_argument(
        '-w', '--workflowname',
        default=None,
        help='The id of the workflow to execute'
    )

    execute_parser.add_argument(
        '-m', '--mount_folder',
        default="/application",
        help='Folder where to save application\'s file'
    )

    execute_parser.set_defaults(
        func=execute.run
    )
    ###
    getstatus_parser = subparsers.add_parser(
        'getstatus',
        help='Get workflow status'
    )
    getstatus_parser.add_argument(
        '-w', '--workflowname',
        default=None,
        help='The id of the workflow to execute'
    )
    getstatus_parser.add_argument(
        '-n', '--namespace',
        default=None,
        help='The name of the namespace where to execute the cwl'
    )
    getstatus_parser.set_defaults(
        func=status.run
    )



    ###
    getresult_parser = subparsers.add_parser(
        'getresult',
        help='Get workflow result'
    )
    getresult_parser.add_argument(
        '-c', '--cwl-file',
        help='The path to the cwl file'
    )

    getresult_parser.add_argument(
        '-v', '--volume_name_prefix',
        help='The volume name prefix. Volume name: <prefix>-input-data',
        required=True
    )

    getresult_parser.add_argument(
        '-n', '--namespace',
        default=None,
        help='The name of the namespace where to execute the cwl'
    )

    getresult_parser.add_argument(
        '-w', '--workflowname',
        default=None,
        help='The id of the workflow to execute'
    )

    getresult_parser.add_argument(
        '-m', '--mount_folder',
        default="/application",
        help='Folder where to save application\'s file'
    )
    getresult_parser.set_defaults(
        func=result.run
    )

    return parser



def main(args=None):
    """
    Main entry point for your project.
    """
    parser = get_parser()
    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)
    args = parser.parse_args(args)

    if args.config:
        config.load_kube_config(args.config)
    else:
        config.load_kube_config()

    args.func(args)


if __name__ == '__main__':
    main()
