cwlVersion: v1.0
baseCommand: Stars
doc: "Run Stars for staging input data"
class: CommandLineTool
hints:
  DockerRequirement:
    dockerPull: terradue/stars:latest
id: stars
arguments:
- copy
- -v
- -rel
- -r
- '4'
- -o
- ./
inputs: 
  input_reference:
    inputBinding:
      position: 6
    type: string[]
  ADES_STAGEIN_AWS_SERVICEURL: 
    type: string?
outputs: {}
requirements:
  EnvVarRequirement:
    envDef:
      PATH: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
      # AWS__Profile: $(inputs.aws_profile)
      # AWS__ProfilesLocation: $(inputs.aws_profiles_location.path)
      AWS__ServiceURL: $(inputs.ADES_STAGEIN_AWS_SERVICEURL)
  ResourceRequirement: {}