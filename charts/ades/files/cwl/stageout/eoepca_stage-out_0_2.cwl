class: CommandLineTool
baseCommand: stage-out
inputs:
  job:
    type: string
    inputBinding:
      position: 1
      prefix: --job
      valueFrom: $( inputs.job )

  ADES_STAGEOUT_STORAGE_HOST:
    type: string
    inputBinding:
      position: 2
      prefix: --store-host


  ADES_STAGEOUT_STORAGE_USERNAME:
    type: string  
    inputBinding:
      position: 3
      prefix: --store-username


  ADES_STAGEOUT_STORAGE_APIKEY:
    type: string
    inputBinding:
      position: 4
      prefix: --store-apikey


  outputfile:
    type: string
    inputBinding:
      position: 5
      prefix: --outputfile
      valueFrom: $( inputs.outputfile )

outputs: {}
requirements:
  InlineJavascriptRequirement: {}
  EnvVarRequirement:
    envDef:
      PATH: /opt/anaconda/envs/env_stageout/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

  ResourceRequirement: {}
hints:
  DockerRequirement:
    dockerPull: eoepca/stage-out:0.2