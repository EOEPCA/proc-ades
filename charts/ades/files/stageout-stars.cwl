cwlVersion: v1.0
baseCommand: Stars
doc: "Run Stars for staging data"
class: CommandLineTool
hints:
  DockerRequirement:
    dockerPull: terradue/stars:latest
id: stars
arguments:
- copy
- -v
- -r
- '4'
inputs: 
  ADES_STAGEOUT_AWS_PROFILE: string
  ADES_STAGEOUT_AWS_SERVICEURL: string
  ADES_STAGEOUT_AWS_ACCESS_KEY_ID: string
  ADES_STAGEOUT_AWS_SECRET_ACCESS_KEY: string
  aws_profiles_location:
    type: File
  output_destination:
    type: string
    inputBinding:
      position: 5
      prefix: -o
outputs: {}
requirements:
  EnvVarRequirement:
    envDef:
      PATH: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
      AWS__Profile: $(inputs.ADES_STAGEOUT_AWS_PROFILE)
      AWS__ProfilesLocation: $(inputs.aws_profiles_location.path)
      AWS__ServiceURL: $(inputs.ADES_STAGEOUT_AWS_SERVICEURL)
      AWS_ACCESS_KEY_ID: $(inputs.ADES_STAGEOUT_AWS_ACCESS_KEY_ID)
      AWS_SECRET_ACCESS_KEY: $(inputs.ADES_STAGEOUT_AWS_SECRET_ACCESS_KEY)
  ResourceRequirement: {}