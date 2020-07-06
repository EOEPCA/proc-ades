cwlVersion: v1.0
class: CommandLineTool
inputs:
  main_input:
    type: string?
    label: Main input

  corellated_input:
    type:
      - "null"
      - type: array
        items: string
    label: 2 to 10 correlated inputs

  test_enum:
    type:
      type: array
      items:
        - "null"
        - type: enum
          symbols: ['test1', 'test2', 'test3']
    label: enum

  opt_string_array:
    type: string[]?

baseCommand: echo
outputs: []