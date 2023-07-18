# json_parser

## purpose
- a simple C++ library to parse and index JSON according to [ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)

## features
- single header
- no external dependencies
- notable parsing and indexing runtime improvements compared to [nlohmann/json](https://github.com/nlohmann/json)

## benchmarks
- a table of the 10-run average of the time it takes to parse relatively large JSON files compared to the de facto standard C++ JSON library

| input (mb) | nlohmann/json (ms) | najepaliya/json_parser with vs without object members (ms) | runtime improvement with / without key-value containers |
| :--------: | :----------------: | :--------------------------------------------------------: | :-----------------------------------------------------: |
| 1          | 112                | 83 vs 71                                                   | 26% / 37%                                               |
| 5          | 402                | 305 vs 251                                                 | 24% / 38%                                               |
| 25         | 2048               | 1584 vs 1253                                               | 23% / 39%                                               |