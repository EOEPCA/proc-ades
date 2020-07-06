$graph:
  - baseCommand: s3-slstr-tiling
    class: CommandLineTool
    hints:
      DockerRequirement:
        dockerPull: 'wfp:0.1'
    id: clt
    inputs:
      inp1:
        inputBinding:
          position: 1
          prefix: '--input_reference'
        type: Directory
      inp2:
        inputBinding:
          position: 2
          prefix: '--tiling_level'
        type: string
    outputs:
      results:
        outputBinding:
          glob: .
        type: Any
    requirements:
      EnvVarRequirement:
        envDef:
          PATH: '/opt/anaconda/envs/env_s3/bin/:/opt/anaconda/envs/notebook/bin:/opt/anaconda/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
          PREFIX: /opt/anaconda/envs/env_s3
      ResourceRequirement: {}
    stderr: std.err
    stdout: std.out
  - class: Workflow
    doc: This service takes as input a Sentinel-3 SLSTR Level 2 (SL_2_LST____) product on DESCENDING pass and does the reprojection and tiling
    id: slstr-tiling
    inputs:
      input_reference:
        doc: This service takes as input a Sentinel-3 SLSTR Level 2 (SL_2_LST____) product on DESCENDING pass
        label: Sentinel-3 SLSTR Level-2 (SL_2_LST____ descending pass)
        'stac:collection': input_reference
        type: 'Directory[]'
      tiling_level:
        doc: Tiling level
        label: Tiling level
        type: string
    label: Sentinel-3 SLSTR Level-2 reprojection and tiling
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
          inp2: tiling_level
        out:
          - results
        run: '#clt'
        scatter: inp1
        scatterMethod: dotproduct
$namespaces:
  stac: 'http://www.me.net/stac/cwl/extension'
cwlVersion: v1.0