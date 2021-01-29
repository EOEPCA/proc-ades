
baseCommand: stage-in
arguments: ['-t', './']
class: CommandLineTool
hints:
  DockerRequirement:
    dockerPull: eoepca/stage-in:0.9
id: stagein
inputs:
  stage_in_username:
    inputBinding:
      position: 1
      prefix: -u
    type: string?
  stage_in_password:
    inputBinding:
      position: 2
      prefix: -p
    type: string?
  stage_in_s3_endpoint:
    inputBinding:
      position: 3
      prefix: -e
    type: string?
  stage_in_s3_region:
    inputBinding:
      position: 4
      prefix: -r
    type: string?
  stage_in_s3_signature_version:
    inputBinding:
      position: 5
      prefix: -s
    type: string?
  input_reference:
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