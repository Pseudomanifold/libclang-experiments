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

## `ast-walker`

This programs walks an abstract syntax tree. It follows the
implementation given in [this blog
post](http://bastian.rieck.ru/blog/posts/2015/baby_steps_libclang_ast).
To use it, you require the creation of an abstract syntax tree:

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


# Additional information

* https://clang.llvm.org/doxygen/group__CINDEX.html
* https://stackoverflow.com/questions/20280744/libclang-returns-too-much-info-about-function-declarations
