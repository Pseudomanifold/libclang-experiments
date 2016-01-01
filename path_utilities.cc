#include "path_utilities.hh"

#ifdef __unix__
  #include <limits.h>
  #include <stdlib.h>
#endif

std::string resolvePath( const char* path )
{
  std::string resolvedPath;

#ifdef __unix__
  char* resolvedPathRaw = new char[ PATH_MAX ];
  char* result          = realpath( path, resolvedPathRaw );

  if( result )
    resolvedPath = resolvedPathRaw;

  delete[] resolvedPathRaw;
#else
  resolvedPath = path;
#endif

  return resolvedPath;
}
