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

char** makeFlags( const std::vector<std::string>& flags )
{
  char** rawFlags    = new char*[ flags.size() ];
  unsigned int index = 0;

  for( auto&& flag : flags )
  {
    rawFlags[ index ] = new char[ flag.size() + 1 ];
    std::fill( rawFlags[index], rawFlags[index] + flag.size(), 0 );
    std::copy( flag.begin(), flag.end(), rawFlags[index ] );

    ++index;
  }

  return rawFlags;
}

CXChildVisitResult functionVisitor( CXCursor cursor, CXCursor parent, CXClientData clientData )
{
  if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) == 0 )
    return CXChildVisit_Recurse;

  CXCursorKind kind = clang_getCursorKind( cursor );
  CXString nameStr  = clang_getCursorSpelling( cursor );

  std::string name  = clang_getCString( nameStr );

  clang_disposeString( nameStr );

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
    std::vector<std::string> flags
      = { "-I/usr/include/c++/5",
          "-I/usr/include/x86_64-linux-gnu/c++/5",
          "-I/usr/include/c++/5/backward",
          "-I/usr/lib/gcc/x86_64-linux-gnu/5/include",
          "-I/usr/local/include",
          "-I/usr/lib/gcc/x86_64-linux-gnu/5/include-fixed",
          "-I/usr/include/x86_64-linux-gnu",
          "-I/usr/include",
          "-I/usr/bin/../lib/gcc/x86_64-linux-gnu/5.2.1/../../../../include/c++/5.2.1",
          "-I/usr/bin/../lib/gcc/x86_64-linux-gnu/5.2.1/../../../../include/x86_64-linux-gnu/c++/5.2.1",
          "-I/usr/bin/../lib/gcc/x86_64-linux-gnu/5.2.1/../../../../include/c++/5.2.1/backward",
          "-I/usr/local/include",
          "-I/usr/lib/llvm-3.6/bin/../lib/clang/3.6.2/include",
          "-I/usr/include/x86_64-linux-gnu",
          "-I/usr/include",
       "-I/usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/5.3.0/../../../../include/c++/5.3.0",
       "-I/usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/5.3.0/../../../../include/c++/5.3.0/x86_64-unknown-linux-gnu",
       "-I/usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/5.3.0/../../../../include/c++/5.3.0/backward",
       "-I/usr/local/include",
       "-I/usr/bin/../lib/clang/3.7.0/include",
       "-I/usr/include",
       "-I/home/bastian/PhD/Scifer/src"
      };

    auto rawFlags = makeFlags( flags );

    CXIndex index                     = clang_createIndex( false, true );
    CXTranslationUnit translationUnit = clang_parseTranslationUnit( index, resolvedPath.c_str(), rawFlags, flags.size(), 0, 0, CXTranslationUnit_None );

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
