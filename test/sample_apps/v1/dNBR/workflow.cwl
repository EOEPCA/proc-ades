$graph:
  - class: Workflow
    label: dNBR - produce the delta normalized difference between NIR and SWIR 22 over a pair of stac items
    doc: dNBR - produce the delta normalized difference between NIR and SWIR 22 over a pair of stac items
    id: dnbr
    requirements:
      - class: ScatterFeatureRequirement
      - class: SubworkflowFeatureRequirement
      - class: MultipleInputFeatureRequirement
    inputs:
      pre_stac_item:
        doc: Pre-event Sentinel-2 item
        type: string
      post_stac_item:
        doc: Post-event Sentinel-2 item
        type: string
      aoi:
        doc: area of interest as a bounding box
        type: string
      bands:
        type: string[]
        default: ["B8A", "B12", "SCL"]
    outputs:
      stac:
        outputSource:
          - node_stac/stac
        type: Directory
    steps:
      node_nbr:
        run: "#nbr_wf"
        in:
          stac_item: [pre_stac_item, post_stac_item]
          aoi: aoi
        out:
          - nbr
        scatter: stac_item
        scatterMethod: dotproduct
      node_dnbr:
        run: "#dnbr_clt"
        in:
          tifs:
            source: node_nbr/nbr
        out:
          - dnbr
      node_cog:
        run: "#gdal_cog_clt"
        in:
          tif:
            source: [node_dnbr/dnbr]
        out:
          - cog_tif
      node_stac:
        run: "#stacme_clt"
        in:
          tif:
            source: [node_cog/cog_tif]
          pre_stac_item: pre_stac_item
          post_stac_item: post_stac_item
        out:
          - stac
  - class: Workflow
    label: NBR - produce the normalized difference between NIR and SWIR 22 and convert to COG
    doc: NBR - produce the normalized difference between NIR and SWIR 22 and convert to COG
    id: nbr_wf
    requirements:
      - class: ScatterFeatureRequirement
      - class: SubworkflowFeatureRequirement
    inputs:
      stac_item:
        doc: Sentinel-2 item
        type: string
      aoi:
        doc: area of interest as a bounding box
        type: string
      bands:
        type: string[]
        default: ["B8A", "B12", "SCL"]
    outputs:
      nbr:
        outputSource:
          - node_cog/cog_tif
        type: File
    steps:
      node_stac:
        run: "#asset_single_clt"
        in:
          stac_item: stac_item
          asset: bands
        out:
          - asset_href
        scatter: asset
        scatterMethod: dotproduct
      node_subset:
        run: "#translate_clt"
        in:
          asset:
            source: node_stac/asset_href
          bbox: aoi
        out:
          - tifs
        scatter: asset
        scatterMethod: dotproduct
      node_nbr:
        run: "#band_math_clt"
        in:
          stac_item: stac_item
          tifs:
            source: [node_subset/tifs]
        out:
          - nbr_tif
      node_cog:
        run: "#gdal_cog_clt"
        in:
          tif:
            source: [node_nbr/nbr_tif]
        out:
          - cog_tif
  - class: CommandLineTool
    id: asset_single_clt
    requirements:
      DockerRequirement:
        dockerPull: docker.io/curlimages/curl:latest
      ShellCommandRequirement: {}
      InlineJavascriptRequirement: {}
    baseCommand: [curl, -s]
    arguments:
      - $( inputs.stac_item )
    stdout: message
    inputs:
      stac_item:
        type: string
      asset:
        type: string
    outputs:
      asset_href:
        type: Any
        outputBinding:
          glob: message
          loadContents: true
          outputEval: |-
            ${ var assets = JSON.parse(self[0].contents).assets;
            return assets[inputs.asset].href; }
  - class: CommandLineTool
    id: translate_clt
    requirements:
      InlineJavascriptRequirement: {}
      DockerRequirement:
        dockerPull: docker.io/osgeo/gdal
    baseCommand: gdal_translate
    arguments:
      - -projwin
      - valueFrom: ${ return inputs.bbox.split(",")[0]; }
      - valueFrom: ${ return inputs.bbox.split(",")[3]; }
      - valueFrom: ${ return inputs.bbox.split(",")[2]; }
      - valueFrom: ${ return inputs.bbox.split(",")[1]; }
      - -projwin_srs
      - valueFrom: ${ return inputs.epsg; }
      - valueFrom: "${ if (inputs.asset.startsWith(\"http\")) {\n\n     return \"/vsicurl/\" + inputs.asset; \n\n   } else { \n\n     return inputs.asset;\n\n   } \n}\n"
      - valueFrom: ${ return inputs.asset.split("/").slice(-1)[0]; }
    inputs:
      asset:
        type: string
      bbox:
        type: string
      epsg:
        type: string
        default: "EPSG:4326"
    outputs:
      tifs:
        outputBinding:
          glob: '*.tif'
        type: File
  - class: CommandLineTool
    id: band_math_clt
    requirements:
      InlineJavascriptRequirement: {}
      DockerRequirement:
        dockerPull: docker.io/terradue/otb-7.2.0
    baseCommand: otbcli_BandMathX
    arguments:
      - -out
      - valueFrom: ${ return inputs.stac_item.split("/").slice(-1)[0] + ".tif"; }
      - -exp
      - '(im3b1 == 8 or im3b1 == 9 or im3b1 == 0 or im3b1 == 1 or im3b1 == 2 or im3b1 == 10 or im3b1 == 11) ? -2 : (im1b1 - im2b1) / (im1b1 + im2b1)'
    inputs:
      tifs:
        type: File[]
        inputBinding:
          position: 5
          prefix: -il
          separate: true
      stac_item:
        type: string
    outputs:
      nbr_tif:
        outputBinding:
          glob: "*.tif"
        type: File
  - class: CommandLineTool
    id: gdal_cog_clt
    requirements:
      InlineJavascriptRequirement: {}
      DockerRequirement:
        dockerPull: osgeo/gdal
    baseCommand: gdal_translate
    arguments:
      - -co
      - COMPRESS=DEFLATE
      - -of
      - COG
      - valueFrom: ${ return inputs.tif }
      - valueFrom: ${ return inputs.tif.basename.replace(".tif", "") + '_cog.tif'; }
    inputs:
      tif:
        type: File
    outputs:
      cog_tif:
        outputBinding:
          glob: '*_cog.tif'
        type: File
  - class: CommandLineTool
    id: dnbr_clt
    requirements:
      InlineJavascriptRequirement: {}
      DockerRequirement:
        dockerPull: docker.io/terradue/otb-7.2.0
    baseCommand: otbcli_BandMathX
    arguments:
      - -out
      - dnbr.tif
      - -exp
      - '(im1b1 == -2 or im2b1 == -2 ) ? -20000 : (im2b1 - im1b1) * 10000'
    inputs:
      tifs:
        type: File[]
        inputBinding:
          position: 5
          prefix: -il
          separate: true
    outputs:
      dnbr:
        outputBinding:
          glob: "dnbr.tif"
        type: File
  - class: CommandLineTool
    id: stacme_clt
    requirements:
      InlineJavascriptRequirement: {}
      DockerRequirement:
        dockerPull: registry.gitlab.com/app-packages/terradue/dnbr-sentinel-2-cog/stacme:0.1.0-develop
    baseCommand: stacme
    arguments: []
    inputs:
      tif:
        type: File
        inputBinding:
          position: 1
      pre_stac_item:
        type: string
        inputBinding:
          position: 2
      post_stac_item:
        type: string
        inputBinding:
          position: 3
    outputs:
      stac:
        outputBinding:
          glob: "."
        type: Directory
cwlVersion: v1.0
$namespaces:
  s: https://schema.org/
s:softwareVersion: 0.1.0
schemas:
  - http://schema.org/version/9.0/schemaorg-current-http.rdf
