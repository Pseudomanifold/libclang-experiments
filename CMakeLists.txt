CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( libclang-experiments CXX )

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

LIST( APPEND CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic" )

FIND_PACKAGE( libclang REQUIRED )
INCLUDE_DIRECTORIES( ${LIBCLANG_INCLUDE_DIRS} SYSTEM )

ADD_EXECUTABLE( function-visitor
  arguments.cc
  function-visitor.cc
  path_utilities.cc
)

ADD_EXECUTABLE( ast-dumper             ast-dumper.cc )
ADD_EXECUTABLE( ast-walker             ast-walker.cc )
ADD_EXECUTABLE( count-function-extents count-function-extents.cc )
ADD_EXECUTABLE( stockhausen            stockhausen.cc )

TARGET_LINK_LIBRARIES( function-visitor       ${LIBCLANG_LIBRARIES} )
TARGET_LINK_LIBRARIES( ast-dumper             ${LIBCLANG_LIBRARIES} )
TARGET_LINK_LIBRARIES( ast-walker             ${LIBCLANG_LIBRARIES} )
TARGET_LINK_LIBRARIES( stockhausen            ${LIBCLANG_LIBRARIES} )
TARGET_LINK_LIBRARIES( count-function-extents ${LIBCLANG_LIBRARIES} )
