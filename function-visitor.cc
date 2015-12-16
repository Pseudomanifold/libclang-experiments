#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>

#include <algorithm>
#include <iostream>
#include <string>

#include <limits.h>
#include <stdlib.h>

std::string resolvePath( const char* path )
{
  std::string result;

  char* resolvedPath = new char[PATH_MAX];
  auto resolved      = realpath( path, resolvedPath );
  if( resolved )
    result = resolvedPath;

  delete[] resolvedPath;
  return result;
}

CXChildVisitResult functionVisitor( CXCursor cursor, CXCursor parent, CXClientData clientData )
{
  if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) == 0 )
    return CXChildVisit_Continue;

  CXCursorKind kind = clang_getCursorKind( cursor );
  std::string name  = clang_getCString( clang_getCursorSpelling( cursor ) );

  if( kind == CXCursorKind::CXCursor_FunctionDecl || kind == CXCursorKind::CXCursor_CXXMethod || kind == CXCursorKind::CXCursor_FunctionTemplate )
  {
    std::cerr << __PRETTY_FUNCTION__ << ": Detected a function declaration: " << name << std::endl;

    CXSourceRange extent           = clang_getCursorExtent( cursor );
    CXSourceLocation startLocation = clang_getRangeStart( extent );
    CXSourceLocation endLocation   = clang_getRangeEnd( extent );

    unsigned int startLine = 0, startColumn = 0;
    unsigned int endLine   = 0, endColumn   = 0;

    clang_getSpellingLocation( startLocation, nullptr, &startLine, &startColumn, nullptr );
    clang_getSpellingLocation( endLocation,   nullptr, &endLine, &endColumn, nullptr );
    std::cerr << "  Extent: " << startLine << "," << startColumn <<  "--" << endLine << "," << endColumn << "\n";

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

  std::cout << resolvedPath << "\n";

  CXCompilationDatabase_Error compilationDatabaseError;
  CXCompilationDatabase compilationDatabase = clang_CompilationDatabase_fromDirectory( ".", &compilationDatabaseError );

  CXCompileCommands compileCommands         = clang_CompilationDatabase_getCompileCommands( compilationDatabase, resolvedPath.c_str() );

  unsigned int numCompileCommands = clang_CompileCommands_getSize( compileCommands );
  std::cerr << "I have obtained " << numCompileCommands << " compile commands\n"; 

  if( numCompileCommands == 0 )
  {
    CXIndex index                     = clang_createIndex( false, true );
    CXTranslationUnit translationUnit = clang_parseTranslationUnit( index, resolvedPath.c_str(), nullptr, 0, 0, 0, CXTranslationUnit_None );

    CXCursor rootCursor = clang_getTranslationUnitCursor( translationUnit );
    clang_visitChildren( rootCursor, functionVisitor, nullptr );
  }
  else
  {
    // FIXME: Get the first compile command. I can always extend this later...
    CXCompileCommand compileCommand   = clang_CompileCommands_getCommand( compileCommands, 0 );

    unsigned int numArguments = clang_CompileCommand_getNumArgs( compileCommand );
    std::cerr << "Selected compile command has " << numArguments << " arguments\n";

    // FIXME: This is as leaky as the roof in the shed at my parents where I used
    // to live
    char** arguments = new char*[ numArguments ];

    for( unsigned int i = 0; i < numArguments; i++ )
    {
      CXString argument = clang_CompileCommand_getArg( compileCommand, i );

      std::string strArgument = clang_getCString( argument ); 
      arguments[i]            = new char[ strArgument.size() + 1 ];
      
      std::fill( arguments[i], arguments[i] + strArgument.size() + 1, 0 );

      std::copy( strArgument.begin(), strArgument.end(),
                 arguments[i] );

      std::cerr << "  " << strArgument << "\n";

      clang_disposeString( argument ); 
    }

    CXIndex index                     = clang_createIndex( false, true );
    CXTranslationUnit translationUnit = clang_parseTranslationUnit( index, 0, arguments, numArguments, 0, 0, CXTranslationUnit_None );

    CXCursor rootCursor = clang_getTranslationUnitCursor( translationUnit );
    clang_visitChildren( rootCursor, functionVisitor, nullptr );

    clang_disposeTranslationUnit( translationUnit );
    clang_disposeIndex( index );
  }

  clang_CompileCommands_dispose( compileCommands );
  clang_CompilationDatabase_dispose( compilationDatabase );
  return 0;
}
