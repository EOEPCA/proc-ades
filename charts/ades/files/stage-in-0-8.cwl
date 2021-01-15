$graph:
- baseCommand: stage-in
  arguments: ['-t', './']
  class: CommandLineTool
  hints:
    DockerRequirement:
      dockerPull: eoepca/stage-in:0.8
  id: stagein
  inputs:
    inp1:
      inputBinding:
        position: 1
        prefix: -u
      type: string?
    inp2:
      inputBinding:
        position: 2
        prefix: -p
      type: string?
    inp3:
      inputBinding:
        position: 3
        prefix: -e
      type: string?
    inp4:
      inputBinding:
        position: 4
        prefix: -r
      type: string?
    inp5:
      inputBinding:
        position: 5
        prefix: -s
      type: string?
    inp6:
      inputBinding:
        position: 6
      type: string[]
  outputs:
    results:
      outputBinding:
        glob: .
      type: Any
  requirements:
    EnvVarRequirement:
      envDef:
        PATH: /opt/anaconda/envs/env_stagein/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
    ResourceRequirement: {}
  stderr: std.err
  stdout: std.out
- class: Workflow
  label: Instac stage-in
  doc: Stage-in using Instac
  id: main
  inputs:
    input_reference:
      doc: A reference to an opensearch catalog
      label: A reference to an opensearch catalog
      type: string[]
    stage_in_username:
      type: string?
    stage_in_password:
      type: string?
    stage_in_s3_endpoint:
      type: string?
    stage_in_s3_region:
      type: string?
    stage_in_s3_signature_version:
      type: string?
  outputs:
  - id: wf_outputs
    outputSource:
    - node_1/results
    type:
      items: Directory
      type: array
  steps:
    node_1:
      in:
        inp1: stage_in_username
        inp2: stage_in_password
        inp3: stage_in_s3_endpoint
        inp4: stage_in_s3_region
        inp5: stage_in_s3_signature_version
        inp6: input_reference
      out:
      - results
      run: '#stagein'
cwlVersion: v1.0