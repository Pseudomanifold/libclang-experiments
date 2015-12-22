# Requirements

* `libclang` (for parsing ASTs)
* `clang` (for creating ASTs)

# Obtain an AST

    clang++ -std=c++11 -emit-ast tuple.cc

Not sure whether it's possible to filter out some stuff such as includes
that are not used. The visitor is trying to take care of that as much as
possible.

# Additional information

https://stackoverflow.com/questions/20280744/libclang-returns-too-much-info-about-function-declarations
