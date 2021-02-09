cwlVersion: v1.0
baseCommand: Stars
doc: "Run Stars for staging data"
class: CommandLineTool
hints:
  DockerRequirement:
    dockerPull: terradue/stars-t2:latest
id: stars
arguments:
- copy
- --harvest
- -v
- -rel
- -r
- '4'
- -o
- ./
inputs: 
  aws_profile: 
    type: string?
  aws_service_url:
    type: string?
  aws_profiles_location:
    type: File?
outputs: {}
requirements:
  EnvVarRequirement:
    envDef:
      PATH: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
      # AWS__Profile: $(inputs.aws_profile)
      # AWS__ProfilesLocation: $(inputs.aws_profiles_location.path)
      # AWS__ServiceURL: $(inputs.aws_service_url)
  ResourceRequirement: {}
