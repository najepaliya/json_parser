# json_parser

## purpose
- a simple C++ library to parse and index JSON according to [ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)

## features
- single header
- no external dependencies
- notable parsing and indexing runtime improvements compared to [nlohmann/json](https://github.com/nlohmann/json)

## benchmarks
- a table of the 10-run average time it takes to parse relatively large JSON files compared to the de facto standard C++ JSON library

| input (mb) | nlohmann/json (ms) | najepaliya/json_parser (ms) | runtime improvement |
| :--------- | :----------------: | :-------------------------: | :-----------------: |
| 10         |    395             | 222                         | 43.7%               |
| 25         | 1667               | 1371                        |  17.8%              |