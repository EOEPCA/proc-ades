$graph:
  - baseCommand: vegetation-index
    class: CommandLineTool
    hints:
      DockerRequirement:
        dockerPull: eoepca/vegetation-index:0.2
    id: clt
    inputs:
      inp1:
        inputBinding:
          position: 1
          prefix: --input_reference
        type: Directory
      inp2:
        inputBinding:
          position: 2
          prefix: --aoi
        type: string
    outputs:
      results:
        outputBinding:
          glob: .
        type: Directory
    requirements:
      ResourceRequirement:
        ramMax: 4096
        coresMax: 2
        tmpdirMax: 5120
        outdirMax: 5120
      EnvVarRequirement:
        envDef:
          PATH: /opt/anaconda/envs/env_vi/bin:/opt/anaconda/envs/env_vi/bin:/home/fbrito/.nvm/versions/node/v10.21.0/bin:/opt/anaconda/envs/notebook/bin:/opt/anaconda/bin:/usr/share/java/maven/bin:/opt/anaconda/bin:/opt/anaconda/envs/notebook/bin:/opt/anaconda/bin:/usr/share/java/maven/bin:/opt/anaconda/bin:/opt/anaconda/condabin:/opt/anaconda/envs/notebook/bin:/opt/anaconda/bin:/usr/lib64/qt-3.3/bin:/usr/share/java/maven/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/home/fbrito/.local/bin:/home/fbrito/bin:/home/fbrito/.local/bin:/home/fbrito/bin
          PREFIX: /opt/anaconda/envs/env_vi
    stderr: std.err
    stdout: std.out
  - class: Workflow
    doc: Vegetation index processor, the greatest
    id: vegetation-index
    inputs:
      aoi:
        doc: Area of interest in WKT
        label: Area of interest
        type: string
      input_reference:
        doc: EO product for vegetation index
        label: EO product for vegetation index
        type: Directory[]
    label: Vegetation index
    outputs:
      - id: wf_outputs
        outputSource:
          - node_1/results
        type:
          items: Directory
          type: array
    requirements:
      - class: ScatterFeatureRequirement
    steps:
      node_1:
        in:
          inp1: input_reference
          inp2: aoi
        out:
          - results
        run: '#clt'
        scatter: inp1
        scatterMethod: dotproduct
cwlVersion: v1.0