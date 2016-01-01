#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>

#ifdef __unix__
  #include <limits.h>
  #include <stdlib.h>
#endif

#include <iostream>
#include <string>
#include <type_traits>

std::string getCursorSpelling( CXCursor cursor )
{
  CXString cursorSpelling = clang_getCursorSpelling( cursor );
  std::string result      = clang_getCString( cursorSpelling );

  clang_disposeString( cursorSpelling );
  return result;
}

std::string getCursorKindSpelling( CXCursorKind cursorKind )
{
  CXString kindName  = clang_getCursorKindSpelling( cursorKind );
  std::string result = clang_getCString( kindName );

  clang_disposeString( kindName );
  return result;
}

/* Auxiliary function for resolving a (relative) path into an absolute path */
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

CXChildVisitResult functionVisitor( CXCursor cursor, CXCursor /* parent */, CXClientData /* clientData */ )
{
  if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) == 0 )
    return CXChildVisit_Continue;

  CXCursorKind kind = clang_getCursorKind( cursor );
  auto name         = getCursorSpelling( cursor );

  if( kind == CXCursorKind::CXCursor_FunctionDecl || kind == CXCursorKind::CXCursor_CXXMethod || kind == CXCursorKind::CXCursor_FunctionTemplate )
  {
    CXSourceRange extent           = clang_getCursorExtent( cursor );
    CXSourceLocation startLocation = clang_getRangeStart( extent );
    CXSourceLocation endLocation   = clang_getRangeEnd( extent );

    unsigned int startLine = 0, startColumn = 0;
    unsigned int endLine   = 0, endColumn   = 0;

    clang_getSpellingLocation( startLocation, nullptr, &startLine, &startColumn, nullptr );
    clang_getSpellingLocation( endLocation,   nullptr, &endLine, &endColumn, nullptr );

    std::cout << "  " << name << ": " << endLine - startLine << "\n";
  }

  return CXChildVisit_Recurse;
}

int main( int argc, char** argv )
{
  if( argc < 2 )
    return -1;

  auto resolvedPath = resolvePath( argv[1] );
  std::cerr << "Parsing " << resolvedPath << "...\n";

  CXCompilationDatabase_Error compilationDatabaseError;
  CXCompilationDatabase compilationDatabase = clang_CompilationDatabase_fromDirectory( ".", &compilationDatabaseError );
  CXCompileCommands compileCommands         = clang_CompilationDatabase_getCompileCommands( compilationDatabase, resolvedPath.c_str() );
  unsigned int numCompileCommands           = clang_CompileCommands_getSize( compileCommands );

  std::cerr << "Obtained " << numCompileCommands << " compile commands\n";

  CXIndex index = clang_createIndex( 0, 1 );
  CXTranslationUnit translationUnit;

  if( numCompileCommands == 0 )
  {
    constexpr const char* defaultArguments[] = {
      "-std=c++11",
      "-I/usr/include",
      "-I/usr/local/include"
    };

    translationUnit = clang_parseTranslationUnit( index,
                                                  resolvedPath.c_str(),
                                                  defaultArguments,
                                                  std::extent<decltype(defaultArguments)>::value,
                                                  0,
                                                  0,
                                                  CXTranslationUnit_None );

  }
  else
  {
    CXCompileCommand compileCommand = clang_CompileCommands_getCommand( compileCommands, 0 );
    unsigned int numArguments       = clang_CompileCommand_getNumArgs( compileCommand );
    char** arguments                = new char*[ numArguments ];

    for( unsigned int i = 0; i < numArguments; i++ )
    {
      CXString argument       = clang_CompileCommand_getArg( compileCommand, i );
      std::string strArgument = clang_getCString( argument );
      arguments[i]            = new char[ strArgument.size() + 1 ];

      std::fill( arguments[i],
                 arguments[i] + strArgument.size() + 1,
                 0 );

      std::copy( strArgument.begin(), strArgument.end(),
                 arguments[i] );

      clang_disposeString( argument );
    }

    translationUnit = clang_parseTranslationUnit( index, 0, arguments, numArguments, 0, 0, CXTranslationUnit_None );

    for( unsigned int i = 0; i < numArguments; i++ )
      delete[] arguments[i];

    delete[] arguments;
  }

  CXCursor rootCursor = clang_getTranslationUnitCursor( translationUnit );
  clang_visitChildren( rootCursor, functionVisitor, nullptr );

  clang_disposeTranslationUnit( translationUnit );
  clang_disposeIndex( index );

  clang_CompileCommands_dispose( compileCommands );
  clang_CompilationDatabase_dispose( compilationDatabase );
  return 0;
}
