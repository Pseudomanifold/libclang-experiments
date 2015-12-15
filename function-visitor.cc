#include <clang-c/Index.h>

#include <iostream>
#include <string>

CXChildVisitResult functionVisitor( CXCursor cursor, CXCursor parent, CXClientData clientData )
{
  CXCursorKind kind = clang_getCursorKind( cursor );
  std::string name  = clang_getCString( clang_getCursorSpelling( cursor ) );

  if( kind == CXCursorKind::CXCursor_FunctionDecl )
    std::cout << __PRETTY_FUNCTION__ << ": Detected a function declaration: " << name << std::endl;

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
