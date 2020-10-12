$graph:
  - baseCommand: vegetation-index
    class: CommandLineTool
    hints:
      DockerRequirement:
        dockerPull: terradue/eoepca-vegetation-index:0.1
    id: clt
    inputs:
      inp1:
        inputBinding:
          position: 1
          prefix: --input-reference
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
        type: Any
    requirements:
      EnvVarRequirement:
        envDef:
          PATH: /opt/anaconda/envs/env_vi/bin:/opt/anaconda/envs/env_vi/bin:/opt/anaconda/envs/env_default/bin:/opt/anaconda/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
          PREFIX: /opt/anaconda/envs/env_vi
      ResourceRequirement: {}
  - class: Workflow
    doc: Vegetation index processor
    id: vegetation-index
    inputs:
      aoi:
        doc: Area of interest in WKT
        label: Area of interest
        type: string
      input_reference:
        doc: EO product for vegetation index
        label: EO product for vegetation index
        stac:catalog: catalog.json
        stac:collection: source
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
  $namespaces:
    stac: http://www.me.net/stac/cwl/extension
  cwlVersion: v1.0