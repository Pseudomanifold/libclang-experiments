#include <clang-c/Index.h>

#include <iostream>
#include <string>

CXChildVisitResult functionVisitor( CXCursor cursor, CXCursor parent, CXClientData clientData )
{
  if( clang_Location_isFromMainFile( clang_getCursorLocation( cursor ) ) == 0 )
    return CXChildVisit_Continue;

  CXCursorKind kind = clang_getCursorKind( cursor );
  std::string name  = clang_getCString( clang_getCursorSpelling( cursor ) );

  if( kind == CXCursorKind::CXCursor_FunctionDecl )
  {
    std::cout << __PRETTY_FUNCTION__ << ": Detected a function declaration: " << name << std::endl;

    CXSourceRange extent           = clang_getCursorExtent( cursor );
    CXSourceLocation startLocation = clang_getRangeStart( extent );
    CXSourceLocation endLocation   = clang_getRangeEnd( extent );

    unsigned int startLine = 0, startColumn = 0;
    unsigned int endLine   = 0, endColumn   = 0;

    clang_getSpellingLocation( startLocation, nullptr, &startLine, &startColumn, nullptr );
    clang_getSpellingLocation( endLocation,   nullptr, &endLine, &endColumn, nullptr );
    std::cout << "  Extent: " << startLine << "," << startColumn <<  "--" << endLine << "," << endColumn << "\n";
  }

  return CXChildVisit_Recurse;
}

int main( int argc, char** argv )
{
  CXIndex index                     = clang_createIndex( false, true );
  CXTranslationUnit translationUnit = clang_parseTranslationUnit( index, 0, argv, argc, 0, 0, CXTranslationUnit_None );

  CXCursor rootCursor = clang_getTranslationUnitCursor( translationUnit );
  clang_visitChildren( rootCursor, functionVisitor, nullptr );

  clang_disposeTranslationUnit( translationUnit );
  clang_disposeIndex( index );

  return 0;
}
