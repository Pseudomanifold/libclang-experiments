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

    clang++ -std=c++11 -emit-ast FILE

Optionally, you can add `-I` or any other parameter that is required for
compiling a given program.

# The experiments

Let me briefly explain how to use some of the experiments.

## `ast-dumper` & `ast-walker`

These programs walk an abstract syntax tree. They roughly follow the
implementation given in [this blog post](http://bastian.rieck.ru/blog/posts/2015/baby_steps_libclang_ast).
To use one of them&nbsp;(I am exemplarily demonstrating `ast-walker`),
you require the creation of an abstract syntax tree:

    $ clang++ -std=c++11 -emit-ast ../examples/tuple.cc
    $ ./ast-walker tuple.ast

This should result in an output similar to this:

     ClassDecl (foo)
    - CXXAccessSpecifier ()
    - CXXConstructor (foo)
    -- ParmDecl (n_)
    -- ParmDecl (c_)
    -- ParmDecl (d_)
    -- MemberRef (n)
    -- InitListExpr ()
    --- UnexposedExpr (n_)
    ---- DeclRefExpr (n_)
    -- MemberRef (c)
    -- InitListExpr ()
    --- UnexposedExpr (c_)
    ---- DeclRefExpr (c_)
    -- MemberRef (d)
    -- InitListExpr ()
    --- UnexposedExpr (d_)
    ---- DeclRefExpr (d_)
    -- CompoundStmt ()
    - FriendDecl ()
    -- FunctionDecl (operator<)
    --- ParmDecl (lh)
    ---- TypeRef (class foo)
    --- ParmDecl (rh)
    ---- TypeRef (class foo)
    --- CompoundStmt ()
    ---- ReturnStmt ()
    ----- UnexposedExpr ()
    ------ CallExpr (operator<)
    ------- UnexposedExpr ()
    -------- UnexposedExpr (tie)
    --------- CallExpr (tie)
    ---------- UnexposedExpr (tie)
    ----------- DeclRefExpr (tie)
    ------------ NamespaceRef (std)
    ---------- MemberRefExpr (n)
    ----------- DeclRefExpr (lh)
    ---------- MemberRefExpr (c)
    ----------- DeclRefExpr (lh)
    ---------- MemberRefExpr (d)
    ----------- DeclRefExpr (lh)
    ------- UnexposedExpr (operator<)
    -------- DeclRefExpr (operator<)
    ------- UnexposedExpr ()
    -------- UnexposedExpr (tie)
    --------- CallExpr (tie)
    ---------- UnexposedExpr (tie)
    ----------- DeclRefExpr (tie)
    ------------ NamespaceRef (std)
    ---------- MemberRefExpr (n)
    ----------- DeclRefExpr (rh)
    ---------- MemberRefExpr (c)
    ----------- DeclRefExpr (rh)
    ---------- MemberRefExpr (d)
    ----------- DeclRefExpr (rh)
    - CXXAccessSpecifier ()
    - FieldDecl (n)
    - FieldDecl (c)
    - FieldDecl (d)

## `count-function-extents`

Getting this program to run can be a *tiny* bit more trickier. We should
supply it with the default paths required by `clang++`. To determine
them, call  

    $ clang++ -v -c FILE

where `FILE` is a source file of your choice. The resulting output
should contain the line 

    #include <...> search starts here:

and a list of paths. Copy them and put them into one command-line
variable, like so:

    $ export CPLUS_INCLUDE_PATH=/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/7.1.1/../../../../include/c++/7.1.1:/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/7.1.1/../../../../include/c++/7.1.1/x86_64-pc-linux-gnu:/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/7.1.1/../../../../include/c++/7.1.1/backward:/usr/local/include:/usr/bin/../lib/clang/4.0.1/include

Afterwards, you can call the program on a test executable:

    $ ./count-function-extents ../examples/functions.cc

This should result in the following output:

    Obtained 0 compile commands
      theAnswerToLifeTheUniverseAndEverything: 3
      sum: 3
      square: 3
      cube: 3
      factorial: 6
      fibonacci: 14

You can also call the function for any executable specified in
a `compile_commands.json` data base:

    $ ./count-function-extents ../count-function-extents.cc
    Parsing /home/brieck/Projects/libclang-experiments/count-function-extents.cc...
    Obtained 1 compile commands
      getCursorSpelling: 7
      resolvePath: 17
      functionVisitor: 24
      main: 74

I am sure that these programs could be easily improved. I look forward
to your changes!

# Additional information

* https://clang.llvm.org/doxygen/group__CINDEX.html
* https://stackoverflow.com/questions/20280744/libclang-returns-too-much-info-about-function-declarations
