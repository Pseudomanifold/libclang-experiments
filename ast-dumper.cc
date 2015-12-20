#include <clang-c/Index.h>

#include <iostream>
#include <string>
#include <sstream>

std::string getCursorKindName( CXCursorKind cursorKind )
{
  CXString kindName  = clang_getCursorKindSpelling( cursorKind );
  std::string result = clang_getCString( kindName );

  clang_disposeString( kindName );
  return result;
}

std::string getCursorSpelling( CXCursor cursor )
{
  CXString cursorSpelling = clang_getCursorSpelling( cursor );
  std::string result      = clang_getCString( cursorSpelling );

  clang_disposeString( cursorSpelling );
  return result;
}

CXChildVisitResult visitor( CXCursor cursor, CXCursor /* parent */, CXClientData clientData )
{
  CXSourceLocation location = clang_getCursorLocation( cursor );
  if( clang_Location_isFromMainFile( location ) == 0 )
    return CXChildVisit_Continue;

  CXCursorKind cursorKind = clang_getCursorKind( cursor );

  unsigned int curLevel = *( reinterpret_cast<unsigned int*>( clientData ) );

  bool isFunctionOrMethod = cursorKind == CXCursorKind::CXCursor_CXXMethod
                         || cursorKind == CXCursorKind::CXCursor_FunctionDecl
                         || cursorKind == CXCursorKind::CXCursor_FunctionTemplate;

  std::ostringstream stream;
  stream << std::string( curLevel, '-' ) << " " << getCursorKindName( cursorKind );

  if( isFunctionOrMethod )
    stream << " (" << getCursorSpelling( cursor ) << ")\n";
  else
    stream << "\n";

  std::cerr << stream.str();

  unsigned int nextLevel = curLevel + 1;

  clang_visitChildren( cursor,
                       visitor,
                       &nextLevel ); 

  if( curLevel == 0 )
    std::cerr << "\n";

  return CXChildVisit_Continue;
}

int main( int argc, char** argv )
{
  if( argc < 2 )
    return -1;

  CXIndex index        = clang_createIndex( 0, 1 );
  CXTranslationUnit tu = clang_createTranslationUnit( index, argv[1] );
  CXCursor rootCursor  = clang_getTranslationUnitCursor( tu );

  // FIXME: Need to handle errors in TU creation and index creation. Need to
  // check the documentation here...

  unsigned int treeLevel = 0;

  clang_visitChildren( rootCursor, visitor, &treeLevel );

  clang_disposeTranslationUnit( tu );
  clang_disposeIndex( index );
  return 0;
}
