# `libclang-experiments` &mdash; Some experiments with `libclang` 

This repository contains some of [my](http://bastian.rieck.ru)
experiments with `libclang`, or, to be more precise, the `C` bindings of
`libclang`. I already detailed some of my experiments in two blog posts.
The first deals with [walking an abstract syntax tree](http://bastian.rieck.ru/blog/posts/2015/baby_steps_libclang_ast),
while the second deals with [counting the extents of a function](http://bastian.rieck.ru/blog/posts/2016/baby_steps_libclang_function_extents).  

# Requirements

* `libclang` (for parsing ASTs)
* `clang` (for creating ASTs)

# Building the examples

  $ mkdir build
  $ cd build
  $ cmake ../
  $ make

# Obtaining an AST

Some of the example programs require the explicit creation of an
abstract syntax tree, while others make use of a database of compile
commands. In order to obtain an abstract syntax tree, call `clang` with
the following parameters:

  clang++ -std=c++11 -emit-ast tuple.cc

Optionally, you can add `-I` or any other parameter that is required for
compiling a given program.

# Additional information

https://stackoverflow.com/questions/20280744/libclang-returns-too-much-info-about-function-declarations
